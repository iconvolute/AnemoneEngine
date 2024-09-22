#include "AnemoneRuntime/Diagnostic/Performance.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"

#include <array>
#include <sys/sysinfo.h>
#include <sys/resource.h>

namespace Anemone::Diagnostic
{
    MemoryProperties GetMemoryProperties()
    {
        // Get memory properties
        struct sysinfo info{};

        sysinfo(&info);
        // Get address space limits
        struct rlimit64 limits{};

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

    PowerUsage GetPowerUsage()
    {
        return {};
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

    ProcessorUsageState GetProcessorUsageState()
    {
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);

        return ProcessorUsageState{
            .UserTime = Platform::posix_into_Duration(usage.ru_utime),
            .KernelTime = Platform::posix_into_Duration(usage.ru_stime),
        };
    }
}
