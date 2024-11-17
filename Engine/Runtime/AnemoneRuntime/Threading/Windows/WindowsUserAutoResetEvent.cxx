#include "AnemoneRuntime/Threading/Windows/WindowsUserAutoResetEvent.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

namespace Anemone
{
    WindowsUserAutoResetEvent::WindowsUserAutoResetEvent(bool signaled)
        : m_Inner{signaled ? StateSignaled : StateReset}
    {
    }

    bool WindowsUserAutoResetEvent::Reset()
    {
        return !!this->m_Inner.exchange(StateReset);
    }

    void WindowsUserAutoResetEvent::Set()
    {
        this->m_Inner.store(StateSignaled, std::memory_order::release);

        WakeByAddressSingle(&this->m_Inner);
    }

    bool WindowsUserAutoResetEvent::IsSignaled() const
    {
        return !!this->m_Inner.load(std::memory_order::acquire);
    }

    void WindowsUserAutoResetEvent::Wait()
    {
        while (not this->TryAcquire())
        {
            Platform::win32_FutexWait(this->m_Inner, StateReset);
        }
    }
}
/*

    bool WindowsUserAutoResetEvent::Wait(Duration const& timeout)
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
*/
