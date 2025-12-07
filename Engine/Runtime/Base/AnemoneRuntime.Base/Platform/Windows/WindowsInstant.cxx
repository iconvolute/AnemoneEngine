#include "AnemoneRuntime.Base/Platform/Windows/WindowsInstant.hxx"
#include "AnemoneRuntime.Interop/Windows/DateTime.hxx"
#include "AnemoneRuntime.Diagnostics/Debug.hxx"

namespace Anemone
{
    int64_t WindowsInstant::QueryInstantFrequency()
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

    int64_t WindowsInstant::QueryInstantValue()
    {
        LARGE_INTEGER counter;
        [[maybe_unused]] BOOL const success = QueryPerformanceCounter(&counter);
        AE_ASSERT(success != FALSE);

        return std::bit_cast<int64_t>(counter);
    }

    Instant WindowsInstant::Now()
    {
        static constexpr int64_t QpcFrequency = 10'000'000;

        int64_t const value = QueryInstantValue();

        if (int64_t const frequency = QueryInstantFrequency(); frequency == QpcFrequency)
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
            int64_t const nanosecond_conversion = Internal::NanosecondsInSecond / frequency;
            return Instant{
                .Inner = {
                    .Seconds = value / frequency,
                    .Nanoseconds = (value % frequency) * nanosecond_conversion,
                },
            };
        }
    }
}

namespace Anemone
{
    Instant Instant::Now()
    {
        return WindowsInstant::Now();
    }
}
