#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Duration.hxx"
#include "AnemoneRuntime/Platform/Windows/Headers.hxx"

namespace Anemone
{
    class WindowsCriticalSection;

    class RUNTIME_API WindowsConditionVariable final
    {
        friend WindowsCriticalSection;

    private:
        CONDITION_VARIABLE m_native;

    public:
        WindowsConditionVariable();
        WindowsConditionVariable(WindowsConditionVariable const&) = delete;
        WindowsConditionVariable(WindowsConditionVariable&&) = delete;
        WindowsConditionVariable& operator=(WindowsConditionVariable const&) = delete;
        WindowsConditionVariable& operator=(WindowsConditionVariable&&) = delete;
        ~WindowsConditionVariable();

    public:
        void Wait(WindowsCriticalSection& lock);
        bool TryWait(WindowsCriticalSection& lock, Duration const& timeout);
        void Notify();
        void NotifyAll();
    };

    using ConditionVariable = WindowsConditionVariable;
}
