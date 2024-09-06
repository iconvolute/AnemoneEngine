#pragma once
#include "AnemoneRuntime/Flags.hxx"
#include "AnemoneRuntime/Intrusive.hxx"
#include "AnemoneRuntime/Reference.hxx"
#include "AnemoneRuntime/Tasks/Awaiter.hxx"

namespace Anemone::Tasks
{
    class Task;
    class TaskScheduler;
    class TaskWorker;
    class TaskDispatcher;
    class TaskContext;

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

    public:
        static std::atomic<size_t> s_TotalAllocations;

        void* operator new(std::size_t size, std::align_val_t al);
        void* operator new(std::size_t size);
        void operator delete(void* ptr, std::align_val_t al) noexcept;
        void operator delete(void* ptr) noexcept;

        void* operator new[](std::size_t size, std::align_val_t al) = delete;
        void* operator new[](std::size_t size) = delete;
        void operator delete[](void* ptr, std::align_val_t al) noexcept = delete;
        void operator delete[](void* ptr) noexcept = delete;

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

    public: // TaskOperations?
        void Execute()
        {
            AE_ASSERT(this->m_Awaiter != nullptr);
            AE_ASSERT(this->m_DependencyAwaiter != nullptr);

            AE_ASSERT(this->m_Id != 0);

            switch (this->m_Status)
            {
            case TaskStatus::Dispatched:
            case TaskStatus::Pending:
                this->m_Status = TaskStatus::Executing;
                this->OnExecute();
                this->m_Status = TaskStatus::Completed;
                break;

            case TaskStatus::Cancelled:
            case TaskStatus::Abandoned:
                break;

            case TaskStatus::Executing:
            case TaskStatus::Completed:
            case TaskStatus::Created:
                AE_PANIC("Invalid task state");
            }
        }

        void Abandon()
        {
            switch (this->m_Status)
            {
            case TaskStatus::Dispatched:
            case TaskStatus::Pending:
            case TaskStatus::Executing:
                this->m_Status = TaskStatus::Abandoned;
                break;

            case TaskStatus::Cancelled:
            case TaskStatus::Abandoned:
                break;

            case TaskStatus::Completed:
            case TaskStatus::Created:
                AE_PANIC("Invalid task state");
            }
        }

        void Dispatched(uint32_t id, AwaiterHandle const& awaiter, AwaiterHandle const& dependencyAwaiter)
        {
            AE_ASSERT(this->m_Awaiter == nullptr);
            AE_ASSERT(this->m_DependencyAwaiter == nullptr);

            this->m_Awaiter = awaiter;
            this->m_DependencyAwaiter = dependencyAwaiter;

            AE_ASSERT(this->m_Id == 0);
            AE_ENSURE(this->m_Status == TaskStatus::Created, "Invalid task state");

            this->m_Status = TaskStatus::Dispatched;
            this->m_Id = id;
        }

        void DispatchedToPending()
        {
            AE_ENSURE(this->m_Status == TaskStatus::Dispatched, "Invalid task state");
            this->m_Status = TaskStatus::Pending;
        }

        void PendingToDispatched()
        {
            AE_ENSURE(this->m_Status == TaskStatus::Pending, "Invalid task state");
            this->m_Status = TaskStatus::Dispatched;
        }

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

        uint32_t AcquireReference()
        {
            return this->m_ReferenceCount.fetch_add(1, std::memory_order::relaxed);
        }

        uint32_t ReleaseReference()
        {
            uint32_t const result = this->m_ReferenceCount.fetch_sub(1, std::memory_order::acq_rel) - 1;
            if (result == 0)
            {
                if (this->m_Options.Has(TaskOption::Dispose))
                {
                    delete this;
                }

                return 0;
            }

            return result;
        }
    };

    using TaskHandle = Reference<Task>;
}
