#include "AnemoneRuntime/Platform/Platform.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsPlatform.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsWindow.hxx"
#include "AnemoneRuntime/Platform/Windows/CrashHandler.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInput.hxx"
#include "AnemoneRuntime/System/Path.hxx"

#include <locale>

#include <Windows.h>
#include <Psapi.h>
#include <bcrypt.h>

#define ENABLE_HEAP_CORRUPTION_CRASHES false

namespace Anemone::Platform::Internal
{
    UninitializedObject<WindowsPlatformStatics> GWindowsPlatformStatics;
    UninitializedObject<PlatformStatics> GPlatformStatics;

    static constexpr ULONG_PTR IDI_MAIN_ICON = 2137u;
}

namespace Anemone::Platform
{
    void ReportApplicationStop(std::string_view reason)
    {
        Interop::win32_string_buffer<wchar_t, 128> buffer{};
        Interop::win32_WidenString(buffer, reason);

        MessageBoxW(
            nullptr,
            buffer,
            L"Application Stop",
            MB_OK | MB_ICONERROR);

        ExitProcess(static_cast<UINT>(-1));
    }

    bool IsProcessEmulated()
    {
        USHORT processMachine = IMAGE_FILE_MACHINE_UNKNOWN;
        USHORT nativeMachine = IMAGE_FILE_MACHINE_UNKNOWN;

        if (IsWow64Process2(GetCurrentProcess(), &processMachine, &nativeMachine))
        {
#if ANEMONE_ARCHITECTURE_X64
            if (nativeMachine != IMAGE_FILE_MACHINE_AMD64)
            {
                return true;
            }
#endif

#if ANEMONE_ARCHITECTURE_ARM64
            if (nativeMachine != IMAGE_FILE_MACHINE_ARM64)
            {
                return true;
            }
#endif
        }

        return false;
    }

    void HandleCrash(EXCEPTION_POINTERS* pExceptionPointers)
    {
        static SRWLOCK lock = SRWLOCK_INIT;

        AcquireSRWLockExclusive(&lock);

        HANDLE const hProcess = GetCurrentProcess();
        HANDLE const hThread = GetCurrentThread();

        DWORD const dwProcessId = GetProcessId(hProcess);
        DWORD const dwThreadId = GetThreadId(hThread);

        Interop::CrashDetails crashDetails{};
        crashDetails.ProcessId = dwProcessId;
        crashDetails.ThreadId = dwThreadId;

        if (pExceptionPointers != nullptr)
        {
            crashDetails.ExceptionRecord = *pExceptionPointers->ExceptionRecord;
            crashDetails.Context = *pExceptionPointers->ContextRecord;
        }

        {
            Interop::Mapping mapping{dwProcessId, dwThreadId};
            mapping.Write(crashDetails);

            wchar_t commandLine[MAX_PATH + 1];

            if (swprintf_s(
                    commandLine,
                    L"AnemoneCrashReporter.exe --pid %u --tid %u",
                    static_cast<UINT>(dwProcessId),
                    static_cast<UINT>(dwThreadId)) < 0)
            {
                // Terminate process immediately
                ReportApplicationStop("Could not format command line");
            }

            STARTUPINFOW startupInfo{
                .cb = sizeof(startupInfo),
                .lpReserved = nullptr,
                .lpDesktop = nullptr,
                .lpTitle = nullptr,
                .dwX = static_cast<DWORD>(CW_USEDEFAULT),
                .dwY = static_cast<DWORD>(CW_USEDEFAULT),
                .dwXSize = static_cast<DWORD>(CW_USEDEFAULT),
                .dwYSize = static_cast<DWORD>(CW_USEDEFAULT),
                .dwXCountChars = 0,
                .dwYCountChars = 0,
                .dwFillAttribute = 0,
                .dwFlags = 0,
                .wShowWindow = 0,
                .cbReserved2 = 0,
                .lpReserved2 = nullptr,
                .hStdInput = nullptr,
                .hStdOutput = nullptr,
                .hStdError = nullptr,
            };
            PROCESS_INFORMATION processInformation{};

            BOOL bCreated = CreateProcessW(
                nullptr,
                commandLine,
                nullptr,
                nullptr,
                FALSE,
                CREATE_DEFAULT_ERROR_MODE | CREATE_UNICODE_ENVIRONMENT | NORMAL_PRIORITY_CLASS,
                nullptr,
                nullptr,
                &startupInfo,
                &processInformation);

            if (not bCreated)
            {
                // Terminate process immediately
                ReportApplicationStop("Could not start AnemoneCrashReporter");
            }
            else
            {
                // Wait until crash reporter finishes
                WaitForSingleObject(processInformation.hProcess, INFINITE);
                CloseHandle(processInformation.hThread);
                CloseHandle(processInformation.hProcess);
            }
        }

        ReleaseSRWLockExclusive(&lock);

        // Terminate process immediately
        TerminateProcess(hProcess, static_cast<UINT>(-1));
    }

    LONG CALLBACK OnUnhandledExceptionFilter(LPEXCEPTION_POINTERS lpExceptionPointers)
    {
        HandleCrash(lpExceptionPointers);
        return EXCEPTION_CONTINUE_SEARCH;
    }

    LONG CALLBACK OnUnhandledExceptionVEH(LPEXCEPTION_POINTERS lpExceptionPointers)
    {
        if (lpExceptionPointers->ExceptionRecord->ExceptionCode == STATUS_HEAP_CORRUPTION)
        {
            HandleCrash(lpExceptionPointers);
        }

        return EXCEPTION_EXECUTE_HANDLER;
    }
}

namespace Anemone::Platform::Internal
{
    namespace
    {
        void VerifyRequirements()
        {
            if (IsProcessEmulated())
            {
                // VERIFY: AVX is not supported in WoA process emulation.
                ReportApplicationStop("Emulated process not supported.");
            }

            if (not IsProcessorFeaturePresent(PF_COMPARE_EXCHANGE_DOUBLE))
            {
                ReportApplicationStop("64-bit compare-exchange not supported.");
            }

#if ANEMONE_ARCHITECTURE_X64
            if (not IsProcessorFeaturePresent(PF_COMPARE_EXCHANGE128))
            {
                ReportApplicationStop("128-bit compare-exchange not supported");
            }
#endif

#if ANEMONE_FEATURE_AVX

            if (not IsProcessorFeaturePresent(PF_XSAVE_ENABLED))
            {
                ReportApplicationStop("XSAVE not enabled.");
            }

            if (not IsProcessorFeaturePresent(PF_AVX_INSTRUCTIONS_AVAILABLE))
            {
                ReportApplicationStop("AVX not supported.");
            }
#endif

#if ANEMONE_FEATURE_AVX2
            if (not IsProcessorFeaturePresent(PF_AVX2_INSTRUCTIONS_AVAILABLE))
            {
                ReportApplicationStop("AVX2 not supported.");
            }
#endif

#if ANEMONE_FEATURE_NEON
            if (not IsProcessorFeaturePresent(PF_ARM_NEON_INSTRUCTIONS_AVAILABLE))
            {
                ReportApplicationStop("NEON not supported.");
            }
#endif

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
                ReportApplicationStop("Windows 10 or newer required");
            }
        }

        void SetupConsoleMode(HANDLE hStream)
        {
            DWORD dwMode;

            if (GetConsoleMode(hStream, &dwMode))
            {
                dwMode |= ENABLE_PROCESSED_OUTPUT;
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hStream, dwMode);
            }
        }

        ProcessorProperties DetermineProcessorProperties()
        {
            ProcessorProperties result{};

            // Determine CPU properties.
            HANDLE hThisProcess = GetCurrentProcess();
            DWORD dwSize{};
            GetSystemCpuSetInformation(nullptr, 0, &dwSize, hThisProcess, 0);

            if ((GetLastError() == ERROR_INSUFFICIENT_BUFFER) and (dwSize != 0))
            {
                std::unique_ptr<std::byte[]> buffer = std::make_unique_for_overwrite<std::byte[]>(dwSize);

                GetSystemCpuSetInformation(
                    reinterpret_cast<PSYSTEM_CPU_SET_INFORMATION>(buffer.get()),
                    dwSize,
                    &dwSize,
                    hThisProcess,
                    0);

                std::byte const* first = buffer.get();
                std::byte const* last = buffer.get() + dwSize;
                std::byte const* it = first;

                while (it < last)
                {
                    auto const& current = *reinterpret_cast<SYSTEM_CPU_SET_INFORMATION const*>(it);

                    switch (current.Type)
                    {
                    case CpuSetInformation:
                        if (current.CpuSet.CoreIndex == current.CpuSet.LogicalProcessorIndex)
                        {
                            if (current.CpuSet.EfficiencyClass == 0)
                            {
                                ++result.EfficiencyCores;
                            }
                            else
                            {
                                ++result.PerformanceCores;
                            }

                            ++result.PhysicalCores;
                        }

                        ++result.LogicalCores;
                        break;
                    }

                    it += current.Size;
                }

                if (result.PerformanceCores == 0)
                {
                    // Non-hybrid CPUs report all cores as efficient cores.
                    result.PerformanceCores = result.EfficiencyCores;
                    result.EfficiencyCores = 0;
                }

                // Determine Hyper-Threading. CPUs with more logical cores than physical cores are considered hyper-threaded.
                result.HyperThreadingEnabled = result.LogicalCores > result.PhysicalCores;
            }

            // Determine Cache properties.
            dwSize = 0;
            GetLogicalProcessorInformationEx(RelationCache, nullptr, &dwSize);

            if ((GetLastError() == ERROR_INSUFFICIENT_BUFFER) and (dwSize != 0))
            {
                std::unique_ptr<std::byte[]> buffer = std::make_unique_for_overwrite<std::byte[]>(dwSize);

                GetLogicalProcessorInformationEx(
                    RelationCache,
                    reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*>(buffer.get()),
                    &dwSize);

                std::byte const* first = buffer.get();
                std::byte const* last = buffer.get() + dwSize;
                std::byte const* it = first;

                uint32_t cacheLineSize = std::numeric_limits<uint32_t>::max();

                while (it < last)
                {
                    auto const& current = *reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX const*>(it);

                    AE_ASSERT(current.Relationship == RelationCache);

                    // Choose smallest cache line size as the "safest" value.
                    cacheLineSize = std::min<uint32_t>(cacheLineSize, current.Cache.LineSize);

                    switch (current.Cache.Level)
                    {
                    case 1:
                        result.CacheSizeLevel1 += current.Cache.CacheSize;
                        break;

                    case 2:
                        result.CacheSizeLevel2 += current.Cache.CacheSize;
                        break;

                    case 3:
                        result.CacheSizeLevel3 += current.Cache.CacheSize;
                        break;

                    default:
                        // Unknown cache line level. Are you some kind of server from the future?
                        break;
                    }

                    it += current.Size;
                }

                result.CacheLineSize = cacheLineSize;
            }

#if false
            // Query CPU name from registry
            Interop::win32_string_buffer<wchar_t, 128> buffer{};

            Interop::win32_QueryRegistry(buffer, HKEY_LOCAL_MACHINE, LR"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)", LR"(ProcessorNameString)");
            Interop::win32_NarrowString(properties.Name, buffer.as_view());

            Interop::win32_QueryRegistry(buffer, HKEY_LOCAL_MACHINE, LR"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)", LR"(VendorIdentifier)");
            Interop::win32_NarrowString(properties.Vendor, buffer.as_view());
#endif

            return result;
        }
    }

    void Initialize(int argc, char* argv[])
    {
        (void)argc;
        (void)argv;

        using System::Path;

        GPlatformStatics.Create();
        GWindowsPlatformStatics.Create();
        GWindowsInputStatics.Create();

        // Initialize COM
        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

        // Initialize DPI awareness.
        SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        // Enable UTF8 console.
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);

        // When process hangs, DWM will ghost the window. This is not desirable for us.
        DisableProcessWindowsGhosting();

        SetupConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE));
        SetupConsoleMode(GetStdHandle(STD_ERROR_HANDLE));

        // Set locale.
        (void)std::setlocale(LC_ALL, "en_US.UTF-8"); // NOLINT(concurrency-mt-unsafe); this is invoked in main thread.

        VerifyRequirements();

        // Setup crash handling callbacks.
        SetUnhandledExceptionFilter(OnUnhandledExceptionFilter);

#if ENABLE_HEAP_CORRUPTION_CRASHES
        AddVectoredExceptionHandler(0, OnUnhandledExceptionVEH);
#endif

        _set_error_mode(_OUT_TO_STDERR);

        WSADATA wsaData{};
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            ReportApplicationStop("Failed to initialize networking.");
        }

        // Setup handles for common resources.
        GWindowsPlatformStatics->InstanceHandle = GetModuleHandleW(nullptr);

        GWindowsPlatformStatics->ApplicationIcon = static_cast<HICON>(LoadImageW(

            GWindowsPlatformStatics->InstanceHandle,
            MAKEINTRESOURCE(IDI_MAIN_ICON),
            IMAGE_ICON,
            0,
            0,
            LR_SHARED));

        GWindowsPlatformStatics->ArrowCursor = Interop::win32_LoadSystemCursor(IDC_ARROW);
        GWindowsPlatformStatics->IBeamCursor = Interop::win32_LoadSystemCursor(IDC_IBEAM);
        GWindowsPlatformStatics->WaitCursor = Interop::win32_LoadSystemCursor(IDC_WAIT);
        GWindowsPlatformStatics->CrossCursor = Interop::win32_LoadSystemCursor(IDC_CROSS);
        GWindowsPlatformStatics->SizeNWSECursor = Interop::win32_LoadSystemCursor(IDC_SIZENWSE);
        GWindowsPlatformStatics->SizeNESWCursor = Interop::win32_LoadSystemCursor(IDC_SIZENESW);
        GWindowsPlatformStatics->SizeWECursor = Interop::win32_LoadSystemCursor(IDC_SIZEWE);
        GWindowsPlatformStatics->SizeNSCursor = Interop::win32_LoadSystemCursor(IDC_SIZENS);
        GWindowsPlatformStatics->SizeAllCursor = Interop::win32_LoadSystemCursor(IDC_SIZEALL);

        GPlatformStatics->ApplicationStartupTime = GetCurrentDateTime();
        GPlatformStatics->Processor = DetermineProcessorProperties();

        Interop::win32_string_buffer<wchar_t, 512> buffer{};

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
                        GPlatformStatics->SystemVersion = fmt::format(
                            "{}.{}.{}.{}",
                            HIWORD(pFileInfo->dwFileVersionMS),
                            LOWORD(pFileInfo->dwFileVersionMS),
                            HIWORD(pFileInfo->dwFileVersionLS),
                            LOWORD(pFileInfo->dwFileVersionLS));
                    }
                }
            }
        }

        if (GPlatformStatics->SystemVersion.empty())
        {
            GPlatformStatics->SystemVersion = "Unknown";
        }

        // Determine system ID
        Interop::win32_QueryRegistry(buffer, HKEY_LOCAL_MACHINE, LR"(Software\Microsoft\Cryptography)", LR"(MachineGuid)");
        Interop::win32_NarrowString(GPlatformStatics->SystemId, buffer.as_view());

        // Executable path
        Interop::win32_QueryFullProcessImageName(buffer);
        Interop::win32_NarrowString(GPlatformStatics->ExecutablePath, buffer.as_view());
        Path::NormalizeDirectorySeparators(GPlatformStatics->ExecutablePath);

        // Startup path
        Interop::win32_GetCurrentDirectory(buffer);
        Interop::win32_NarrowString(GPlatformStatics->StartupPath, buffer.as_view());
        Path::NormalizeDirectorySeparators(GPlatformStatics->StartupPath);

        // Computer name
        Interop::win32_GetComputerName(buffer);
        Interop::win32_NarrowString(GPlatformStatics->ComputerName, buffer.as_view());

        // User name
        Interop::win32_GetUserName(buffer);
        Interop::win32_NarrowString(GPlatformStatics->UserName, buffer.as_view());

        // Profile path
        Interop::win32_GetKnownFolderPath(FOLDERID_Profile, [&](std::wstring_view value)
        {
            Interop::win32_NarrowString(GPlatformStatics->ProfilePath, value);
        });
        Path::NormalizeDirectorySeparators(GPlatformStatics->ProfilePath);

        // Desktop path
        Interop::win32_GetKnownFolderPath(FOLDERID_Desktop, [&](std::wstring_view value)
        {
            Interop::win32_NarrowString(GPlatformStatics->DesktopPath, value);
        });
        Path::NormalizeDirectorySeparators(GPlatformStatics->DesktopPath);

        // Documents path
        Interop::win32_GetKnownFolderPath(FOLDERID_Documents, [&](std::wstring_view value)
        {
            Interop::win32_NarrowString(GPlatformStatics->DocumentsPath, value);
        });
        Path::NormalizeDirectorySeparators(GPlatformStatics->DocumentsPath);

        // Downloads path
        Interop::win32_GetKnownFolderPath(FOLDERID_Downloads, [&](std::wstring_view value)
        {
            Interop::win32_NarrowString(GPlatformStatics->DownloadsPath, value);
        });
        Path::NormalizeDirectorySeparators(GPlatformStatics->DownloadsPath);

        // Temp path
        Interop::win32_GetTempPath(buffer);
        Interop::win32_NarrowString(GPlatformStatics->TemporaryPath, buffer.as_view());
        Path::NormalizeDirectorySeparators(GPlatformStatics->TemporaryPath);

        // Register window class
        WNDCLASSEXW wc{
            .cbSize = sizeof(wc),
            .style = CS_DBLCLKS,
            .lpfnWndProc = WindowsWindow::WndProc,
            .cbClsExtra = 0,
            .cbWndExtra = 0,
            .hInstance = GWindowsPlatformStatics->InstanceHandle,
            .hIcon = GWindowsPlatformStatics->ApplicationIcon,
            .hCursor = GWindowsPlatformStatics->ArrowCursor,
            .hbrBackground = GetSysColorBrush(COLOR_WINDOW),
            .lpszMenuName = nullptr,
            .lpszClassName = L"AnemoneWindow",
            .hIconSm = GWindowsPlatformStatics->ApplicationIcon,
        };

        GWindowsPlatformStatics->MainWindowClass = RegisterClassExW(&wc);

        if (GWindowsPlatformStatics->MainWindowClass == 0)
        {
            ReportApplicationStop("Failed to register window class.");
        }

        std::string buildMachine{};

        if (Platform::GetEnvironmentVariable(buildMachine, "ANEMONE_BUILD_MACHINE"))
        {
            _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
            _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
            _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
            _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
        }
    }

    void Finalize()
    {
        UnregisterClassW(
            MAKEINTATOM(GWindowsPlatformStatics->MainWindowClass),
            GWindowsPlatformStatics->InstanceHandle);

        if (WSACleanup() != 0)
        {
            ReportApplicationStop("Failed to finalize networking.");
        }

        // Finalize COM
        CoUninitialize();

        GWindowsInputStatics.Destroy();
        GWindowsPlatformStatics.Destroy();
        GPlatformStatics.Destroy();
    }
}

namespace Anemone::Platform
{
#if false
        {
            // 'HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion'
            //  - 'ProductId'
            //  - 'LCUVer' - version
            //  - 'ProductName' - win10 pro etc
        }

#endif

    bool GetClipboardText(std::string& result)
    {
        result.clear();

        bool success = false;

        if (OpenClipboard(GetActiveWindow()) != FALSE)
        {
            if (HGLOBAL const hUnicodeText = GetClipboardData(CF_UNICODETEXT); hUnicodeText != nullptr)
            {
                wchar_t const* const data = static_cast<wchar_t const*>(GlobalLock(hUnicodeText));
                if (data != nullptr)
                {
                    Interop::win32_NarrowString(result, data);
                    GlobalUnlock(hUnicodeText);
                    success = true;
                }
            }
            else if (HGLOBAL const hText = GetClipboardData(CF_TEXT); hText != nullptr)
            {
                char const* const data = static_cast<char const*>(GlobalLock(hText));
                if (data != nullptr)
                {
                    result = data;
                    GlobalUnlock(hText);
                    success = true;
                }
            }

            CloseClipboard();
        }

        return success;
    }

    bool SetClipboardText(std::string_view value)
    {
        bool success = false;

        if (OpenClipboard(GetActiveWindow()) != FALSE)
        {
            EmptyClipboard();

            Interop::win32_string_buffer<wchar_t, 512> buffer{};
            Interop::win32_WidenString(buffer, value);
            std::wstring_view const data = buffer.as_view();

            if (HGLOBAL const handle = GlobalAlloc(GMEM_MOVEABLE, data.size() * sizeof(wchar_t)); handle != nullptr)
            {
                if (void* const destination = GlobalLock(handle); destination != nullptr)
                {
                    // Copy zero terminated string.
                    std::memcpy(destination, data.data(), data.size() * sizeof(wchar_t));
                    GlobalUnlock(handle);

                    if (SetClipboardData(CF_UNICODETEXT, handle) != nullptr)
                    {
                        success = true;
                    }
                }
            }

            CloseClipboard();
        }

        return success;
    }

    bool GetEnvironmentVariable(std::string& result, std::string_view name)
    {
        result.clear();
        Interop::win32_string_buffer<wchar_t, 128> wname{};
        Interop::win32_WidenString(wname, name);

        if (Interop::win32_string_buffer<wchar_t, 512> wresult{};
            Interop::win32_GetEnvironmentVariable(wresult, wname))
        {
            Interop::win32_NarrowString(result, wresult.as_view());
            return true;
        }

        return false;
    }

    bool SetEnvironmentVariable(std::string_view name, std::string_view value)
    {
        Interop::win32_string_buffer<wchar_t, 128> wname{};
        Interop::win32_WidenString(wname, name);

        Interop::win32_string_buffer<wchar_t, 512> wvalue{};
        Interop::win32_WidenString(wvalue, value);

        return SetEnvironmentVariableW(wname, wvalue) != FALSE;
    }

    bool RemoveEnvironmentVariable(std::string_view name)
    {
        Interop::win32_string_buffer<wchar_t, 128> wname{};
        Interop::win32_WidenString(wname, name);

        return SetEnvironmentVariableW(wname, nullptr) != FALSE;
    }

    std::vector<ProcessorTopology> GetProcessorTopology()
    {
        AE_PANIC("Not implemented");
    }

    namespace
    {
        BOOL CALLBACK FillDisplayDevicesInformation(
            HMONITOR handle,
            HDC,
            LPRECT,
            LPARAM context) noexcept
        {
            DisplayMetrics& displayMetrics = *reinterpret_cast<DisplayMetrics*>(context); // NOLINT(performance-no-int-to-ptr)

            MONITORINFOEXW miex{};
            miex.cbSize = sizeof(miex);

            GetMonitorInfoW(handle, &miex);

            Interop::win32_string_buffer<char, 128> name{};
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
    }

    void GetDisplayMetrics(DisplayMetrics& displayMetrics)
    {
        displayMetrics.Displays.clear();

        // Get display size.
        displayMetrics.PrimaryDisplaySize = Math::SizeF{
            .Width = static_cast<float>(GetSystemMetrics(SM_CXSCREEN)),
            .Height = static_cast<float>(GetSystemMetrics(SM_CYSCREEN)),
        };

        RECT workArea = {-1, -1, -1, -1};

        if (SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0) == FALSE)
        {
            workArea = {};
        }

        // Get workspace area.
        displayMetrics.PrimaryDisplayWorkArea = Math::RectF{
            .X = static_cast<float>(workArea.left),
            .Y = static_cast<float>(workArea.top),
            .Width = static_cast<float>(workArea.right - workArea.left),
            .Height = static_cast<float>(workArea.bottom - workArea.top),
        };

        // Virtual display rect
        displayMetrics.VirtualDisplayRect = Math::RectF{
            .X = static_cast<float>(GetSystemMetrics(SM_XVIRTUALSCREEN)),
            .Y = static_cast<float>(GetSystemMetrics(SM_YVIRTUALSCREEN)),
            .Width = static_cast<float>(GetSystemMetrics(SM_CXVIRTUALSCREEN)),
            .Height = static_cast<float>(GetSystemMetrics(SM_CYVIRTUALSCREEN)),
        };

        // Get displays
        displayMetrics.Displays.clear();

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

            DisplayInfo& display = displayMetrics.Displays.emplace_back();
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
                reinterpret_cast<LPARAM>(&displayMetrics));
        }
    }

    ColorRef GetScreenPixel(Math::PointF position, float gamma)
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

    std::string_view GetOperatingSystem()
    {
        AE_PANIC("Not implemented");
    }

    Duration GetSystemUptime()
    {
        return Duration::FromMilliseconds(static_cast<int64_t>(GetTickCount64()));
    }

    MemoryProperties GetMemoryProperties()
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

    MemoryUsage GetMemoryUsage()
    {
        MEMORYSTATUSEX memoryStatus{.dwLength = sizeof(memoryStatus)};

        if (!GlobalMemoryStatusEx(&memoryStatus))
        {
            AE_PANIC("GlobalMemoryStatusEx: {}", GetLastError());
        }

        PROCESS_MEMORY_COUNTERS memoryCounters{.cb = sizeof(memoryCounters)};
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

    PowerUsage GetPowerUsage()
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

    ProcessorUsage GetProcessorUsage()
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

    void Terminate(bool force)
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

    std::string_view GetDeviceUniqueId()
    {
        AE_PANIC("Not implemented");
    }

    std::string_view GetDeviceName()
    {
        AE_PANIC("Not implemented");
    }

    std::string_view GetDeviceModel()
    {
        AE_PANIC("Not implemented");
    }

    DeviceType GetDeviceType()
    {
        AE_PANIC("Not implemented");
    }

    DeviceProperties GetDeviceProperties()
    {
        AE_PANIC("Not implemented");
    }

    AssertAction HandleAssertion(std::source_location const& location, std::string_view expression, std::string_view message)
    {
        constexpr std::array<TASKDIALOG_BUTTON, 3> buttons{
            {
                {
                    .nButtonID = static_cast<int>(AssertAction::Abort),
                    .pszButtonText = L"&Abort",
                },
                {
                    .nButtonID = static_cast<int>(AssertAction::Debug),
                    .pszButtonText = L"&Debug",
                },
                {
                    .nButtonID = static_cast<int>(AssertAction::Continue),
                    .pszButtonText = L"&Continue",
                },
            }};

        fmt::memory_buffer buffer{};
        auto out = std::back_inserter(buffer);
        out = fmt::format_to(out, "location: {}:{}\n\n", location.file_name(), location.line());
        out = fmt::format_to(out, "expression: {}\n\n", expression);
        out = fmt::format_to(out, "function: {}\n\n", location.function_name());
        out = fmt::format_to(out, "message: {}", message);
        std::string_view viewBuffer{buffer.data(), buffer.size()};

        using namespace Anemone::Interop;
        win32_string_buffer<wchar_t, 512> wContent{};
        win32_WidenString(wContent, viewBuffer);

        TASKDIALOGCONFIG tgc{};
        tgc.cbSize = sizeof(tgc);
        tgc.hInstance = GetModuleHandleW(nullptr);
        tgc.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION | TDF_SIZE_TO_CONTENT;
        tgc.pszWindowTitle = L"Assertion Failed";
        tgc.pszMainIcon = TD_ERROR_ICON;
        tgc.pszMainInstruction = L"Assertion Failed";
        tgc.pszContent = wContent;
        tgc.cButtons = static_cast<UINT>(buttons.size());
        tgc.pButtons = buttons.data();
        tgc.nDefaultButton = static_cast<int>(AssertAction::Debug);

        INT result{};
        if (FAILED(TaskDialogIndirect(&tgc, &result, nullptr, nullptr)))
        {
            return AssertAction::Abort;
        }

        return static_cast<AssertAction>(result);
    }

    void HandlePanic()
    {
        TASKDIALOGCONFIG tgc{};
        tgc.cbSize = sizeof(TASKDIALOGCONFIG);
        tgc.hwndParent = nullptr;
        tgc.hInstance = GetModuleHandleW(nullptr);
        tgc.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION | TDF_SIZE_TO_CONTENT;
        tgc.pszWindowTitle = L"Critical Error";
        tgc.pszMainIcon = TD_ERROR_ICON;
        tgc.pszMainInstruction = L"Critical Error Occured";
        tgc.pszContent = L"Application will stop. Please provide crash dump and logs";
        tgc.dwCommonButtons = TDCBF_OK_BUTTON;

        TaskDialogIndirect(&tgc, nullptr, nullptr, nullptr);
    }

    // Diagnostics
    void DebugBreak()
    {
        anemone_debugbreak();
    }

    void Crash()
    {
#if !ANEMONE_BUILD_SHIPPING
        anemone_debugbreak();
#endif

        __fastfail(FAST_FAIL_FATAL_APP_EXIT);
    }

    bool IsDebuggerAttached()
    {
#if ANEMONE_BUILD_SHIPPING
        return false;
#else
        return IsDebuggerPresent() != FALSE;
#endif
    }

    void WaitForDebugger()
    {
        while (not IsDebuggerAttached())
        {
            SleepEx(100, TRUE);
        }
    }

    bool AttachDebugger()
    {
#if ANEMONE_BUILD_SHIPPING
        return false;
#elif ANEMONE_FEATURE_JITDEBUGGER
        if (::IsDebuggerPresent())
        {
            return false;
        }

        wchar_t commandLine[MAX_PATH + 1];
        swprintf_s(commandLine, L"vsjitdebugger.exe -p %lu", GetCurrentProcessId());

        STARTUPINFOW si{
            .cb = sizeof(si),
            .lpReserved = nullptr,
            .lpDesktop = nullptr,
            .lpTitle = nullptr,
            .dwX = static_cast<DWORD>(CW_USEDEFAULT),
            .dwY = static_cast<DWORD>(CW_USEDEFAULT),
            .dwXSize = static_cast<DWORD>(CW_USEDEFAULT),
            .dwYSize = static_cast<DWORD>(CW_USEDEFAULT),
            .dwXCountChars = 0,
            .dwYCountChars = 0,
            .dwFillAttribute = 0,
            .dwFlags = 0,
            .wShowWindow = 0,
            .cbReserved2 = 0,
            .lpReserved2 = nullptr,
            .hStdInput = nullptr,
            .hStdOutput = nullptr,
            .hStdError = nullptr,
        };
        PROCESS_INFORMATION pi{};

        if (CreateProcessW(
                nullptr,
                commandLine,
                nullptr,
                nullptr,
                FALSE,
                0,
                nullptr,
                nullptr,
                &si,
                &pi))
        {
            WaitForSingleObject(pi.hProcess, INFINITE);
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);

            return true;
        }

        return false;
#else
        return false;
#endif
    }

    DateTime GetCurrentDateTime()
    {
        FILETIME ft;
        Interop::win32_GetLocalTimeAsFileTime(ft);
        return Interop::win32_into_DateTime(ft);
    }

    DateTime GetCurrentDateTimeUtc()
    {
        FILETIME ft;
        GetSystemTimePreciseAsFileTime(&ft);

        return Interop::win32_into_DateTime(ft);
    }

    Duration GetCurrentTimeZoneBias()
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

    void GetRandom(std::span<std::byte> buffer)
    {
        NTSTATUS status = BCryptGenRandom(
            nullptr,
            reinterpret_cast<PUCHAR>(buffer.data()),
            static_cast<ULONG>(buffer.size()),
            BCRYPT_USE_SYSTEM_PREFERRED_RNG);

        AE_ENSURE(BCRYPT_SUCCESS(status));
    }

    void ProcessMessages()
    {
        // Process messages.
        MSG message{};

        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        // Pool input devices.
        Internal::GWindowsInputStatics->Poll();
    }
}
