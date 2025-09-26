#include "AnemoneRuntime/Base/Platform/Linux/LinuxInstant.hxx"
#include "AnemoneRuntime/Interop/Linux/DateTime.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"

namespace Anemone
{
    Instant LinuxInstant::Now()
    {
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
    }
}

namespace Anemone
{
    Instant Instant::Now()
    {
        return LinuxInstant::Now();
    }
}
