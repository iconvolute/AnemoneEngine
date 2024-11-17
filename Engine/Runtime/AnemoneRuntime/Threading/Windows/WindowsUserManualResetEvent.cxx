#include "AnemoneRuntime/Threading/UserManualResetEvent.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

namespace Anemone
{
    UserManualResetEvent::UserManualResetEvent(bool signaled)
        : m_Inner{signaled ? StateSignaled : StateReset}
    {
    }

    bool UserManualResetEvent::Reset()
    {
        return !!this->m_Inner.exchange(StateReset);
    }

    void UserManualResetEvent::Set()
    {
        this->m_Inner.store(StateSignaled, std::memory_order::release);

        WakeByAddressAll(&this->m_Inner);
    }

    bool UserManualResetEvent::IsSignaled() const
    {
        return !!this->m_Inner.load(std::memory_order::acquire);
    }

    void UserManualResetEvent::Wait()
    {
        while (not this->TryAcquire())
        {
            Platform::win32_FutexWait(this->m_Inner, StateReset);
        }
    }

#if false
    bool ManualResetEventSlim::Wait(Duration const& timeout)
    {
        Private::NativeEvent& nativeThis = Platform::Get(this->_native);

        DWORD dwTimeout = Platform::Private::win32_ValidateTimeoutDuration(timeout);

        if (dwTimeout == 0)
        {
            return this->TryAcquire();
        }

        if (dwTimeout == INFINITE)
        {
            return this->Wait();
        }

        UINT64 start;
        QueryUnbiasedInterruptTime(&start);

        UINT64 elapsed = 0;

        while (not this->TryAcquire())
        {
            if (elapsed >= dwTimeout)
            {
                return false;
            }

            DWORD dwPartialTimeout = static_cast<DWORD>(dwTimeout - elapsed);

            if (not Private::WaitForSignal(nativeThis, dwPartialTimeout))
            {
                return false;
            }

            UINT64 current;
            QueryUnbiasedInterruptTime(&current);

            elapsed = (current - start) / 10'000;
        }
        return true;
    }

    bool ManualResetEvent::Wait(Duration const& timeout)
    {
        Private::NativeEvent& nativeThis = Platform::Get(this->_native);

        timespec tsTimeout = Platform::Private::posix_FromDuration(timeout);

        timespec tsStart{};
        clock_gettime(CLOCK_MONOTONIC, &tsStart);

        timespec tsElapsed{};

        while (not this->TryAcquire())
        {
            if (not Platform::Private::posix_CompareGreaterEqual(tsElapsed, tsTimeout))
            {
                return false;
            }

            timespec tsPartialTimeout = Platform::Private::posix_TimespecDifference(tsTimeout, tsElapsed);

            auto const rc = Platform::Private::linux_FutexWait(nativeThis.Inner, 1, &tsPartialTimeout);

            if (rc != 0)
            {
                int const error = errno;

                if (error == ETIMEDOUT)
                {
                    return false;
                }

                if (error != EAGAIN)
                {
                    AE_PANIC("futex wait (rc: {}, `{}`", error, strerror(error));
                }
            }

            timespec tsCurrent{};
            clock_gettime(CLOCK_MONOTONIC, &tsCurrent);

            tsElapsed = Platform::Private::posix_TimespecDifference(tsCurrent, tsStart);
        }

        return true;
    }
#endif
}
