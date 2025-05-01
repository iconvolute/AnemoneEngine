#include "AnemoneRuntime/System/Environment.hxx"
#include "AnemoneRuntime/Interop/StringBuffer.hxx"
#include "AnemoneRuntime/Interop/Windows/DateTime.hxx"
#include "AnemoneRuntime/Interop/Windows/Environment.hxx"
#include "AnemoneRuntime/Interop/Windows/Process.hxx"
#include "AnemoneRuntime/Interop/Windows/Registry.hxx"
#include "AnemoneRuntime/Interop/Windows/UI.hxx"
#include "AnemoneRuntime/Interop/Windows/FileSystem.hxx"
#include "anemoneRuntime/Interop/Windows/Text.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"
#include "AnemoneRuntime/Diagnostics/Debugger.hxx"
#include "AnemoneRuntime/Platform/FilePath.hxx"
#include "AnemoneRuntime/Diagnostics/Platform/Windows/WindowsError.hxx"

#include <locale>

#include <Windows.h>
#include <Psapi.h>
#include <objbase.h>
#include <bcrypt.h>
#include <wrl/client.h>
#include <netlistmgr.h>
#include <shellapi.h>

namespace Anemone::Internal::Windows
{
    static void VerifyRequirements()
    {
        if (Interop::Windows::IsProcessEmulated())
        {
            // VERIFY: AVX is not supported in WoA process emulation.
            Diagnostics::ReportApplicationStop("Emulated process not supported.");
        }

        if (not IsProcessorFeaturePresent(PF_COMPARE_EXCHANGE_DOUBLE))
        {
            Diagnostics::ReportApplicationStop("64-bit compare-exchange not supported.");
        }

#if ANEMONE_ARCHITECTURE_X64
        if (not IsProcessorFeaturePresent(PF_COMPARE_EXCHANGE128))
        {
            Diagnostics::ReportApplicationStop("128-bit compare-exchange not supported");
        }
#endif

#if ANEMONE_FEATURE_AVX

        if (not IsProcessorFeaturePresent(PF_XSAVE_ENABLED))
        {
            Diagnostics::ReportApplicationStop("XSAVE not enabled.");
        }

        if (not IsProcessorFeaturePresent(PF_AVX_INSTRUCTIONS_AVAILABLE))
        {
            Diagnostics::ReportApplicationStop("AVX not supported.");
        }
#endif

#if ANEMONE_FEATURE_AVX2
        if (not IsProcessorFeaturePresent(PF_AVX2_INSTRUCTIONS_AVAILABLE))
        {
            Diagnostics::ReportApplicationStop("AVX2 not supported.");
        }
#endif

#if ANEMONE_FEATURE_NEON
        if (not IsProcessorFeaturePresent(PF_ARM_NEON_INSTRUCTIONS_AVAILABLE))
        {
            Diagnostics::ReportApplicationStop("NEON not supported.");
        }
#endif

        // Verify Windows version
        OSVERSIONINFOEXW osie{
            .dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW),
            .dwMajorVersion = static_cast<DWORD>(10),
        };

        ULONGLONG conditionMask{};
        conditionMask = VerSetConditionMask(
            conditionMask,
            VER_MAJORVERSION,
            VER_GREATER_EQUAL);
        conditionMask = VerSetConditionMask(
            conditionMask,
            VER_MINORVERSION,
            VER_GREATER_EQUAL);

        if (not VerifyVersionInfoW(
                &osie,
                VER_MAJORVERSION | VER_MINORVERSION,
                conditionMask))
        {
            Diagnostics::ReportApplicationStop("Windows 10 or newer required");
        }
    }

    struct EnvironmentStatics final
    {
        DateTime m_StartupTime;
        Uuid m_SystemId;
        DeviceType m_DeviceType;
        DeviceProperties m_DeviceProperties;
        
        std::string m_SystemVersion;
        std::string m_SystemName;
        std::string m_DeviceId;
        std::string m_DeviceName;
        std::string m_DeviceModel;
        std::string m_DeviceManufacturer;
        std::string m_DeviceVersion;
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
        //
        // Capture application startup time.
        //

        statics.m_StartupTime = Environment::GetCurrentDateTime();


        //
        // Determine environment properties.
        //

        Interop::string_buffer<wchar_t, 512> buffer{};

        if (auto key = Interop::Windows::RegistryKey::Open(HKEY_LOCAL_MACHINE, LR"(SOFTWARE\Microsoft\Windows NT\CurrentVersion)"))
        {
            DWORD major{};
            (void)key.Read(L"CurrentMajorVersionNumber", major);

            DWORD minor{};
            (void)key.Read(L"CurrentMinorVersionNumber", minor);

            Interop::string_buffer<char, 64> build{};
            (void)key.ReadString("BuildLabEx", build);

            statics.m_SystemVersion = fmt::format(
                "{}.{}.{}",
                major,
                minor,
                build.as_view());

            Interop::string_buffer<char, 64> productName{};
            (void)key.ReadString("ProductName", productName);
            statics.m_SystemName = productName.as_view();
        }

        // Determine system ID
        if (auto key = Interop::Windows::RegistryKey::Open(HKEY_LOCAL_MACHINE, LR"(SOFTWARE\Microsoft\Cryptography)"))
        {
            if (key.ReadString(L"MachineGuid", buffer))
            {
                Interop::string_buffer<char, 64> utf8Buffer;
                Interop::Windows::NarrowString(utf8Buffer, buffer.as_view());

                statics.m_SystemId = UuidParser::Parse(utf8Buffer.as_view()).value_or(Uuid{});
            }
        }

        // Executable path
        Interop::Windows::QueryFullProcessImageName(buffer);
        Interop::Windows::NarrowString(statics.m_ExecutablePath, buffer.as_view());
        FilePath::NormalizeDirectorySeparators(statics.m_ExecutablePath);

        // Startup path
        Interop::Windows::GetCurrentDirectory(buffer);
        Interop::Windows::NarrowString(statics.m_StartupPath, buffer.as_view());
        FilePath::NormalizeDirectorySeparators(statics.m_StartupPath);

        // Computer name
        Interop::Windows::GetComputerName(buffer);
        Interop::Windows::NarrowString(statics.m_ComputerName, buffer.as_view());

        // User name
        Interop::Windows::GetUserName(buffer);
        Interop::Windows::NarrowString(statics.m_UserName, buffer.as_view());

        // Profile path
        Interop::Windows::GetKnownFolderPath(FOLDERID_Profile, [&](std::wstring_view value)
        {
            Interop::Windows::NarrowString(statics.m_ProfilePath, value);
        });
        FilePath::NormalizeDirectorySeparators(statics.m_ProfilePath);

        // Desktop path
        Interop::Windows::GetKnownFolderPath(FOLDERID_Desktop, [&](std::wstring_view value)
        {
            Interop::Windows::NarrowString(statics.m_DesktopPath, value);
        });
        FilePath::NormalizeDirectorySeparators(statics.m_DesktopPath);

        // Documents path
        Interop::Windows::GetKnownFolderPath(FOLDERID_Documents, [&](std::wstring_view value)
        {
            Interop::Windows::NarrowString(statics.m_DocumentsPath, value);
        });
        FilePath::NormalizeDirectorySeparators(statics.m_DocumentsPath);

        // Downloads path
        Interop::Windows::GetKnownFolderPath(FOLDERID_Downloads, [&](std::wstring_view value)
        {
            Interop::Windows::NarrowString(statics.m_DownloadsPath, value);
        });
        FilePath::NormalizeDirectorySeparators(statics.m_DownloadsPath);

        // Temp path
        Interop::Windows::GetTempPath(buffer);
        Interop::Windows::NarrowString(statics.m_TemporaryPath, buffer.as_view());
        FilePath::NormalizeDirectorySeparators(statics.m_TemporaryPath);

        if (auto const keySystem = Interop::Windows::RegistryKey::Open(HKEY_LOCAL_MACHINE, LR"(HARDWARE\DESCRIPTION\System)"))
        {
            if (keySystem.ReadString(L"SystemBiosVersion", buffer))
            {
                Interop::Windows::NarrowString(statics.m_DeviceId, buffer.as_view());
            }
            else
            {
                Diagnostics::ReportApplicationStop("Failed to get system information");
            }

            if (auto const keyBios = keySystem.OpenSubKey(LR"(BIOS)"))
            {
                if (keyBios.ReadString(L"SystemManufacturer", buffer))
                {
                    Interop::Windows::NarrowString(statics.m_DeviceManufacturer, buffer.as_view());
                }
                else
                {
                    Diagnostics::ReportApplicationStop("Failed to get system information");
                }

                if (keyBios.ReadString(L"SystemProductName", buffer))
                {
                    Interop::Windows::NarrowString(statics.m_DeviceName, buffer.as_view());
                }
                else
                {
                    Diagnostics::ReportApplicationStop("Failed to get system information");
                }

                if (keyBios.ReadString(L"SystemVersion", buffer))
                {
                    Interop::Windows::NarrowString(statics.m_DeviceVersion, buffer.as_view());
                }
                else
                {
                    Diagnostics::ReportApplicationStop("Failed to get system information");
                }
            }
            else
            {
                Diagnostics::ReportApplicationStop("Failed to get system information");
            }
        }
        else
        {
            Diagnostics::ReportApplicationStop("Failed to get system information");
        }
    }
}

namespace Anemone::Internal
{
    static int64_t QueryInstantFrequency()
    {
        static std::atomic<int64_t> cache{};

        int64_t result = cache.load(std::memory_order::relaxed);

        if (result != 0)
        {
            return result;
        }

        // Implementation detail:
        //  QPC Frequency is stored in KUSER_SHARED_DATA.
        //  This function just performs read from that struct on all known platforms.
        LARGE_INTEGER value;
        [[maybe_unused]] BOOL const success = QueryPerformanceFrequency(&value);
        AE_ASSERT(success != FALSE);

        result = std::bit_cast<int64_t>(value);
        cache.store(result, std::memory_order::relaxed);
        return result;
    }

    static int64_t QueryInstantValue()
    {
        LARGE_INTEGER counter;
        [[maybe_unused]] BOOL const success = QueryPerformanceCounter(&counter);
        AE_ASSERT(success != FALSE);

        return std::bit_cast<int64_t>(counter);
    }
}

namespace Anemone::Internal
{
    extern void InitializeEnvironment()
    {
        // Initialize COM
        if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
        {
            Diagnostics::ReportApplicationStop("Failed to initialize COM.");
        }

        // Initialize DPI awareness.
        SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        // Enable UTF8 console.
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);

        // When process hangs, DWM will ghost the window. This is not desirable for us.
        DisableProcessWindowsGhosting();

        Interop::Windows::SetupConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE));
        Interop::Windows::SetupConsoleMode(GetStdHandle(STD_ERROR_HANDLE));

        SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);
        _set_error_mode(_OUT_TO_STDERR);

        // Set locale.
        (void)std::setlocale(LC_ALL, "en_US.UTF-8"); // NOLINT(concurrency-mt-unsafe); this is invoked in main thread.

        Internal::Windows::VerifyRequirements();

        WSADATA wsaData{};
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            Diagnostics::ReportApplicationStop("Failed to initialize networking.");
        }


        //
        // Determine if we are on build machine
        //

#if !ANEMONE_BUILD_SHIPPING
        std::string buildMachine{};

        if (Environment::GetEnvironmentVariable(buildMachine, "ANEMONE_BUILD_MACHINE"))
        {
            _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
            _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
            _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
            _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
        }
#endif

        Windows::GEnvironmentStatics.Create();
        Windows::InitializeStatics(*Windows::GEnvironmentStatics);
    }

    extern void FinalizeEnvironment()
    {
        Windows::GEnvironmentStatics.Destroy();

        if (WSACleanup() != 0)
        {
            Diagnostics::ReportApplicationStop("Failed to finalize networking.");
        }

        // Finalize COM
        CoUninitialize();
    }
}

namespace Anemone
{
    bool Environment::GetEnvironmentVariable(std::string& result, std::string_view name)
    {
        result.clear();
        Interop::string_buffer<wchar_t, 128> wname{};
        Interop::Windows::WidenString(wname, name);

        if (Interop::string_buffer<wchar_t, 512> wresult{};
            Interop::Windows::GetEnvironmentVariable(wresult, wname))
        {
            Interop::Windows::NarrowString(result, wresult.as_view());
            return true;
        }

        return false;
    }

    bool Environment::SetEnvironmentVariable(std::string name, std::string_view value)
    {
        Interop::string_buffer<wchar_t, 128> wname{};
        Interop::Windows::WidenString(wname, name);

        Interop::string_buffer<wchar_t, 512> wvalue{};
        Interop::Windows::WidenString(wvalue, value);

        return SetEnvironmentVariableW(wname, wvalue) != FALSE;
    }

    bool Environment::RemoveEnvironmentVariable(std::string_view name)
    {
        Interop::string_buffer<wchar_t, 128> wname{};
        Interop::Windows::WidenString(wname, name);

        return SetEnvironmentVariableW(wname, nullptr) != FALSE;
    }

    void Environment::GetDisplayMetrics(DisplayMetrics& metrics)
    {
        metrics.Displays.clear();

        // Get display size.
        metrics.PrimaryDisplaySize = Math::SizeF{
            .Width = static_cast<float>(GetSystemMetrics(SM_CXSCREEN)),
            .Height = static_cast<float>(GetSystemMetrics(SM_CYSCREEN)),
        };

        MONITORENUMPROC fillDisplayDevicesInformation = [](HMONITOR handle, HDC, LPRECT, LPARAM context) -> BOOL
        {
            DisplayMetrics& displayMetrics = *reinterpret_cast<DisplayMetrics*>(context); // NOLINT(performance-no-int-to-ptr)

            MONITORINFOEXW miex{};
            miex.cbSize = sizeof(miex);

            GetMonitorInfoW(handle, &miex);

            Interop::string_buffer<char, 128> name{};
            Interop::Windows::NarrowString(name, miex.szDevice);

            AE_ASSERT(!displayMetrics.Displays.empty());

            DisplayInfo& last = displayMetrics.Displays.back();

            if (last.Name == name.as_view())
            {
                last.DisplayRect = Interop::Windows::ToRectF(miex.rcMonitor);
                last.WorkAreaRect = Interop::Windows::ToRectF(miex.rcWork);
            }

            return TRUE;
        };

        RECT workArea = {-1, -1, -1, -1};

        if (SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0) == FALSE)
        {
            workArea = {};
        }

        // Get workspace area.
        metrics.PrimaryDisplayWorkArea = Interop::Windows::ToRectF(workArea);

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
            Interop::Windows::NarrowString(display.Id, device.DeviceID);
            Interop::Windows::NarrowString(display.Name, device.DeviceName);
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
                fillDisplayDevicesInformation,
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
        return Internal::Windows::GEnvironmentStatics->m_SystemVersion;
    }

    Uuid Environment::GetSystemId()
    {
        return Internal::Windows::GEnvironmentStatics->m_SystemId;
    }

    std::string_view Environment::GetSystemName()
    {
        return Internal::Windows::GEnvironmentStatics->m_SystemName;
    }

    Duration Environment::GetSystemUptime()
    {
        return Duration::FromMilliseconds(static_cast<int64_t>(GetTickCount64()));
    }

    DateTime Environment::GetApplicationStartupTime()
    {
        return Internal::Windows::GEnvironmentStatics->m_StartupTime;
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
            .UserTime = Interop::Windows::ToDuration(ftUser),
            .KernelTime = Interop::Windows::ToDuration(ftKernel),
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
        return Internal::Windows::GEnvironmentStatics->m_DeviceId;
    }

    std::string_view Environment::GetDeviceName()
    {
        return Internal::Windows::GEnvironmentStatics->m_DeviceName;
    }

    std::string Environment::GetDeviceModel()
    {
        return Internal::Windows::GEnvironmentStatics->m_DeviceModel;
    }

    DeviceType Environment::GetDeviceType()
    {
        return Internal::Windows::GEnvironmentStatics->m_DeviceType;
    }

    DeviceProperties Environment::GetDeviceProperties()
    {
        return Internal::Windows::GEnvironmentStatics->m_DeviceProperties;
    }

    std::string_view Environment::GetComputerName()
    {
        return Internal::Windows::GEnvironmentStatics->m_ComputerName;
    }

    std::string_view Environment::GetUserName()
    {
        return Internal::Windows::GEnvironmentStatics->m_UserName;
    }

    std::string_view Environment::GetExecutablePath()
    {
        return Internal::Windows::GEnvironmentStatics->m_ExecutablePath;
    }

    std::string_view Environment::GetStartupPath()
    {
        return Internal::Windows::GEnvironmentStatics->m_StartupPath;
    }

    std::string_view Environment::GetHomePath()
    {
        return Internal::Windows::GEnvironmentStatics->m_ProfilePath;
    }

    std::string_view Environment::GetDesktopPath()
    {
        return Internal::Windows::GEnvironmentStatics->m_DesktopPath;
    }

    std::string_view Environment::GetDocumentsPath()
    {
        return Internal::Windows::GEnvironmentStatics->m_DocumentsPath;
    }

    std::string_view Environment::GetDownloadsPath()
    {
        return Internal::Windows::GEnvironmentStatics->m_DownloadsPath;
    }

    std::string_view Environment::GetTemporaryPath()
    {
        return Internal::Windows::GEnvironmentStatics->m_TemporaryPath;
    }

    DateTime Environment::GetCurrentDateTime()
    {
        FILETIME ft;
        Interop::Windows::GetLocalTimeAsFileTime(ft);
        return Interop::Windows::ToDateTime(ft);
    }

    DateTime Environment::GetCurrentDateTimeUtc()
    {
        FILETIME ft;
        GetSystemTimePreciseAsFileTime(&ft);

        return Interop::Windows::ToDateTime(ft);
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

    Instant Environment::GetCurrentInstant()
    {
        static constexpr int64_t QpcFrequency = 10'000'000;

        int64_t const value = Internal::QueryInstantValue();

        if (int64_t const frequency = Internal::QueryInstantFrequency(); frequency == QpcFrequency)
        {
            return Instant{
                .Inner = {
                    .Seconds = value / QpcFrequency,
                    .Nanoseconds = (value % QpcFrequency) * 100,
                },
            };
        }
        else
        {
            int64_t const nanosecond_conversion = Internal::NanosecondsInSecond / frequency;
            return Instant{
                .Inner = {
                    .Seconds = value / frequency,
                    .Nanoseconds = (value % frequency) * nanosecond_conversion,
                },
            };
        }
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

    void Environment::LaunchUrl(std::string_view url)
    {
        Interop::string_buffer<wchar_t, 512> wUrl{};
        Interop::Windows::WidenString(wUrl, url);

        if (reinterpret_cast<INT_PTR>(ShellExecuteW(nullptr, L"open", wUrl.data(), nullptr, nullptr, SW_SHOWNORMAL)) <= 32)
        {
            AE_VERIFY_WIN32(GetLastError());
        }
    }

    bool Environment::IsOnline()
    {
        Microsoft::WRL::ComPtr<INetworkListManager> networkListManager{};

        HRESULT hr = CoCreateInstance(
            CLSID_NetworkListManager,
            nullptr,
            CLSCTX_ALL,
            IID_PPV_ARGS(networkListManager.GetAddressOf()));

        if (FAILED(hr))
        {
            return false;
        }

        VARIANT_BOOL connected = VARIANT_FALSE;
        hr = networkListManager->get_IsConnectedToInternet(&connected);

        if (SUCCEEDED(hr))
        {
            return connected != VARIANT_FALSE;
        }

        // Assume offline.
        return false;
    }
}
