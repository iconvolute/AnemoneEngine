#include "AnemoneRuntime/Tasks/Task.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

#include <utility>
#include <type_traits>

#define ENABLE_LOGGING false

namespace Anemone::Tasks
{
    std::atomic<size_t> Task::s_TotalAllocations{};

    void* Task::operator new(std::size_t size, std::align_val_t al)
    {
#if ENABLE_LOGGING
        auto const total =
#endif
            ++s_TotalAllocations;
#if ENABLE_LOGGING
        AE_TRACE(Warning, "Task::operator new(size={}, alignment={}, total={})", size, static_cast<std::size_t>(al), total);
#endif
        return ::operator new(size, al);
    }

    void* Task::operator new(std::size_t size)
    {
#if ENABLE_LOGGING
        auto const total =
#endif
            ++s_TotalAllocations;
#if ENABLE_LOGGING
        AE_TRACE(Warning, "Task::operator new(size={}, total={})", size, total);
#endif
        return ::operator new(size);
    }

    void Task::operator delete(void* ptr, std::align_val_t al) noexcept
    {
#if ENABLE_LOGGING
        auto const total =
#endif
            --s_TotalAllocations;
#if ENABLE_LOGGING
        AE_TRACE(Warning, "Task::operator delete(ptr={}, alignment={}, total={})", fmt::ptr(ptr), static_cast<std::size_t>(al), total);
#endif
        ::operator delete(ptr, al);
    }

    void Task::operator delete(void* ptr) noexcept
    {
#if ENABLE_LOGGING
        auto const total =
#endif
            --s_TotalAllocations;
#if ENABLE_LOGGING
        AE_TRACE(Warning, "Task::operator delete(ptr={}, total={})", fmt::ptr(ptr), total);
#endif
        ::operator delete(ptr);
    }
#undef ENABLE_LOGGING
}

namespace Anemone::Tasks
{
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

    void Task::Dispatched(uint32_t id, AwaiterHandle const& awaiter, AwaiterHandle const& dependencyAwaiter)
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
