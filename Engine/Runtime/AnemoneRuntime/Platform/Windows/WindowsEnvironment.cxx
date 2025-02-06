#include "AnemoneRuntime/Platform/Environment.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsEnvironment.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsPlatform.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

#include <locale>
#include <iterator>

#include <Windows.h>
#include <Psapi.h>
#include <bcrypt.h>

namespace Anemone
{
    bool Environment::GetEnvironmentVariable(std::string& result, std::string_view name)
    {
        result.clear();
        Interop::string_buffer<wchar_t, 128> wname{};
        Interop::win32_WidenString(wname, name);

        if (Interop::string_buffer<wchar_t, 512> wresult{};
            Interop::win32_GetEnvironmentVariable(wresult, wname))
        {
            Interop::win32_NarrowString(result, wresult.as_view());
            return true;
        }

        return false;
    }

    bool Environment::SetEnvironmentVariable(std::string name, std::string_view value)
    {
        Interop::string_buffer<wchar_t, 128> wname{};
        Interop::win32_WidenString(wname, name);

        Interop::string_buffer<wchar_t, 512> wvalue{};
        Interop::win32_WidenString(wvalue, value);

        return SetEnvironmentVariableW(wname, wvalue) != FALSE;
    }

    bool Environment::RemoveEnvironmentVariable(std::string_view name)
    {
        Interop::string_buffer<wchar_t, 128> wname{};
        Interop::win32_WidenString(wname, name);

        return SetEnvironmentVariableW(wname, nullptr) != FALSE;
    }

    ProcessorProperties const& Environment::GetProcessorProperties()
    {
        return Internal::GWindowsPlatformStatics->Processor;
    }

    std::vector<ProcessorTopology> Environment::GetProcessorTopology()
    {
        AE_PANIC("Not implemented");
        return {};
    }

    static BOOL CALLBACK FillDisplayDevicesInformation(
        HMONITOR handle,
        HDC,
        LPRECT,
        LPARAM context) noexcept
    {
        DisplayMetrics& displayMetrics = *reinterpret_cast<DisplayMetrics*>(context); // NOLINT(performance-no-int-to-ptr)

        MONITORINFOEXW miex{};
        miex.cbSize = sizeof(miex);

        GetMonitorInfoW(handle, &miex);

        Interop::string_buffer<char, 128> name{};
        Interop::win32_NarrowString(name, miex.szDevice);

        AE_ASSERT(!displayMetrics.Displays.empty());

        DisplayInfo& last = displayMetrics.Displays.back();

        if (last.Name == name.as_view())
        {
            last.DisplayRect = Interop::win32_into_Rectangle(miex.rcMonitor);
            last.WorkAreaRect = Interop::win32_into_Rectangle(miex.rcWork);
        }

        return TRUE;
    }

    void Environment::GetDisplayMetrics(DisplayMetrics& metrics)
    {
        metrics.Displays.clear();

        // Get display size.
        metrics.PrimaryDisplaySize = Math::SizeF{
            .Width = static_cast<float>(GetSystemMetrics(SM_CXSCREEN)),
            .Height = static_cast<float>(GetSystemMetrics(SM_CYSCREEN)),
        };

        RECT workArea = {-1, -1, -1, -1};

        if (SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0) == FALSE)
        {
            workArea = {};
        }

        // Get workspace area.
        metrics.PrimaryDisplayWorkArea = Math::RectF{
            .X = static_cast<float>(workArea.left),
            .Y = static_cast<float>(workArea.top),
            .Width = static_cast<float>(workArea.right - workArea.left),
            .Height = static_cast<float>(workArea.bottom - workArea.top),
        };

        // Virtual display rect
        metrics.VirtualDisplayRect = Math::RectF{
            .X = static_cast<float>(GetSystemMetrics(SM_XVIRTUALSCREEN)),
            .Y = static_cast<float>(GetSystemMetrics(SM_YVIRTUALSCREEN)),
            .Width = static_cast<float>(GetSystemMetrics(SM_CXVIRTUALSCREEN)),
            .Height = static_cast<float>(GetSystemMetrics(SM_CYVIRTUALSCREEN)),
        };

        // Get displays
        metrics.Displays.clear();

        DISPLAY_DEVICEW device{};
        device.cb = sizeof(device);

        for (DWORD id = 0; EnumDisplayDevicesW(nullptr, id, &device, 0) != FALSE; ++id)
        {
            if ((device.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) != 0)
            {
                // Mirroring devices are not supported.
                continue;
            }

            if ((device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) == 0)
            {
                // Only devices attached to desktop.
                continue;
            }

            DisplayInfo& display = metrics.Displays.emplace_back();
            Interop::win32_NarrowString(display.Id, device.DeviceID);
            Interop::win32_NarrowString(display.Name, device.DeviceName);
            display.Primary = (device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) != 0;

            DEVMODEW dm{};
            dm.dmSize = sizeof(dm);

            if (EnumDisplaySettingsExW(
                    device.DeviceName,
                    ENUM_CURRENT_SETTINGS,
                    &dm,
                    EDS_RAWMODE))
            {
                if (dm.dmFields & DM_DISPLAYORIENTATION)
                {
                    switch (dm.dmDisplayOrientation)
                    {
                    default:
                        display.Orientation = DisplayOrientation::Unknown;
                        break;

                    case DMDO_DEFAULT:
                        display.Orientation = DisplayOrientation::Landscape;
                        break;

                    case DMDO_90:
                        display.Orientation = DisplayOrientation::Portrait;
                        break;

                    case DMDO_180:
                        display.Orientation = DisplayOrientation::LandscapeFlipped;
                        break;

                    case DMDO_270:
                        display.Orientation = DisplayOrientation::PortraitFlipped;
                        break;
                    }
                }
            }
            else
            {
                display.Orientation = DisplayOrientation::Unknown;
            }

            // Find detailed properties.
            EnumDisplayMonitors(
                nullptr,
                nullptr,
                FillDisplayDevicesInformation,
                reinterpret_cast<LPARAM>(&metrics));
        }
    }

    ColorRef Environment::GetScreenPixel(Math::PointF position, float gamma)
    {
        COLORREF const color = GetPixel(GetDC(HWND_DESKTOP), static_cast<int>(position.X), static_cast<int>(position.Y));

        float const r = static_cast<float>(static_cast<uint8_t>(color)) / 255.0f;
        float const g = static_cast<float>(static_cast<uint8_t>(color >> 8)) / 255.0f;
        float const b = static_cast<float>(static_cast<uint8_t>(color >> 16)) / 255.0f;

        float const exp = std::max(gamma, 1.0F);

        return ColorRef{
            .Red = std::pow(r, exp),
            .Green = std::pow(g, exp),
            .Blue = std::pow(b, exp),
            .Alpha = 1.0f,
        };
    }

    std::string_view Environment::GetSystemVersion()
    {
        return Internal::GWindowsPlatformStatics->SystemVersion;
    }

    std::string_view Environment::GetSystemId()
    {
        return Internal::GWindowsPlatformStatics->SystemId;
    }

    std::string_view Environment::GetSystemName()
    {
        AE_PANIC("Not implemented");
        return {};
    }

    Duration Environment::GetSystemUptime()
    {
        return Duration::FromMilliseconds(static_cast<int64_t>(GetTickCount64()));
    }

    DateTime Environment::GetApplicationStartupTime()
    {
        return Internal::GWindowsPlatformStatics->ApplicationStartupTime;
    }

    MemoryProperties Environment::GetMemoryProperties()
    {
        MEMORYSTATUSEX memoryStatus{};
        memoryStatus.dwLength = sizeof(memoryStatus);

        if (!GlobalMemoryStatusEx(&memoryStatus))
        {
            AE_PANIC("GlobalMemoryStatusEx: {}", GetLastError());
        }

        SYSTEM_INFO systemInfo{};
        GetNativeSystemInfo(&systemInfo);

        return MemoryProperties{
            .TotalPhysicalMemory = memoryStatus.ullTotalPhys,
            .TotalVirtualMemory = memoryStatus.ullTotalVirtual,
            .TotalPageFile = memoryStatus.ullTotalPageFile,
            .AvailablePhysicalMemory = memoryStatus.ullAvailPhys,
            .AvailableVirtualMemory = memoryStatus.ullAvailVirtual,
            .AvailablePageFile = memoryStatus.ullAvailPageFile,
            .SystemAllocationGranularity = systemInfo.dwAllocationGranularity,
            .PageSize = systemInfo.dwPageSize,
            .AddressLimitLow = reinterpret_cast<uintptr_t>(systemInfo.lpMinimumApplicationAddress),
            .AddressLimitHigh = reinterpret_cast<uintptr_t>(systemInfo.lpMaximumApplicationAddress),
        };
    }

    MemoryUsage Environment::GetMemoryUsage()
    {
        MEMORYSTATUSEX memoryStatus{};
        memoryStatus.dwLength = sizeof(memoryStatus);

        if (!GlobalMemoryStatusEx(&memoryStatus))
        {
            AE_PANIC("GlobalMemoryStatusEx: {}", GetLastError());
        }

        PROCESS_MEMORY_COUNTERS memoryCounters{};
        memoryCounters.cb = sizeof(memoryCounters);

        if (!GetProcessMemoryInfo(GetCurrentProcess(), &memoryCounters, sizeof(memoryCounters)))
        {
            AE_PANIC("GetProcessMemoryInfo: {}", GetLastError());
        }

        return MemoryUsage{
            .AvailableVirtual = memoryStatus.ullAvailVirtual,
            .AvailablePhysical = memoryStatus.ullAvailPhys,
            .UsedVirtual = memoryCounters.WorkingSetSize,
            .UsedPhysical = memoryCounters.WorkingSetSize,
            .PeakUsedVirtual = memoryCounters.PeakWorkingSetSize,
            .PeakUsedPhysical = memoryCounters.PeakWorkingSetSize,
        };
    }

    PowerUsage Environment::GetPowerUsage()
    {
        SYSTEM_POWER_STATUS powerStatus{};
        if (!GetSystemPowerStatus(&powerStatus))
        {
            AE_PANIC("GetSystemPowerStatus: {}", GetLastError());
        }

        PowerUsage result{
            .State = PowerState::Unknown,
            .BatteryLevel = -1,
        };

        if (powerStatus.BatteryFlag != BATTERY_FLAG_UNKNOWN)
        {
            if (powerStatus.BatteryFlag & BATTERY_FLAG_NO_BATTERY)
            {
                result.State = PowerState::NoBattery;
            }
            else if (powerStatus.BatteryFlag & BATTERY_FLAG_CHARGING)
            {
                result.State = PowerState::Charging;
            }
            else if (powerStatus.ACLineStatus & AC_LINE_ONLINE)
            {
                result.State = PowerState::Charged;
            }
            else
            {
                result.State = PowerState::Discharging;
            }
        }

        if (powerStatus.BatteryLifePercent != BATTERY_PERCENTAGE_UNKNOWN)
        {
            result.BatteryLevel = std::min<int>(powerStatus.BatteryLifePercent, 100);
        }

        return result;
    }

    ProcessorUsage Environment::GetProcessorUsage()
    {
        FILETIME ftCreation{};
        FILETIME ftExit{};
        FILETIME ftKernel{};
        FILETIME ftUser{};

        if (!GetProcessTimes(GetCurrentProcess(), &ftCreation, &ftExit, &ftKernel, &ftUser))
        {
            AE_PANIC("GetProcessTimes: {}", GetLastError());
        }

        return ProcessorUsage{
            .UserTime = Interop::win32_into_Duration(ftUser),
            .KernelTime = Interop::win32_into_Duration(ftKernel),
        };
    }

    void Environment::Terminate(bool force)
    {
        if (force)
        {
            TerminateProcess(GetCurrentProcess(), 0);
        }
        else
        {
            PostQuitMessage(0);
        }
    }

    std::string_view Environment::GetDeviceUniqueId()
    {
        return Internal::GWindowsPlatformStatics->DeviceId;
    }

    std::string_view Environment::GetDeviceName()
    {
        return Internal::GWindowsPlatformStatics->DeviceName;
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
        return Internal::GWindowsPlatformStatics->ComputerName;
    }

    std::string_view Environment::GetUserName()
    {
        return Internal::GWindowsPlatformStatics->UserName;
    }

    std::string_view Environment::GetExecutablePath()
    {
        return Internal::GWindowsPlatformStatics->ExecutablePath;
    }

    std::string_view Environment::GetStartupPath()
    {
        return Internal::GWindowsPlatformStatics->StartupPath;
    }

    std::string_view Environment::GetHomePath()
    {
        return Internal::GWindowsPlatformStatics->ProfilePath;
    }

    std::string_view Environment::GetDesktopPath()
    {
        return Internal::GWindowsPlatformStatics->DesktopPath;
    }

    std::string_view Environment::GetDocumentsPath()
    {
        return Internal::GWindowsPlatformStatics->DocumentsPath;
    }

    std::string_view Environment::GetDownloadsPath()
    {
        return Internal::GWindowsPlatformStatics->DownloadsPath;
    }

    std::string_view Environment::GetTemporaryPath()
    {
        return Internal::GWindowsPlatformStatics->TemporaryPath;
    }

    DateTime Environment::GetCurrentDateTime()
    {
        FILETIME ft;
        Interop::win32_GetLocalTimeAsFileTime(ft);
        return Interop::win32_into_DateTime(ft);
    }

    DateTime Environment::GetCurrentDateTimeUtc()
    {
        FILETIME ft;
        GetSystemTimePreciseAsFileTime(&ft);

        return Interop::win32_into_DateTime(ft);
    }

    Duration Environment::GetCurrentTimeZoneBias()
    {
        DYNAMIC_TIME_ZONE_INFORMATION dtzi;

        int64_t seconds = 0;

        switch (GetDynamicTimeZoneInformation(&dtzi))
        {
        case TIME_ZONE_ID_INVALID:
            AE_PANIC("Cannot obtain timezone ID: {}", GetLastError());
            break;

        default:
        case TIME_ZONE_ID_UNKNOWN:
            break;

        case TIME_ZONE_ID_STANDARD:
            seconds += static_cast<int64_t>(dtzi.StandardBias) * 60;
            break;

        case TIME_ZONE_ID_DAYLIGHT:
            seconds += static_cast<int64_t>(dtzi.DaylightBias) * 60;
            break;
        }

        seconds += static_cast<int64_t>(dtzi.Bias) * 60;

        return Duration{
            .Seconds = seconds,
            .Nanoseconds = 0,
        };
    }

    void Environment::GetRandom(std::span<std::byte> buffer)
    {
        NTSTATUS status = BCryptGenRandom(
            nullptr,
            reinterpret_cast<PUCHAR>(buffer.data()),
            static_cast<ULONG>(buffer.size()),
            BCRYPT_USE_SYSTEM_PREFERRED_RNG);

        AE_ENSURE(BCRYPT_SUCCESS(status));
    }
}

namespace Anemone
{
    bool WindowsEnvironment::IsConsoleApplication()
    {
        HMODULE hExecutable = GetModuleHandleW(nullptr);

        return Interop::win32_IsConsoleApplication(hExecutable);
    }

    bool WindowsEnvironment::IsConsoleRedirecting()
    {
        return GetStdHandle(STD_OUTPUT_HANDLE) != nullptr;
    }

    bool WindowsEnvironment::IsOnline()
    {
        VARIANT_BOOL connected = VARIANT_FALSE;
        HRESULT const hr = Internal::GWindowsPlatformStatics->NetworkListManager->get_IsConnectedToInternet(&connected);

        if (SUCCEEDED(hr))
        {
            return connected != VARIANT_FALSE;
        }

        // Assume offline.
        return false;
    }
}
