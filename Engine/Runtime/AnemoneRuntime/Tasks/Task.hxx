#pragma once
#include "AnemoneRuntime/Flags.hxx"
#include "AnemoneRuntime/Intrusive.hxx"
#include "AnemoneRuntime/Reference.hxx"
#include "AnemoneRuntime/Tasks/Awaiter.hxx"

namespace Anemone
{
    struct TaskScheduler;

    enum class TaskStatus : uint8_t
    {
        // Task was created.
        Created,

        // All dependencies of the task were completed. It was dispatched to the worker threads.
        Dispatched,

        // Task is waiting for dependencies to complete. It will be dispatched once all dependencies are completed.
        Pending,

        // Task is being executed.
        Executing,

        // Task is completed.
        Completed,

        // Task was cancelled.
        Cancelled,

        // Task execution was abandoned.
        Abandoned,
    };

    enum class TaskPriority : uint8_t
    {
        Critical,
        High,
        Normal,
        Low,
        Background,
        Inherited,
    };

    enum class TaskOption : uint8_t
    {
        None = 0u,
        LongRunning = 1u << 0u,
        Dispose = 1u << 1u,
    };

    using TaskOptions = Flags<TaskOption>;

    class RUNTIME_API Task : private IntrusiveListNode<Task, Task>
    {
        friend struct IntrusiveList<Task, Task>;
        friend class Reference<Task>;
        friend struct TaskScheduler;

    private:
        AwaiterHandle m_Awaiter{};
        AwaiterHandle m_DependencyAwaiter{};
        std::atomic<uint32_t> m_ReferenceCount{};
        TaskOptions m_Options{TaskOption::Dispose};
        TaskPriority m_Priority{TaskPriority::Inherited};
        TaskStatus m_Status{TaskStatus::Created};
        uint32_t m_Id{};

    public:
        Task() = default;
        Task(Task const&) = delete;
        Task(Task&&) = delete;
        Task& operator=(Task const&) = delete;
        Task& operator=(Task&&) = delete;
        virtual ~Task() = default;

    protected:
        virtual void OnExecute() { }

    public:
        // TaskOperations?
        void Execute();
        void Abandon();
        void Dispatched(uint32_t id, AwaiterHandle const& awaiter, AwaiterHandle const& dependencyAwaiter);
        void DispatchedToPending();
        void PendingToDispatched();

    public:
        AwaiterHandle& GetAwaiter()
        {
            return this->m_Awaiter;
        }

        AwaiterHandle& GetDependencyAwaiter()
        {
            return this->m_DependencyAwaiter;
        }

        TaskOptions GetOptions() const
        {
            return this->m_Options;
        }

        TaskPriority GetPriority() const
        {
            return this->m_Priority;
        }

        void SetPriority(TaskPriority value)
        {
            this->m_Priority = value;
        }

        TaskStatus GetStatus() const
        {
            return this->m_Status;
        }

        uint32_t GetId() const
        {
            return this->m_Id;
        }

    public:
        uint32_t AcquireReference();

        uint32_t ReleaseReference();
    };

    using TaskHandle = Reference<Task>;
}
