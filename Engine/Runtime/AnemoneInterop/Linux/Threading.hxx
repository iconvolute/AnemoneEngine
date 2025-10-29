#pragma once
#include "AnemoneInterop/Linux/Headers.hxx"
#include "AnemoneInterop/Linux/DateTime.hxx"
#include "AnemoneDiagnostics/Debug.hxx"
#include "AnemoneBase/Duration.hxx"

#include <atomic>

namespace Anemone::Interop::Linux
{
    inline void ValidateTimeout(timespec& self, Duration const& value)
    {
        AddDuration(self, value);
    }

    inline void ValidateTimeout(timespec& self, int32_t milliseconds)
    {
        AddDuration(self, Duration::FromMilliseconds(milliseconds));
    }

    inline int32_t ValidateTimeoutDuration(Duration const& value)
    {
        int64_t const milliseconds = value.ToMilliseconds();
        if ((milliseconds < 0) || (milliseconds > std::numeric_limits<int32_t>::max()))
        {
            AE_PANIC("Duration timeout out of range");
        }
        return static_cast<int32_t>(milliseconds);
    }

    inline void SetCurrentThreadAffinity(size_t cpu)
    {
        cpu_set_t set;
        CPU_ZERO(&set);
        CPU_SET(cpu, &set);

        sched_setaffinity(0, sizeof(cpu_set_t), &set);
    }
}

// https://www.remlab.net/op/futex-condvar.shtml
// https://www.remlab.net/op/futex-misc.shtml

namespace Anemone::Interop::Linux
{
    anemone_forceinline void FutexWait(std::atomic<int>& futex, int expected, timespec const* timeout) noexcept
    {
        int const rc = syscall(SYS_futex, &futex, FUTEX_WAIT_PRIVATE, expected, timeout, nullptr, 0);

        if (rc == -1)
        {
            AE_PANIC("FutexWait (rc: {}, '{}')", errno, strerror(errno));
        }
    }

    anemone_forceinline void FutexWakeOne(std::atomic<int>& futex)
    {
        int const rc = syscall(SYS_futex, &futex, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, 1, nullptr, nullptr, 0);

        if (rc == -1)
        {
            AE_PANIC("FutexWakeOne (rc: {}, '{}')", errno, strerror(errno));
        }
    }

    anemone_forceinline void FutexWakeAll(std::atomic<int>& futex)
    {
        int const rc = syscall(SYS_futex, &futex, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, INT32_MAX, nullptr, nullptr, 0);

        if (rc == -1)
        {
            AE_PANIC("FutexWakeAll (rc: {}, '{}')", errno, strerror(errno));
        }
    }

    anemone_forceinline void FutexWait(std::atomic<int>& futex, int expected)
    {
        while (true)
        {
            int const rc = syscall(SYS_futex, &futex, FUTEX_WAIT | FUTEX_PRIVATE_FLAG, expected, nullptr, nullptr, 0);

            if (rc == -1)
            {
                if (errno == EAGAIN)
                {
                    return;
                }
                else
                {
                    AE_PANIC("FutexWait (rc: {}, '{}')", errno, strerror(errno));
                }
            }
            else if (rc == 0)
            {
                if (futex.load() != expected)
                {
                    return;
                }
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
        timespec tsTimeout = ToTimespec(timeout);
        timespec tsStart{};
        clock_gettime(CLOCK_MONOTONIC, &tsStart);

        timespec tsElapsed{};

        while (true)
        {
            if (not CompareGreaterEqual(tsElapsed, tsTimeout))
            {
                return false;
            }

            timespec tsPartialTimeout = TimespecDifference(tsTimeout, tsElapsed);

            int const rc = syscall(SYS_futex, &futex, FUTEX_WAIT | FUTEX_PRIVATE_FLAG, expected, &tsPartialTimeout, nullptr, 0);

            if (rc != 0)
            {
                int const error = errno;

                if (error == ETIMEDOUT)
                {
                    return false;
                }

                if (error != EAGAIN)
                {
                    AE_PANIC("FutexWaitTimeout (rc: {}, '{}')", error, strerror(error));
                }
            }

            timespec tsCurrent{};
            clock_gettime(CLOCK_MONOTONIC, &tsCurrent);

            tsElapsed = TimespecDifference(tsCurrent, tsStart);

            if (futex.load() != expected)
            {
                return true;
            }
        }
    }
}

namespace Anemone::Interop::Linux
{
    class PthreadMutexLock final
    {
    private:
        pthread_mutex_t& _lock;

    public:
        PthreadMutexLock() = delete;

        PthreadMutexLock(
            pthread_mutex_t& lock)
            : _lock{lock}
        {
            pthread_mutex_lock(&this->_lock);
        }

        ~PthreadMutexLock()
        {
            pthread_mutex_unlock(&this->_lock);
        }

        auto operator=(
            PthreadMutexLock const&) = delete;

        auto operator=(
            PthreadMutexLock&&) = delete;
    };

    class PthreadRwlockReadLock final
    {
    private:
        pthread_rwlock_t& _lock;

    public:
        PthreadRwlockReadLock() = delete;

        PthreadRwlockReadLock(
            pthread_rwlock_t& lock)
            : _lock{lock}
        {
            pthread_rwlock_rdlock(&this->_lock);
        }

        PthreadRwlockReadLock(
            PthreadRwlockReadLock const&) = delete;

        PthreadRwlockReadLock(
            PthreadRwlockReadLock&&) = delete;

        ~PthreadRwlockReadLock()
        {
            pthread_rwlock_unlock(&this->_lock);
        }

        auto operator=(
            PthreadRwlockReadLock const&) -> PthreadRwlockReadLock& = delete;

        auto operator=(
            PthreadRwlockReadLock&&) -> PthreadRwlockReadLock& = delete;
    };

    class PthreadRwlockWriteLock final
    {
    private:
        pthread_rwlock_t& _lock;

    public:
        PthreadRwlockWriteLock() = delete;

        PthreadRwlockWriteLock(
            pthread_rwlock_t& lock)
            : _lock{lock}
        {
            pthread_rwlock_wrlock(&this->_lock);
        }

        PthreadRwlockWriteLock(
            PthreadRwlockWriteLock const&) = delete;

        PthreadRwlockWriteLock(
            PthreadRwlockWriteLock&&) = delete;

        ~PthreadRwlockWriteLock()
        {
            pthread_rwlock_unlock(&this->_lock);
        }

        auto operator=(
            PthreadRwlockWriteLock const&) -> PthreadRwlockWriteLock& = delete;

        auto operator=(
            PthreadRwlockWriteLock&&) -> PthreadRwlockWriteLock& = delete;
    };
}
