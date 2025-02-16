#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include "AnemoneRuntime/Duration.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Threading/Windows/WindowsConditionVariable.hxx"
#elif ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Threading/Unix/UnixConditionVariable.hxx"
#else
#error Not implemented
#endif

namespace Anemone
{
    class CriticalSection;

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

        template <typename Predicate>
        void Wait(CriticalSection& cs, Predicate&& predicate)
        {
            while (!std::forward<Predicate>(predicate)())
            {
                this->Wait(cs);
            }
        }

        bool TryWait(CriticalSection& cs, Duration const& timeout);

        void NotifyOne();

        void NotifyAll();
    };
}
