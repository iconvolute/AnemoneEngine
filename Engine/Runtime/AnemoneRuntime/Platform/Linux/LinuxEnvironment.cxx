#include "AnemoneRuntime/Platform/Environment.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"
#include "AnemoneRuntime/Platform/Linux/LinuxPlatform.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <sys/random.h>
#include <sys/mman.h>
#include <time.h>

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

    ProcessorProperties const& Environment::GetProcessorProperties()
    {
        return Internal::GLinuxPlatformStatics->Processor;
    }

    std::vector<ProcessorTopology> Environment::GetProcessorTopology()
    {
        AE_PANIC("Not implemented");
        return {};
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
        return Internal::GLinuxPlatformStatics->SystemVersion;
    }

    std::string_view Environment::GetSystemId()
    {
        return Internal::GLinuxPlatformStatics->SystemId;
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
        return Internal::GLinuxPlatformStatics->ApplicationStartupTime;
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
        return Internal::GLinuxPlatformStatics->ComputerName;
    }

    std::string_view Environment::GetUserName()
    {
        return Internal::GLinuxPlatformStatics->UserName;
    }

    std::string_view Environment::GetExecutablePath()
    {
        return Internal::GLinuxPlatformStatics->ExecutablePath;
    }

    std::string_view Environment::GetStartupPath()
    {
        return Internal::GLinuxPlatformStatics->StartupPath;
    }

    std::string_view Environment::GetHomePath()
    {
        return Internal::GLinuxPlatformStatics->ProfilePath;
    }

    std::string_view Environment::GetDesktopPath()
    {
        return Internal::GLinuxPlatformStatics->DesktopPath;
    }

    std::string_view Environment::GetDocumentsPath()
    {
        return Internal::GLinuxPlatformStatics->DocumentsPath;
    }

    std::string_view Environment::GetDownloadsPath()
    {
        return Internal::GLinuxPlatformStatics->DownloadsPath;
    }

    std::string_view Environment::GetTemporaryPath()
    {
        return Internal::GLinuxPlatformStatics->TemporaryPath;
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
        Interop::string_buffer<char_t, 512> sUrl{url};

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
}
