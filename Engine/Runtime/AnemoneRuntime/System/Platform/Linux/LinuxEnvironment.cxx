#include "AnemoneRuntime/System/Environment.hxx"
#include "AnemoneRuntime/Interop/Linux/Headers.hxx"
#include "AnemoneRuntime/Interop/Linux/DateTime.hxx"
#include "AnemoneRuntime/Interop/Linux/FileSystem.hxx"
#include "AnemoneRuntime/Interop/StringBuffer.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Platform/FilePath.hxx"
#include "AnemoneRuntime/Diagnostics/Debugger.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

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

namespace Anemone::Internal::Linux
{
    struct EnvironmentStatics final
    {
        DateTime m_StartupTime;

        std::string m_SystemVersion;
        Uuid m_SystemId;
        std::string m_SystemName;
        std::string m_DeviceId;
        std::string m_DeviceName;
        std::string m_DeviceModel;
        std::string m_DeviceManufacturer;
        std::string m_DeviceVersion;
        DeviceType m_DeviceType;
        DeviceProperties m_DeviceProperties;
        std::string m_ComputerName;
        std::string m_UserName;
        std::string m_ExecutablePath;
        std::string m_StartupPath;
        std::string m_ProfilePath;
        std::string m_DesktopPath;
        std::string m_DocumentsPath;
        std::string m_DownloadsPath;
        std::string m_TemporaryPath;
    };

    static UninitializedObject<EnvironmentStatics> GEnvironmentStatics{};

    static void InitializeStatics(EnvironmentStatics& statics)
    {
        statics.m_StartupTime = Environment::GetCurrentDateTime();

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
                statics.m_SystemVersion = fmt::format("{}-{}-{}", name.sysname, name.release, name.version);
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
                statics.m_SystemId = *parsed;
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
                statics.m_StartupPath.assign(procpath.data());
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

            statics.m_ExecutablePath.assign(execpath.data(), static_cast<size_t>(length));
        }

        // Computer name
        {
            std::array<char, HOST_NAME_MAX> hostname{};
            struct utsname name = {};

            if (uname(&name) == 0)
            {
                statics.m_ComputerName.assign(name.nodename);
            }
            else if (gethostname(hostname.data(), hostname.size()) == 0)
            {
                statics.m_ComputerName.assign(hostname.data());
            }
            else
            {
                statics.m_ComputerName.assign("Linux Machine");
            }
        }

        // User name
        {
            uid_t uid = geteuid();
            struct passwd* pw = getpwuid(uid);

            if (pw != nullptr)
            {
                statics.m_UserName = pw->pw_name;
            }
            else
            {
                statics.m_UserName = "Unknown";
            }
        }

        // Profile path
        {
#if !ANEMONE_PLATFORM_ANDROID
            const char* value = secure_getenv("HOME");

            if (value != nullptr)
            {
                statics.m_ProfilePath.assign(value);
            }
            else
            {
                struct passwd* userinfo = getpwuid(geteuid());

                if (userinfo != nullptr && userinfo->pw_dir != nullptr)
                {
                    statics.m_ProfilePath.assign(userinfo->pw_dir);
                }
            }
#endif
        }

        // UserDesktopPath
        {
            statics.m_DesktopPath = statics.m_ProfilePath;
            FilePath::PushFragment(statics.m_DesktopPath, "Desktop");
        }

        // UserDocumentsPath
        {
            statics.m_DocumentsPath = statics.m_ProfilePath;
            FilePath::PushFragment(statics.m_DocumentsPath, "Documents");
        }

        // UserDownloadsPath
        {
            statics.m_DownloadsPath = statics.m_ProfilePath;
            FilePath::PushFragment(statics.m_DownloadsPath, "Downloads");
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

            statics.m_TemporaryPath.assign(value);
#endif
        }
    }

    static void VerifyRequirements()
    {

#if ANEMONE_ARCHITECTURE_X64

        int level0[4]{-1};
        __cpuid(0, level0[0], level0[1], level0[2], level0[3]);

        if (level0[0] < 1)
        {
            Diagnostics::ReportApplicationStop("Application stop: CPU Features not available\n");
        }

        int level1[4]{-1};
        __cpuid(1, level1[0], level1[1], level1[2], level1[3]);

        if (!(level1[2] & bit_CMPXCHG16B))
        {
            Diagnostics::ReportApplicationStop("Application stop: CMPXCHG16B is not supported\n");
        }

        if (!(level1[2] & bit_POPCNT))
        {
            Diagnostics::ReportApplicationStop("Application stop: POPCNT is not supported\n");
        }

#if ANEMONE_FEATURE_AVX
        if (!(level1[2] & bit_OSXSAVE))
        {
            Diagnostics::ReportApplicationStop("Application stop: OSXSAVE is not supported\n");
        }

        if (!(level1[2] & bit_SSE3))
        {
            Diagnostics::ReportApplicationStop("Application stop: SSE3 is not supported\n");
        }

        if (!(level1[2] & bit_SSE4_1))
        {
            Diagnostics::ReportApplicationStop("Application stop: SSE4_1 is not supported\n");
        }

        if (!(level1[2] & bit_AVX))
        {
            Diagnostics::ReportApplicationStop("Application stop: AVX is not supported\n");
        }

#if ANEMONE_FEATURE_AVX2

        if (!(level1[2] & bit_F16C))
        {
            Diagnostics::ReportApplicationStop("Application stop: F16C is not supported\n");
        }

        if (!(level1[2] & bit_FMA))
        {
            Diagnostics::ReportApplicationStop("Application stop: FMA is not supported\n");
        }

        int level7_0[4]{-1};
        __cpuid_count(7, 0, level7_0[0], level7_0[1], level7_0[2], level7_0[3]);

        if (!(level7_0[1] & bit_AVX2))
        {
            Diagnostics::ReportApplicationStop("Application stop: AVX2 is not supported\n");
        }

#endif

#endif

#endif


#if ANEMONE_ARCHITECTURE_ARM64

        auto const cap = getauxval(AT_HWCAP);

        if ((cap & HWCAP_FP) != HWCAP_FP)
        {
            Diagnostics::ReportApplicationStop("Application stop: NEON not supported\n");
        }

        if ((cap & HWCAP_ASIMD) != HWCAP_ASIMD)
        {
            Diagnostics::ReportApplicationStop("Application stop: ASIMD not supported\n");
        }

        if ((cap & HWCAP_ATOMICS) != HWCAP_ATOMICS)
        {
            Diagnostics::ReportApplicationStop("Application stop: ATOMICS not supported\n");
        }

#endif
    }
}

namespace Anemone::Internal
{
    extern void InitializeEnvironment()
    {
        Linux::GEnvironmentStatics.Create();
        Linux::InitializeStatics(*Linux::GEnvironmentStatics);

        Linux::VerifyRequirements();
    }

    extern void FinalizeEnvironment()
    {
        Linux::GEnvironmentStatics.Destroy();
    }
}

namespace Anemone
{
    static pthread_mutex_t GEnvironmentLock = PTHREAD_MUTEX_INITIALIZER;

    bool Environment::GetEnvironmentVariable(std::string& result, std::string_view name)
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

    bool Environment::SetEnvironmentVariable(std::string name, std::string_view value)
    {
        pthread_mutex_lock(&GEnvironmentLock);

        bool const result = setenv(std::string{name}.c_str(), std::string{value}.c_str(), 1) == 0;

        pthread_mutex_unlock(&GEnvironmentLock);

        return result;
    }

    bool Environment::RemoveEnvironmentVariable(std::string_view name)
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

    ColorRef Environment::GetScreenPixel(Math::PointF position, float gamma)
    {
        (void)position;
        (void)gamma;
        AE_PANIC("Not implemented");
        return {};
    }

    std::string_view Environment::GetSystemVersion()
    {
        return Internal::Linux::GEnvironmentStatics->m_SystemVersion;
    }

    Uuid Environment::GetSystemId()
    {
        return Internal::Linux::GEnvironmentStatics->m_SystemId;
    }

    std::string_view Environment::GetSystemName()
    {
        AE_PANIC("Not implemented");
        return {};
    }

    Duration Environment::GetSystemUptime()
    {
        struct sysinfo info = {0};
        sysinfo(&info);

        return Duration::FromSeconds(info.uptime);
    }

    DateTime Environment::GetApplicationStartupTime()
    {
        return Internal::Linux::GEnvironmentStatics->m_StartupTime;
    }

    MemoryProperties Environment::GetMemoryProperties()
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

    MemoryUsage Environment::GetMemoryUsage()
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

    PowerUsage Environment::GetPowerUsage()
    {
        AE_PANIC("Not implemented");
        return {};
    }

    ProcessorUsage Environment::GetProcessorUsage()
    {
        struct rusage usage = {0};
        getrusage(RUSAGE_SELF, &usage);

        return ProcessorUsage{
            .UserTime = Interop::Linux::ToDuration(usage.ru_utime),
            .KernelTime = Interop::Linux::ToDuration(usage.ru_stime),
        };
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

    std::string_view Environment::GetDeviceUniqueId()
    {
        return Internal::Linux::GEnvironmentStatics->m_DeviceId;
    }

    std::string_view Environment::GetDeviceName()
    {
        return Internal::Linux::GEnvironmentStatics->m_DeviceName;
    }

    std::string Environment::GetDeviceModel()
    {
        return Internal::Linux::GEnvironmentStatics->m_DeviceModel;
    }

    DeviceType Environment::GetDeviceType()
    {
        return Internal::Linux::GEnvironmentStatics->m_DeviceType;
    }

    DeviceProperties Environment::GetDeviceProperties()
    {
        return Internal::Linux::GEnvironmentStatics->m_DeviceProperties;
    }

    std::string_view Environment::GetComputerName()
    {
        return Internal::Linux::GEnvironmentStatics->m_ComputerName;
    }

    std::string_view Environment::GetUserName()
    {
        return Internal::Linux::GEnvironmentStatics->m_UserName;
    }

    std::string_view Environment::GetExecutablePath()
    {
        return Internal::Linux::GEnvironmentStatics->m_ExecutablePath;
    }

    std::string_view Environment::GetStartupPath()
    {
        return Internal::Linux::GEnvironmentStatics->m_StartupPath;
    }

    std::string_view Environment::GetHomePath()
    {
        return Internal::Linux::GEnvironmentStatics->m_ProfilePath;
    }

    std::string_view Environment::GetDesktopPath()
    {
        return Internal::Linux::GEnvironmentStatics->m_DesktopPath;
    }

    std::string_view Environment::GetDocumentsPath()
    {
        return Internal::Linux::GEnvironmentStatics->m_DocumentsPath;
    }

    std::string_view Environment::GetDownloadsPath()
    {
        return Internal::Linux::GEnvironmentStatics->m_DownloadsPath;
    }

    std::string_view Environment::GetTemporaryPath()
    {
        return Internal::Linux::GEnvironmentStatics->m_TemporaryPath;
    }

    DateTime Environment::GetCurrentDateTime()
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

    DateTime Environment::GetCurrentDateTimeUtc()
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

    Duration Environment::GetCurrentTimeZoneBias()
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

    Instant Environment::GetCurrentInstant()
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
}
