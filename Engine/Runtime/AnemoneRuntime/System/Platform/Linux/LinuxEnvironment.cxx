#include "AnemoneRuntime/System/Platform/Linux/LinuxEnvironment.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"
#include "AnemoneRuntime/Interop/Linux/Headers.hxx"
#include "AnemoneRuntime/Interop/Linux/DateTime.hxx"
#include "AnemoneRuntime/Interop/Linux/FileSystem.hxx"
#include "AnemoneRuntime/Interop/StringBuffer.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Platform/FilePath.hxx"

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
    namespace
    {
        UninitializedObject<LinuxEnvironment> gLinuxEnvironment{};
        pthread_mutex_t gLinuxEnvironmentLock = PTHREAD_MUTEX_INITIALIZER;
    }

    void LinuxEnvironment::Initialize()
    {
        gLinuxEnvironment.Create();

        gLinuxEnvironment->startupTime = DateTime::Now();

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
                gLinuxEnvironment->systemVersion = fmt::format("{}-{}-{}", name.sysname, name.release, name.version);
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
                gLinuxEnvironment->systemId = *parsed;
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
                gLinuxEnvironment->startupPath.assign(procpath.data());
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

            gLinuxEnvironment->executablePath.assign(execpath.data(), static_cast<size_t>(length));
        }

        // Computer name
        {
            std::array<char, HOST_NAME_MAX> hostname{};
            struct utsname name = {};

            if (uname(&name) == 0)
            {
                gLinuxEnvironment->computerName.assign(name.nodename);
            }
            else if (gethostname(hostname.data(), hostname.size()) == 0)
            {
                gLinuxEnvironment->computerName.assign(hostname.data());
            }
            else
            {
                gLinuxEnvironment->computerName.assign("Linux Machine");
            }
        }

        // User name
        {
            uid_t uid = geteuid();
            struct passwd* pw = getpwuid(uid);

            if (pw != nullptr)
            {
                gLinuxEnvironment->userName = pw->pw_name;
            }
            else
            {
                gLinuxEnvironment->userName = "Unknown";
            }
        }

        // Profile path
        {
#if !ANEMONE_PLATFORM_ANDROID
            const char* value = secure_getenv("HOME");

            if (value != nullptr)
            {
                gLinuxEnvironment->profilePath.assign(value);
            }
            else
            {
                struct passwd* userinfo = getpwuid(geteuid());

                if (userinfo != nullptr && userinfo->pw_dir != nullptr)
                {
                    gLinuxEnvironment->profilePath.assign(userinfo->pw_dir);
                }
            }
#endif
        }

        // UserDesktopPath
        {
            gLinuxEnvironment->desktopPath = gLinuxEnvironment->profilePath;
            FilePath::PushFragment(gLinuxEnvironment->desktopPath, "Desktop");
        }

        // UserDocumentsPath
        {
            gLinuxEnvironment->documentsPath = gLinuxEnvironment->profilePath;
            FilePath::PushFragment(gLinuxEnvironment->documentsPath, "Documents");
        }

        // UserDownloadsPath
        {
            gLinuxEnvironment->downloadsPath = gLinuxEnvironment->profilePath;
            FilePath::PushFragment(gLinuxEnvironment->downloadsPath, "Downloads");
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

            gLinuxEnvironment->temporaryPath.assign(value);
#endif
        }
    }

    void LinuxEnvironment::Finalize()
    {
        gLinuxEnvironment.Destroy();
    }

    LinuxEnvironment& LinuxEnvironment::Get()
    {
        return *gLinuxEnvironment;
    }
}

namespace Anemone
{
    void Environment::Initialize()
    {
        LinuxEnvironment::Initialize();
    }

    void Environment::Finalize()
    {
        LinuxEnvironment::Finalize();
    }

    auto Environment::GetEnvironmentVariable(std::string& result, std::string_view name) -> bool
    {
        pthread_mutex_lock(&gLinuxEnvironmentLock);

        char const* variable = secure_getenv(std::string{name}.c_str());

        bool const success = variable != nullptr;

        if (success)
        {
            result = variable;
        }

        pthread_mutex_unlock(&gLinuxEnvironmentLock);

        return success;
    }

    auto Environment::SetEnvironmentVariable(std::string_view name, std::string_view value) -> bool
    {
        pthread_mutex_lock(&gLinuxEnvironmentLock);

        bool const result = setenv(std::string{name}.c_str(), std::string{value}.c_str(), 1) == 0;

        pthread_mutex_unlock(&gLinuxEnvironmentLock);

        return result;
    }

    auto Environment::RemoveEnvironmentVariable(std::string_view name) -> bool
    {
        pthread_mutex_lock(&gLinuxEnvironmentLock);

        bool const result = unsetenv(std::string{name}.c_str()) == 0;

        pthread_mutex_unlock(&gLinuxEnvironmentLock);

        return result;
    }

    auto Environment::GetSystemVersion() -> std::string_view
    {
        return gLinuxEnvironment->systemVersion;
    }

    auto Environment::GetSystemId() -> Uuid
    {
        return gLinuxEnvironment->systemId;
    }

    auto Environment::GetSystemName() -> std::string_view
    {
        return gLinuxEnvironment->systemName;
    }

    auto Environment::GetSystemUptime() -> Duration
    {
        struct sysinfo info = {0};
        sysinfo(&info);

        return Duration::FromSeconds(info.uptime);
    }

    auto Environment::GetApplicationStartupTime() -> DateTime
    {
        return gLinuxEnvironment->startupTime;
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

    auto Environment::GetDeviceUniqueId() -> std::string_view
    {
        return gLinuxEnvironment->deviceId;
    }

    auto Environment::GetDeviceName() -> std::string_view
    {
        return gLinuxEnvironment->deviceName;
    }

    auto Environment::GetDeviceModel() -> std::string_view
    {
        return gLinuxEnvironment->deviceModel;
    }

    auto Environment::GetDeviceType() -> DeviceType
    {
        return gLinuxEnvironment->deviceType;
    }

    auto Environment::GetDeviceProperties() -> DeviceProperties
    {
        return gLinuxEnvironment->deviceProperties;
    }

    auto Environment::GetComputerName() -> std::string_view
    {
        return gLinuxEnvironment->computerName;
    }

    auto Environment::GetUserName() -> std::string_view
    {
        return gLinuxEnvironment->userName;
    }

    auto Environment::GetExecutablePath() -> std::string_view
    {
        return gLinuxEnvironment->executablePath;
    }

    auto Environment::GetStartupPath() -> std::string_view
    {
        return gLinuxEnvironment->startupPath;
    }

    auto Environment::GetHomePath() -> std::string_view
    {
        return gLinuxEnvironment->profilePath;
    }

    auto Environment::GetDesktopPath() -> std::string_view
    {
        return gLinuxEnvironment->desktopPath;
    }

    auto Environment::GetDocumentsPath() -> std::string_view
    {
        return gLinuxEnvironment->documentsPath;
    }

    auto Environment::GetDownloadsPath() -> std::string_view
    {
        return gLinuxEnvironment->downloadsPath;
    }

    auto Environment::GetTemporaryPath() -> std::string_view
    {
        return gLinuxEnvironment->temporaryPath;
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
