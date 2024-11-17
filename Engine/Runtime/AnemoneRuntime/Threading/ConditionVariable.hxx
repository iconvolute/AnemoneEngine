#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Duration.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Threading/Windows/WindowsNative.hxx"
#elif ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Threading/Posix/PosixNative.hxx"
#else
#error Not implemented
#endif

namespace Anemone
{
    class CriticalSection;

    class RUNTIME_API ConditionVariable final
    {
        friend CriticalSection;

    private:
        Detail::NativeConditionVariable m_native;

    public:
        ConditionVariable();
        ConditionVariable(ConditionVariable const&) = delete;
        ConditionVariable(ConditionVariable&&) = delete;
        ConditionVariable& operator=(ConditionVariable const&) = delete;
        ConditionVariable& operator=(ConditionVariable&&) = delete;
        ~ConditionVariable();

    public:
        void Wait(CriticalSection& lock);
        bool TryWait(CriticalSection& lock, Duration const& timeout);
        void Notify();
        void NotifyAll();
    };
}
