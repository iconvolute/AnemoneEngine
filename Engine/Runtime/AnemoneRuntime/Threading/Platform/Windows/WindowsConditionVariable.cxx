#include "AnemoneRuntime/Threading/ConditionVariable.hxx"

#if ANEMONE_PLATFORM_WINDOWS

#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#include "AnemoneRuntime/Interop/Windows/Threading.hxx"

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
