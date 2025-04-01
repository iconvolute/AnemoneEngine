#pragma once
#include "AnemoneRuntime/Duration.hxx"
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

    private:
        void WaitImpl(CriticalSection& lock);
        void WaitImpl(RecursiveCriticalSection& lock);
        bool TryWaitImpl(CriticalSection& lock, Duration const& timeout);
        bool TryWaitImpl(RecursiveCriticalSection& lock, Duration const& timeout); 

    public:
        template <UniqueLockable Lock>
        void Wait(UniqueLock<Lock>& lock)
        {
            this->WaitImpl(lock.GetLock());
        }

        template <UniqueLockable Lock, typename Predicate>
        void Wait(UniqueLock<Lock>& lock, Predicate&& predicate)
        {
            while (!std::forward<Predicate>(predicate)())
            {
                this->WaitImpl(lock.GetLock());
            }
        }

        template <UniqueLockable Lock>
        bool TryWait(UniqueLock<Lock>& lock, Duration const& timeout)
        {
            return this->TryWaitImpl(lock.GetLock(), timeout);
        }

        void NotifyOne();

        void NotifyAll();
    };
}
