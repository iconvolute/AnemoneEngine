#include "AnemoneRuntime/Threading/ConditionVariable.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

namespace Anemone::Threading
{
    ConditionVariable::ConditionVariable()
    {
        InitializeConditionVariable(&this->m_native.Inner);
    }

    ConditionVariable::~ConditionVariable() = default;

    void ConditionVariable::Wait(CriticalSection& lock)
    {
        SleepConditionVariableCS(
            &this->m_native.Inner,
            &lock.m_native.Inner,
            INFINITE);
    }

    bool ConditionVariable::TryWait(CriticalSection& lock, Duration const& timeout)
    {
        return SleepConditionVariableCS(
                   &this->m_native.Inner,
                   &lock.m_native.Inner,
                   Platform::win32_ValidateTimeoutDuration(timeout)) != FALSE;
    }

    void ConditionVariable::Notify()
    {
        WakeConditionVariable(
            &this->m_native.Inner);
    }
    void ConditionVariable::NotifyAll()
    {
        WakeAllConditionVariable(
            &this->m_native.Inner);
    }
}
