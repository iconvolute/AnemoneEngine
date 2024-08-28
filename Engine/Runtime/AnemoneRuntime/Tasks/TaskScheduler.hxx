#pragma once
#include "AnemoneRuntime/Duration.hxx"
#include "AnemoneRuntime/Intrusive.hxx"
#include "AnemoneRuntime/Threading/Spinlock.hxx"
#include "AnemoneRuntime/Threading/Thread.hxx"
#include "AnemoneRuntime/Threading/CancellationToken.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/Diagnostic/Log.hxx"
#include "AnemoneRuntime/Tasks/Task.hxx"

#include <vector>

namespace Anemone::Tasks::Private
{
    RUNTIME_API void InitializeTaskScheduler();
    RUNTIME_API void FinalizeTaskScheduler();
}

namespace Anemone::Tasks
{
    class Awaiter;
    class Task;
}

namespace Anemone::Tasks
{
    RUNTIME_API void DispatchTask(
        Task& task,
        AwaiterHandle const& dependency,
        AwaiterHandle const& awaiter,
        TaskPriority priority);


    inline void DispatchTask(
        Task& task,
        AwaiterHandle const& dependency,
        AwaiterHandle const& awaiter)
    {
        return DispatchTask(
            task,
            dependency,
            awaiter,
            TaskPriority::Inherited);
    }

    RUNTIME_API void Wait(AwaiterHandle& awaiter);

    RUNTIME_API bool TryWait(AwaiterHandle& awaiter, Duration const& timeout);

    RUNTIME_API void Delay(Duration const& timeout);

    RUNTIME_API uint32_t GetWorkerCount();
}
