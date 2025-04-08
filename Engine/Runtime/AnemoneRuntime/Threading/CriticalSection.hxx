#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Threading/Windows/WindowsThreading.hxx"
#elif ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Threading/Unix/UnixThreading.hxx"
#else
#error Not implemented
#endif

namespace Anemone
{
    class ConditionVariable;

    class RUNTIME_API CriticalSection final
    {
        friend class ConditionVariable;

    private:
        Internal::PlatformCriticalSection _inner;

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
            UniqueLock scope{*this};
            return std::forward<F>(f)();
        }
    };

    class RUNTIME_API RecursiveCriticalSection final
    {
        friend class ConditionVariable;

    private:
        Internal::PlatformRecursiveCriticalSection _inner;

    public:
        RecursiveCriticalSection();
        RecursiveCriticalSection(RecursiveCriticalSection const&) = delete;
        RecursiveCriticalSection(RecursiveCriticalSection&&) = delete;
        RecursiveCriticalSection& operator=(RecursiveCriticalSection const&) = delete;
        RecursiveCriticalSection& operator=(RecursiveCriticalSection&&) = delete;

        ~RecursiveCriticalSection();

    public:
        void Enter();

        void Leave();

        bool TryEnter();

        template <typename F>
        auto With(F&& f) -> std::invoke_result_t<F&&>
        {
            UniqueLock scope{*this};
            return std::forward<F>(f)();
        }
    };
}
