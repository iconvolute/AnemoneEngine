#pragma once
#include "AnemoneRuntime.Interop/Headers.hxx"
#include "AnemoneRuntime.Base/Duration.hxx"
#include "AnemoneRuntime.Tasks/Task.hxx"

namespace Anemone
{
    class Task;
    class TaskAwaiter;

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
        static void Initialize();
        static void Finalize();

        static TaskScheduler& Get();

        virtual void Schedule(
            Task& task,
            TaskAwaiterHandle const& awaiter,
            TaskAwaiterHandle const& dependency,
            TaskPriority priority) = 0;

        virtual void Wait(TaskAwaiterHandle const& awaiter) = 0;

        virtual bool TryWait(TaskAwaiterHandle const& awaiter, Duration timeout) = 0;

        virtual void Delay(Duration timeout) = 0;

        virtual uint32_t GetThreadsCount() const = 0;
    };
}
