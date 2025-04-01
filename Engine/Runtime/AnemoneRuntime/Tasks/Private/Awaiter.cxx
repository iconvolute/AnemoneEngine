#include "AnemoneRuntime/Tasks/Awaiter.hxx"
#include "AnemoneRuntime/Tasks/Task.hxx"

namespace Anemone
{
    void Awaiter::AddWaitingTask(Task& task)
    {
        UniqueLock scope{this->m_Lock};

        this->m_WaitList.PushBack(&task);
    }
}
