#include "AnemoneRuntime/Tasks/TaskScheduler.hxx"
#include "AnemoneRuntime/Tasks/TaskScheduler2.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
//#include "AnemoneRuntime/Platform/Statics.hxx"
#include "AnemoneRuntime/Instant.hxx"

#include "TaskScheduler.hxx"
#include "TaskWorker.hxx"

namespace Anemone::Tasks::Private
{
    TaskScheduler::TaskScheduler(TaskSchedulerOptions const& options)
        : m_WorkerThreadsCount{options.WorkerThreadsCount}
    {
        this->m_Workers.resize(options.WorkerThreadsCount);
        this->m_Threads.resize(options.WorkerThreadsCount);

        for (uint32_t i = 0; i < options.WorkerThreadsCount; ++i)
        {
            this->m_Workers[i] = std::make_unique<TaskWorker>(this, i);
        }

        for (uint32_t i = 0; i < options.WorkerThreadsCount; ++i)
        {
            this->m_Threads[i] = Threading::Thread{
                Threading::ThreadStart{
                    .Name = fmt::format("TaskWorker {}", i),
                    .Priority = Threading::ThreadPriority::Normal,
                    .Callback = this->m_Workers[i].get(),
                }};
        }
    }

    TaskScheduler::~TaskScheduler()
    {
        AE_LOG(Debug, "Requesting cancellation\n");
        this->m_CancellationToken.Cancel();
        this->m_Semaphore.Release(static_cast<int32_t>(this->m_WorkerThreadsCount));

        AE_LOG(Debug, "Joining threads\n");
        for (auto& thread : this->m_Threads)
        {
            thread.Join();
        }

        AE_LOG(Debug, "Queue length:            {}\n", this->m_Queue.GetCount());

        while (Task* current = this->m_Queue.Pop())
        {
            this->Execute(*current);
        }

        for (auto& worker : this->m_Workers)
        {
            Duration const totalTime = worker->m_WaitingTime + worker->m_ProcessingTime;
            [[maybe_unused]] double const utilization = static_cast<double>(worker->m_ProcessingTime.ToMicroseconds()) / static_cast<double>(totalTime.ToMicroseconds());

            AE_LOG(Debug, "worker: {}, tasks: {}, waiting: {}, working: {}, utilization: {:.2f}%\n",
                worker->m_Index,
                worker->m_ProcessedTasks,
                worker->m_WaitingTime,
                worker->m_ProcessingTime,
                utilization * 100.0);
        }

        AE_LOG(Debug, "Drained queue length:    {}\n", this->m_Queue.GetCount());
        AE_LOG(Debug, "Awaiters leaked:         {}\n", Awaiter::s_TotalAllocations.load());
        AE_LOG(Debug, "Tasks leaked:            {}\n", Task::s_TotalAllocations.load());
    }


    uint32_t TaskScheduler::GenerateTaskId()
    {
        uint32_t result;

        do
        {
            result = this->m_IdGenerator.fetch_add(1, std::memory_order::relaxed);
        } while (result == 0);

        return result;
    }

    void TaskScheduler::Dispatch(
        Task& task,
        AwaiterHandle const& dependency,
        AwaiterHandle const& awaiter,
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
            this->m_Semaphore.Release();
        }
        else
        {
            task.DispatchedToPending();
            dependency->AddWaitingTask(task);
        }
    }

    void TaskScheduler::Execute(Task& task)
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
                AE_FATAL(child->GetDependencyAwaiter()->IsCompleted());
                child->PendingToDispatched();
                this->m_Queue.Push(child);
                this->m_Semaphore.Release();
            }
        }

        task.ReleaseReference();
    }

    void TaskScheduler::Wait(AwaiterHandle& awaiter)
    {
        AE_ASSERT(awaiter);

        Threading::ThisThread::WaitForCompletion(
            [&]
            {
                return awaiter->IsCompleted();
            },
            [&]
            {
                if (Task* current = this->m_Queue.Pop())
                {
                    this->Execute(*current);
                }
                else
                {
                    Threading::ThisThread::Sleep(s_DefaultTaskDelay);
                }

                return awaiter->IsCompleted();
            });
    }

    bool TaskScheduler::TryWait(AwaiterHandle& awaiter, Duration const& timeout)
    {
        if (timeout < Duration{})
        {
            return awaiter->IsCompleted();
        }

        AE_ASSERT(awaiter);

        Instant const started = Instant::Now();
        Duration elapsed{};

        Threading::ThisThread::WaitForCompletion(
            [&]
            {
                return (elapsed >= timeout) or awaiter->IsCompleted();
            },
            [&]
            {
                if (Task* current = this->m_Queue.Pop())
                {
                    this->Execute(*current);
                }
                else
                {
                    Threading::ThisThread::Sleep(s_DefaultTaskDelay);
                }

                elapsed = started.QueryElapsed();
                return (elapsed >= timeout) or awaiter->IsCompleted();
            });

        return awaiter->IsCompleted();
    }

    void TaskScheduler::Delay(Duration const& timeout)
    {
        Instant const started = Instant::Now();
        Duration elapsed{};

        Threading::ThisThread::WaitForCompletion(
            [&]
            {
                return elapsed >= timeout;
            },
            [&]
            {
                if (Task* current = this->m_Queue.Pop())
                {
                    this->Execute(*current);
                }
                else
                {
                    Threading::ThisThread::Sleep(s_DefaultTaskDelay);
                }

                elapsed = started.QueryElapsed();
                return elapsed >= timeout;
            });
    }
}

namespace Anemone::Tasks::Private
{
    static UninitializedObject<TaskScheduler> GTaskScheduler{};

    RUNTIME_API void InitializeTaskScheduler()
    {
        GTaskScheduler.Create(TaskSchedulerOptions{
            .WorkerThreadsCount = 4//Platform::GetProcessorProperties().LogicalCores,
        });
    }

    RUNTIME_API void FinalizeTaskScheduler()
    {
        GTaskScheduler.Destroy();
    }
}

namespace Anemone::Tasks
{
    RUNTIME_API void DispatchTask(
        Task& task,
        AwaiterHandle const& dependency,
        AwaiterHandle const& awaiter,
        TaskPriority priority)
    {
        Private::GTaskScheduler->Dispatch(task, dependency, awaiter, priority);
    }

    RUNTIME_API void Wait(AwaiterHandle& awaiter)
    {
        Private::GTaskScheduler->Wait(awaiter);
    }

    RUNTIME_API bool TryWait(AwaiterHandle& awaiter, Duration const& timeout)
    {
        return Private::GTaskScheduler->TryWait(awaiter, timeout);
    }

    RUNTIME_API void Delay(Duration const& timeout)
    {
        Private::GTaskScheduler->Delay(timeout);
    }

    RUNTIME_API uint32_t GetWorkerCount()
    {
        return Private::GTaskScheduler->GetWorkerCount();
    }
}
