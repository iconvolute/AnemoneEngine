#include "AnemoneInterop/Linux/DateTime.hxx"
#include "AnemoneBase/Platform/Linux/LinuxDateTime.hxx"
#include "AnemoneDiagnostics/Debug.hxx"

namespace Anemone
{
    DateTime LinuxDateTime::GetCurrentDateTime()
    {
        struct timespec ts;

        [[maybe_unused]] int const result = clock_gettime(CLOCK_REALTIME, &ts);
        AE_ASSERT(result == 0);

        int64_t const bias = GetCurrentTimeZoneBias().Seconds;

        return DateTime{
            .Inner = {
                .Seconds = ts.tv_sec - bias + Interop::Linux::DateAdjustOffset,
                .Nanoseconds = ts.tv_nsec,
            }};
    }

    DateTime LinuxDateTime::GetCurrentDateTimeUtc()
    {
        struct timespec ts;

        // Get local time
        [[maybe_unused]] int const result = clock_gettime(CLOCK_REALTIME, &ts);
        AE_ASSERT(result == 0);

        return DateTime{
            .Inner = {
                .Seconds = ts.tv_sec + Interop::Linux::DateAdjustOffset,
                .Nanoseconds = ts.tv_nsec,
            },
        };
    }

    Duration LinuxDateTime::GetCurrentTimeZoneBias()
    {
        time_t seconds = 0;
        tm tmGMT{};
        tm tmLocal{};
        gmtime_r(&seconds, &tmGMT);
        localtime_r(&seconds, &tmLocal);

        return Duration{
            .Seconds = mktime(&tmGMT) - mktime(&tmLocal),
            .Nanoseconds = 0,
        };
    }
}

namespace Anemone
{
    DateTime DateTime::Now()
    {
        return LinuxDateTime::GetCurrentDateTime();
    }

    DateTime DateTime::UtcNow()
    {
        return LinuxDateTime::GetCurrentDateTimeUtc();
    }

    DateTimeOffset DateTimeOffset::Now()
    {
        return DateTimeOffset{
            .Local = LinuxDateTime::GetCurrentDateTime(),
            .Bias = LinuxDateTime::GetCurrentTimeZoneBias(),
        };
    }

    Duration DateTimeOffset::GetCurrentTimeZoneBias()
    {
        return LinuxDateTime::GetCurrentTimeZoneBias();
    }
}
