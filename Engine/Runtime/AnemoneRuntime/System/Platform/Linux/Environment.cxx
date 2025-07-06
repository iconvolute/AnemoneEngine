#include "AnemoneRuntime/System/Environment.hxx"
#include "AnemoneRuntime/Interop/Linux/Headers.hxx"
#include "AnemoneRuntime/Interop/Linux/DateTime.hxx"
#include "AnemoneRuntime/Interop/Linux/FileSystem.hxx"
#include "AnemoneRuntime/Interop/StringBuffer.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Platform/FilePath.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"

#include <clocale>

#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <sys/random.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/utsname.h>
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

namespace Anemone::Internal
{
    extern bool GIsConsoleApplication;
    bool GIsConsoleApplication = false;
}

namespace Anemone
{
    namespace
    {
        struct EnvironmentStatics final
        {
            DateTime startupTime;
            Uuid systemId;
            DeviceType deviceType;
            DeviceProperties deviceProperties;

            std::string systemVersion;
            std::string systemName;
            std::string deviceId;
            std::string deviceName;
            std::string deviceModel;
            std::string deviceManufacturer;
            std::string deviceVersion;
            std::string computerName;
            std::string userName;
            std::string executablePath;
            std::string startupPath;
            std::string profilePath;
            std::string desktopPath;
            std::string documentsPath;
            std::string downloadsPath;
            std::string temporaryPath;

            // Number of physical cores.
            uint32_t physicalCores{};

            // Number of logical cores.
            uint32_t logicalCores{};

            // Number of performance cores.
            uint32_t performanceCores{};

            // Number of efficiency cores.
            uint32_t efficiencyCores{};

            bool hyperThreading{};

            // Smallest cache-line size found.
            uint32_t cacheLineSize{};

            uint32_t cacheSizeLevel1{};
            uint32_t cacheSizeLevel2{};
            uint32_t cacheSizeLevel3{};

            Interop::string_buffer<char, 64> processorName{};

            Interop::string_buffer<char, 64> processorVendor{};
        };

        UninitializedObject<EnvironmentStatics> gEnvironmentStatics{};
        pthread_mutex_t GEnvironmentLock = PTHREAD_MUTEX_INITIALIZER;

        void InitializeEnvironment(EnvironmentStatics& statics)
        {
            statics.startupTime = Environment::GetCurrentDateTime();

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

            // System version
            {
                if (struct utsname name{}; uname(&name) != -1)
                {
                    // TODO: Determine OS version from '/etc/lsb-release'?
                    statics.systemVersion = fmt::format("{}-{}-{}", name.sysname, name.release, name.version);
                }
            }

            // System ID
            {
                Interop::string_buffer<char, 64> buffer{};

                std::optional<Uuid> parsed{};

                if (Interop::Linux::LoadFile(buffer, "/etc/machine-id"))
                {
                    buffer.trim(32);
                    parsed = UuidParser::Parse(buffer.as_view());
                }
                else if (Interop::Linux::LoadFile(buffer, "/var/lib/dbus/machine-id"))
                {
                    buffer.trim(32);
                    parsed = UuidParser::Parse(buffer.as_view());
                }


                if (parsed)
                {
                    statics.systemId = *parsed;
                }
                else
                {
                    AE_TRACE(Warning, "Failed to parse machine id");
                }
            }

            // Startup path
            {
                std::array<char, PATH_MAX> procpath{};

                if (getcwd(procpath.data(), procpath.size()) != nullptr)
                {
                    statics.startupPath.assign(procpath.data());
                }
            }

            // Executable
            {
                std::array<char, PATH_MAX> procpath{};
                std::array<char, PATH_MAX> execpath{};

                pid_t pid = getpid();
                snprintf(std::data(procpath), std::size(procpath), "/proc/%d/exe", pid);
                ssize_t length = readlink(procpath.data(), execpath.data(), execpath.size());
                AE_ASSERT(length >= 0);
                length = std::max<ssize_t>(0, length);

                statics.executablePath.assign(execpath.data(), static_cast<size_t>(length));
            }

            // Computer name
            {
                std::array<char, HOST_NAME_MAX> hostname{};
                struct utsname name = {};

                if (uname(&name) == 0)
                {
                    statics.computerName.assign(name.nodename);
                }
                else if (gethostname(hostname.data(), hostname.size()) == 0)
                {
                    statics.computerName.assign(hostname.data());
                }
                else
                {
                    statics.computerName.assign("Linux Machine");
                }
            }

            // User name
            {
                uid_t uid = geteuid();
                struct passwd* pw = getpwuid(uid);

                if (pw != nullptr)
                {
                    statics.userName = pw->pw_name;
                }
                else
                {
                    statics.userName = "Unknown";
                }
            }

            // Profile path
            {
#if !ANEMONE_PLATFORM_ANDROID
                const char* value = secure_getenv("HOME");

                if (value != nullptr)
                {
                    statics.profilePath.assign(value);
                }
                else
                {
                    struct passwd* userinfo = getpwuid(geteuid());

                    if (userinfo != nullptr && userinfo->pw_dir != nullptr)
                    {
                        statics.profilePath.assign(userinfo->pw_dir);
                    }
                }
#endif
            }

            // UserDesktopPath
            {
                statics.desktopPath = statics.profilePath;
                FilePath::PushFragment(statics.desktopPath, "Desktop");
            }

            // UserDocumentsPath
            {
                statics.documentsPath = statics.profilePath;
                FilePath::PushFragment(statics.documentsPath, "Documents");
            }

            // UserDownloadsPath
            {
                statics.downloadsPath = statics.profilePath;
                FilePath::PushFragment(statics.downloadsPath, "Downloads");
            }

            // Temp path
            {
#if !ANEMONE_PLATFORM_ANDROID
                const char* value = secure_getenv("TMPDIR");

                if (value == nullptr)
                {
                    value = secure_getenv("TMP");
                }

                if (value == nullptr)
                {
                    value = secure_getenv("TEMP");
                }

                if (value == nullptr)
                {
                    value = secure_getenv("TEMPDIR");
                }

                if (value == nullptr)
                {
                    value = "/tmp/";
                }

                statics.temporaryPath.assign(value);
#endif
            }
        }

        void InitializeProcessorProperties(EnvironmentStatics& statics)
        {
            // This may not be accurate on all systems.
            statics.physicalCores = sysconf(_SC_NPROCESSORS_CONF);

            cpu_set_t mask;
            CPU_ZERO(&mask);

            if (sched_getaffinity(0, sizeof(mask), &mask) == 0)
            {
                statics.logicalCores = CPU_COUNT(&mask);
            }
            else
            {
                statics.logicalCores = statics.physicalCores;
            }

            if (statics.performanceCores == 0)
            {
                statics.performanceCores = statics.physicalCores;
                statics.efficiencyCores = 0;
            }
        }

        void VerifyRequirements()
        {
#if ANEMONE_ARCHITECTURE_X64

            int level0[4]{-1};
            __cpuid(0, level0[0], level0[1], level0[2], level0[3]);

            if (level0[0] < 1)
            {
                Diagnostics::Debug::ReportApplicationStop("Application stop: CPU Features not available\n");
            }

            int level1[4]{-1};
            __cpuid(1, level1[0], level1[1], level1[2], level1[3]);

            if (!(level1[2] & bit_CMPXCHG16B))
            {
                Diagnostics::Debug::ReportApplicationStop("Application stop: CMPXCHG16B is not supported\n");
            }

            if (!(level1[2] & bit_POPCNT))
            {
                Diagnostics::Debug::ReportApplicationStop("Application stop: POPCNT is not supported\n");
            }

#if ANEMONE_FEATURE_AVX
            if (!(level1[2] & bit_OSXSAVE))
            {
                Diagnostics::Debug::ReportApplicationStop("Application stop: OSXSAVE is not supported\n");
            }

            if (!(level1[2] & bit_SSE3))
            {
                Diagnostics::Debug::ReportApplicationStop("Application stop: SSE3 is not supported\n");
            }

            if (!(level1[2] & bit_SSE4_1))
            {
                Diagnostics::Debug::ReportApplicationStop("Application stop: SSE4_1 is not supported\n");
            }

            if (!(level1[2] & bit_AVX))
            {
                Diagnostics::Debug::ReportApplicationStop("Application stop: AVX is not supported\n");
            }

#if ANEMONE_FEATURE_AVX2

            if (!(level1[2] & bit_F16C))
            {
                Diagnostics::Debug::ReportApplicationStop("Application stop: F16C is not supported\n");
            }

            if (!(level1[2] & bit_FMA))
            {
                Diagnostics::Debug::ReportApplicationStop("Application stop: FMA is not supported\n");
            }

            int level7_0[4]{-1};
            __cpuid_count(7, 0, level7_0[0], level7_0[1], level7_0[2], level7_0[3]);

            if (!(level7_0[1] & bit_AVX2))
            {
                Diagnostics::Debug::ReportApplicationStop("Application stop: AVX2 is not supported\n");
            }

#endif

#endif

#endif


#if ANEMONE_ARCHITECTURE_ARM64

            auto const cap = getauxval(AT_HWCAP);

            if ((cap & HWCAP_FP) != HWCAP_FP)
            {
                Diagnostics::Debug::ReportApplicationStop("Application stop: NEON not supported\n");
            }

            if ((cap & HWCAP_ASIMD) != HWCAP_ASIMD)
            {
                Diagnostics::Debug::ReportApplicationStop("Application stop: ASIMD not supported\n");
            }

            if ((cap & HWCAP_ATOMICS) != HWCAP_ATOMICS)
            {
                Diagnostics::Debug::ReportApplicationStop("Application stop: ATOMICS not supported\n");
            }

#endif
        }
    }
}

namespace Anemone::Internal
{
    extern void InitializeEnvironment()
    {
        VerifyRequirements();

        gEnvironmentStatics.Create();
        InitializeEnvironment(*gEnvironmentStatics);
        InitializeProcessorProperties(*gEnvironmentStatics);
    }

    extern void FinalizeEnvironment()
    {
        gEnvironmentStatics.Destroy();
    }
}

namespace Anemone
{
    auto Environment::GetEnvironmentVariable(std::string& result, std::string_view name) -> bool
    {
        pthread_mutex_lock(&GEnvironmentLock);

        char const* variable = getenv(std::string{name}.c_str());

        bool const success = variable != nullptr;

        if (success)
        {
            result = variable;
        }

        pthread_mutex_unlock(&GEnvironmentLock);

        return success;
    }

    auto Environment::SetEnvironmentVariable(std::string name, std::string_view value) -> bool
    {
        pthread_mutex_lock(&GEnvironmentLock);

        bool const result = setenv(std::string{name}.c_str(), std::string{value}.c_str(), 1) == 0;

        pthread_mutex_unlock(&GEnvironmentLock);

        return result;
    }

    auto Environment::RemoveEnvironmentVariable(std::string_view name) -> bool
    {
        pthread_mutex_lock(&GEnvironmentLock);

        bool const result = unsetenv(std::string{name}.c_str()) == 0;

        pthread_mutex_unlock(&GEnvironmentLock);

        return result;
    }

    void Environment::GetDisplayMetrics(DisplayMetrics& metrics)
    {
        (void)metrics;
        AE_PANIC("Not implemented");
    }

    auto Environment::GetScreenPixel(Math::PointF position, float gamma) -> ColorRef
    {
        (void)position;
        (void)gamma;
        AE_PANIC("Not implemented");
        return {};
    }

    auto Environment::GetSystemVersion() -> std::string_view
    {
        return gEnvironmentStatics->systemVersion;
    }

    auto Environment::GetSystemId() -> Uuid
    {
        return gEnvironmentStatics->systemId;
    }

    auto Environment::GetSystemName() -> std::string_view
    {
        AE_PANIC("Not implemented");
        return {};
    }

    auto Environment::GetSystemUptime() -> Duration
    {
        struct sysinfo info = {0};
        sysinfo(&info);

        return Duration::FromSeconds(info.uptime);
    }

    auto Environment::GetApplicationStartupTime() -> DateTime
    {
        return gEnvironmentStatics->startupTime;
    }

    auto Environment::GetMemoryProperties() -> MemoryProperties
    {
        // Get memory properties
        struct sysinfo info = {0};
        sysinfo(&info);
        // Get address space limits
        struct rlimit64 limits = {0};

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

    auto Environment::GetMemoryUsage() -> MemoryUsage
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

    auto Environment::GetPowerUsage() -> PowerUsage
    {
        AE_PANIC("Not implemented");
        return {};
    }

    auto Environment::GetProcessorUsage() -> ProcessorUsage
    {
        struct rusage usage = {0};
        getrusage(RUSAGE_SELF, &usage);

        return ProcessorUsage{
            .UserTime = Interop::Linux::ToDuration(usage.ru_utime),
            .KernelTime = Interop::Linux::ToDuration(usage.ru_stime),
        };
    }

    auto Environment::GetProcessMemoryUsage() -> ProcessMemoryUsage
    {
        AE_PANIC("Not implemented");
        return {};
    }


    void Environment::Terminate(bool force)
    {
        if (force)
        {
            abort();
        }
        else
        {
            exit(0);
        }
    }

    bool Environment::IsConsoleApplication()
    {
        return Internal::GIsConsoleApplication;
    }

    auto Environment::GetDeviceUniqueId() -> std::string_view
    {
        return gEnvironmentStatics->deviceId;
    }

    auto Environment::GetDeviceName() -> std::string_view
    {
        return gEnvironmentStatics->deviceName;
    }

    auto Environment::GetDeviceModel() -> std::string_view
    {
        return gEnvironmentStatics->deviceModel;
    }

    auto Environment::GetDeviceType() -> DeviceType
    {
        return gEnvironmentStatics->deviceType;
    }

    auto Environment::GetDeviceProperties() -> DeviceProperties
    {
        return gEnvironmentStatics->deviceProperties;
    }

    auto Environment::GetComputerName() -> std::string_view
    {
        return gEnvironmentStatics->computerName;
    }

    auto Environment::GetUserName() -> std::string_view
    {
        return gEnvironmentStatics->userName;
    }

    auto Environment::GetExecutablePath() -> std::string_view
    {
        return gEnvironmentStatics->executablePath;
    }

    auto Environment::GetStartupPath() -> std::string_view
    {
        return gEnvironmentStatics->startupPath;
    }

    auto Environment::GetHomePath() -> std::string_view
    {
        return gEnvironmentStatics->profilePath;
    }

    auto Environment::GetDesktopPath() -> std::string_view
    {
        return gEnvironmentStatics->desktopPath;
    }

    auto Environment::GetDocumentsPath() -> std::string_view
    {
        return gEnvironmentStatics->documentsPath;
    }

    auto Environment::GetDownloadsPath() -> std::string_view
    {
        return gEnvironmentStatics->downloadsPath;
    }

    auto Environment::GetTemporaryPath() -> std::string_view
    {
        return gEnvironmentStatics->temporaryPath;
    }

    auto Environment::GetCurrentDateTime() -> DateTime
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

    auto Environment::GetCurrentDateTimeUtc() -> DateTime
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

    auto Environment::GetCurrentTimeZoneBias() -> Duration
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

    auto Environment::GetCurrentInstant() -> Instant
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

    void Environment::GetRandom(std::span<std::byte> buffer)
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

    void Environment::LaunchUrl(std::string_view url)
    {
        Interop::string_buffer<char, 512> sUrl{url};

        int status = 0;

        if (pid_t pid1 = fork())
        {
            // Wait for child process to exit
            waitpid(pid1, &status, 0);
        }
        else if (pid1 == 0)
        {
            if (pid_t pid2 = fork())
            {
                // Child process exits itself.
                exit(0);
            }
            else if (pid2 == 0)
            {
                // Grandchild executes the command. It exits immediately and gets orphaned.
                exit(execl("/usr/bin/xdg-open", "xdg-open", "file-to-open.html", static_cast<char*>(nullptr)));
            }
            else
            {
                AE_TRACE(Error, "Failed to spawn second child!");
            }
        }
        else
        {
            AE_TRACE(Error, "Failed to spawn first child!");
        }
    }

    bool Environment::IsOnline()
    {
        AE_PANIC("Not implemented");
        return false;
    }

    auto Environment::GetPhysicalCoresCount() -> size_t
    {
        return gEnvironmentStatics->physicalCores;
    }

    auto Environment::GetLogicalCoresCount() -> size_t
    {
        return gEnvironmentStatics->logicalCores;
    }

    auto Environment::GetPerformanceCoresCount() -> size_t
    {
        return gEnvironmentStatics->performanceCores;
    }

    auto Environment::GetEfficiencyCoresCount() -> size_t
    {
        return gEnvironmentStatics->efficiencyCores;
    }

    bool Environment::IsHyperThreadingEnabled()
    {
        return gEnvironmentStatics->hyperThreading;
    }

    auto Environment::GetCacheLineSize() -> size_t
    {
        return gEnvironmentStatics->cacheLineSize;
    }

    auto Environment::GetCacheSizeLevel1() -> size_t
    {
        return gEnvironmentStatics->cacheSizeLevel1;
    }

    auto Environment::GetCacheSizeLevel2() -> size_t
    {
        return gEnvironmentStatics->cacheSizeLevel2;
    }

    auto Environment::GetCacheSizeLevel3() -> size_t
    {
        return gEnvironmentStatics->cacheSizeLevel3;
    }

    auto Environment::GetProcessorName() -> std::string_view
    {
        return gEnvironmentStatics->processorName.as_view();
    }

    auto Environment::GetProcessorVendor() -> std::string_view
    {
        return gEnvironmentStatics->processorVendor.as_view();
    }
}
