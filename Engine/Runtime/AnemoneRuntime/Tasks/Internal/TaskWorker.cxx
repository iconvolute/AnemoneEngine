#include "AnemoneRuntime/Tasks/Internal/TaskWorker.hxx"
#include "AnemoneRuntime/Tasks/Internal/TaskScheduler.hxx"
#include "AnemoneRuntime/Tasks/TaskScheduler.hxx"
#include "AnemoneRuntime/Tasks/Task.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"

namespace Anemone::Internal
{
    void TaskWorker::OnRun()
    {
        GTaskSchedulerStatics->TaskWorkerEntryPoint(this->m_Index);
    }
}
