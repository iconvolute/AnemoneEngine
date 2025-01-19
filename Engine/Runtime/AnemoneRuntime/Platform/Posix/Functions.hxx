#pragma once
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Duration.hxx"
#include "AnemoneRuntime/DateTime.hxx"
#include "AnemoneRuntime/Platform/Posix/Headers.hxx"

#include <atomic>

namespace Anemone::Interop
{
    inline constexpr int64_t posix_DateAdjustOffset = 62135596800;
    inline constexpr int64_t posix_NanosecondsInSecond = 1'000'000'000;

    constexpr Duration posix_into_Duration(struct timeval const& tv)
    {
        return Duration{
            .Seconds = tv.tv_sec,
            .Nanoseconds = tv.tv_usec * 1'000,
        };
    }

    constexpr Duration posix_into_Duration(struct timespec const& ts)
    {
        return Duration{
            .Seconds = ts.tv_sec,
            .Nanoseconds = ts.tv_nsec,
        };
    }

    constexpr DateTime posix_into_DateTime(struct timespec const& ts, int64_t bias)
    {
        return DateTime{
            .Inner = {
                .Seconds = ts.tv_sec - bias + posix_DateAdjustOffset,
                .Nanoseconds = ts.tv_nsec,
            },
        };
    }

    constexpr DateTime posix_into_DateTime(struct timespec const& ts)
    {
        return DateTime{
            .Inner = {
                .Seconds = ts.tv_sec + posix_DateAdjustOffset,
                .Nanoseconds = ts.tv_nsec,
            },
        };
    }

    constexpr void posix_Normalize(timespec& self)
    {
        self.tv_sec += (self.tv_nsec / posix_NanosecondsInSecond);

        if ((self.tv_nsec %= posix_NanosecondsInSecond) < 0)
        {
            self.tv_nsec += posix_NanosecondsInSecond;
            --self.tv_sec;
        }
    }

    constexpr void posix_AddDuration(timespec& self, Duration const& value)
    {
        self.tv_sec += value.Seconds;
        self.tv_nsec += value.Nanoseconds;
        posix_Normalize(self);
    }

    constexpr void posix_SubtractDuration(timespec& self, Duration const& value)
    {
        self.tv_sec -= value.Seconds;
        self.tv_nsec -= value.Nanoseconds;
        posix_Normalize(self);
    }

    constexpr void posix_SubtractTimespec(timespec& self, timespec const& value)
    {
        self.tv_sec -= value.tv_sec;
        self.tv_nsec -= value.tv_nsec;
        posix_Normalize(self);
    }

    constexpr timespec posix_TimespecDifference(timespec const& self, timespec const& other)
    {
        timespec result{
            .tv_sec = self.tv_sec - other.tv_sec,
            .tv_nsec = self.tv_nsec - other.tv_nsec,
        };
        posix_Normalize(result);
        return result;
    }

    [[nodiscard]] constexpr bool posix_IsNegative(timespec const& self)
    {
        if (self.tv_sec < 0)
        {
            return self.tv_nsec < 0;
        }

        return self.tv_sec < 0;
    }

    // self < other
    [[nodiscard]] constexpr bool posix_CompareLess(timespec const& self, timespec const& other)
    {
        if (self.tv_sec == other.tv_sec)
        {
            return self.tv_nsec < other.tv_nsec;
        }
        else
        {
            return self.tv_sec < other.tv_sec;
        }
    }

    // self == other
    [[nodiscard]] constexpr bool posix_CompareEqual(timespec const& self, timespec const& other)
    {
        return (self.tv_sec == other.tv_sec) and (self.tv_nsec == other.tv_nsec);
    }

    // self > other
    [[nodiscard]] constexpr bool posix_CompareGreater(timespec const& self, timespec const& other)
    {
        if (self.tv_sec == other.tv_sec)
        {
            return self.tv_nsec > other.tv_nsec;
        }
        else
        {
            return self.tv_sec > other.tv_sec;
        }
    }

    // self >= other
    [[nodiscard]] constexpr bool posix_CompareGreaterEqual(timespec const& self, timespec const& other)
    {
        return not posix_CompareLess(self, other);
    }

    // self <= other
    [[nodiscard]] constexpr bool posix_CompareLessEqual(timespec const& self, timespec const& other)
    {
        return not posix_CompareGreater(self, other);
    }

    inline void posix_ValidateTimeout(timespec& self, Duration const& value)
    {
        posix_AddDuration(self, value);
    }

    inline void posix_ValidateTimeout(timespec& self, int32_t milliseconds)
    {
        posix_AddDuration(self, Duration::FromMilliseconds(milliseconds));
    }

    constexpr timespec posix_FromDuration(Duration const& value)
    {
        return timespec{
            .tv_sec = value.Seconds,
            .tv_nsec = value.Nanoseconds,
        };
    }
}

// https://www.remlab.net/op/futex-condvar.shtml
// https://www.remlab.net/op/futex-misc.shtml

namespace Anemone::Interop
{
    anemone_forceinline void posix_FutexWait(std::atomic<int>& futex, int expected, timespec const* timeout) noexcept
    {
        int const rc = syscall(SYS_futex, &futex, FUTEX_WAIT_PRIVATE, expected, timeout, nullptr, 0);

        if (rc == -1)
        {
            AE_PANIC("FutexWait (rc: {}, '{}')", errno, strerror(errno));
        }
    }

    anemone_forceinline void posix_FutexWakeOne(std::atomic<int>& futex)
    {
        int const rc = syscall(SYS_futex, &futex, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, 1, nullptr, nullptr, 0);

        if (rc == -1)
        {
            AE_PANIC("FutexWakeOne (rc: {}, '{}')", errno, strerror(errno));
        }
    }

    anemone_forceinline void posix_FutexWakeAll(std::atomic<int>& futex)
    {
        int const rc = syscall(SYS_futex, &futex, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, INT32_MAX, nullptr, nullptr, 0);

        if (rc == -1)
        {
            AE_PANIC("FutexWakeAll (rc: {}, '{}')", errno, strerror(errno));
        }
    }

    anemone_forceinline void posix_FutexWait(std::atomic<int>& futex, int expected)
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
    anemone_forceinline void posix_FutexWaitUntil(std::atomic<int>& futex, PredicateT&& predicate)
    {
        int value = futex.load();

        while (not std::forward<PredicateT>(predicate)(value))
        {
            posix_FutexWait(futex, value);
            value = futex.load();
        }
    }

    anemone_forceinline bool posix_FutexWaitTimeout(std::atomic<int>& futex, int expected, Duration const& timeout)
    {
        timespec tsTimeout = Interop::posix_FromDuration(timeout);
        timespec tsStart{};
        clock_gettime(CLOCK_MONOTONIC, &tsStart);

        timespec tsElapsed{};

        while (true)
        {
            if (not Interop::posix_CompareGreaterEqual(tsElapsed, tsTimeout))
            {
                return false;
            }

            timespec tsPartialTimeout = Interop::posix_TimespecDifference(tsTimeout, tsElapsed);

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

            tsElapsed = Interop::posix_TimespecDifference(tsCurrent, tsStart);

            if (futex.load() != expected)
            {
                return true;
            }
        }
    }
}
