#include "AnemoneRuntime/Threading/CriticalSection.hxx"

#if ANEMONE_PLATFORM_LINUX || ANEMONE_PLATFORM_ANDROID

#include "AnemoneRuntime/Threading/ConditionVariable.hxx"
#include "AnemoneRuntime/Interop/Linux/Threading.hxx"

namespace Anemone
{
    ConditionVariable::ConditionVariable()
    {
        pthread_cond_init(&this->_inner, nullptr);
    }

    ConditionVariable::~ConditionVariable()
    {
        pthread_cond_destroy(&this->_inner);
    }

    void ConditionVariable::WaitImpl(CriticalSection& cs)
    {
        pthread_cond_wait(&this->_inner, &cs._inner);
    }

    void ConditionVariable::WaitImpl(RecursiveCriticalSection& cs)
    {
        pthread_cond_wait(&this->_inner, &cs._inner);
    }

    bool ConditionVariable::TryWaitImpl(CriticalSection& cs, Duration const& timeout)
    {
        timespec ts{};
        clock_gettime(CLOCK_REALTIME, &ts);
        Interop::Linux::ValidateTimeout(ts, timeout);

        // Should fail on ETIMEDOUT only.
        return pthread_cond_timedwait(&this->_inner, &cs._inner, &ts) == 0;
    }

    bool ConditionVariable::TryWaitImpl(RecursiveCriticalSection& cs, Duration const& timeout)
    {
        timespec ts{};
        clock_gettime(CLOCK_REALTIME, &ts);
        Interop::Linux::ValidateTimeout(ts, timeout);

        // Should fail on ETIMEDOUT only.
        return pthread_cond_timedwait(&this->_inner, &cs._inner, &ts) == 0;
    }

    void ConditionVariable::NotifyOne()
    {
        pthread_cond_signal(&this->_inner);
    }

    void ConditionVariable::NotifyAll()
    {
        pthread_cond_broadcast(&this->_inner);
    }
}

#endif
