#include "AnemoneRuntime/Threading/UserAutoResetEvent.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"

namespace Anemone
{
    UserAutoResetEvent::UserAutoResetEvent(bool signaled)
        : m_Inner{signaled ? StateSignaled : StateReset}
    {
    }

    bool UserAutoResetEvent::Reset()
    {
        return !!this->m_Inner.exchange(StateReset);
    }

    void UserAutoResetEvent::Set()
    {
        this->m_Inner.store(StateSignaled, std::memory_order::release);

        Interop::posix_FutexWakeOne(this->m_Inner);
    }

    bool UserAutoResetEvent::IsSignaled() const
    {
        return !!this->m_Inner.load(std::memory_order::acquire);
    }

    void UserAutoResetEvent::Wait()
    {
        while (not this->TryAcquire())
        {
            Interop::posix_FutexWait(this->m_Inner, StateReset);
        }
    }

#if false
    bool UserAutoResetEvent::Wait(Duration const& timeout)
    {
        Private::NativeEvent& nativeThis = Interop::Get(this->_native);

        timespec tsTimeout = Interop::Private::posix_FromDuration(timeout);

        timespec tsStart{};
        clock_gettime(CLOCK_MONOTONIC, &tsStart);

        timespec tsElapsed{};

        while (not this->TryAcquire())
        {
            if (not Interop::Private::posix_CompareGreaterEqual(tsElapsed, tsTimeout))
            {
                return false;
            }

            timespec tsPartialTimeout = Interop::Private::posix_TimespecDifference(tsTimeout, tsElapsed);

            auto const rc = Interop::Private::linux_FutexWait(nativeThis.Inner, 1, &tsPartialTimeout);

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

            tsElapsed = Interop::Private::posix_TimespecDifference(tsCurrent, tsStart);
        }

        return true;
    }
#endif
}
