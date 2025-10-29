#include "AnemoneThreading/Monitor.hxx"

#if ANEMONE_PLATFORM_WINDOWS

#include "AnemoneInterop/Windows/Threading.hxx"

namespace Anemone
{
    Monitor::Monitor()
    {
        InitializeSRWLock(&this->_inner._lock);
        InitializeConditionVariable(&this->_inner._condition);
    }

    Monitor::~Monitor() = default;

    void Monitor::Enter()
    {
        AcquireSRWLockExclusive(&this->_inner._lock);
    }

    void Monitor::Leave()
    {
        ReleaseSRWLockExclusive(&this->_inner._lock);
    }

    bool Monitor::TryEnter()
    {
        return TryAcquireSRWLockExclusive(&this->_inner._lock);
    }

    void Monitor::Wait()
    {
        SleepConditionVariableSRW(&this->_inner._condition, &this->_inner._lock, INFINITE, 0);
    }

    bool Monitor::TryWait(Duration const& timeout)
    {
        return SleepConditionVariableSRW(&this->_inner._condition, &this->_inner._lock, Interop::Windows::ValidateTimeoutDuration(timeout), 0);
    }

    void Monitor::NotifyOne()
    {
        WakeConditionVariable(&this->_inner._condition);
    }

    void Monitor::NotifyAll()
    {
        WakeAllConditionVariable(&this->_inner._condition);
    }
}

#endif
