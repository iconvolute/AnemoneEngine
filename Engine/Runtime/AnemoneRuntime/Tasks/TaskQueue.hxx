#pragma once
#include "AnemoneRuntime/Threading/Spinlock.hxx"
#include "AnemoneRuntime/Intrusive.hxx"
#include "AnemoneRuntime/Tasks/Task.hxx"
#include "AnemoneRuntime/Diagnostic/Debug.hxx"

namespace Anemone::Tasks
{
    class alignas(64) TaskQueue final
    {
    private:
        Threading::Spinlock m_Lock{};
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

            Threading::UniqueLock lock{this->m_Lock};
            this->m_Items.PushBack(task);
            ++this->m_Count;
        }

        Task* Pop()
        {
            Threading::UniqueLock lock{this->m_Lock};
            Task* result = this->m_Items.PopFront();

            if (result)
            {
                --this->m_Count;
            }

            return result;
        }

        bool IsEmpty()
        {
            Threading::UniqueLock lock{this->m_Lock};
            return this->m_Items.IsEmpty();
        }

        size_t GetCount()
        {
            Threading::UniqueLock lock{this->m_Lock};
            return this->m_Count;
        }
    };
}
