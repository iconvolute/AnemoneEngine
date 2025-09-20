#include "AnemoneRuntime/Tasks/Internal/TaskScheduler.hxx"
#include "AnemoneRuntime/Base/Instant.hxx"
#include "AnemoneRuntime/Tasks/Internal/TaskWorker.hxx"
#include "AnemoneRuntime/Tasks/Task.hxx"
#include "AnemoneRuntime/System/Environment.hxx"
#include "AnemoneRuntime/Profiler/Profiler.hxx"

namespace Anemone
{
    namespace
    {
        Anemone::UninitializedObject<DefaultTaskScheduler> gDefaultTaskScheduler{};
    }

    namespace Internal
    {
        extern void InitializeTaskScheduler()
        {
            gDefaultTaskScheduler.Create();
        }

        extern void FinalizeTaskScheduler()
        {
            gDefaultTaskScheduler.Destroy();
        }
    }

    TaskScheduler& TaskScheduler::Get()
    {
        return *gDefaultTaskScheduler;
    }
}

namespace Anemone
{
    AE_DECLARE_PROFILE(TaskWorkerWait);
    AE_DECLARE_PROFILE(TaskWorkerProcess);

    DefaultTaskScheduler::DefaultTaskScheduler()
    {
        //
        // Assume that main thread will contribute as well.
        //

        size_t const workerCount = Environment::GetLogicalCoresCount() - 1uz;

        this->m_Workers.resize(workerCount);
        this->m_Threads.resize(workerCount);

        for (uint32_t i = 0; i < workerCount; ++i)
        {
            this->m_Workers[i] = MakeReference<DefaultTaskWorker>(i, this);
        }

        for (uint32_t i = 0; i < workerCount; ++i)
        {
            this->m_Threads[i] = Thread::Start(
                ThreadStart{
                    .Name = fmt::format("TaskWorker-{}", i),
                    .Priority = ThreadPriority::Normal,
                    .Callback = Reference{this->m_Workers[i].Get()},
                });
        }
    }

    DefaultTaskScheduler::~DefaultTaskScheduler()
    {
        this->m_CancellationToken.Cancel();

        {
            //
            // Notify all threads that we are shutting down.
            //

            UniqueLock scope{this->m_TasksLock};
            this->m_TasksCondition.NotifyAll();
        }


        //
        // Stop threads.
        //

        for (Reference<Thread>& thread : this->m_Threads)
        {
            thread->Join();
        }

        //
        // Drain task queues.
        //

        while (Task* current = this->m_Queue.Pop())
        {
            this->ExecuteInplace(*current);
        }
    }

    uint32_t DefaultTaskScheduler::GenerateTaskId()
    {
        uint32_t result;

        do
        {
            result = this->m_LastTaskId.fetch_add(1, std::memory_order::relaxed);
        } while (result == 0);

        return result;
    }

    void DefaultTaskScheduler::ExecuteInplace(Task& task)
    {
        task.Execute();

        // Try to get list of dependent tasks to flush them to queues.
        if (task.GetAwaiter()->NotifyCompleted())
        {
            //
            // Implementation detail:
            //
            // Concurrent task dispatch may cause the awaiter to be completed more than once.
            // Every time the awaiter is completed, it will try to flush the waiting list.
            //

            IntrusiveList<Task, Task> list{};

            task.GetAwaiter()->FlushWaitList(list);

            while (Task* child = list.PopFront())
            {
                AE_ENSURE(child->GetDependencyAwaiter()->IsCompleted());
                child->PendingToDispatched();
                this->m_Queue.Push(child);
                this->m_TasksCondition.NotifyOne();
            }
        }

        // Release task reference acquired in DefaultTaskScheduler::Schedule.
        task.ReleaseReference();
    }

    void DefaultTaskScheduler::TaskWorkerEntryPoint([[maybe_unused]] uint32_t workerId)
    {
        while (true)
        {
            //
            // Wait for tasks.
            //
            {
                AE_PROFILE_SCOPE(TaskWorkerWait);

                UniqueLock scope{this->m_TasksLock};

                this->m_TasksCondition.Wait(scope, [&]
                {
                    return this->m_CancellationToken.IsCancelled() or not this->m_Queue.IsEmpty();
                });
            }

            //
            // Process tasks.
            //
            {
                AE_PROFILE_SCOPE(TaskWorkerProcess);

                while (Task* task = this->m_Queue.Pop())
                {
                    this->ExecuteInplace(*task);
                }
            }

            //
            // Worker thread could be notified to stop during the processing of a task.
            //

            if (this->m_CancellationToken.IsCancelled())
            {
                break;
            }
        }
    }

    void DefaultTaskScheduler::Schedule(
        Task& task,
        AwaiterHandle const& awaiter,
        AwaiterHandle const& dependency,
        TaskPriority priority)
    {
        task.SetPriority(priority);

        // Scheduler takes reference to this task internally.
        task.AcquireReference();

        // Notify awaiter that there is a pending dependent task.
        awaiter->AddDependency();

        // Dispatch task.
        task.Dispatched(this->GenerateTaskId(), awaiter, dependency);

        if (dependency->IsCompleted())
        {
            // Dependency counter is completed, push task to the queue.
            this->m_Queue.Push(&task);
            this->m_TasksCondition.NotifyOne();
        }
        else
        {
            // Dependency is not completed, add task to the pending list.
            task.DispatchedToPending();
            dependency->AddWaitingTask(task);
        }
    }

    void DefaultTaskScheduler::Wait(
        AwaiterHandle const& awaiter)
    {
        AE_ASSERT(awaiter);

        WaitForCompletion(
            [&]
        {
            return awaiter->IsCompleted();
        },
            [&]
        {
            if (Task* current = this->m_Queue.Pop())
            {
                ExecuteInplace(*current);
            }

            return awaiter->IsCompleted();
        });
    }

    bool DefaultTaskScheduler::TryWait(AwaiterHandle const& awaiter, Duration timeout)
    {
        AE_ASSERT(awaiter);

        if (timeout <= Duration{})
        {
            return awaiter->IsCompleted();
        }

        Instant const started = Instant::Now();

        Duration elapsed{};

        WaitForCompletion(
            [&]
        {
            return (elapsed >= timeout) or awaiter->IsCompleted();
        },
            [&]
        {
            if (Task* current = this->m_Queue.Pop())
            {
                this->ExecuteInplace(*current);
            }

            elapsed = started.QueryElapsed();

            return (elapsed >= timeout) or awaiter->IsCompleted();
        });

        return awaiter->IsCompleted();
    }

    void DefaultTaskScheduler::Delay(Duration timeout)
    {
        if (timeout <= Duration{})
        {
            return;
        }

        Instant const started = Instant::Now();
        Duration elapsed{};

        WaitForCompletion(
            [&]
        {
            return elapsed >= timeout;
        },
            [&]
        {
            if (Task* current = this->m_Queue.Pop())
            {
                this->ExecuteInplace(*current);
            }

            elapsed = started.QueryElapsed();

            return elapsed >= timeout;
        });
    }

    uint32_t DefaultTaskScheduler::GetThreadsCount() const
    {
        return static_cast<uint32_t>(this->m_Workers.size());
    }

}
