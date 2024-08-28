#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Platform/UninitializedStorage.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"

#include <type_traits>
#include <atomic>

namespace Anemone::Platform
{
#if ANEMONE_PLATFORM_WINDOWS
    using NativeCriticalSectionStorage = UninitializedStorage<struct NativeCriticalSection, 40, 8>;
#elif ANEMONE_PLATFORM_LINUX
    using NativeCriticalSectionStorage = UninitializedStorage<struct NativeCriticalSection, 48, 8>;
#elif ANEMONE_PLATFORM_ANDROID
    using NativeCriticalSectionStorage = UninitializedStorage<struct NativeCriticalSection, 40, 4>;
#else
#error "Not implemented"
#endif
}

namespace Anemone::Threading
{
    class ConditionVariable;

    class RUNTIME_API CriticalSection final
    {
        friend class ConditionVariable;

    private:
        Platform::NativeCriticalSectionStorage _native;

    public:
        CriticalSection();
        CriticalSection(CriticalSection const&) = delete;
        CriticalSection(CriticalSection&&) = delete;
        CriticalSection& operator=(CriticalSection const&) = delete;
        CriticalSection& operator=(CriticalSection&&) = delete;
        ~CriticalSection();

    public:
        void Enter();
        bool TryEnter();
        void Leave();

        template <typename F>
        auto With(F&& f) -> std::invoke_result_t<F&&>
        {
            UniqueLock lock{*this};
            return std::forward<F>(f)();
        }
    };
}
