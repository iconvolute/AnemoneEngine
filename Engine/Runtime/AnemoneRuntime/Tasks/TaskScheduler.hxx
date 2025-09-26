#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Tasks/Awaiter.hxx"
#include "AnemoneRuntime/Tasks/Task.hxx"

namespace Anemone
{
    class TaskScheduler
    {
    public:
        TaskScheduler() = default;
        TaskScheduler(TaskScheduler const&) = delete;
        TaskScheduler(TaskScheduler&&) = delete;
        TaskScheduler& operator=(TaskScheduler const&) = delete;
        TaskScheduler& operator=(TaskScheduler&&) = delete;
        virtual ~TaskScheduler() = default;

    public:
        RUNTIME_API static void Initialize();
        RUNTIME_API static void Finalize();

        RUNTIME_API static TaskScheduler& Get();

        virtual void Schedule(
            Task& task,
            AwaiterHandle const& awaiter,
            AwaiterHandle const& dependency,
            TaskPriority priority) = 0;

        virtual void Wait(AwaiterHandle const& awaiter) = 0;

        virtual bool TryWait(AwaiterHandle const& awaiter, Duration timeout) = 0;

        virtual void Delay(Duration timeout) = 0;

        virtual uint32_t GetThreadsCount() const = 0;
    };
}
