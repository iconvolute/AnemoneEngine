#include "AnemoneRuntime/Tasks/Private/TaskWorker.hxx"
#include "AnemoneRuntime/Tasks/Private/TaskScheduler.hxx"
#include "AnemoneRuntime/Tasks/TaskScheduler.hxx"
#include "AnemoneRuntime/Tasks/Task.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"

namespace Anemone::Private
{
    void TaskWorker::OnRun()
    {
        GTaskSchedulerStatics->TaskWorkerEntryPoint(this->m_Index);
    }
}
