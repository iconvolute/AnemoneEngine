#pragma once
#include "AnemoneRuntime/Diagnostic/Assert.hxx"
#include "AnemoneRuntime/Threading/Thread.hxx"
#include "AnemoneRuntime/Threading/CancellationToken.hxx"
#include "AnemoneRuntime/Threading/SemaphoreSlim.hxx"

#include "TaskQueue.hxx"

#include <atomic>
#include <vector>

namespace Anemone::Tasks::Private
{
    class TaskWorker;

    struct TaskSchedulerOptions final
    {
        uint32_t WorkerThreadsCount{};
    };

    class TaskScheduler final
    {
    private:
        std::atomic_uint32_t m_IdGenerator{};

        static constexpr Duration s_DefaultTaskDelay{Duration::FromMilliseconds(5)};

    public:
        uint32_t GenerateTaskId();

    public:
        explicit TaskScheduler(TaskSchedulerOptions const& options);
        TaskScheduler(TaskScheduler const&) = delete;
        TaskScheduler(TaskScheduler&&) = delete;
        TaskScheduler& operator=(TaskScheduler const&) = delete;
        TaskScheduler& operator=(TaskScheduler&&) = delete;
        ~TaskScheduler();

    public:
        TaskQueue m_Queue{};
        std::vector<Threading::Thread> m_Threads{};
        std::vector<std::unique_ptr<TaskWorker>> m_Workers{};
        Threading::CancellationToken m_CancellationToken{};
        Threading::SemaphoreSlim m_Semaphore{0};
        uint32_t m_WorkerThreadsCount{};

    public:
        void Dispatch(
            Task& task,
            AwaiterHandle const& awaiter,
            AwaiterHandle const& dependency,
            TaskPriority priority);

        void Execute(Task& task);

        void Wait(AwaiterHandle& awaiter);

        bool TryWait(AwaiterHandle& awaiter, Duration const& timeout);

        void Delay(Duration const& timeout);

        uint32_t GetWorkerCount() const
        {
            return this->m_WorkerThreadsCount;
        }
    };
}
