#pragma once
#include "AnemoneRuntime/Duration.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Threading/Windows/WindowsThreading.hxx"
#elif ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Threading/Unix/UnixThreading.hxx"
#else
#error Not implemented
#endif

namespace Anemone
{
    class CriticalSection;
    class RecursiveCriticalSection;

    class RUNTIME_API ConditionVariable final
    {
        friend class CriticalSection;

    private:
        Internal::PlatformConditionVariable _inner;

    public:
        ConditionVariable();
        ConditionVariable(ConditionVariable const&) = delete;
        ConditionVariable(ConditionVariable&&) = delete;
        ConditionVariable& operator=(ConditionVariable const&) = delete;
        ConditionVariable& operator=(ConditionVariable&&) = delete;
        ~ConditionVariable();

    public:
        void Wait(CriticalSection& cs);

        void Wait(RecursiveCriticalSection& cs);

        template <typename Predicate>
        void Wait(CriticalSection& cs, Predicate&& predicate)
        {
            while (!std::forward<Predicate>(predicate)())
            {
                this->Wait(cs);
            }
        }

        template <typename Predicate>
        void Wait(RecursiveCriticalSection& cs, Predicate&& predicate)
        {
            while (!std::forward<Predicate>(predicate)())
            {
                this->Wait(cs);
            }
        }

        bool TryWait(CriticalSection& cs, Duration const& timeout);

        bool TryWait(RecursiveCriticalSection& cs, Duration const& timeout);

        void NotifyOne();

        void NotifyAll();
    };
}
