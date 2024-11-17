#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Threading/Windows/WindowsNative.hxx"
#elif ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Threading/Posix/PosixNative.hxx"
#else
#error Not implemented
#endif

#include <type_traits>

namespace Anemone
{
    class ConditionVariable;

    class RUNTIME_API CriticalSection
    {
        friend ConditionVariable;

    private:
        Detail::NativeCriticalSection m_native;

    public:
        CriticalSection();

        CriticalSection(CriticalSection const&) = delete;
        CriticalSection(CriticalSection&&) = delete;
        CriticalSection& operator=(CriticalSection const&) = delete;
        CriticalSection& operator=(CriticalSection&&) = delete;

        ~CriticalSection();

    public:
        void Enter();

        void Leave();

        bool TryEnter();

        template <typename F>
        auto With(F&& f) -> std::invoke_result_t<F&&>
        {
            UniqueLock _{*this};
            return std::forward<F>(f)();
        }
    };
}
