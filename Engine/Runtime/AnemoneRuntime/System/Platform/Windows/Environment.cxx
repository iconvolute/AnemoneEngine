#include "AnemoneRuntime/System/Environment.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"
#include "AnemoneRuntime/Interop/Windows/Registry.hxx"
#include "AnemoneRuntime/Interop/Windows/Text.hxx"
#include "AnemoneRuntime/Interop/Windows/Process.hxx"
#include "AnemoneRuntime/Interop/Windows/Environment.hxx"
#include "AnemoneRuntime/Interop/Windows/DateTime.hxx"
#include "AnemoneRuntime/Interop/Windows/FileSystem.hxx"
#include "AnemoneRuntime/Interop/Windows/UI.hxx"
#include "AnemoneRuntime/Interop/Windows/Graphics.hxx"
#include "AnemoneRuntime/Platform/FilePath.hxx"
#include "AnemoneRuntime/Diagnostics/Platform/Windows/Debug.hxx"

#include <locale>
#include <algorithm>
#include <vector>

#include <VersionHelpers.h>
#include <Windows.h>
#include <Psapi.h>
#include <objbase.h>
#include <bcrypt.h>
#include <wrl/client.h>
#include <netlistmgr.h>
#include <shellapi.h>

namespace Anemone::Internal
{
    extern bool GIsConsoleApplication; 
    bool GIsConsoleApplication = false;
}

namespace Anemone
{
    namespace
    {
        struct CoreInfoImpl
        {
            uint8_t Efficiency{};
            bool HyperThreaded{};
        };

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

        void InitializeEnvironment(EnvironmentStatics& statics)
        {
            //
            // Capture application startup time.
            //

            statics.startupTime = Environment::GetCurrentDateTime();


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

                statics.systemVersion = fmt::format(
                    "{}.{}.{}",
                    major,
                    minor,
                    build.as_view());

                Interop::string_buffer<char, 64> productName{};
                (void)key.ReadString("ProductName", productName);
                statics.systemName = productName.as_view();
            }

            // Determine system ID
            if (auto key = Interop::Windows::RegistryKey::Open(HKEY_LOCAL_MACHINE, LR"(SOFTWARE\Microsoft\Cryptography)"))
            {
                if (key.ReadString(L"MachineGuid", buffer))
                {
                    Interop::string_buffer<char, 64> utf8Buffer;
                    Interop::Windows::NarrowString(utf8Buffer, buffer.as_view());

                    statics.systemId = UuidParser::Parse(utf8Buffer.as_view()).value_or(Uuid{});
                }
            }

            // Executable path
            Interop::Windows::QueryFullProcessImageName(buffer, GetCurrentProcess());
            Interop::Windows::NarrowString(statics.executablePath, buffer.as_view());
            FilePath::NormalizeDirectorySeparators(statics.executablePath);

            // Startup path
            Interop::Windows::GetCurrentDirectory(buffer);
            Interop::Windows::NarrowString(statics.startupPath, buffer.as_view());
            FilePath::NormalizeDirectorySeparators(statics.startupPath);

            // Computer name
            Interop::Windows::GetComputerName(buffer);
            Interop::Windows::NarrowString(statics.computerName, buffer.as_view());

            // User name
            Interop::Windows::GetUserName(buffer);
            Interop::Windows::NarrowString(statics.userName, buffer.as_view());

            // Profile path
            Interop::Windows::GetKnownFolderPath(FOLDERID_Profile, [&](std::wstring_view value)
            {
                Interop::Windows::NarrowString(statics.profilePath, value);
            });
            FilePath::NormalizeDirectorySeparators(statics.profilePath);

            // Desktop path
            Interop::Windows::GetKnownFolderPath(FOLDERID_Desktop, [&](std::wstring_view value)
            {
                Interop::Windows::NarrowString(statics.desktopPath, value);
            });
            FilePath::NormalizeDirectorySeparators(statics.desktopPath);

            // Documents path
            Interop::Windows::GetKnownFolderPath(FOLDERID_Documents, [&](std::wstring_view value)
            {
                Interop::Windows::NarrowString(statics.documentsPath, value);
            });
            FilePath::NormalizeDirectorySeparators(statics.documentsPath);

            // Downloads path
            Interop::Windows::GetKnownFolderPath(FOLDERID_Downloads, [&](std::wstring_view value)
            {
                Interop::Windows::NarrowString(statics.downloadsPath, value);
            });
            FilePath::NormalizeDirectorySeparators(statics.downloadsPath);

            // Temp path
            Interop::Windows::GetTempPath(buffer);
            Interop::Windows::NarrowString(statics.temporaryPath, buffer.as_view());
            FilePath::NormalizeDirectorySeparators(statics.temporaryPath);

            if (auto const keySystem = Interop::Windows::RegistryKey::Open(HKEY_LOCAL_MACHINE, LR"(HARDWARE\DESCRIPTION\System)"))
            {
                if (keySystem.ReadString(L"SystemBiosVersion", buffer))
                {
                    Interop::Windows::NarrowString(statics.deviceId, buffer.as_view());
                }
                else
                {
                    Diagnostics::Debug::ReportApplicationStop("Failed to get system information");
                }

                if (auto const keyBios = keySystem.OpenSubKey(LR"(BIOS)"))
                {
                    if (keyBios.ReadString(L"SystemManufacturer", buffer))
                    {
                        Interop::Windows::NarrowString(statics.deviceManufacturer, buffer.as_view());
                    }
                    else
                    {
                        Diagnostics::Debug::ReportApplicationStop("Failed to get system information");
                    }

                    if (keyBios.ReadString(L"SystemProductName", buffer))
                    {
                        Interop::Windows::NarrowString(statics.deviceName, buffer.as_view());
                    }
                    else
                    {
                        Diagnostics::Debug::ReportApplicationStop("Failed to get system information");
                    }

                    if (keyBios.ReadString(L"SystemVersion", buffer))
                    {
                        Interop::Windows::NarrowString(statics.deviceVersion, buffer.as_view());
                    }
                    else
                    {
                        Diagnostics::Debug::ReportApplicationStop("Failed to get system information");
                    }
                }
                else
                {
                    Diagnostics::Debug::ReportApplicationStop("Failed to get system information");
                }
            }
            else
            {
                Diagnostics::Debug::ReportApplicationStop("Failed to get system information");
            }
        }

        void InitializeProcessorProperties(EnvironmentStatics& statics)
        {
            Interop::memory_buffer<4096> buffer{};

            if (SUCCEEDED(Interop::Windows::GetLogicalProcessorInformationEx(buffer, RelationAll)))
            {
                uint32_t cacheLineSize = std::numeric_limits<uint32_t>::max();

                bool hasEfficiency = IsWindows10OrGreater();

                std::vector<CoreInfoImpl> coreInfo;
                uint8_t performanceEfficiencyClass = 0;

                std::span view = buffer.as_span();

                while (not view.empty())
                {
                    SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX const& current = *reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX const*>(view.data());

                    switch (current.Relationship)
                    {
                    case RelationCache:
                        {
                            // Choose the smallest cache line size as the "safest" value.
                            cacheLineSize = std::min<uint32_t>(cacheLineSize, current.Cache.LineSize);

                            switch (current.Cache.Level)
                            {
                            case 1:
                                statics.cacheSizeLevel1 += current.Cache.CacheSize;
                                break;

                            case 2:
                                statics.cacheSizeLevel2 += current.Cache.CacheSize;
                                break;

                            case 3:
                                statics.cacheSizeLevel3 += current.Cache.CacheSize;
                                break;

                            default:
                                // Unknown cache line level. Are you some kind of server from the future?
                                break;
                            }

                            break;
                        }

                    case RelationProcessorCore:
                        {
                            if (current.Processor.GroupCount == 1)
                            {
                                // On 64-bit windows, we count only cores on first group.
                                // Maybe in future we enhance this to support more than 64 cores?

                                if (hasEfficiency)
                                {
                                    performanceEfficiencyClass = std::max<uint8_t>(performanceEfficiencyClass, current.Processor.EfficiencyClass);

                                    if (current.Processor.Flags & LTP_PC_SMT)
                                    {
                                        // Hyper-threaded CPU.
                                        statics.hyperThreading = true;
                                    }
                                }

                                coreInfo.emplace_back(
                                    hasEfficiency ? current.Processor.EfficiencyClass : uint8_t{},
                                    (current.Processor.Flags & LTP_PC_SMT) != 0);
                            }

                            break;
                        }

                    default:
                        {
                            break;
                        }
                    }

                    view = view.subspan(current.Size);
                }

                statics.cacheLineSize = cacheLineSize;

                statics.logicalCores = 0;
                statics.physicalCores = 0;
                statics.performanceCores = 0;
                statics.efficiencyCores = 0;

                for (auto const& core : coreInfo)
                {
                    // Classify core by efficiency.
                    if (core.Efficiency == performanceEfficiencyClass)
                    {
                        ++statics.performanceCores;
                    }
                    else
                    {
                        ++statics.efficiencyCores;
                    }

                    // Count logical cores.
                    ++statics.logicalCores;

                    if (core.HyperThreaded)
                    {
                        ++statics.logicalCores;
                    }

                    // And physical cores.
                    ++statics.physicalCores;
                }
            }

            // Get name and vendor of the CPU
            if (auto const key = Interop::Windows::RegistryKey::Open(HKEY_LOCAL_MACHINE, LR"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)"))
            {
                if (not key.ReadString("ProcessorNameString", statics.processorName))
                {
                    Diagnostics::Debug::ReportApplicationStop("Cannot obtain processor information");
                }

                if (not key.ReadString("VendorIdentifier", statics.processorVendor))
                {
                    Diagnostics::Debug::ReportApplicationStop("Cannot obtain processor information");
                }
            }
            else
            {
                Diagnostics::Debug::ReportApplicationStop("Cannot obtain processor information");
            }
        }

        void VerifyRequirements()
        {
            if (Interop::Windows::IsProcessEmulated())
            {
                // VERIFY: AVX is not supported in WoA process emulation.
                Diagnostics::Debug::ReportApplicationStop("Emulated process not supported.");
            }

            if (not IsProcessorFeaturePresent(PF_COMPARE_EXCHANGE_DOUBLE))
            {
                Diagnostics::Debug::ReportApplicationStop("64-bit compare-exchange not supported.");
            }

#if ANEMONE_ARCHITECTURE_X64
            if (not IsProcessorFeaturePresent(PF_COMPARE_EXCHANGE128))
            {
                Diagnostics::Debug::ReportApplicationStop("128-bit compare-exchange not supported");
            }
#endif

#if ANEMONE_FEATURE_AVX

            if (not IsProcessorFeaturePresent(PF_XSAVE_ENABLED))
            {
                Diagnostics::Debug::ReportApplicationStop("XSAVE not enabled.");
            }

            if (not IsProcessorFeaturePresent(PF_AVX_INSTRUCTIONS_AVAILABLE))
            {
                Diagnostics::Debug::ReportApplicationStop("AVX not supported.");
            }
#endif

#if ANEMONE_FEATURE_AVX2
            if (not IsProcessorFeaturePresent(PF_AVX2_INSTRUCTIONS_AVAILABLE))
            {
                Diagnostics::Debug::ReportApplicationStop("AVX2 not supported.");
            }
#endif

#if ANEMONE_FEATURE_NEON
            if (not IsProcessorFeaturePresent(PF_ARM_NEON_INSTRUCTIONS_AVAILABLE))
            {
                Diagnostics::Debug::ReportApplicationStop("NEON not supported.");
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
                Diagnostics::Debug::ReportApplicationStop("Windows 10 or newer required");
            }
        }

        int64_t QueryInstantFrequency()
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

        int64_t QueryInstantValue()
        {
            LARGE_INTEGER counter;
            [[maybe_unused]] BOOL const success = QueryPerformanceCounter(&counter);
            AE_ASSERT(success != FALSE);

            return std::bit_cast<int64_t>(counter);
        }
    }
}

namespace Anemone::Internal
{
    extern void InitializeEnvironment()
    {
        // Initialize COM
        if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
        {
            Diagnostics::Debug::ReportApplicationStop("Failed to initialize COM.");
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

        VerifyRequirements();

        WSADATA wsaData{};
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            Diagnostics::Debug::ReportApplicationStop("Failed to initialize networking.");
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

        gEnvironmentStatics.Create();
        InitializeEnvironment(*gEnvironmentStatics);
        InitializeProcessorProperties(*gEnvironmentStatics);
    }

    extern void FinalizeEnvironment()
    {
        gEnvironmentStatics.Destroy();

        if (WSACleanup() != 0)
        {
            Diagnostics::Debug::ReportApplicationStop("Failed to finalize networking.");
        }

        // Finalize COM
        CoUninitialize();
    }
}

namespace Anemone
{
    auto Environment::GetEnvironmentVariable(std::string& result, std::string_view name) -> bool
    {
        result.clear();
        Interop::string_buffer<wchar_t, 128> wname{};
        Interop::Windows::WidenString(wname, name);

        if (Interop::string_buffer<wchar_t, 512> wresult{};
            SUCCEEDED(Interop::Windows::GetEnvironmentVariable(wresult, wname)))
        {
            Interop::Windows::NarrowString(result, wresult.as_view());
            return true;
        }

        return false;
    }

    auto Environment::SetEnvironmentVariable(std::string name, std::string_view value) -> bool
    {
        Interop::string_buffer<wchar_t, 128> wname{};
        Interop::Windows::WidenString(wname, name);

        Interop::string_buffer<wchar_t, 512> wvalue{};
        Interop::Windows::WidenString(wvalue, value);

        return SetEnvironmentVariableW(wname, wvalue) != FALSE;
    }

    auto Environment::RemoveEnvironmentVariable(std::string_view name) -> bool
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
            (void)Interop::Windows::EnumDisplayMonitors(
                nullptr,
                nullptr,
                [&](HMONITOR handle, HDC dc, LPRECT rect) -> bool
            {
                (void)rect;
                (void)dc;

                MONITORINFOEXW miex{};
                miex.cbSize = sizeof(miex);

                GetMonitorInfoW(handle, &miex);

                Interop::string_buffer<char, 128> name{};
                Interop::Windows::NarrowString(name, miex.szDevice);

                AE_ASSERT(!metrics.Displays.empty());

                DisplayInfo& last = metrics.Displays.back();

                if (last.Name == name.as_view())
                {
                    last.DisplayRect = Interop::Windows::ToRectF(miex.rcMonitor);
                    last.WorkAreaRect = Interop::Windows::ToRectF(miex.rcWork);
                }

                return true;
            });
        }
    }

    auto Environment::GetScreenPixel(Math::PointF position, float gamma) -> ColorRef
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
        return gEnvironmentStatics->systemName;
    }

    auto Environment::GetSystemUptime() -> Duration
    {
        return Duration::FromMilliseconds(static_cast<int64_t>(GetTickCount64()));
    }

    auto Environment::GetApplicationStartupTime() -> DateTime
    {
        return gEnvironmentStatics->startupTime;
    }

    auto Environment::GetMemoryProperties() -> MemoryProperties
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

    auto Environment::GetMemoryUsage() -> MemoryUsage
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

    auto Environment::GetPowerUsage() -> PowerUsage
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

    auto Environment::GetProcessorUsage() -> ProcessorUsage
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

    auto Environment::GetProcessMemoryUsage() -> ProcessMemoryUsage
    {
        PROCESS_MEMORY_COUNTERS process_memory_counters{};
        process_memory_counters.cb = sizeof(process_memory_counters);

        GetProcessMemoryInfo(GetCurrentProcess(), &process_memory_counters, sizeof(process_memory_counters));

        return ProcessMemoryUsage {
            .PageFaultCount = process_memory_counters.PageFaultCount,
            .PeakWorkingSetSize = process_memory_counters.PeakWorkingSetSize,
            .WorkingSetSize = process_memory_counters.WorkingSetSize,
            .QuotaPeakPagedPoolUsage = process_memory_counters.QuotaPeakPagedPoolUsage,
            .QuotaPagedPoolUsage = process_memory_counters.QuotaPagedPoolUsage,
            .QuotaPeakNonPagedPoolUsage = process_memory_counters.QuotaPeakNonPagedPoolUsage,
            .QuotaNonPagedPoolUsage = process_memory_counters.QuotaNonPagedPoolUsage,
            .PageFileUsage = process_memory_counters.PagefileUsage,
            .PeakPageFileUsage = process_memory_counters.PeakPagefileUsage,
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
        FILETIME ft;
        Interop::Windows::GetLocalTimeAsFileTime(ft);
        return Interop::Windows::ToDateTime(ft);
    }

    auto Environment::GetCurrentDateTimeUtc() -> DateTime
    {
        FILETIME ft;
        GetSystemTimePreciseAsFileTime(&ft);

        return Interop::Windows::ToDateTime(ft);
    }

    auto Environment::GetCurrentTimeZoneBias() -> Duration
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

    auto Environment::GetCurrentInstant() -> Instant
    {
        static constexpr int64_t QpcFrequency = 10'000'000;

        int64_t const value = QueryInstantValue();

        if (int64_t const frequency = QueryInstantFrequency(); frequency == QpcFrequency)
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
