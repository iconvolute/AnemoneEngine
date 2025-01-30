#include "AnemoneRuntime/Platform/Windows/WindowsPlatform.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsDebugger.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsEnvironment.hxx"

#include "AnemoneRuntime/System/Path.hxx"

#include <clocale>

#define ENABLE_HEAP_CORRUPTION_CRASHES false

namespace Anemone::Internal
{
    UninitializedObject<WindowsPlatformStatics> GWindowsPlatformStatics;

    namespace
    {
        void VerifyRequirements()
        {
            if (Debugger::IsProcessEmulated())
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

        void SetupConsoleMode(HANDLE hStream)
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

        ProcessorProperties DetermineProcessorProperties()
        {
            ProcessorProperties result{};

            {

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
            }
            {
                // Determine Cache properties.
                DWORD dwSize{};
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
            }

            // Query CPU name from registry
            Interop::string_buffer<wchar_t, 128> buffer{};

            Interop::win32_QueryRegistry(buffer, HKEY_LOCAL_MACHINE, LR"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)", LR"(ProcessorNameString)");
            Interop::win32_NarrowString(result.Name, buffer.as_view());

            Interop::win32_QueryRegistry(buffer, HKEY_LOCAL_MACHINE, LR"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)", LR"(VendorIdentifier)");
            Interop::win32_NarrowString(result.Vendor, buffer.as_view());

            return result;
        }

        LONG CALLBACK OnUnhandledExceptionFilter(LPEXCEPTION_POINTERS lpExceptionPointers)
        {
            if (lpExceptionPointers->ExceptionRecord->ExceptionCode == DBG_PRINTEXCEPTION_C)
            {
                return EXCEPTION_CONTINUE_EXECUTION;
            }

            Debugger::HandleCrash(lpExceptionPointers);
            return EXCEPTION_CONTINUE_SEARCH;
        }

#if ENABLE_HEAP_CORRUPTION_CRASHES
        LONG CALLBACK OnUnhandledExceptionVEH(LPEXCEPTION_POINTERS lpExceptionPointers)
        {
            if (lpExceptionPointers->ExceptionRecord->ExceptionCode == STATUS_HEAP_CORRUPTION)
            {
                Debugger::HandleCrash(lpExceptionPointers);
            }

            return EXCEPTION_EXECUTE_HANDLER;
        }
#endif
    }
}

namespace Anemone
{
    void WindowsPlatform::Initialize()
    {
        // Initialize COM
        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

        // Initialize DPI awareness.
        SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        // Enable UTF8 console.
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);

        // When process hangs, DWM will ghost the window. This is not desirable for us.
        DisableProcessWindowsGhosting();

        Internal::SetupConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE));
        Internal::SetupConsoleMode(GetStdHandle(STD_ERROR_HANDLE));

        SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);

        // Set locale.
        (void)std::setlocale(LC_ALL, "en_US.UTF-8"); // NOLINT(concurrency-mt-unsafe); this is invoked in main thread.

        Internal::VerifyRequirements();

        // Setup crash handling callbacks.
        SetUnhandledExceptionFilter(Internal::OnUnhandledExceptionFilter);

#if ENABLE_HEAP_CORRUPTION_CRASHES
        AddVectoredExceptionHandler(0, OnUnhandledExceptionVEH);
#endif

        _set_error_mode(_OUT_TO_STDERR);

        WSADATA wsaData{};
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            Debugger::ReportApplicationStop("Failed to initialize networking.");
        }

        Internal::GWindowsPlatformStatics.Create();
        Internal::GWindowsPlatformStatics->InstanceHandle = GetModuleHandleW(nullptr);

        Internal::GWindowsPlatformStatics->ApplicationStartupTime = Environment::GetCurrentDateTime();
        Internal::GWindowsPlatformStatics->Processor = Internal::DetermineProcessorProperties();

        // Setup network list manager.
        {
            HRESULT const hr = CoCreateInstance(
                CLSID_NetworkListManager,
                nullptr,
                CLSCTX_ALL,
                IID_PPV_ARGS(Internal::GWindowsPlatformStatics->NetworkListManager.GetAddressOf()));

            if (FAILED(hr))
            {
                Debugger::ReportApplicationStop("Failed to initialize network list manager.");
            }
        }

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
                        Internal::GWindowsPlatformStatics->SystemVersion = fmt::format(
                            "{}.{}.{}.{}",
                            HIWORD(pFileInfo->dwFileVersionMS),
                            LOWORD(pFileInfo->dwFileVersionMS),
                            HIWORD(pFileInfo->dwFileVersionLS),
                            LOWORD(pFileInfo->dwFileVersionLS));
                    }
                }
            }
        }

        if (Internal::GWindowsPlatformStatics->SystemVersion.empty())
        {
            Internal::GWindowsPlatformStatics->SystemVersion = "Unknown";
        }

        // Determine system ID
        Interop::win32_QueryRegistry(buffer, HKEY_LOCAL_MACHINE, LR"(Software\Microsoft\Cryptography)", LR"(MachineGuid)");
        Interop::win32_NarrowString(Internal::GWindowsPlatformStatics->SystemId, buffer.as_view());

        // Executable path
        Interop::win32_QueryFullProcessImageName(buffer);
        Interop::win32_NarrowString(Internal::GWindowsPlatformStatics->ExecutablePath, buffer.as_view());
        System::Path::NormalizeDirectorySeparators(Internal::GWindowsPlatformStatics->ExecutablePath);

        // Startup path
        Interop::win32_GetCurrentDirectory(buffer);
        Interop::win32_NarrowString(Internal::GWindowsPlatformStatics->StartupPath, buffer.as_view());
        System::Path::NormalizeDirectorySeparators(Internal::GWindowsPlatformStatics->StartupPath);

        // Computer name
        Interop::win32_GetComputerName(buffer);
        Interop::win32_NarrowString(Internal::GWindowsPlatformStatics->ComputerName, buffer.as_view());

        // User name
        Interop::win32_GetUserName(buffer);
        Interop::win32_NarrowString(Internal::GWindowsPlatformStatics->UserName, buffer.as_view());

        // Profile path
        Interop::win32_GetKnownFolderPath(FOLDERID_Profile, [&](std::wstring_view value)
        {
            Interop::win32_NarrowString(Internal::GWindowsPlatformStatics->ProfilePath, value);
        });
        System::Path::NormalizeDirectorySeparators(Internal::GWindowsPlatformStatics->ProfilePath);

        // Desktop path
        Interop::win32_GetKnownFolderPath(FOLDERID_Desktop, [&](std::wstring_view value)
        {
            Interop::win32_NarrowString(Internal::GWindowsPlatformStatics->DesktopPath, value);
        });
        System::Path::NormalizeDirectorySeparators(Internal::GWindowsPlatformStatics->DesktopPath);

        // Documents path
        Interop::win32_GetKnownFolderPath(FOLDERID_Documents, [&](std::wstring_view value)
        {
            Interop::win32_NarrowString(Internal::GWindowsPlatformStatics->DocumentsPath, value);
        });
        System::Path::NormalizeDirectorySeparators(Internal::GWindowsPlatformStatics->DocumentsPath);

        // Downloads path
        Interop::win32_GetKnownFolderPath(FOLDERID_Downloads, [&](std::wstring_view value)
        {
            Interop::win32_NarrowString(Internal::GWindowsPlatformStatics->DownloadsPath, value);
        });
        System::Path::NormalizeDirectorySeparators(Internal::GWindowsPlatformStatics->DownloadsPath);

        // Temp path
        Interop::win32_GetTempPath(buffer);
        Interop::win32_NarrowString(Internal::GWindowsPlatformStatics->TemporaryPath, buffer.as_view());
        System::Path::NormalizeDirectorySeparators(Internal::GWindowsPlatformStatics->TemporaryPath);

        Interop::win32_QueryRegistry(buffer, HKEY_LOCAL_MACHINE, LR"(HARDWARE\DESCRIPTION\System)", LR"(SystemBiosVersion)");
        Interop::win32_NarrowString(Internal::GWindowsPlatformStatics->DeviceId, buffer.as_view());

        Interop::win32_QueryRegistry(buffer, HKEY_LOCAL_MACHINE, LR"(HARDWARE\DESCRIPTION\System\BIOS)", LR"(SystemManufacturer)");
        Interop::win32_NarrowString(Internal::GWindowsPlatformStatics->DeviceManufacturer, buffer.as_view());

        Interop::win32_QueryRegistry(buffer, HKEY_LOCAL_MACHINE, LR"(HARDWARE\DESCRIPTION\System\BIOS)", LR"(SystemProductName)");
        Interop::win32_NarrowString(Internal::GWindowsPlatformStatics->DeviceName, buffer.as_view());

        Interop::win32_QueryRegistry(buffer, HKEY_LOCAL_MACHINE, LR"(HARDWARE\DESCRIPTION\System\BIOS)", LR"(SystemVersion)");
        Interop::win32_NarrowString(Internal::GWindowsPlatformStatics->DeviceVersion, buffer.as_view());

        std::string buildMachine{};

        if (Environment::GetEnvironmentVariable(buildMachine, "ANEMONE_BUILD_MACHINE"))
        {
            _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
            _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
            _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
            _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
        }
    }

    void WindowsPlatform::Finalize()
    {
        Internal::GWindowsPlatformStatics->NetworkListManager = nullptr;

        if (WSACleanup() != 0)
        {
            Debugger::ReportApplicationStop("Failed to finalize networking.");
        }

        // Finalize COM
        CoUninitialize();

        Internal::GWindowsPlatformStatics.Destroy();
    }
}
