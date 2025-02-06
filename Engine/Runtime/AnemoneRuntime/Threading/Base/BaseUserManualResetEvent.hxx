#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#include <atomic>

namespace Anemone
{
    template <typename FutexTraits>
    class BaseUserManualResetEvent final
    {
    private:
        std::atomic<std::int32_t> m_Inner{};

        static constexpr std::int32_t StateSignaled = 1;
        static constexpr std::int32_t StateReset = 0;

    public:
        BaseUserManualResetEvent() = default;

        explicit BaseUserManualResetEvent(bool signaled)
            : m_Inner{signaled ? StateSignaled : StateReset}
        {
        }

        BaseUserManualResetEvent(BaseUserManualResetEvent const&) = delete;

        BaseUserManualResetEvent(BaseUserManualResetEvent&&) = delete;

        BaseUserManualResetEvent& operator=(BaseUserManualResetEvent const&) = delete;

        BaseUserManualResetEvent& operator=(BaseUserManualResetEvent&&) = delete;

        ~BaseUserManualResetEvent() = default;

    private:
        bool TryAcquire() const
        {
            return this->IsSignaled();
        }

    public:
        bool Reset()
        {
            return !!this->m_Inner.exchange(StateReset);
        }

        void Set()
        {
            this->m_Inner.store(StateSignaled, std::memory_order::release);

            FutexTraits::WakeAll(this->m_Inner);
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
