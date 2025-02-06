#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#include <atomic>

namespace Anemone
{
    template <typename FutexTraits>
    class BaseUserAutoResetEvent final
    {
    private:
        std::atomic<std::int32_t> m_Inner{};

        static constexpr std::int32_t StateSignaled = 1;
        static constexpr std::int32_t StateReset = 0;

    public:
        BaseUserAutoResetEvent() = default;

        explicit BaseUserAutoResetEvent(bool signaled)
            : m_Inner{signaled ? StateSignaled : StateReset}
        {
        }

        BaseUserAutoResetEvent(BaseUserAutoResetEvent const&) = delete;

        BaseUserAutoResetEvent(BaseUserAutoResetEvent&&) = delete;

        BaseUserAutoResetEvent& operator=(BaseUserAutoResetEvent const&) = delete;

        BaseUserAutoResetEvent& operator=(BaseUserAutoResetEvent&&) = delete;

        ~BaseUserAutoResetEvent() = default;

    private:
        bool TryAcquire()
        {
            return this->Reset();
        }

    public:
        bool Reset()
        {
            return !!this->m_Inner.exchange(StateReset);
        }

        void Set()
        {
            this->m_Inner.store(StateSignaled, std::memory_order::release);

            FutexTraits::WakeOne(this->m_Inner);
        }

        bool IsSignaled() const
        {
            return !!this->m_Inner.load(std::memory_order::acquire);
        }

        void Wait()
        {
            while (not this->TryAcquire())
            {
                FutexTraits::Wait(this->m_Inner, StateReset);
            }
        }
    };
}
