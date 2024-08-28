#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Platform/UninitializedStorage.hxx"
#include "AnemoneRuntime/Duration.hxx"

namespace Anemone::Platform
{
#if ANEMONE_PLATFORM_WINDOWS
    using NativeConditionVariableStorage = UninitializedStorage<struct NativeConditionVariable, 8, 8>;
#elif ANEMONE_PLATFORM_LINUX
    using NativeConditionVariableStorage = UninitializedStorage<struct NativeConditionVariable, 48, 8>;
#elif ANEMONE_PLATFORM_ANDROID
    using NativeConditionVariableStorage = UninitializedStorage<struct NativeConditionVariable, 48, 4>;
#else
#error "Not implemented"
#endif
}

namespace Anemone::Threading
{
    class CriticalSection;

    class RUNTIME_API ConditionVariable final
    {
        friend class CriticalSection;

    private:
        Platform::NativeConditionVariableStorage _native;

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
