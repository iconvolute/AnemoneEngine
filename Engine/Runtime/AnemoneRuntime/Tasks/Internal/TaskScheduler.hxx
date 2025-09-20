#pragma once
#include "AnemoneRuntime/Tasks/TaskScheduler.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/Threading/ConditionVariable.hxx"
#include "AnemoneRuntime/Threading/CancellationToken.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"
#include "AnemoneRuntime/Tasks/Internal/TaskWorker.hxx"
#include "AnemoneRuntime/Tasks/Internal/TaskQueue.hxx"

#include <atomic>
#include <vector>

namespace Anemone
{
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
            AwaiterHandle const& awaiter,
            AwaiterHandle const& dependency,
            TaskPriority priority) override;

        void Wait(AwaiterHandle const& awaiter) override;

        bool TryWait(AwaiterHandle const& awaiter, Duration timeout) override;

        void Delay(Duration timeout) override;

        uint32_t GetThreadsCount() const override;
    };
}
