#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

#include <atomic>

namespace Anemone
{
    template <typename FutexTraits>
    class BaseUserCriticalSection final
    {
    private:
        std::atomic<int32_t> m_Flag{};

        static constexpr int32_t StateLocked = 1;
        static constexpr int32_t StateUnlocked = 0;

    public:
        BaseUserCriticalSection() = default;

        BaseUserCriticalSection(BaseUserCriticalSection const&) = delete;

        BaseUserCriticalSection(BaseUserCriticalSection&&) = delete;

        BaseUserCriticalSection& operator=(BaseUserCriticalSection const&) = delete;

        BaseUserCriticalSection& operator=(BaseUserCriticalSection&&) = delete;

        ~BaseUserCriticalSection() = default;

        void Enter()
        {
            while (true)
            {
                if (this->m_Flag.exchange(StateLocked, std::memory_order::acquire) == StateUnlocked)
                {
                    return;
                }

                FutexTraits::Wait(this->m_Flag, StateLocked);
            }
        }

        bool TryEnter()
        {
            return this->m_Flag.exchange(StateLocked, std::memory_order::acquire) == StateUnlocked;
        }

        void Leave()
        {
            AE_ASSERT(this->m_Flag.load(std::memory_order::relaxed) != StateUnlocked);

            this->m_Flag.store(StateUnlocked, std::memory_order::release);

            FutexTraits::WakeOne(&this->m_Flag);
        }
    };
}
