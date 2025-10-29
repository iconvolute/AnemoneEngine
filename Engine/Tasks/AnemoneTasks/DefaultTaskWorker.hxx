#pragma once
#include "AnemoneInterop/Headers.hxx"
#include "AnemoneThreading/Runnable.hxx"

namespace Anemone
{
    class DefaultTaskScheduler;

    class DefaultTaskWorker final : public Runnable
    {
    private:
        uint32_t m_Index{};

        DefaultTaskScheduler* m_Scheduler{};

    public:
        explicit DefaultTaskWorker(uint32_t index, DefaultTaskScheduler* scheduler)
            : m_Index{index}
            , m_Scheduler{scheduler}
        {
            AE_ASSERT(scheduler != nullptr);
        }

        uint32_t GetIndex() const
        {
            return this->m_Index;
        }

    protected:
        void OnRun() override;
    };
}
