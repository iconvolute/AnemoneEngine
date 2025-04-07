#include "AnemoneRuntime/System/Environment.hxx"
#include "AnemoneRuntime/System/Platform/Windows/WindowsEnvironment.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsError.hxx"
#include "AnemoneRuntime/Diagnostics/Platform/Windows/WindowsDebugger.hxx"
#include "AnemoneRuntime/Platform/FilePath.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"
#include "AnemoneRuntime/Diagnostics/Debugger.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

#include <locale>
#include <iterator>

#include <Windows.h>
#include <Psapi.h>
#include <objbase.h>
#include <bcrypt.h>
#include <wrl/client.h>
#include <netlistmgr.h>

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

namespace Anemone::Private
{
    UninitializedObject<WindowsEnvironmentStatics> GEnvironmentStatics{};

    static void SetupConsoleMode(HANDLE hStream)
    {
        if (hStream)
        {
            DWORD dwMode = 0;

            if (GetConsoleMode(hStream, &dwMode))
            {
                dwMode |= ENABLE_PROCESSED_OUTPUT;
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hStream, dwMode);
            }
        }
    }

    WindowsEnvironmentStatics::WindowsEnvironmentStatics()
    {
        // Initialize COM
        if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
        {
            Debugger::ReportApplicationStop("Failed to initialize COM.");
        }

        // Initialize DPI awareness.
        SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        // Enable UTF8 console.
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);

        // When process hangs, DWM will ghost the window. This is not desirable for us.
        DisableProcessWindowsGhosting();

        SetupConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE));
        SetupConsoleMode(GetStdHandle(STD_ERROR_HANDLE));

        SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);
        _set_error_mode(_OUT_TO_STDERR);

        // Set locale.
        (void)std::setlocale(LC_ALL, "en_US.UTF-8"); // NOLINT(concurrency-mt-unsafe); this is invoked in main thread.

        VerifyRequirements();

        WSADATA wsaData{};
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            Debugger::ReportApplicationStop("Failed to initialize networking.");
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

        //
        // Capture application startup time.
        //

        this->m_StartupTime = Environment::GetCurrentDateTime();


        //
        // Determine environment properties.
        //

        Interop::string_buffer<wchar_t, 512> buffer{};

        if (Interop::win32_GetSystemDirectory(buffer))
        {
            std::wstring kernelPath{buffer.as_view()};
            Interop::win32_PathPushFragment(kernelPath, L"kernel32.dll");

            DWORD const dwSize = GetFileVersionInfoSizeW(kernelPath.c_str(), nullptr);

            if (dwSize != 0)
            {
                std::unique_ptr<std::byte[]> versionInfo = std::make_unique_for_overwrite<std::byte[]>(dwSize);

                if (GetFileVersionInfoW(kernelPath.c_str(), 0, dwSize, versionInfo.get()) != FALSE)
                {
                    VS_FIXEDFILEINFO* pFileInfo = nullptr;
                    UINT uLen = 0;

                    if (VerQueryValueW(versionInfo.get(), L"", reinterpret_cast<void**>(&pFileInfo), &uLen) != FALSE)
                    {
                        this->m_SystemVersion = fmt::format(
                            "{}.{}.{}.{}",
                            HIWORD(pFileInfo->dwFileVersionMS),
                            LOWORD(pFileInfo->dwFileVersionMS),
                            HIWORD(pFileInfo->dwFileVersionLS),
                            LOWORD(pFileInfo->dwFileVersionLS));
                    }
                }
            }
        }

        if (this->m_SystemVersion.empty())
        {
            this->m_SystemVersion = "Unknown";
        }

        // Determine system ID
        if (Interop::win32_registry_key key{
                HKEY_LOCAL_MACHINE,
                LR"(SOFTWARE\Microsoft\Cryptography)",
                KEY_READ,
            })
        {
            if (key.read_string(L"MachineGuid", buffer))
            {
                Interop::win32_NarrowString(this->m_SystemId, buffer.as_view());
            }
        }

        // Executable path
        Interop::win32_QueryFullProcessImageName(buffer);
        Interop::win32_NarrowString(this->m_ExecutablePath, buffer.as_view());
        FilePath::NormalizeDirectorySeparators(this->m_ExecutablePath);

        // Startup path
        Interop::win32_GetCurrentDirectory(buffer);
        Interop::win32_NarrowString(this->m_StartupPath, buffer.as_view());
        FilePath::NormalizeDirectorySeparators(this->m_StartupPath);

        // Computer name
        Interop::win32_GetComputerName(buffer);
        Interop::win32_NarrowString(this->m_ComputerName, buffer.as_view());

        // User name
        Interop::win32_GetUserName(buffer);
        Interop::win32_NarrowString(this->m_UserName, buffer.as_view());

        // Profile path
        Interop::win32_GetKnownFolderPath(FOLDERID_Profile, [&](std::wstring_view value)
        {
            Interop::win32_NarrowString(this->m_ProfilePath, value);
        });
        FilePath::NormalizeDirectorySeparators(this->m_ProfilePath);

        // Desktop path
        Interop::win32_GetKnownFolderPath(FOLDERID_Desktop, [&](std::wstring_view value)
        {
            Interop::win32_NarrowString(this->m_DesktopPath, value);
        });
        FilePath::NormalizeDirectorySeparators(this->m_DesktopPath);

        // Documents path
        Interop::win32_GetKnownFolderPath(FOLDERID_Documents, [&](std::wstring_view value)
        {
            Interop::win32_NarrowString(this->m_DocumentsPath, value);
        });
        FilePath::NormalizeDirectorySeparators(this->m_DocumentsPath);

        // Downloads path
        Interop::win32_GetKnownFolderPath(FOLDERID_Downloads, [&](std::wstring_view value)
        {
            Interop::win32_NarrowString(this->m_DownloadsPath, value);
        });
        FilePath::NormalizeDirectorySeparators(this->m_DownloadsPath);

        // Temp path
        Interop::win32_GetTempPath(buffer);
        Interop::win32_NarrowString(this->m_TemporaryPath, buffer.as_view());
        FilePath::NormalizeDirectorySeparators(this->m_TemporaryPath);

        if (Interop::win32_registry_key keySystem{
                HKEY_LOCAL_MACHINE,
                LR"(HARDWARE\DESCRIPTION\System)",
                KEY_READ,
            })
        {
            if (keySystem.read_string(L"SystemBiosVersion", buffer))
            {
                Interop::win32_NarrowString(this->m_DeviceId, buffer.as_view());
            }

            if (Interop::win32_registry_key keyBios{
                    keySystem.get(),
                    LR"(BIOS)",
                    KEY_READ,
                })
            {
                if (keyBios.read_string(L"SystemManufacturer", buffer))
                {
                    Interop::win32_NarrowString(this->m_DeviceManufacturer, buffer.as_view());
                }
                if (keyBios.read_string(L"SystemProductName", buffer))
                {
                    Interop::win32_NarrowString(this->m_DeviceName, buffer.as_view());
                }
                if (keyBios.read_string(L"SystemVersion", buffer))
                {
                    Interop::win32_NarrowString(this->m_DeviceVersion, buffer.as_view());
                }
            }
        }

        Interop::win32_QueryRegistry(buffer, HKEY_LOCAL_MACHINE, LR"(HARDWARE\DESCRIPTION\System)", LR"(SystemBiosVersion)");
        Interop::win32_NarrowString(this->m_DeviceId, buffer.as_view());

        Interop::win32_QueryRegistry(buffer, HKEY_LOCAL_MACHINE, LR"(HARDWARE\DESCRIPTION\System\BIOS)", LR"(SystemManufacturer)");
        Interop::win32_NarrowString(this->m_DeviceManufacturer, buffer.as_view());

        Interop::win32_QueryRegistry(buffer, HKEY_LOCAL_MACHINE, LR"(HARDWARE\DESCRIPTION\System\BIOS)", LR"(SystemProductName)");
        Interop::win32_NarrowString(this->m_DeviceName, buffer.as_view());

        Interop::win32_QueryRegistry(buffer, HKEY_LOCAL_MACHINE, LR"(HARDWARE\DESCRIPTION\System\BIOS)", LR"(SystemVersion)");
        Interop::win32_NarrowString(this->m_DeviceVersion, buffer.as_view());
    }

    WindowsEnvironmentStatics::~WindowsEnvironmentStatics()
    {
        if (WSACleanup() != 0)
        {
            Debugger::ReportApplicationStop("Failed to finalize networking.");
        }

        // Finalize COM
        CoUninitialize();
    }

    void WindowsEnvironmentStatics::VerifyRequirements()
    {
        if (Interop::win32_IsProcessEmulated())
        {
            // VERIFY: AVX is not supported in WoA process emulation.
            Debugger::ReportApplicationStop("Emulated process not supported.");
        }

        if (not IsProcessorFeaturePresent(PF_COMPARE_EXCHANGE_DOUBLE))
        {
            Debugger::ReportApplicationStop("64-bit compare-exchange not supported.");
        }

#if ANEMONE_ARCHITECTURE_X64
        if (not IsProcessorFeaturePresent(PF_COMPARE_EXCHANGE128))
        {
            Debugger::ReportApplicationStop("128-bit compare-exchange not supported");
        }
#endif

#if ANEMONE_FEATURE_AVX

        if (not IsProcessorFeaturePresent(PF_XSAVE_ENABLED))
        {
            Debugger::ReportApplicationStop("XSAVE not enabled.");
        }

        if (not IsProcessorFeaturePresent(PF_AVX_INSTRUCTIONS_AVAILABLE))
        {
            Debugger::ReportApplicationStop("AVX not supported.");
        }
#endif

#if ANEMONE_FEATURE_AVX2
        if (not IsProcessorFeaturePresent(PF_AVX2_INSTRUCTIONS_AVAILABLE))
        {
            Debugger::ReportApplicationStop("AVX2 not supported.");
        }
#endif

#if ANEMONE_FEATURE_NEON
        if (not IsProcessorFeaturePresent(PF_ARM_NEON_INSTRUCTIONS_AVAILABLE))
        {
            Debugger::ReportApplicationStop("NEON not supported.");
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
            Debugger::ReportApplicationStop("Windows 10 or newer required");
        }
    }
}

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

    void Environment::GetDisplayMetrics(DisplayMetrics& metrics)
    {
        metrics.Displays.clear();

        // Get display size.
        metrics.PrimaryDisplaySize = Math::SizeF{
            .Width = static_cast<float>(GetSystemMetrics(SM_CXSCREEN)),
            .Height = static_cast<float>(GetSystemMetrics(SM_CYSCREEN)),
        };

        MONITORENUMPROC const fillDisplayDevicesInformation = [](HMONITOR handle, HDC, LPRECT, LPARAM context) -> BOOL
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
        return Private::GEnvironmentStatics->m_SystemVersion;
    }

    std::string_view Environment::GetSystemId()
    {
        return Private::GEnvironmentStatics->m_SystemId;
    }

    std::string_view Environment::GetSystemName()
    {
        return Private::GEnvironmentStatics->m_SystemName;
    }

    Duration Environment::GetSystemUptime()
    {
        return Duration::FromMilliseconds(static_cast<int64_t>(GetTickCount64()));
    }

    DateTime Environment::GetApplicationStartupTime()
    {
        return Private::GEnvironmentStatics->m_StartupTime;
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
        return Private::GEnvironmentStatics->m_DeviceId;
    }

    std::string_view Environment::GetDeviceName()
    {
        return Private::GEnvironmentStatics->m_DeviceName;
    }

    std::string Environment::GetDeviceModel()
    {
        return Private::GEnvironmentStatics->m_DeviceModel;
    }

    DeviceType Environment::GetDeviceType()
    {
        return Private::GEnvironmentStatics->m_DeviceType;
    }

    DeviceProperties Environment::GetDeviceProperties()
    {
        return Private::GEnvironmentStatics->m_DeviceProperties;
    }

    std::string_view Environment::GetComputerName()
    {
        return Private::GEnvironmentStatics->m_ComputerName;
    }

    std::string_view Environment::GetUserName()
    {
        return Private::GEnvironmentStatics->m_UserName;
    }

    std::string_view Environment::GetExecutablePath()
    {
        return Private::GEnvironmentStatics->m_ExecutablePath;
    }

    std::string_view Environment::GetStartupPath()
    {
        return Private::GEnvironmentStatics->m_StartupPath;
    }

    std::string_view Environment::GetHomePath()
    {
        return Private::GEnvironmentStatics->m_ProfilePath;
    }

    std::string_view Environment::GetDesktopPath()
    {
        return Private::GEnvironmentStatics->m_DesktopPath;
    }

    std::string_view Environment::GetDocumentsPath()
    {
        return Private::GEnvironmentStatics->m_DocumentsPath;
    }

    std::string_view Environment::GetDownloadsPath()
    {
        return Private::GEnvironmentStatics->m_DownloadsPath;
    }

    std::string_view Environment::GetTemporaryPath()
    {
        return Private::GEnvironmentStatics->m_TemporaryPath;
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
            int64_t const nanosecond_conversion = Private::NanosecondsInSecond / frequency;
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
        Interop::win32_WidenString(wUrl, url);

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
