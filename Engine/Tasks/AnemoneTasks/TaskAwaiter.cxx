#include "AnemoneTasks/TaskAwaiter.hxx"
#include "AnemoneTasks/Task.hxx"

namespace Anemone
{
    void TaskAwaiter::AddWaitingTask(Task& task)
    {
        UniqueLock scope{this->m_Lock};

        this->m_WaitList.PushBack(&task);
    }
}
