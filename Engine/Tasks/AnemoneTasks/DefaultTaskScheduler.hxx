#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneTasks/TaskScheduler.hxx"
#include "AnemoneTasks/TaskQueue.hxx"
#include "AnemoneTasks/Task.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/Threading/ConditionVariable.hxx"
#include "AnemoneRuntime/Threading/CancellationToken.hxx"
#include "AnemoneRuntime/Base/Reference.hxx"
#include "AnemoneRuntime/Base/Duration.hxx"


#include <atomic>
#include <vector>

namespace Anemone
{
    class Thread;

    class DefaultTaskWorker;

    class DefaultTaskScheduler final : public TaskScheduler
    {
    private:
        std::atomic_uint32_t m_LastTaskId{};
        TaskQueue m_Queue{};
        std::vector<Reference<Thread>> m_Threads{};
        std::vector<Reference<DefaultTaskWorker>> m_Workers{};
        CriticalSection m_TasksLock{};
        ConditionVariable m_TasksCondition{};
        CancellationToken m_CancellationToken{};

    public:
        DefaultTaskScheduler();

        DefaultTaskScheduler(DefaultTaskScheduler const&) = delete;

        DefaultTaskScheduler(DefaultTaskScheduler&&) = delete;

        DefaultTaskScheduler& operator=(DefaultTaskScheduler const&) = delete;

        DefaultTaskScheduler& operator=(DefaultTaskScheduler&&) = delete;

        ~DefaultTaskScheduler() override;

    public: // internal
        uint32_t GenerateTaskId();

        void ExecuteInplace(Task& task);

        void TaskWorkerEntryPoint(uint32_t workerId);

    public:
        void Schedule(
            Task& task,
            TaskAwaiterHandle const& awaiter,
            TaskAwaiterHandle const& dependency,
            TaskPriority priority) override;

        void Wait(TaskAwaiterHandle const& awaiter) override;

        bool TryWait(TaskAwaiterHandle const& awaiter, Duration timeout) override;

        void Delay(Duration timeout) override;

        uint32_t GetThreadsCount() const override;
    };
}
