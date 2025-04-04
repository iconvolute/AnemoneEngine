#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#if ANEMONE_FEATURE_FUTEX

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Threading/Windows/WindowsThreading.hxx"
#elif ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Threading/Unix/UnixThreading.hxx"
#else
#error Not implemented
#endif

#include <atomic>

namespace Anemone
{
    class UserAutoResetEvent final
    {
    private:
        std::atomic<std::int32_t> m_Inner{};

        static constexpr std::int32_t StateSignaled = 1;
        static constexpr std::int32_t StateReset = 0;

    public:
        UserAutoResetEvent() = default;

        explicit UserAutoResetEvent(bool signaled)
            : m_Inner{signaled ? StateSignaled : StateReset}
        {
        }

        UserAutoResetEvent(UserAutoResetEvent const&) = delete;

        UserAutoResetEvent(UserAutoResetEvent&&) = delete;

        UserAutoResetEvent& operator=(UserAutoResetEvent const&) = delete;

        UserAutoResetEvent& operator=(UserAutoResetEvent&&) = delete;

        ~UserAutoResetEvent() = default;

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

            Internal::Futex::WakeOne(this->m_Inner);
        }

        bool IsSignaled() const
        {
            return !!this->m_Inner.load(std::memory_order::acquire);
        }

        void Wait()
        {
            while (not this->TryAcquire())
            {
                Internal::Futex::Wait(this->m_Inner, StateReset);
            }
        }
    };
}

#else

#include "AnemoneRuntime/Threading/AutoResetEvent.hxx"

namespace Anemone
{
    using UserAutoResetEvent = AutoResetEvent;
}

#endif
