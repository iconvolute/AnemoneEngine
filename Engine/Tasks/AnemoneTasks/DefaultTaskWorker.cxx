#include "AnemoneTasks/DefaultTaskWorker.hxx"
#include "AnemoneTasks/DefaultTaskScheduler.hxx"

namespace Anemone
{
    void DefaultTaskWorker::OnRun()
    {
        this->m_Scheduler->TaskWorkerEntryPoint(this->m_Index);
    }
}
