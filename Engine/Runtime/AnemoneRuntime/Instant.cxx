#include "AnemoneRuntime/Instant.hxx"
#include "AnemoneRuntime/Diagnostic/Debug.hxx"

#if ANEMONE_PLATFORM_WINDOWS || ANEMONE_PLATFORM_GAMING_DESKTOP
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"
#elif ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX

ANEMONE_EXTERNAL_HEADERS_BEGIN
#include <sys/mman.h>
#include <time.h>
ANEMONE_EXTERNAL_HEADERS_END

#endif

namespace Anemone::Private
{
#if ANEMONE_PLATFORM_WINDOWS
    static int64_t QueryInstantFrequency()
    {
        static std::atomic<int64_t> cache{};

        int64_t result = cache.load(std::memory_order::relaxed);

        if (result != 0)
        {
            return result;
        }

        // Implementation detail:
        //  QPC Frequency is stored in KUSER_SHARED_DATA.
        //  This function just performs read from that struct on all known platforms.
        LARGE_INTEGER value;
        [[maybe_unused]] BOOL const success = QueryPerformanceFrequency(&value);
        AE_ASSERT(success != FALSE);

        result = std::bit_cast<int64_t>(value);
        cache.store(result, std::memory_order::relaxed);
        return result;
    }

    static int64_t QueryInstantValue()
    {
        LARGE_INTEGER counter;
        [[maybe_unused]] BOOL const success = QueryPerformanceCounter(&counter);
        AE_ASSERT(success != FALSE);

        return std::bit_cast<int64_t>(counter);
    }
#endif
}

namespace Anemone
{
    Instant Instant::Now()
    {
#if ANEMONE_PLATFORM_WINDOWS || ANEMONE_PLATFORM_GAMING_DESKTOP

        static constexpr int64_t QpcFrequency = 10'000'000;

        int64_t const value = Private::QueryInstantValue();

        if (int64_t const frequency = Private::QueryInstantFrequency(); frequency == QpcFrequency)
        {
            return Instant{
                .Inner = {
                    .Seconds = value / QpcFrequency,
                    .Nanoseconds = (value % QpcFrequency) * 100,
                },
            };
        }
        else
        {
            int64_t const nanosecond_conversion = Private::NanosecondsInSecond / frequency;
            return Instant{
                .Inner = {
                    .Seconds = value / frequency,
                    .Nanoseconds = (value % frequency) * nanosecond_conversion,
                },
            };
        }

#elif ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX

#if defined(HAVE_CLOCK_GETTIME_NSEC_NP)

        return {.Inner = Duration::FromNanoseconds(clock_gettime_nsec_np(CLOCK_MONOTONIC_RAW))};

#else

        struct timespec ts;

        [[maybe_unused]] int const result = clock_gettime(CLOCK_MONOTONIC, &ts);
        AE_ASSERT(result == 0);

        return Instant{
            .Inner = {
                .Seconds = ts.tv_sec,
                .Nanoseconds = ts.tv_nsec,
            },
        };
#endif

#else
#error "Not implemented"
#endif
    }
}
