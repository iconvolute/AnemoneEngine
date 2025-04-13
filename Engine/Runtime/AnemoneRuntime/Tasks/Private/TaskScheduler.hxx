#pragma once
#include "AnemoneRuntime/Tasks/TaskScheduler.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/Threading/ConditionVariable.hxx"
#include "AnemoneRuntime/Threading/CancellationToken.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Tasks/Private/TaskWorker.hxx"
#include "AnemoneRuntime/Tasks/Private/TaskQueue.hxx"

#include <atomic>
#include <vector>

namespace Anemone::Private
{
    class TaskWorker;

    class TaskSchedulerStatics final
    {
        friend class TaskWorker;

    private:
        std::atomic_uint32_t m_IdGenerator{};
        TaskQueue m_Queue{};
        std::vector<Thread> m_Threads{};
        std::vector<std::unique_ptr<TaskWorker>> m_Workers{};
        CriticalSection m_TasksLock{};
        ConditionVariable m_TasksCondition{};
        CancellationToken m_CancellationToken{};

    public:
        uint32_t GenerateTaskId();

    public:
        TaskSchedulerStatics();
        TaskSchedulerStatics(TaskSchedulerStatics const&) = delete;
        TaskSchedulerStatics(TaskSchedulerStatics&&) = delete;
        TaskSchedulerStatics& operator=(TaskSchedulerStatics const&) = delete;
        TaskSchedulerStatics& operator=(TaskSchedulerStatics&&) = delete;
        ~TaskSchedulerStatics();

    public:
        void Dispatch(
            Task& task,
            AwaiterHandle const& awaiter,
            AwaiterHandle const& dependency,
            TaskPriority priority);

        void Execute(Task& task);
        void Wait(AwaiterHandle const& awaiter);
        bool TryWait(AwaiterHandle const& awaiter, Duration timeout);
        void Delay(Duration timeout);
        uint32_t GetWorkerCount() const;

    private:
        void TaskWorkerEntryPoint(uint32_t workerId);
    };

    extern UninitializedObject<TaskSchedulerStatics> GTaskSchedulerStatics;
}
