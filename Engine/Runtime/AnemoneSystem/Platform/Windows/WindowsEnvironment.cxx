#include "AnemoneSystem/Platform/Windows/WindowsEnvironment.hxx"
#include "AnemoneBase/UninitializedObject.hxx"
#include "AnemoneInterop/Windows/Text.hxx"
#include "AnemoneInterop/Windows/Process.hxx"
#include "AnemoneInterop/Windows/FileSystem.hxx"
#include "AnemoneInterop/Windows/Registry.hxx"
#include "AnemoneInterop/Windows/Environment.hxx"
#include "AnemoneInterop/Windows/DateTime.hxx"
#include "AnemoneStorage/FilePath.hxx"
#include "AnemoneDiagnostics/Platform/Windows/WindowsDebug.hxx"

#include <locale>
#include <algorithm>
#include <vector>

#include <Windows.h>
#include <Psapi.h>
#include <objbase.h>
#include <bcrypt.h>
#include <wrl/client.h>
#include <netlistmgr.h>
#include <shellapi.h>

namespace Anemone
{
    namespace
    {
        UninitializedObject<WindowsEnvironment> gWindowsEnvironment{};
    }

    void WindowsEnvironment::Initialize()
    {
        gWindowsEnvironment.Create();

        // Initialize COM
        if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
        {
            Debug::ReportApplicationStop("Failed to initialize COM.");
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

        WSADATA wsaData{};
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            Debug::ReportApplicationStop("Failed to initialize networking.");
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

        gWindowsEnvironment->startupTime = DateTime::Now();


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

            gWindowsEnvironment->systemVersion = fmt::format(
                "{}.{}.{}",
                major,
                minor,
                build.as_view());

            Interop::string_buffer<char, 64> productName{};
            (void)key.ReadString("ProductName", productName);
            gWindowsEnvironment->systemName = productName.as_view();
        }

        // Determine memory properties.
        {
            MEMORYSTATUSEX memoryStatus{};
            memoryStatus.dwLength = sizeof(memoryStatus);

            if (!GlobalMemoryStatusEx(&memoryStatus))
            {
                AE_PANIC("GlobalMemoryStatusEx: {}", GetLastError());
            }

            SYSTEM_INFO systemInfo{};
            GetNativeSystemInfo(&systemInfo);

            gWindowsEnvironment->memoryProperties.TotalPhysicalMemory = memoryStatus.ullTotalPhys;
            gWindowsEnvironment->memoryProperties.TotalVirtualMemory = memoryStatus.ullTotalVirtual;
            gWindowsEnvironment->memoryProperties.TotalPageFile = memoryStatus.ullTotalPageFile;
            gWindowsEnvironment->memoryProperties.AvailablePhysicalMemory = memoryStatus.ullAvailPhys;
            gWindowsEnvironment->memoryProperties.AvailableVirtualMemory = memoryStatus.ullAvailVirtual;
            gWindowsEnvironment->memoryProperties.AvailablePageFile = memoryStatus.ullAvailPageFile;
            gWindowsEnvironment->memoryProperties.SystemAllocationGranularity = systemInfo.dwAllocationGranularity;
            gWindowsEnvironment->memoryProperties.PageSize = systemInfo.dwPageSize;
            gWindowsEnvironment->memoryProperties.AddressLimitLow = reinterpret_cast<uintptr_t>(systemInfo.lpMinimumApplicationAddress);
            gWindowsEnvironment->memoryProperties.AddressLimitHigh = reinterpret_cast<uintptr_t>(systemInfo.lpMaximumApplicationAddress);
        }

        // Determine system ID
        if (auto key = Interop::Windows::RegistryKey::Open(HKEY_LOCAL_MACHINE, LR"(SOFTWARE\Microsoft\Cryptography)"))
        {
            if (key.ReadString(L"MachineGuid", buffer))
            {
                Interop::string_buffer<char, 64> utf8Buffer;
                Interop::Windows::NarrowString(utf8Buffer, buffer.as_view());

                gWindowsEnvironment->systemId = UuidParser::Parse(utf8Buffer.as_view()).value_or(Uuid{});
            }
        }

        // Executable path
        Interop::Windows::QueryFullProcessImageName(buffer, GetCurrentProcess());
        Interop::Windows::NarrowString(gWindowsEnvironment->executablePath, buffer.as_view());
        FilePath::NormalizeDirectorySeparators(gWindowsEnvironment->executablePath);

        // Startup path
        Interop::Windows::GetCurrentDirectory(buffer);
        Interop::Windows::NarrowString(gWindowsEnvironment->startupPath, buffer.as_view());
        FilePath::NormalizeDirectorySeparators(gWindowsEnvironment->startupPath);

        // Computer name
        Interop::Windows::GetComputerName(buffer);
        Interop::Windows::NarrowString(gWindowsEnvironment->computerName, buffer.as_view());

        // User name
        Interop::Windows::GetUserName(buffer);
        Interop::Windows::NarrowString(gWindowsEnvironment->userName, buffer.as_view());

        // Profile path
        Interop::Windows::GetKnownFolderPath(FOLDERID_Profile, [&](std::wstring_view value)
        {
            Interop::Windows::NarrowString(gWindowsEnvironment->profilePath, value);
        });
        FilePath::NormalizeDirectorySeparators(gWindowsEnvironment->profilePath);

        // Desktop path
        Interop::Windows::GetKnownFolderPath(FOLDERID_Desktop, [&](std::wstring_view value)
        {
            Interop::Windows::NarrowString(gWindowsEnvironment->desktopPath, value);
        });
        FilePath::NormalizeDirectorySeparators(gWindowsEnvironment->desktopPath);

        // Documents path
        Interop::Windows::GetKnownFolderPath(FOLDERID_Documents, [&](std::wstring_view value)
        {
            Interop::Windows::NarrowString(gWindowsEnvironment->documentsPath, value);
        });
        FilePath::NormalizeDirectorySeparators(gWindowsEnvironment->documentsPath);

        // Downloads path
        Interop::Windows::GetKnownFolderPath(FOLDERID_Downloads, [&](std::wstring_view value)
        {
            Interop::Windows::NarrowString(gWindowsEnvironment->downloadsPath, value);
        });
        FilePath::NormalizeDirectorySeparators(gWindowsEnvironment->downloadsPath);

        // Temp path
        Interop::Windows::GetTempPath(buffer);
        Interop::Windows::NarrowString(gWindowsEnvironment->temporaryPath, buffer.as_view());
        FilePath::NormalizeDirectorySeparators(gWindowsEnvironment->temporaryPath);

        if (auto const keySystem = Interop::Windows::RegistryKey::Open(HKEY_LOCAL_MACHINE, LR"(HARDWARE\DESCRIPTION\System)"))
        {
            if (keySystem.ReadString(L"SystemBiosVersion", buffer))
            {
                Interop::Windows::NarrowString(gWindowsEnvironment->deviceId, buffer.as_view());
            }
            else
            {
                Debug::ReportApplicationStop("Failed to get system information");
            }

            if (auto const keyBios = keySystem.OpenSubKey(LR"(BIOS)"))
            {
                if (keyBios.ReadString(L"SystemManufacturer", buffer))
                {
                    Interop::Windows::NarrowString(gWindowsEnvironment->deviceManufacturer, buffer.as_view());
                }
                else
                {
                    Debug::ReportApplicationStop("Failed to get system information");
                }

                if (keyBios.ReadString(L"SystemProductName", buffer))
                {
                    Interop::Windows::NarrowString(gWindowsEnvironment->deviceName, buffer.as_view());
                }
                else
                {
                    Debug::ReportApplicationStop("Failed to get system information");
                }

                if (keyBios.ReadString(L"SystemVersion", buffer))
                {
                    Interop::Windows::NarrowString(gWindowsEnvironment->deviceVersion, buffer.as_view());
                }
                else
                {
                    Debug::ReportApplicationStop("Failed to get system information");
                }
            }
            else
            {
                Debug::ReportApplicationStop("Failed to get system information");
            }
        }
        else
        {
            Debug::ReportApplicationStop("Failed to get system information");
        }
    }

    void WindowsEnvironment::Finalize()
    {
        if (WSACleanup() != 0)
        {
            Debug::ReportApplicationStop("Failed to finalize networking.");
        }

        // Finalize COM
        CoUninitialize();

        gWindowsEnvironment.Destroy();
    }

    WindowsEnvironment& WindowsEnvironment::Get()
    {
        return *gWindowsEnvironment;
    }
}

namespace Anemone
{
    void Environment::Initialize()
    {
        WindowsEnvironment::Initialize();
    }

    void Environment::Finalize()
    {
        WindowsEnvironment::Finalize();
    }

    auto Environment::GetEnvironmentVariable(std::string& result, std::string_view name) -> bool
    {
        result.clear();
        Interop::string_buffer<wchar_t, 128> wName{};
        Interop::Windows::WidenString(wName, name);

        if (Interop::string_buffer<wchar_t, 512> wResult{};
            SUCCEEDED(Interop::Windows::GetEnvironmentVariable(wResult, wName)))
        {
            Interop::Windows::NarrowString(result, wResult.as_view());
            return true;
        }

        return false;
    }

    auto Environment::SetEnvironmentVariable(std::string_view name, std::string_view value) -> bool
    {
        Interop::string_buffer<wchar_t, 128> wName{};
        Interop::Windows::WidenString(wName, name);

        Interop::string_buffer<wchar_t, 512> wValue{};
        Interop::Windows::WidenString(wValue, value);

        return SetEnvironmentVariableW(wName, wValue) != FALSE;
    }

    auto Environment::RemoveEnvironmentVariable(std::string_view name) -> bool
    {
        Interop::string_buffer<wchar_t, 128> wName{};
        Interop::Windows::WidenString(wName, name);

        return SetEnvironmentVariableW(wName, nullptr) != FALSE;
    }

    auto Environment::GetSystemVersion() -> std::string_view
    {
        return gWindowsEnvironment->systemVersion;
    }

    auto Environment::GetSystemId() -> Uuid
    {
        return gWindowsEnvironment->systemId;
    }

    auto Environment::GetSystemName() -> std::string_view
    {
        return gWindowsEnvironment->systemName;
    }

    auto Environment::GetSystemUptime() -> Duration
    {
        return Duration::FromMilliseconds(static_cast<int64_t>(GetTickCount64()));
    }

    auto Environment::GetApplicationStartupTime() -> DateTime
    {
        return gWindowsEnvironment->startupTime;
    }

    auto Environment::GetMemoryProperties() -> MemoryProperties
    {
        return gWindowsEnvironment->memoryProperties;
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

        return ProcessMemoryUsage{
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

    auto Environment::GetDeviceUniqueId() -> std::string_view
    {
        return gWindowsEnvironment->deviceId;
    }

    auto Environment::GetDeviceName() -> std::string_view
    {
        return gWindowsEnvironment->deviceName;
    }

    auto Environment::GetDeviceModel() -> std::string_view
    {
        return gWindowsEnvironment->deviceModel;
    }

    auto Environment::GetDeviceType() -> DeviceType
    {
        return gWindowsEnvironment->deviceType;
    }

    auto Environment::GetDeviceProperties() -> DeviceProperties
    {
        return gWindowsEnvironment->deviceProperties;
    }

    auto Environment::GetComputerName() -> std::string_view
    {
        return gWindowsEnvironment->computerName;
    }

    auto Environment::GetUserName() -> std::string_view
    {
        return gWindowsEnvironment->userName;
    }

    auto Environment::GetExecutablePath() -> std::string_view
    {
        return gWindowsEnvironment->executablePath;
    }

    auto Environment::GetStartupPath() -> std::string_view
    {
        return gWindowsEnvironment->startupPath;
    }

    auto Environment::GetHomePath() -> std::string_view
    {
        return gWindowsEnvironment->profilePath;
    }

    auto Environment::GetDesktopPath() -> std::string_view
    {
        return gWindowsEnvironment->desktopPath;
    }

    auto Environment::GetDocumentsPath() -> std::string_view
    {
        return gWindowsEnvironment->documentsPath;
    }

    auto Environment::GetDownloadsPath() -> std::string_view
    {
        return gWindowsEnvironment->downloadsPath;
    }

    auto Environment::GetTemporaryPath() -> std::string_view
    {
        return gWindowsEnvironment->temporaryPath;
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
