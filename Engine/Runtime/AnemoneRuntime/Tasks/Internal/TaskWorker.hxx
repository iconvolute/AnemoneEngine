#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Duration.hxx"
#include "AnemoneRuntime/Threading/Runnable.hxx"

namespace Anemone::Internal
{
    class TaskWorker final : public Runnable
    {
    private:
        uint32_t m_Index{};

    public:
        explicit TaskWorker(uint32_t index)
            : m_Index{index}
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
