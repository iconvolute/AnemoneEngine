#include "AnemoneRuntime/Threading/UserAutoResetEvent.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"

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

        WakeByAddressSingle(&this->m_Inner);
    }

    bool UserAutoResetEvent::IsSignaled() const
    {
        return !!this->m_Inner.load(std::memory_order::acquire);
    }

    void UserAutoResetEvent::Wait()
    {
        while (not this->TryAcquire())
        {
            Interop::win32_FutexWait(this->m_Inner, StateReset);
        }
    }
}
/*

    bool UserAutoResetEvent::Wait(Duration const& timeout)
    {
        Private::NativeEvent& nativeThis = Interop::Get(this->_native);

        DWORD dwTimeout = Interop::Private::win32_ValidateTimeoutDuration(timeout);

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
*/
