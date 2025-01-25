#include "AnemoneRuntime/Threading/ConditionVariable.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"

namespace Anemone
{
    ConditionVariable::ConditionVariable()
    {
        if (int const rc = pthread_cond_init(&this->m_native.Inner, nullptr); rc != 0)
        {
            AE_PANIC("pthread_cond_init (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    ConditionVariable::~ConditionVariable()
    {
        if (int const rc = pthread_cond_destroy(&this->m_native.Inner); rc != 0)
        {
            AE_PANIC("pthread_cond_destroy (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    void ConditionVariable::Wait(CriticalSection& lock)
    {
        int const rc = pthread_cond_wait(
            &this->m_native.Inner,
            &lock.m_native.Inner);

        if (rc != 0)
        {
            AE_PANIC("pthread_cond_wait (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    bool ConditionVariable::TryWait(CriticalSection& lock, Duration const& timeout)
    {
        timespec ts{};
        clock_gettime(CLOCK_REALTIME, &ts);
        Interop::posix_ValidateTimeout(ts, timeout);

        int const rc = pthread_cond_timedwait(
            &this->m_native.Inner,
            &lock.m_native.Inner,
            &ts);

        if (rc == ETIMEDOUT)
        {
            return false;
        }
        else if (rc != 0)
        {
            AE_PANIC("pthread_cond_timedwait (rc: {}, `{}`)", rc, strerror(rc));
        }

        return true;
    }

    void ConditionVariable::Notify()
    {
        if (int const rc = pthread_cond_signal(&this->m_native.Inner); rc != 0)
        {
            AE_PANIC("pthread_cond_signal (rc: {}, `{}`)", rc, strerror(rc));
        }
    }
    void ConditionVariable::NotifyAll()
    {
        if (int const rc = pthread_cond_broadcast(&this->m_native.Inner); rc != 0)
        {
            AE_PANIC("pthread_cond_signal (rc: {}, `{}`)", rc, strerror(rc));
        }
    }
}
