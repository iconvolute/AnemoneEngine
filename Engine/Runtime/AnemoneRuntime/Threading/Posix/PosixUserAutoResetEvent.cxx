#include "AnemoneRuntime/Threading/Posix/PosixUserAutoResetEvent.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"

namespace Anemone
{
    PosixUserAutoResetEvent::PosixUserAutoResetEvent(bool signaled)
        : m_Inner{signaled ? StateSignaled : StateReset}
    {
    }

    bool PosixUserAutoResetEvent::Reset()
    {
        return !!this->m_Inner.exchange(StateReset);
    }

    void PosixUserAutoResetEvent::Set()
    {
        this->m_Inner.store(StateSignaled, std::memory_order::release);

        Platform::posix_FutexWakeOne(this->m_Inner);
    }

    bool PosixUserAutoResetEvent::IsSignaled() const
    {
        return !!this->m_Inner.load(std::memory_order::acquire);
    }

    void PosixUserAutoResetEvent::Wait()
    {
        while (not this->TryAcquire())
        {
            Platform::posix_FutexWait(this->m_Inner, StateReset);
        }
    }

#if false
    bool PosixUserAutoResetEvent::Wait(Duration const& timeout)
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
