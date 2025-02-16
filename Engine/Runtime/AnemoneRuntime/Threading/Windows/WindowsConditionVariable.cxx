#include "AnemoneRuntime/Threading/ConditionVariable.hxx"

#if ANEMONE_PLATFORM_WINDOWS

#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"

namespace Anemone
{
    ConditionVariable::ConditionVariable()
    {
        InitializeConditionVariable(&this->_inner);
    }

    ConditionVariable::~ConditionVariable() = default;

    void ConditionVariable::Wait(CriticalSection& cs)
    {
        SleepConditionVariableCS(&this->_inner, &cs._inner, INFINITE);
    }

    bool ConditionVariable::TryWait(CriticalSection& cs, Duration const& timeout)
    {
        return SleepConditionVariableCS(&this->_inner, &cs._inner, Interop::win32_ValidateTimeoutDuration(timeout));
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
