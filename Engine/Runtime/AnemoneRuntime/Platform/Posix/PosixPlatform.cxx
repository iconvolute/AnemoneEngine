#include "AnemoneRuntime/Platform/Posix/PosixPlatform.hxx"

namespace Anemone
{
    DateTime Platform::GetCurrentDateTime()
    {
        struct timespec ts;

        [[maybe_unused]] int const result = clock_gettime(CLOCK_REALTIME, &ts);
        AE_ASSERT(result == 0);

        int64_t const bias = Platform::GetCurrentTimeZoneBias().Seconds;

        return DateTime{
            .Inner = {
                .Seconds = ts.tv_sec - bias + Interop::posix_DateAdjustOffset,
                .Nanoseconds = ts.tv_nsec,
            }};
    }

    DateTime Platform::GetCurrentDateTimeUtc()
    {
        struct timespec ts;

        // Get local time
        [[maybe_unused]] int const result = clock_gettime(CLOCK_REALTIME, &ts);
        AE_ASSERT(result == 0);

        return DateTime{
            .Inner = {
                .Seconds = ts.tv_sec + Interop::posix_DateAdjustOffset,
                .Nanoseconds = ts.tv_nsec,
            },
        };
    }

    Duration Platform::GetCurrentTimeZoneBias()
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
