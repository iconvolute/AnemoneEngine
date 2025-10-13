#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Base/FunctionRef.hxx"
#include "AnemoneTasks/Task.hxx"
#include "AnemoneTasks/TaskAwaiter.hxx"

namespace Anemone
{
    struct Parallel final
    {
        Parallel() = delete;

        TASKS_API static void For(
            size_t count,
            size_t batch,
            FunctionRef<void(size_t index, size_t count)> callback,
            FunctionRef<void(size_t count)> finalize,
            size_t workers = 0,
            TaskPriority priority = TaskPriority::Inherited);

        static void For(
            size_t count,
            size_t batch,
            FunctionRef<void(size_t index, size_t count)> callback,
            size_t workers = 0,
            TaskPriority priority = TaskPriority::Inherited)
        {
            For(count, batch, callback, [](size_t)
            {
            }, workers, priority);
        }
    };
}

// New design on tasks public API.
namespace Anemone
{
    class IParallelTask
    {
    public:
        IParallelTask() = default;
        IParallelTask(IParallelTask const&) = default;
        IParallelTask(IParallelTask&&) = default;
        IParallelTask& operator=(IParallelTask const&) = default;
        IParallelTask& operator=(IParallelTask&&) = default;
        virtual ~IParallelTask() = default;

    public:
        virtual void OnExecute(size_t index, size_t count) = 0;
        virtual void OnFinalize(size_t count) = 0;

        TaskAwaiterHandle Schedule(size_t index, size_t count);
        TaskAwaiterHandle Schedule(size_t index, size_t count, TaskAwaiterHandle const& dependency);
    };

    class ITask
    {
    public:
        ITask() = default;
        ITask(ITask const&) = default;
        ITask(ITask&&) = default;
        ITask& operator=(ITask const&) = default;
        ITask& operator=(ITask&&) = default;
        virtual ~ITask() = default;

    public:
        virtual void OnExecute() = 0;

        TaskAwaiterHandle Schedule();
        TaskAwaiterHandle Schedule(TaskAwaiterHandle const& dependency);
    };

}
