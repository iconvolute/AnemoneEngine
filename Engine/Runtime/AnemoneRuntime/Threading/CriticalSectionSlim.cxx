#include "AnemoneRuntime/Threading/CriticalSectionSlim.hxx"
#include "AnemoneRuntime/Diagnostic/Assert.hxx"
#include "AnemoneRuntime/Threading/Futex.hxx"

namespace Anemone::Threading
{
    anemone_noinline void CriticalSectionSlim::Enter()
    {
        while (true)
        {
            if (this->m_Flag.exchange(StateLocked, std::memory_order::acquire) == StateUnlocked)
            {
                return;
            }

            FutexWait(this->m_Flag, StateLocked);
        }
    }

    anemone_noinline bool CriticalSectionSlim::TryEnter()
    {
        return this->m_Flag.exchange(StateLocked, std::memory_order::acquire) == StateUnlocked;
    }

    anemone_noinline void CriticalSectionSlim::Leave()
    {
        AE_ASSERT(this->m_Flag.load(std::memory_order::relaxed) != StateUnlocked);

        this->m_Flag.store(StateUnlocked, std::memory_order::release);

        FutexWakeOne(this->m_Flag);
    }
}
