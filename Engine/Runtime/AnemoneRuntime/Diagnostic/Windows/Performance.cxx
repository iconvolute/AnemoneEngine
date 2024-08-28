#include "AnemoneRuntime/Diagnostic/Performance.hxx"
#include "AnemoneRuntime/Diagnostic/Assert.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

#include <Psapi.h>

namespace Anemone::Diagnostic
{
    MemoryProperties GetMemoryProperties()
    {
        MEMORYSTATUSEX msex{.dwLength = sizeof(msex)};

        if (!GlobalMemoryStatusEx(&msex))
        {
            AE_BUGCHECK("GlobalMemoryStatusEx: {}", GetLastError());
        }

        SYSTEM_INFO si{};
        GetNativeSystemInfo(&si);

        return MemoryProperties{
            .TotalPhysicalMemory = msex.ullTotalPhys,
            .TotalVirtualMemory = msex.ullTotalVirtual,
            .TotalPageFile = msex.ullTotalPageFile,
            .AvailablePhysicalMemory = msex.ullAvailPhys,
            .AvailableVirtualMemory = msex.ullAvailVirtual,
            .AvailablePageFile = msex.ullAvailPageFile,
            .SystemAllocationGranularity = si.dwAllocationGranularity,
            .PageSize = si.dwPageSize,
            .AddressLimitLow = reinterpret_cast<uintptr_t>(si.lpMinimumApplicationAddress),
            .AddressLimitHigh = reinterpret_cast<uintptr_t>(si.lpMaximumApplicationAddress),
        };
    }

    PowerUsage GetPowerUsage()
    {
        SYSTEM_POWER_STATUS sps{};
        if (!GetSystemPowerStatus(&sps))
        {
            AE_BUGCHECK("GetSystemPowerStatus: {}", GetLastError());
        }

        PowerUsage result{
            .State = PowerState::Unknown,
            .BatteryLevel = -1,
        };

        if (sps.BatteryFlag != BATTERY_FLAG_UNKNOWN)
        {
            if (sps.BatteryFlag & BATTERY_FLAG_NO_BATTERY)
            {
                result.State = PowerState::NoBattery;
            }
            else if (sps.BatteryFlag & BATTERY_FLAG_CHARGING)
            {
                result.State = PowerState::Charging;
            }
            else if (sps.ACLineStatus & AC_LINE_ONLINE)
            {
                result.State = PowerState::Charged;
            }
            else
            {
                result.State = PowerState::Discharging;
            }
        }

        if (sps.BatteryLifePercent != BATTERY_PERCENTAGE_UNKNOWN)
        {
            result.BatteryLevel = std::min<int>(sps.BatteryLifePercent, 100);
        }

        return result;
    }

    MemoryUsage GetMemoryUsage()
    {
        MEMORYSTATUSEX msex{.dwLength = sizeof(msex)};

        if (!GlobalMemoryStatusEx(&msex))
        {
            AE_BUGCHECK("GlobalMemoryStatusEx: {}", GetLastError());
        }

        PROCESS_MEMORY_COUNTERS pmc{.cb = sizeof(pmc)};
        if (!GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
        {
            AE_BUGCHECK("GetProcessMemoryInfo: {}", GetLastError());
        }

        return MemoryUsage{
            .AvailableVirtual = msex.ullAvailVirtual,
            .AvailablePhysical = msex.ullAvailPhys,
            .UsedVirtual = pmc.WorkingSetSize,
            .UsedPhysical = pmc.WorkingSetSize,
            .PeakUsedVirtual = pmc.PeakWorkingSetSize,
            .PeakUsedPhysical = pmc.PeakWorkingSetSize,
        };
    }

    ProcessorUsageState GetProcessorUsageState()
    {
        FILETIME ftCreation{};
        FILETIME ftExit{};
        FILETIME ftKernel{};
        FILETIME ftUser{};

        if (!GetProcessTimes(GetCurrentProcess(), &ftCreation, &ftExit, &ftKernel, &ftUser))
        {
            AE_BUGCHECK("GetProcessTimes: {}", GetLastError());
        }

        return ProcessorUsageState{
            .UserTime = Platform::win32_into_Duration(ftUser),
            .KernelTime = Platform::win32_into_Duration(ftKernel),
        };
    }
}
