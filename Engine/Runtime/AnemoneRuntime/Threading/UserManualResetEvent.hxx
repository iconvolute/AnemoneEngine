#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#if ANEMONE_FEATURE_FUTEX

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Threading/Platform/Windows/WindowsThreading.hxx"
#elif ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Threading/Platform/Unix/UnixThreading.hxx"
#else
#error Not implemented
#endif

#include <atomic>

namespace Anemone
{
    class UserManualResetEvent final
    {
    private:
        std::atomic<std::int32_t> m_Inner{};

        static constexpr std::int32_t StateSignaled = 1;
        static constexpr std::int32_t StateReset = 0;

    public:
        UserManualResetEvent() = default;

        explicit UserManualResetEvent(bool signaled)
            : m_Inner{signaled ? StateSignaled : StateReset}
        {
        }

        UserManualResetEvent(UserManualResetEvent const&) = delete;

        UserManualResetEvent(UserManualResetEvent&&) = delete;

        UserManualResetEvent& operator=(UserManualResetEvent const&) = delete;

        UserManualResetEvent& operator=(UserManualResetEvent&&) = delete;

        ~UserManualResetEvent() = default;

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

            Internal::Futex::WakeAll(this->m_Inner);
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

#include "AnemoneRuntime/Threading/ManualResetEvent.hxx"

namespace Anemone
{
    using UserManualResetEvent = ManualResetEvent;
}

#endif
