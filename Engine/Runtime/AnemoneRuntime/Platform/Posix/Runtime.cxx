#include "AnemoneRuntime/Platform/Runtime.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"

#include <sys/resource.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/auxv.h>
#include <pwd.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sched.h>

#if ANEMONE_ARCHITECTURE_X64
#include <cpuid.h>
#elif ANEMONE_ARCHITECTURE_ARM64
#include <asm/hwcap.h>
#endif

#include <clocale>

namespace Anemone::Platform
{
    void SetCrashReportHandling(bool enable)
    {
        // TODO: Implement this
        (void)enable;
    }

    void InitializeRuntime_PlatformSpecific()
    {
        // Set locale.
        (void)std::setlocale(LC_ALL, "en_US.UTF-8"); // NOLINT(concurrency-mt-unsafe); this is invoked in main thread.

        // Bump up the file descriptor limit
        if (rlimit64 rl; getrlimit64(RLIMIT_NOFILE, &rl) == 0)
        {
            if ((rl.rlim_cur != RLIM_INFINITY) and (rl.rlim_cur < 8192))
            {
                rl.rlim_cur = 8192;
                setrlimit64(RLIMIT_NOFILE, &rl);
            }
        }
    }

    void FinalizeRuntime_PlatformSpecific()
    {
        // TODO: Implement this
    }
}
