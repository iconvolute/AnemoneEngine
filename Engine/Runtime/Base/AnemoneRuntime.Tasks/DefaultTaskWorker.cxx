#include "AnemoneRuntime.Tasks/DefaultTaskWorker.hxx"
#include "AnemoneRuntime.Tasks/DefaultTaskScheduler.hxx"

namespace Anemone
{
    void DefaultTaskWorker::OnRun()
    {
        this->m_Scheduler->TaskWorkerEntryPoint(this->m_Index);
    }
}
