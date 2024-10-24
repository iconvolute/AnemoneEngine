#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Platform/Types.hxx"
#include "AnemoneRuntime/Duration.hxx"

namespace Anemone::Threading
{
    class CriticalSection;

    class RUNTIME_API ConditionVariable final
    {
        friend class CriticalSection;

    private:
        Platform::NativeConditionVariable m_native;

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
