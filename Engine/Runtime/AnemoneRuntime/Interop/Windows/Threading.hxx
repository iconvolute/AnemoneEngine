#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"

namespace Anemone::Interop::Windows
{
    inline DWORD ValidateTimeoutDuration(Duration const& value) noexcept
    {
        int64_t const milliseconds = value.ToMilliseconds();

        if ((milliseconds < 0) || (milliseconds > std::numeric_limits<int32_t>::max()))
        {
            AE_PANIC("Duration timeout out of range");
        }

        return static_cast<DWORD>(milliseconds);
    }

    inline DWORD ValidateTimeoutDuration(int32_t milliseconds) noexcept
    {
        if (milliseconds < 0)
        {
            AE_PANIC("Duration timeout out of range");
        }

        return static_cast<DWORD>(milliseconds);
    }

    inline void SetCurrentThreadAffinity(size_t cpu)
    {
        GROUP_AFFINITY affinity{
            .Mask = uint64_t{1} << (cpu % 64),
            .Group = static_cast<WORD>(cpu / 64),
        };

        SetThreadGroupAffinity(GetCurrentThread(), &affinity, nullptr);
    }
}

namespace Anemone::Interop::Windows
{
    anemone_forceinline void FutexWait(std::atomic<int>& futex, int expected)
    {
        // Handle spurious wakes by checking the value after the wait.

        while (true)
        {
            WaitOnAddress(&futex, &expected, sizeof(expected), INFINITE);

            if (futex.load() != expected)
            {
                break;
            }
        }
    }

    template <typename PredicateT>
    anemone_forceinline void FutexWaitUntil(std::atomic<int>& futex, PredicateT&& predicate)
    {
        int value = futex.load();

        while (not std::forward<PredicateT>(predicate)(value))
        {
            FutexWait(futex, value);
            value = futex.load();
        }
    }

    anemone_forceinline bool FutexWaitTimeout(std::atomic<int>& futex, int expected, Duration const& timeout)
    {
        DWORD dwTimeout = ValidateTimeoutDuration(timeout);

        UINT64 start;
        QueryUnbiasedInterruptTime(&start);

        UINT64 elapsed = 0;

        while (true)
        {
            if (elapsed >= dwTimeout)
            {
                return false;
            }

            DWORD const dwPartialTimeout = static_cast<DWORD>(dwTimeout - elapsed);

            BOOL const rc = WaitOnAddress(&futex, &expected, sizeof(expected), dwPartialTimeout);

            if (rc == FALSE)
            {
                DWORD const dwError = GetLastError();

                if (dwError == ERROR_TIMEOUT)
                {
                    return false;
                }
                else
                {
                    AE_PANIC("Futex wait failed: {}", dwError);
                }
            }
            else
            {
                if (futex.load() != expected)
                {
                    return true;
                }
            }

            UINT64 current;
            QueryUnbiasedInterruptTime(&current);

            elapsed = (current - start) / 10'000;
        }
    }

    anemone_forceinline bool FutexWait(std::atomic<int>& futex, int expected, Duration const& timeout)
    {
        // Handle spurious wakes by checking the value after the wait.

        DWORD const dwTimeout = ValidateTimeoutDuration(timeout);

        while (true)
        {
            BOOL const rc = WaitOnAddress(&futex, &expected, sizeof(expected), dwTimeout);

            if (rc == FALSE)
            {
                DWORD const dwError = GetLastError();

                if (dwError == ERROR_TIMEOUT)
                {
                    // Wait timed out
                    return false;
                }
                else
                {
                    AE_PANIC("Futex wait failed: {}", dwError);
                }
            }
            else
            {
                if (futex.load() != expected)
                {
                    // Value changed before timeout.
                    return true;
                }
            }
        }
    }
}

namespace Anemone::Interop::Windows
{
    class CriticalSectionLock final
    {
    private:
        CRITICAL_SECTION& _lock;

    public:
        CriticalSectionLock() = delete;

        CriticalSectionLock(CRITICAL_SECTION& lock)
            : _lock{lock}
        {
            EnterCriticalSection(&this->_lock);
        }

        CriticalSectionLock(CriticalSectionLock const&) = delete;

        CriticalSectionLock(CriticalSectionLock&&) = delete;

        CriticalSectionLock& operator=(CriticalSectionLock const&) = delete;

        CriticalSectionLock& operator=(CriticalSectionLock&&) = delete;

        ~CriticalSectionLock()
        {
            LeaveCriticalSection(&this->_lock);
        }
    };

    class SrwLockSharedLock final
    {
    private:
        SRWLOCK& _lock;

    public:
        SrwLockSharedLock() = delete;

        SrwLockSharedLock(SRWLOCK& lock)
            : _lock{lock}
        {
            AcquireSRWLockShared(&this->_lock);
        }

        SrwLockSharedLock(SrwLockSharedLock const&) = delete;

        SrwLockSharedLock(SrwLockSharedLock&&) = delete;

        SrwLockSharedLock& operator=(SrwLockSharedLock const&) = delete;

        SrwLockSharedLock& operator=(SrwLockSharedLock&&) = delete;

        ~SrwLockSharedLock()
        {
            ReleaseSRWLockShared(&this->_lock);
        }
    };

    class SrwLockExclusiveLock final
    {
    private:
        SRWLOCK& _lock;

    public:
        SrwLockExclusiveLock() = delete;

        SrwLockExclusiveLock(SRWLOCK& lock)
            : _lock{lock}
        {
            AcquireSRWLockExclusive(&this->_lock);
        }

        SrwLockExclusiveLock(SrwLockExclusiveLock const&) = delete;

        SrwLockExclusiveLock(SrwLockExclusiveLock&&) = delete;

        SrwLockExclusiveLock& operator=(SrwLockExclusiveLock const&) = delete;

        SrwLockExclusiveLock& operator=(SrwLockExclusiveLock&&) = delete;

        ~SrwLockExclusiveLock()
        {
            ReleaseSRWLockExclusive(&this->_lock);
        }
    };
}
