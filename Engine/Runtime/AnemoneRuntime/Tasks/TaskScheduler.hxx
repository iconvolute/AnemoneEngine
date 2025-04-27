#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Tasks/Awaiter.hxx"
#include "AnemoneRuntime/Tasks/Task.hxx"

namespace Anemone
{
    struct TaskScheduler final
    {
        TaskScheduler() = delete;

        RUNTIME_API static void Dispatch(
            Task& task,
            AwaiterHandle const& awaiter,
            AwaiterHandle const& dependency,
            TaskPriority priority);

        static void Dispatch(
            Task& task,
            AwaiterHandle const& awaiter,
            AwaiterHandle const& dependency)
        {
            Dispatch(task, awaiter, dependency, TaskPriority::Normal);
        }

        RUNTIME_API static void Execute(Task& task);

        RUNTIME_API static void Wait(AwaiterHandle const& awaiter);

        RUNTIME_API static bool TryWait(AwaiterHandle const& awaiter, Duration timeout);

        RUNTIME_API static void Delay(Duration timeout);

        RUNTIME_API static uint32_t GetWorkerCount();
    };
}
