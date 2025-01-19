#include "AnemoneRuntime/Platform/Linux/LinuxPlatform.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"

#include <sys/resource.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/auxv.h>
#include <sys/random.h>
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

namespace Anemone::Platform::Internal
{
    UninitializedObject<LinuxPlatformStatics> GLinuxPlatformStatics;
    UninitializedObject<PlatformStatics> GPlatformStatics;
}

namespace Anemone::Platform::Internal
{
    void ReportApplicationStop(std::string_view reason)
    {
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

    void Initialize(int argc, char* argv[])
    {
        (void)argc;
        (void)argv;

        GPlatformStatics.Create();
        GLinuxPlatformStatics.Create();

        VerifyRequirements();

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

    void Finalize()
    {
        GLinuxPlatformStatics.Destroy();
        GPlatformStatics.Destroy();
    }
}

namespace Anemone::Platform
{
    bool GetClipboardText(std::string& result)
    {
        result = {};
        return false;
    }

    bool SetClipboardText(std::string_view value)
    {
        (void)value;
        return false;
    }

    bool GetEnvironmentVariable(std::string& result, std::string_view name)
    {
        char const* variable = secure_getenv(std::string{name}.c_str());

        if (variable != nullptr)
        {
            result.assign(variable);
        }

        return variable != nullptr;
    }

    bool SetEnvironmentVariable(std::string_view name, std::string_view value)
    {
        return setenv(std::string{name}.c_str(), std::string{value}.c_str(), 1) == 0;
    }

    bool RemoveEnvironmentVariable(std::string_view name)
    {
        return unsetenv(std::string{name}.c_str()) == 0;
    }

    std::vector<ProcessorTopology> GetProcessorTopology()
    {
        return {};
    }

    void GetDisplayMetrics(DisplayMetrics& displayMetrics)
    {
        displayMetrics = {};
    }

    ColorRef GetScreenPixel(Math::PointF position, float gamma)
    {
        (void)position;
        (void)gamma;
        return {};
    }

    std::string_view GetOperatingSystem()
    {
        return {};
    }

    Duration GetSystemUptime()
    {
        return {};
    }

    MemoryProperties GetMemoryProperties()
    {
        // Get memory properties
        struct sysinfo info
        {
        };

        sysinfo(&info);
        // Get address space limits
        struct rlimit64 limits
        {
        };

        getrlimit64(RLIMIT_AS, &limits);

        uint64_t const pageSize = static_cast<uint64_t>(sysconf(_SC_PAGE_SIZE));

        return MemoryProperties{
            .TotalPhysicalMemory = info.mem_unit * info.totalram,
            .TotalVirtualMemory = info.mem_unit * info.totalswap,
            .TotalPageFile = info.mem_unit * info.totalswap,
            .AvailablePhysicalMemory = std::numeric_limits<uint64_t>::max(),
            .AvailableVirtualMemory = std::numeric_limits<uint64_t>::max(),
            .AvailablePageFile = pageSize,
            .SystemAllocationGranularity = pageSize * 16,
            .PageSize = pageSize,
            .AddressLimitLow = 0,
            .AddressLimitHigh = limits.rlim_max,
        };
    }

    MemoryUsage GetMemoryUsage()
    {
        MemoryUsage result{};

        std::array<char, 128> path{};
        sprintf(path.data(), "/proc/%u/status", static_cast<uint32_t>(getpid()));

        if (FILE* f = fopen(path.data(), "r"); f != nullptr)
        {
            std::array<char, 256> line{};
            unsigned long value{};

            while (fgets(line.data(), static_cast<int>(line.size()), f))
            {
                if (sscanf(line.data(), "VmHWM: %lu", &value) == 1)
                {
                    result.PeakUsedPhysical = static_cast<uint64_t>(value * 1024);
                }
                else if (sscanf(line.data(), "VmRSS: %lu", &value) == 1)
                {
                    result.UsedPhysical = static_cast<uint64_t>(value * 1024);
                }
                else if (sscanf(line.data(), "RssAnon: %lu", &value) == 1)
                {
                    result.UsedVirtual += static_cast<uint64_t>(value * 1024);
                }
                else if (sscanf(line.data(), "VmSwap: %lu", &value) == 1)
                {
                    result.UsedVirtual += static_cast<uint64_t>(value * 1024);
                }
            }

            result.PeakUsedVirtual = result.UsedVirtual;

            fclose(f);
        }

        if (FILE* f = fopen("/proc/meminfo", "r"); f != nullptr)
        {
            std::array<char, 256> line{};
            unsigned long value{};

            while (fgets(line.data(), static_cast<int>(line.size()), f))
            {
                if (sscanf(line.data(), "MemFree: %lu", &value) == 1)
                {
                    result.AvailablePhysical = static_cast<uint64_t>(value * 1024);
                }
                else if (sscanf(line.data(), "Buffers: %lu", &value) == 1)
                {
                    result.AvailablePhysical += static_cast<uint64_t>(value * 1024);
                }
                else if (sscanf(line.data(), "Cached: %lu", &value) == 1)
                {
                    result.AvailablePhysical += static_cast<uint64_t>(value * 1024);
                }
            }

            fclose(f);
        }

        result.AvailableVirtual = (~static_cast<uintptr_t>(0)) - result.UsedVirtual;

        return result;
    }

    PowerUsage GetPowerUsage()
    {
        return {};
    }

    ProcessorUsage GetProcessorUsage()
    {
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);

        return ProcessorUsage{
            .UserTime = Interop::posix_into_Duration(usage.ru_utime),
            .KernelTime = Interop::posix_into_Duration(usage.ru_stime),
        };
    }

    void Terminate(bool force)
    {
        (void)force;
        abort();
    }

    std::string_view GetDeviceUniqueId()
    {
        return {};
    }

    std::string_view GetDeviceName()
    {
        return {};
    }

    std::string_view GetDeviceModel()
    {
        return {};
    }

    DeviceType GetDeviceType()
    {
        return {};
    }

    DeviceProperties GetDeviceProperties()
    {
        return {};
    }

    AssertAction HandleAssertion(std::source_location const& location, std::string_view expression, std::string_view message)
    {
        (void)location;
        (void)expression;
        (void)message;
        return AssertAction::Abort;
    }

    void HandlePanic()
    {
    }

    // Diagnostics
    void DebugBreak()
    {
        anemone_debugbreak();
    }

    void Crash()
    {
#if !ANEMONE_BUILD_SHIPPING
        anemone_debugbreak();
#endif

        abort();
    }

    bool IsDebuggerAttached()
    {
        return false;
    }

    void WaitForDebugger()
    {
    }

    bool AttachDebugger()
    {
        return false;
    }

    DateTime GetCurrentDateTime()
    {
        struct timespec ts;

        [[maybe_unused]] int const result = clock_gettime(CLOCK_REALTIME, &ts);
        AE_ASSERT(result == 0);

        int64_t const bias = DateTimeOffset::GetCurrentTimeZoneBias().Seconds;

        return DateTime{
            .Inner = {
                .Seconds = ts.tv_sec - bias + Interop::posix_DateAdjustOffset,
                .Nanoseconds = ts.tv_nsec,
            }};
    }

    DateTime GetCurrentDateTimeUtc()
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

    Duration GetCurrentTimeZoneBias()
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

    void GetRandom(std::span<std::byte> buffer)
    {
        size_t processed = 0;

        // Use getrandom first
        while (processed < buffer.size())
        {
            ssize_t result = getrandom(buffer.data() + processed, buffer.size() - processed, 0);

            if (result < 0)
            {
                auto const error = errno;

                if ((error == EAGAIN) or (error == EINTR))
                {
                    continue;
                }

                break;
            }

            processed += static_cast<size_t>(result);
        }
    }

    void ProcessMessages()
    {
    }
}
