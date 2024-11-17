#include "AnemoneRuntime/Threading/Posix/PosixUserCriticalSection.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"

namespace Anemone
{
    void PosixUserCriticalSection::Enter()
    {
        while (true)
        {
            if (this->m_Flag.exchange(StateLocked, std::memory_order::acquire) == StateUnlocked)
            {
                return;
            }

            Platform::posix_FutexWait(this->m_Flag, StateLocked);
        }
    }

    bool PosixUserCriticalSection::TryEnter()
    {
        return this->m_Flag.exchange(StateLocked, std::memory_order::acquire) == StateUnlocked;
    }

    void PosixUserCriticalSection::Leave()
    {
        AE_ASSERT(this->m_Flag.load(std::memory_order::relaxed) != StateUnlocked);

        this->m_Flag.store(StateUnlocked, std::memory_order::release);

        Platform::posix_FutexWakeOne(this->m_Flag);
    }
}
