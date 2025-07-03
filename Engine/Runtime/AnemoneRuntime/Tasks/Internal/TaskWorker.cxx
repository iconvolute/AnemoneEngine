#include "AnemoneRuntime/Tasks/Internal/TaskWorker.hxx"
#include "AnemoneRuntime/Tasks/Internal/TaskScheduler.hxx"
#include "AnemoneRuntime/Tasks/TaskScheduler.hxx"
#include "AnemoneRuntime/Tasks/Task.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"

namespace Anemone
{
    void DefaultTaskWorker::OnRun()
    {
        this->m_Scheduler->TaskWorkerEntryPoint(this->m_Index);
    }
}
