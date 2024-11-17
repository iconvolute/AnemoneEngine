#include "AnemoneRuntime/Threading/ConditionVariable.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"

namespace Anemone
{
    PosixConditionVariable::PosixConditionVariable()
    {
        if (int const rc = pthread_cond_init(&this->m_native, nullptr); rc != 0)
        {
            AE_PANIC("pthread_cond_init (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    PosixConditionVariable::~PosixConditionVariable()
    {
        if (int const rc = pthread_cond_destroy(&this->m_native); rc != 0)
        {
            AE_PANIC("pthread_cond_destroy (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    void PosixConditionVariable::Wait(PosixCriticalSection& lock)
    {
        int const rc = pthread_cond_wait(
            &this->m_native,
            &lock.m_native);

        if (rc != 0)
        {
            AE_PANIC("pthread_cond_wait (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    bool PosixConditionVariable::TryWait(PosixCriticalSection& lock, Duration const& timeout)
    {
        timespec ts{};
        clock_gettime(CLOCK_REALTIME, &ts);
        Platform::posix_ValidateTimeout(ts, timeout);

        int const rc = pthread_cond_timedwait(
            &this->m_native,
            &lock.m_native,
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

    void PosixConditionVariable::Notify()
    {
        if (int const rc = pthread_cond_signal(&this->m_native); rc != 0)
        {
            AE_PANIC("pthread_cond_signal (rc: {}, `{}`)", rc, strerror(rc));
        }
    }
    void PosixConditionVariable::NotifyAll()
    {
        if (int const rc = pthread_cond_broadcast(&this->m_native); rc != 0)
        {
            AE_PANIC("pthread_cond_signal (rc: {}, `{}`)", rc, strerror(rc));
        }
    }
}
