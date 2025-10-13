#include "AnemoneTasks/Task.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"

namespace Anemone
{
    // TaskOperations?
    void Task::Execute()
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

    void Task::Abandon()
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

    void Task::Dispatched(uint32_t id, TaskAwaiterHandle const& awaiter, TaskAwaiterHandle const& dependencyAwaiter)
    {
        AE_ASSERT(this->m_Awaiter == nullptr);
        AE_ASSERT(this->m_DependencyAwaiter == nullptr);
        AE_ASSERT(awaiter != nullptr);
        AE_ASSERT(dependencyAwaiter != nullptr);
        AE_ASSERT(awaiter != dependencyAwaiter);

        this->m_Awaiter = awaiter;
        this->m_DependencyAwaiter = dependencyAwaiter;

        AE_ASSERT(this->m_Id == 0);
        AE_ENSURE(this->m_Status == TaskStatus::Created, "Invalid task state");

        this->m_Status = TaskStatus::Dispatched;
        this->m_Id = id;
    }

    void Task::DispatchedToPending()
    {
        AE_ENSURE(this->m_Status == TaskStatus::Dispatched, "Invalid task state");
        this->m_Status = TaskStatus::Pending;
    }

    void Task::PendingToDispatched()
    {
        AE_ENSURE(this->m_Status == TaskStatus::Pending, "Invalid task state");
        this->m_Status = TaskStatus::Dispatched;
    }

    uint32_t Task::AcquireReference()
    {
        return this->m_ReferenceCount.fetch_add(1, std::memory_order::relaxed);
    }

    uint32_t Task::ReleaseReference()
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
}
