#include "AnemoneRuntime/Platform/Environment.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"
#include "AnemoneRuntime/Platform/Linux/LinuxPlatform.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Platform/FilePath.hxx"
#include "AnemoneRuntime/Platform/Debugger.hxx"
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

namespace Anemone
{
    struct LinuxEnvironmentStatics final
    {
        LinuxEnvironmentStatics();
        ~LinuxEnvironmentStatics();

        static void VerifyRequirements();

    public:
        DateTime m_StartupTime;

        std::string m_SystemVersion;
        std::string m_SystemId;
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

    LinuxEnvironmentStatics::LinuxEnvironmentStatics()
    {
        VerifyRequirements();

        this->m_StartupTime = Environment::GetCurrentDateTime();

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
                this->m_SystemVersion = fmt::format("{}-{}-{}", name.sysname, name.release, name.version);
            }
        }

        // System ID
        {
            std::array<char, 128> buffer{};

            if (int fd = open("/etc/machine-id", O_RDONLY); fd != -1)
            {
                ssize_t processed = read(fd, buffer.data(), buffer.size() - 1);

                if (processed > 0)
                {
                    buffer[32] = 0;
                    this->m_SystemId = buffer.data();
                }

                close(fd);
            }
            else if (int fd = open("/var/lib/dbus/machine-id", O_RDONLY); fd != -1)
            {
                ssize_t processed = read(fd, buffer.data(), buffer.size() - 1);

                if (processed > 0)
                {
                    buffer[32] = 0;
                    this->m_SystemId = buffer.data();
                }

                close(fd);
            }
            else
            {
                this->m_SystemId = "Unknown";
            }
        }

        // Startup path
        {
            std::array<char, PATH_MAX> procpath{};

            if (getcwd(procpath.data(), procpath.size()) != nullptr)
            {
                this->m_StartupPath.assign(procpath.data());
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

            this->m_ExecutablePath.assign(execpath.data(), static_cast<size_t>(length));
        }

        // Computer name
        {
            std::array<char, HOST_NAME_MAX> hostname{};
            struct utsname name = {};

            if (uname(&name) == 0)
            {
                this->m_ComputerName.assign(name.nodename);
            }
            else if (gethostname(hostname.data(), hostname.size()) == 0)
            {
                this->m_ComputerName.assign(hostname.data());
            }
            else
            {
                this->m_ComputerName.assign("Linux Machine");
            }
        }

        // User name
        {
            uid_t uid = geteuid();
            struct passwd* pw = getpwuid(uid);

            if (pw != nullptr)
            {
                this->m_UserName = pw->pw_name;
            }
            else
            {
                this->m_UserName = "Unknown";
            }
        }

        // Profile path
        {
#if !ANEMONE_PLATFORM_ANDROID
            const char* value = secure_getenv("HOME");

            if (value != nullptr)
            {
                this->m_ProfilePath.assign(value);
            }
            else
            {
                struct passwd* userinfo = getpwuid(geteuid());

                if (userinfo != nullptr && userinfo->pw_dir != nullptr)
                {
                    this->m_ProfilePath.assign(userinfo->pw_dir);
                }
            }
#endif
        }

        // UserDesktopPath
        {
            this->m_DesktopPath = this->m_ProfilePath;
            FilePath::PushFragment(this->m_DesktopPath, "Desktop");
        }

        // UserDocumentsPath
        {
            this->m_DocumentsPath = this->m_ProfilePath;
            FilePath::PushFragment(this->m_DocumentsPath, "Documents");
        }

        // UserDownloadsPath
        {
            this->m_DownloadsPath = this->m_ProfilePath;
            FilePath::PushFragment(this->m_DownloadsPath, "Downloads");
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

            this->m_TemporaryPath.assign(value);
#endif
        }

        // Startup time
        {
            this->m_StartupTime = DateTime::Now();
        }
    }

    LinuxEnvironmentStatics::~LinuxEnvironmentStatics()
    {
    }

    void LinuxEnvironmentStatics::VerifyRequirements()
    {
#if ANEMONE_ARCHITECTURE_X64

        int level0[4]{-1};
        __cpuid(0, level0[0], level0[1], level0[2], level0[3]);

        if (level0[0] < 1)
        {
            Debugger::ReportApplicationStop("Application stop: CPU Features not available\n");
        }

        int level1[4]{-1};
        __cpuid(1, level1[0], level1[1], level1[2], level1[3]);

        if (!(level1[2] & bit_CMPXCHG16B))
        {
            Debugger::ReportApplicationStop("Application stop: CMPXCHG16B is not supported\n");
        }

        if (!(level1[2] & bit_POPCNT))
        {
            Debugger::ReportApplicationStop("Application stop: POPCNT is not supported\n");
        }

#if ANEMONE_FEATURE_AVX
        if (!(level1[2] & bit_OSXSAVE))
        {
            Debugger::ReportApplicationStop("Application stop: OSXSAVE is not supported\n");
        }

        if (!(level1[2] & bit_SSE3))
        {
            Debugger::ReportApplicationStop("Application stop: SSE3 is not supported\n");
        }

        if (!(level1[2] & bit_SSE4_1))
        {
            Debugger::ReportApplicationStop("Application stop: SSE4_1 is not supported\n");
        }

        if (!(level1[2] & bit_AVX))
        {
            Debugger::ReportApplicationStop("Application stop: AVX is not supported\n");
        }

#if ANEMONE_FEATURE_AVX2

        if (!(level1[2] & bit_F16C))
        {
            Debugger::ReportApplicationStop("Application stop: F16C is not supported\n");
        }

        if (!(level1[2] & bit_FMA))
        {
            Debugger::ReportApplicationStop("Application stop: FMA is not supported\n");
        }

        int level7_0[4]{-1};
        __cpuid_count(7, 0, level7_0[0], level7_0[1], level7_0[2], level7_0[3]);

        if (!(level7_0[1] & bit_AVX2))
        {
            Debugger::ReportApplicationStop("Application stop: AVX2 is not supported\n");
        }

#endif

#endif

#endif


#if ANEMONE_ARCHITECTURE_ARM64

        auto const cap = getauxval(AT_HWCAP);

        if ((cap & HWCAP_FP) != HWCAP_FP)
        {
            Debugger::ReportApplicationStop("Application stop: NEON not supported\n");
        }

        if ((cap & HWCAP_ASIMD) != HWCAP_ASIMD)
        {
            Debugger::ReportApplicationStop("Application stop: ASIMD not supported\n");
        }

        if ((cap & HWCAP_ATOMICS) != HWCAP_ATOMICS)
        {
            Debugger::ReportApplicationStop("Application stop: ATOMICS not supported\n");
        }

#endif
    }

    static UninitializedObject<LinuxEnvironmentStatics> GLinuxEnvironmentStatics{};

    void Environment::Initialize()
    {
        GLinuxEnvironmentStatics.Create();
    }

    void Environment::Finalize()
    {
        GLinuxEnvironmentStatics.Destroy();
    }

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
        return GLinuxEnvironmentStatics->m_SystemVersion;
    }

    std::string_view Environment::GetSystemId()
    {
        return GLinuxEnvironmentStatics->m_SystemId;
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
        return GLinuxEnvironmentStatics->m_StartupTime;
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
            .UserTime = Interop::posix_into_Duration(usage.ru_utime),
            .KernelTime = Interop::posix_into_Duration(usage.ru_stime),
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
        AE_PANIC("Not implemented");
        return {};
    }

    std::string_view Environment::GetDeviceName()
    {
        AE_PANIC("Not implemented");
        return {};
    }

    std::string Environment::GetDeviceModel()
    {
        AE_PANIC("Not implemented");
        return {};
    }

    DeviceType Environment::GetDeviceType()
    {
        AE_PANIC("Not implemented");
        return {};
    }

    DeviceProperties Environment::GetDeviceProperties()
    {
        AE_PANIC("Not implemented");
        return {};
    }

    std::string_view Environment::GetComputerName()
    {
        return GLinuxEnvironmentStatics->m_ComputerName;
    }

    std::string_view Environment::GetUserName()
    {
        return GLinuxEnvironmentStatics->m_UserName;
    }

    std::string_view Environment::GetExecutablePath()
    {
        return GLinuxEnvironmentStatics->m_ExecutablePath;
    }

    std::string_view Environment::GetStartupPath()
    {
        return GLinuxEnvironmentStatics->m_StartupPath;
    }

    std::string_view Environment::GetHomePath()
    {
        return GLinuxEnvironmentStatics->m_ProfilePath;
    }

    std::string_view Environment::GetDesktopPath()
    {
        return GLinuxEnvironmentStatics->m_DesktopPath;
    }

    std::string_view Environment::GetDocumentsPath()
    {
        return GLinuxEnvironmentStatics->m_DocumentsPath;
    }

    std::string_view Environment::GetDownloadsPath()
    {
        return GLinuxEnvironmentStatics->m_DownloadsPath;
    }

    std::string_view Environment::GetTemporaryPath()
    {
        return GLinuxEnvironmentStatics->m_TemporaryPath;
    }

    DateTime Environment::GetCurrentDateTime()
    {
        struct timespec ts;

        [[maybe_unused]] int const result = clock_gettime(CLOCK_REALTIME, &ts);
        AE_ASSERT(result == 0);

        int64_t const bias = GetCurrentTimeZoneBias().Seconds;

        return DateTime{
            .Inner = {
                .Seconds = ts.tv_sec - bias + Interop::posix_DateAdjustOffset,
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
                .Seconds = ts.tv_sec + Interop::posix_DateAdjustOffset,
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
