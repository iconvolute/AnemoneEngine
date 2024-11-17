#include "AnemoneRuntime/Threading/Windows/WindowsConditionVariable.hxx"
#include "AnemoneRuntime/Threading/Windows/WindowsCriticalSection.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

namespace Anemone
{
    WindowsConditionVariable::WindowsConditionVariable()
    {
        InitializeConditionVariable(&this->m_native);
    }

    WindowsConditionVariable::~WindowsConditionVariable() = default;

    void WindowsConditionVariable::Wait(WindowsCriticalSection& lock)
    {
        SleepConditionVariableCS(&this->m_native, &lock.m_native, INFINITE);
    }

    bool WindowsConditionVariable::TryWait(WindowsCriticalSection& lock, Duration const& timeout)
    {
        return SleepConditionVariableCS(&this->m_native, &lock.m_native, Platform::win32_ValidateTimeoutDuration(timeout)) != FALSE;
    }

    void WindowsConditionVariable::Notify()
    {
        WakeConditionVariable(&this->m_native);
    }

    void WindowsConditionVariable::NotifyAll()
    {
        WakeAllConditionVariable(&this->m_native);
    }
}
