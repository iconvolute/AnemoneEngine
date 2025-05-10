#include "AnemoneRuntime/Tasks/Internal/TaskScheduler.hxx"
#include "AnemoneRuntime/Base/Instant.hxx"
#include "AnemoneRuntime/Tasks/Internal/TaskWorker.hxx"
#include "AnemoneRuntime/Tasks/Task.hxx"
#include "AnemoneRuntime/System/Environment.hxx"
#include "AnemoneRuntime/Profiler/Profiler.hxx"

namespace Anemone::Internal
{
    UninitializedObject<TaskSchedulerStatics> GTaskSchedulerStatics{};
}

namespace Anemone::Threading
{
    extern void InitializeTaskScheduler()
    {
        Internal::GTaskSchedulerStatics.Create();
    }

    extern void FinalizeTaskScheduler()
    {
        Internal::GTaskSchedulerStatics.Destroy();
    }
}


namespace Anemone::Internal
{
    AE_DECLARE_PROFILE(TaskWorkerWait);
    AE_DECLARE_PROFILE(TaskWorkerProcess);

    uint32_t TaskSchedulerStatics::GenerateTaskId()
    {
        uint32_t result;

        do
        {
            result = this->m_IdGenerator.fetch_add(1, std::memory_order_relaxed);
        } while (result == 0);

        return result;
    }

    TaskSchedulerStatics::TaskSchedulerStatics()
    {
        //
        // Assume that main thread will contribute as well.
        //

        size_t const workerCount = Environment::GetLogicalCoresCount() - 1uz;

        this->m_Workers.resize(workerCount);
        this->m_Threads.resize(workerCount);

        for (uint32_t i = 0; i < workerCount; ++i)
        {
            this->m_Workers[i] = std::make_unique<TaskWorker>(i);
        }

        for (uint32_t i = 0; i < workerCount; ++i)
        {
            this->m_Threads[i] = Thread{
                ThreadStart{
                    .Name = fmt::format("TaskWorker {}", i),
                    .Priority = ThreadPriority::Normal,
                    .Callback = this->m_Workers[i].get(),
                }};
        }
    }

    TaskSchedulerStatics::~TaskSchedulerStatics()
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

        for (auto& thread : this->m_Threads)
        {
            thread.Join();
        }


        //
        // Drain queues.
        //

        while (Task* current = this->m_Queue.Pop())
        {
            TaskScheduler::Execute(*current);
        }
    }

    void TaskSchedulerStatics::Dispatch(
        Task& task,
        AwaiterHandle const& awaiter,
        AwaiterHandle const& dependency,
        TaskPriority priority)
    {
        task.SetPriority(priority);

        // Scheduler owns the task.
        task.AcquireReference();

        awaiter->AddDependency();

        task.Dispatched(this->GenerateTaskId(), awaiter, dependency);

        if (dependency->IsCompleted())
        {
            this->m_Queue.Push(&task);
            this->m_TasksCondition.NotifyOne();
        }
        else
        {
            task.DispatchedToPending();
            dependency->AddWaitingTask(task);
        }
    }

    void TaskSchedulerStatics::Execute(Task& task)
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

        task.ReleaseReference();
    }

    void TaskSchedulerStatics::Wait(AwaiterHandle const& awaiter)
    {
        AE_ASSERT(awaiter);

        WaitForCompletion(
            [&]
        {
            return awaiter->IsCompleted();
        },
            [&]
        {
            if (Task* current = Internal::GTaskSchedulerStatics->m_Queue.Pop())
            {
                Execute(*current);
            }

            return awaiter->IsCompleted();
        });
    }

    bool TaskSchedulerStatics::TryWait(AwaiterHandle const& awaiter, Duration timeout)
    {
        if (timeout < Duration{})
        {
            return awaiter->IsCompleted();
        }

        AE_ASSERT(awaiter);

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
                Execute(*current);
            }

            elapsed = started.QueryElapsed();
            return (elapsed >= timeout) or awaiter->IsCompleted();
        });

        return awaiter->IsCompleted();
    }

    void TaskSchedulerStatics::Delay(Duration timeout)
    {
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
                Execute(*current);
            }

            elapsed = started.QueryElapsed();
            return elapsed >= timeout;
        });
    }

    uint32_t TaskSchedulerStatics::GetWorkerCount() const
    {
        return static_cast<uint32_t>(this->m_Workers.size());
    }

    void TaskSchedulerStatics::TaskWorkerEntryPoint([[maybe_unused]] uint32_t workerId)
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
                    TaskScheduler::Execute(*task);
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
}

namespace Anemone
{
    void TaskScheduler::Dispatch(
        Task& task,
        AwaiterHandle const& awaiter,
        AwaiterHandle const& dependency,
        TaskPriority priority)
    {
        Internal::GTaskSchedulerStatics->Dispatch(task, awaiter, dependency, priority);
    }

    void TaskScheduler::Execute(Task& task)
    {
        Internal::GTaskSchedulerStatics->Execute(task);
    }

    void TaskScheduler::Wait(AwaiterHandle const& awaiter)
    {
        Internal::GTaskSchedulerStatics->Wait(awaiter);
    }

    bool TaskScheduler::TryWait(AwaiterHandle const& awaiter, Duration timeout)
    {
        return Internal::GTaskSchedulerStatics->TryWait(awaiter, timeout);
    }

    void TaskScheduler::Delay(Duration timeout)
    {
        Internal::GTaskSchedulerStatics->Delay(timeout);
    }

    uint32_t TaskScheduler::GetWorkerCount()
    {
        return Internal::GTaskSchedulerStatics->GetWorkerCount();
    }
}
