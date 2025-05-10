#pragma once
#include "AnemoneRuntime/Interop/Linux/Headers.hxx"
#include "AnemoneRuntime/Base/Duration.hxx"
#include "AnemoneRuntime/Base/DateTime.hxx"

namespace Anemone::Interop::Linux
{
    inline constexpr int64_t DateAdjustOffset = 62135596800;
    inline constexpr int64_t NanosecondsInSecond = 1'000'000'000;

    constexpr Duration ToDuration(struct timeval const& tv)
    {
        return Duration{
            .Seconds = tv.tv_sec,
            .Nanoseconds = tv.tv_usec * 1'000,
        };
    }

    constexpr Duration ToDuration(struct timespec const& ts)
    {
        return Duration{
            .Seconds = ts.tv_sec,
            .Nanoseconds = ts.tv_nsec,
        };
    }

    constexpr DateTime ToDateTime(struct timespec const& ts, int64_t bias)
    {
        return DateTime{
            .Inner = {
                .Seconds = ts.tv_sec - bias + DateAdjustOffset,
                .Nanoseconds = ts.tv_nsec,
            },
        };
    }

    constexpr DateTime ToDateTime(struct timespec const& ts)
    {
        return DateTime{
            .Inner = {
                .Seconds = ts.tv_sec + DateAdjustOffset,
                .Nanoseconds = ts.tv_nsec,
            },
        };
    }
    
    constexpr timespec ToTimespec(Duration const& value)
    {
        return timespec{
            .tv_sec = value.Seconds,
            .tv_nsec = value.Nanoseconds,
        };
    }

    constexpr void Normalize(timespec& self)
    {
        self.tv_sec += (self.tv_nsec / NanosecondsInSecond);

        if ((self.tv_nsec %= NanosecondsInSecond) < 0)
        {
            self.tv_nsec += NanosecondsInSecond;
            --self.tv_sec;
        }
    }

    constexpr void AddDuration(timespec& self, Duration const& value)
    {
        self.tv_sec += value.Seconds;
        self.tv_nsec += value.Nanoseconds;
        Normalize(self);
    }

    constexpr void SubtractDuration(timespec& self, Duration const& value)
    {
        self.tv_sec -= value.Seconds;
        self.tv_nsec -= value.Nanoseconds;
        Normalize(self);
    }

    constexpr void SubtractTimespec(timespec& self, timespec const& value)
    {
        self.tv_sec -= value.tv_sec;
        self.tv_nsec -= value.tv_nsec;
        Normalize(self);
    }

    constexpr timespec TimespecDifference(timespec const& self, timespec const& other)
    {
        timespec result{
            .tv_sec = self.tv_sec - other.tv_sec,
            .tv_nsec = self.tv_nsec - other.tv_nsec,
        };
        Normalize(result);
        return result;
    }

    [[nodiscard]] constexpr bool IsNegative(timespec const& self)
    {
        if (self.tv_sec < 0)
        {
            return self.tv_nsec < 0;
        }

        return self.tv_sec < 0;
    }

    // self < other
    [[nodiscard]] constexpr bool CompareLess(timespec const& self, timespec const& other)
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
    [[nodiscard]] constexpr bool CompareEqual(timespec const& self, timespec const& other)
    {
        return (self.tv_sec == other.tv_sec) and (self.tv_nsec == other.tv_nsec);
    }

    // self > other
    [[nodiscard]] constexpr bool CompareGreater(timespec const& self, timespec const& other)
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
    [[nodiscard]] constexpr bool CompareGreaterEqual(timespec const& self, timespec const& other)
    {
        return not CompareLess(self, other);
    }

    // self <= other
    [[nodiscard]] constexpr bool CompareLessEqual(timespec const& self, timespec const& other)
    {
        return not CompareGreater(self, other);
    }

}
