#include "AnemoneRuntime/Tasks/TaskWorker.hxx"
#include "AnemoneRuntime/Tasks/TaskScheduler.hxx"
#include "AnemoneRuntime/Threading/CurrentThread.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Profiler/Profiler.hxx"

namespace Anemone::Tasks
{
    AE_DECLARE_PROFILE(TaskWorkerWait);
    AE_DECLARE_PROFILE(TaskWorkerExecute);

    void TaskWorker::OnRun()
    {
        while (true)
        {
            this->m_WaitingTime += Instant::Now().Measure([&]
            {
                AE_PROFILE_SCOPE(TaskWorkerWait);

                this->m_Scheduler->m_Semaphore.Acquire();
            });

            if (this->m_Scheduler->m_CancellationToken.IsCancelled())
            {
                break;
            }

            this->m_ProcessingTime += Instant::Now().Measure([&]
            {
                AE_PROFILE_SCOPE(TaskWorkerExecute);

                if (Task* task = this->m_Scheduler->m_Queue.Pop())
                {
                    this->m_Scheduler->Execute(*task);
                    ++this->m_ProcessedTasks;
                }
            });
        }

        AE_TRACE(Verbose, "Worker completed");
    }
}
