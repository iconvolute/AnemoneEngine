#pragma once
#include "AnemoneRuntime/Threading/Runnable.hxx"
#include "AnemoneRuntime/Instant.hxx"

namespace Anemone::Tasks::Private
{
    class TaskScheduler;

    class TaskWorker final : public Threading::Runnable
    {
    public:
        TaskScheduler* m_Scheduler{};
        uint32_t m_Index{};
        size_t m_ProcessedTasks{};
        Duration m_ProcessingTime{};
        Duration m_WaitingTime{};

    public:
        TaskWorker(TaskScheduler* scheduler, uint32_t index)
            : m_Scheduler{scheduler}
            , m_Index{index}
        {
        }

        uint32_t GetIndex() const
        {
            return this->m_Index;
        }

    protected:
        void OnRun() override;
    };
}
