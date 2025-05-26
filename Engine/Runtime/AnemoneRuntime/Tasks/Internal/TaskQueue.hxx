#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Threading/Spinlock.hxx"
#include "AnemoneRuntime/Base/Intrusive.hxx"
#include "AnemoneRuntime/Tasks/Task.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"

namespace Anemone::Internal
{
    class alignas(64) TaskQueue final
    {
    private:
        mutable Spinlock m_lock{};
        IntrusiveList<Task, Task> m_items{};
        size_t m_count{};

    public:
        TaskQueue() = default;
        TaskQueue(TaskQueue const&) = delete;
        TaskQueue(TaskQueue&&) = delete;
        TaskQueue& operator=(TaskQueue const&) = delete;
        TaskQueue& operator=(TaskQueue&&) = delete;
        ~TaskQueue() = default;

    public:
        void Push(Task* task)
        {
            AE_ASSERT(task->GetDependencyAwaiter()->IsCompleted());

            UniqueLock scope{this->m_lock};
            this->m_items.PushBack(task);
            ++this->m_count;
        }

        Task* Pop()
        {
            UniqueLock scope{this->m_lock};
            Task* result = this->m_items.PopFront();

            if (result)
            {
                --this->m_count;
            }

            return result;
        }

        bool IsEmpty() const
        {
            UniqueLock scope{this->m_lock};
            return this->m_items.IsEmpty();
        }

        size_t GetCount() const
        {
            UniqueLock scope{this->m_lock};
            return this->m_count;
        }
    };
}
