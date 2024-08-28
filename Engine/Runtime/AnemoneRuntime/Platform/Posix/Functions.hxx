#pragma once
#include "AnemoneRuntime/Diagnostic/Assert.hxx"
#include "AnemoneRuntime/Duration.hxx"
#include "AnemoneRuntime/DateTime.hxx"
#include "AnemoneRuntime/Platform/Posix/Headers.hxx"
#include "AnemoneRuntime/Platform/Posix/Types.hxx"

#include <atomic>

namespace Anemone::Platform
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

namespace Anemone::Platform
{
    anemone_forceinline long linux_FutexWait(std::atomic<int>& futex, int expected, timespec const* timeout) noexcept
    {
        return syscall(SYS_futex, &futex, FUTEX_WAIT_PRIVATE, expected, timeout, nullptr, 0);
    }

    anemone_forceinline long linux_FutexWakeAll(std::atomic<int>& futex) noexcept
    {
        return syscall(SYS_futex, &futex, FUTEX_WAKE_PRIVATE, INT_MAX, nullptr, nullptr, 0);
    }

    anemone_forceinline long linux_FutexWakeOne(std::atomic<int>& futex) noexcept
    {
        return syscall(SYS_futex, &futex, FUTEX_WAKE_PRIVATE, 1, nullptr, nullptr, 0);
    }
}
