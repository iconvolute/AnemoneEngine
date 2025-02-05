#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Threading/Windows/WindowsCriticalSection.hxx"
#include "AnemoneRuntime/Duration.hxx"

namespace Anemone
{
    class WindowsCriticalSection;

    class WindowsConditionVariable final
    {
        friend class WindowsCriticalSection;

    private:
        CONDITION_VARIABLE _inner;

    public:
        WindowsConditionVariable()
        {
            InitializeConditionVariable(&this->_inner);
        }

        WindowsConditionVariable(WindowsConditionVariable const&) = delete;
        WindowsConditionVariable(WindowsConditionVariable&&) = delete;
        WindowsConditionVariable& operator=(WindowsConditionVariable const&) = delete;
        WindowsConditionVariable& operator=(WindowsConditionVariable&&) = delete;

        ~WindowsConditionVariable() = default;

        void Wait(WindowsCriticalSection& cs)
        {
            SleepConditionVariableCS(&this->_inner, &cs._inner, INFINITE);
        }

        bool TryWait(WindowsCriticalSection& cs, Duration const& timeout)
        {
            return SleepConditionVariableCS(&this->_inner, &cs._inner, Interop::win32_ValidateTimeoutDuration(timeout));
        }

        void Notify()
        {
            WakeConditionVariable(&this->_inner);
        }

        void NotifyAll()
        {
            WakeAllConditionVariable(&this->_inner);
        }
    };

    using ConditionVariable = WindowsConditionVariable;
}
