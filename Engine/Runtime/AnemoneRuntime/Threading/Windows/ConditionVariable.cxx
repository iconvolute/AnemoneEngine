#include "AnemoneRuntime/Threading/ConditionVariable.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

namespace Anemone::Threading
{
    ConditionVariable::ConditionVariable()
    {
        Platform::NativeConditionVariable& native = Platform::Create(this->_native);

        InitializeConditionVariable(&native.Inner);
    }

    ConditionVariable::~ConditionVariable()
    {
        Platform::Destroy(this->_native);
    }

    void ConditionVariable::Wait(CriticalSection& lock)
    {
        Platform::NativeConditionVariable& nativeThis = Platform::Get(this->_native);
        Platform::NativeCriticalSection& nativeLock = Platform::Get(lock._native);

        SleepConditionVariableCS(
            &nativeThis.Inner,
            &nativeLock.Inner,
            INFINITE);
    }

    bool ConditionVariable::TryWait(CriticalSection& lock, Duration const& timeout)
    {
        Platform::NativeConditionVariable& nativeThis = Platform::Get(this->_native);
        Platform::NativeCriticalSection& nativeLock = Platform::Get(lock._native);

        return SleepConditionVariableCS(
                   &nativeThis.Inner,
                   &nativeLock.Inner,
                   Platform::win32_ValidateTimeoutDuration(timeout)) != FALSE;
    }

    void ConditionVariable::Notify()
    {
        Platform::NativeConditionVariable& nativeThis = Platform::Get(this->_native);

        WakeConditionVariable(
            &nativeThis.Inner);
    }
    void ConditionVariable::NotifyAll()
    {
        Platform::NativeConditionVariable& nativeThis = Platform::Get(this->_native);

        WakeAllConditionVariable(
            &nativeThis.Inner);
    }
}
