#include "AnemoneRuntime/Platform/Runtime.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

#include <locale>

namespace Anemone::Platform
{
    bool GEnableCrashReportHandling = true;

    void SetCrashReportHandling(bool enable)
    {
        GEnableCrashReportHandling = enable;
    }

    static void ReportApplicationStop(std::string_view reason)
    {
        Platform::win32_string_buffer<wchar_t, 128> buffer{};
        Platform::win32_WidenString(buffer, reason);

        MessageBoxW(
            nullptr,
            buffer,
            L"Application Stop",
            MB_OK | MB_ICONERROR);

        ExitProcess(static_cast<UINT>(-1));
    }

    static bool IsProcessEmulated()
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

    static void VerifyRequirements()
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

    static void HandleCrash()
    {
        static SRWLOCK lock = SRWLOCK_INIT;

        AcquireSRWLockExclusive(&lock);

        HANDLE const hProcess = GetCurrentProcess();
        HANDLE const hThread = GetCurrentThread();

        DWORD const dwProcessId = GetProcessId(hProcess);
        DWORD const dwThreadId = GetThreadId(hThread);

        wchar_t commandLine[512];
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
            .wShowWindow = SW_SHOWDEFAULT,
            .cbReserved2 = 0,
            .lpReserved2 = nullptr,
            .hStdInput = nullptr,
            .hStdOutput = nullptr,
            .hStdError = nullptr,
        };
        PROCESS_INFORMATION processInformation{};

        BOOL bCreated = CreateProcessW(
            L"AnemoneCrashReporter.exe",
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
            CloseHandle(processInformation.hThread);

            // Wait until crash reporter finishes
            WaitForSingleObject(processInformation.hProcess, INFINITE);
            CloseHandle(processInformation.hProcess);
        }

        ReleaseSRWLockExclusive(&lock);

        // Terminate process immediately
        TerminateProcess(hProcess, static_cast<UINT>(-1));
    }

    static LONG CALLBACK windows_OnUnhandledExceptionFilter(LPEXCEPTION_POINTERS)
    {
        HandleCrash();
        return EXCEPTION_CONTINUE_SEARCH;
    }

    static LONG CALLBACK windows_OnUnhandledExceptionVEH(LPEXCEPTION_POINTERS lpExceptionPointers)
    {
        if (lpExceptionPointers->ExceptionRecord->ExceptionCode == STATUS_HEAP_CORRUPTION)
        {
            HandleCrash();
        }

        return EXCEPTION_EXECUTE_HANDLER;
    }

    static void __cdecl windows_OnTerminate()
    {
        HandleCrash();
    }

    static void __cdecl windows_OnPureCall()
    {
        HandleCrash();
    }

    static void __cdecl windows_OnInvalidParameter(
        [[maybe_unused]] const wchar_t* expression,
        [[maybe_unused]] const wchar_t* function,
        [[maybe_unused]] const wchar_t* file,
        [[maybe_unused]] unsigned int line,
        [[maybe_unused]] uintptr_t reserved)
    {
        HandleCrash();
    }

    void InitializeRuntime_PlatformSpecific()
    {
        // Initialize COM
        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

        // Initialize Networking.
        WSADATA wsaData{};
        WSAStartup(MAKEWORD(2, 2), &wsaData);

        // Initialize DPI awareness.
        SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        // Enable UTF8 console.
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);

        // When process hangs, DWM will ghost the window. This is not desirable for us.
        DisableProcessWindowsGhosting();

        // Set locale.
        (void)std::setlocale(LC_ALL, "en_US.UTF-8"); // NOLINT(concurrency-mt-unsafe); this is invoked in main thread.

        VerifyRequirements();

        if (GEnableCrashReportHandling)
        {
            SetUnhandledExceptionFilter(windows_OnUnhandledExceptionFilter);
            AddVectoredExceptionHandler(0, windows_OnUnhandledExceptionVEH);

            _set_error_mode(_OUT_TO_STDERR);
            std::set_terminate(windows_OnTerminate);
            _set_purecall_handler(windows_OnPureCall);
            _set_invalid_parameter_handler(windows_OnInvalidParameter);
        }
    }

    void FinalizeRuntime_PlatformSpecific()
    {
        // Finalize Networking.
        WSACleanup();

        // Finalize COM
        CoUninitialize();
    }
}
