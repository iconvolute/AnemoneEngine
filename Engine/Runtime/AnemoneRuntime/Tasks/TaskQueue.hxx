#pragma once
#include "AnemoneRuntime/Threading/Spinlock.hxx"
#include "AnemoneRuntime/Intrusive.hxx"
#include "AnemoneRuntime/Tasks/Task.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"

namespace Anemone::Tasks
{
    class alignas(64) TaskQueue final
    {
    private:
        Spinlock m_Lock{};
        IntrusiveList<Task, Task> m_Items{};
        size_t m_Count{};

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

            UniqueLock _{this->m_Lock};
            this->m_Items.PushBack(task);
            ++this->m_Count;
        }

        Task* Pop()
        {
            UniqueLock _{this->m_Lock};
            Task* result = this->m_Items.PopFront();

            if (result)
            {
                --this->m_Count;
            }

            return result;
        }

        bool IsEmpty()
        {
            UniqueLock _{this->m_Lock};
            return this->m_Items.IsEmpty();
        }

        size_t GetCount()
        {
            UniqueLock _{this->m_Lock};
            return this->m_Count;
        }
    };
}
