#include "AnemoneRuntime/Platform/Static.hxx"
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
    static void ReportApplicationStop(std::string_view reason)
    {
        // TODO: Move this to diagnostics
        fwrite(reason.data(), 1, reason.size(), stderr);
        fflush(stderr);
        abort();
    }

    static void VerifyRequirements()
    {
#if ANEMONE_ARCHITECTURE_X64

        int level0[4]{-1};
        __cpuid(0, level0[0], level0[1], level0[2], level0[3]);

        if (level0[0] < 1)
        {
            ReportApplicationStop("Application stop: CPU Features not available\n");
        }

        int level1[4]{-1};
        __cpuid(1, level1[0], level1[1], level1[2], level1[3]);

        if (!(level1[2] & bit_CMPXCHG16B))
        {
            ReportApplicationStop("Application stop: CMPXCHG16B is not supported\n");
        }

        if (!(level1[2] & bit_POPCNT))
        {
            ReportApplicationStop("Application stop: POPCNT is not supported\n");
        }

#if ANEMONE_FEATURE_AVX
        if (!(level1[2] & bit_OSXSAVE))
        {
            ReportApplicationStop("Application stop: OSXSAVE is not supported\n");
        }

        if (!(level1[2] & bit_SSE3))
        {
            ReportApplicationStop("Application stop: SSE3 is not supported\n");
        }

        if (!(level1[2] & bit_SSE4_1))
        {
            ReportApplicationStop("Application stop: SSE4_1 is not supported\n");
        }

        if (!(level1[2] & bit_AVX))
        {
            ReportApplicationStop("Application stop: AVX is not supported\n");
        }

#if ANEMONE_FEATURE_AVX2

        if (!(level1[2] & bit_F16C))
        {
            ReportApplicationStop("Application stop: F16C is not supported\n");
        }

        if (!(level1[2] & bit_FMA))
        {
            ReportApplicationStop("Application stop: FMA is not supported\n");
        }

        int level7_0[4]{-1};
        __cpuid_count(7, 0, level7_0[0], level7_0[1], level7_0[2], level7_0[3]);

        if (!(level7_0[1] & bit_AVX2))
        {
            ReportApplicationStop("Application stop: AVX2 is not supported\n");
        }

#endif

#endif

#endif


#if ANEMONE_ARCHITECTURE_ARM64

        auto const cap = getauxval(AT_HWCAP);

        if ((cap & HWCAP_FP) != HWCAP_FP)
        {
            ReportApplicationStop("Application stop: NEON not supported\n");
        }

        if ((cap & HWCAP_ASIMD) != HWCAP_ASIMD)
        {
            ReportApplicationStop("Application stop: ASIMD not supported\n");
        }

        if ((cap & HWCAP_ATOMICS) != HWCAP_ATOMICS)
        {
            ReportApplicationStop("Application stop: ATOMICS not supported\n");
        }

#endif
    }

    void PosixPlatformStatic::Initialize()
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

        VerifyRequirements();
    }

    void PosixPlatformStatic::Finalize()
    {
    }
}
