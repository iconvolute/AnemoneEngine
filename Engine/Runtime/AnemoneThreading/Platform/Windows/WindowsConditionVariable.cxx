#include "AnemoneThreading/ConditionVariable.hxx"

#if ANEMONE_PLATFORM_WINDOWS

#include "AnemoneThreading/CriticalSection.hxx"
#include "AnemoneInterop/Windows/Headers.hxx"
#include "AnemoneInterop/Windows/Threading.hxx"

namespace Anemone
{
    ConditionVariable::ConditionVariable()
    {
        InitializeConditionVariable(&this->_inner);
    }

    ConditionVariable::~ConditionVariable() = default;

    void ConditionVariable::WaitImpl(CriticalSection& cs)
    {
        SleepConditionVariableSRW(&this->_inner, &cs._inner, INFINITE, 0);
    }

    void ConditionVariable::WaitImpl(RecursiveCriticalSection& cs)
    {
        SleepConditionVariableCS(&this->_inner, &cs._inner, INFINITE);
    }

    bool ConditionVariable::TryWaitImpl(CriticalSection& cs, Duration const& timeout)
    {
        return SleepConditionVariableSRW(&this->_inner, &cs._inner, Interop::Windows::ValidateTimeoutDuration(timeout), 0);
    }

    bool ConditionVariable::TryWaitImpl(RecursiveCriticalSection& cs, Duration const& timeout)
    {
        return SleepConditionVariableCS(&this->_inner, &cs._inner, Interop::Windows::ValidateTimeoutDuration(timeout));
    }

    void ConditionVariable::NotifyOne()
    {
        WakeConditionVariable(&this->_inner);
    }

    void ConditionVariable::NotifyAll()
    {
        WakeAllConditionVariable(&this->_inner);
    }
}

#endif
