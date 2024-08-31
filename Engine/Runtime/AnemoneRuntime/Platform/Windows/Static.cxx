#include "AnemoneRuntime/Platform/Static.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

#include <locale>

namespace Anemone::Platform
{
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

    struct CrashDetails
    {
        DWORD ProcessId;
        DWORD ThreadId;
        EXCEPTION_RECORD ExceptionRecord;
        CONTEXT Context;
    };

    struct Mapping
    {
        HANDLE Handle;
        void* Buffer;

        Mapping(DWORD processId, DWORD threadId)
        {
            wchar_t name[MAX_PATH];
            swprintf_s(name, L"AnemoneCrashDetails_pid%u_tid%u", static_cast<UINT>(processId), static_cast<UINT>(threadId));

            Handle = CreateFileMappingW(
                INVALID_HANDLE_VALUE,
                nullptr,
                PAGE_READWRITE,
                0,
                sizeof(CrashDetails),
                name);
            if (Handle == nullptr)
            {
                return;
            }

            Buffer = MapViewOfFile(Handle, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(CrashDetails));
        }

        ~Mapping()
        {
            if (Buffer != nullptr)
            {
                UnmapViewOfFile(Buffer);
            }

            if (Handle != nullptr)
            {
                CloseHandle(Handle);
            }
        }

        void Write(CrashDetails const& crashDetails)
        {
            if (Buffer != nullptr)
            {
                CopyMemory(Buffer, &crashDetails, sizeof(CrashDetails));
            }

            FlushViewOfFile(Buffer, sizeof(CrashDetails));
        }
    };

    static void HandleCrash(EXCEPTION_POINTERS* pExceptionPointers)
    {
        static SRWLOCK lock = SRWLOCK_INIT;

        AcquireSRWLockExclusive(&lock);

        HANDLE const hProcess = GetCurrentProcess();
        HANDLE const hThread = GetCurrentThread();

        DWORD const dwProcessId = GetProcessId(hProcess);
        DWORD const dwThreadId = GetThreadId(hThread);

        CrashDetails crashDetails{};
        crashDetails.ProcessId = dwProcessId;
        crashDetails.ThreadId = dwThreadId;

        if (pExceptionPointers != nullptr)
        {
            crashDetails.ExceptionRecord = *pExceptionPointers->ExceptionRecord;
            crashDetails.Context = *pExceptionPointers->ContextRecord;
        }

        {
            Mapping mapping{dwProcessId, dwThreadId};
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

    static LONG CALLBACK windows_OnUnhandledExceptionFilter(LPEXCEPTION_POINTERS lpExceptionPointers)
    {
        HandleCrash(lpExceptionPointers);
        return EXCEPTION_CONTINUE_SEARCH;
    }

    static LONG CALLBACK windows_OnUnhandledExceptionVEH(LPEXCEPTION_POINTERS lpExceptionPointers)
    {
        if (lpExceptionPointers->ExceptionRecord->ExceptionCode == STATUS_HEAP_CORRUPTION)
        {
            HandleCrash(lpExceptionPointers);
        }

        return EXCEPTION_EXECUTE_HANDLER;
    }

    static void __cdecl windows_OnTerminate()
    {
        HandleCrash(nullptr);
    }

    static void __cdecl windows_OnPureCall()
    {
        HandleCrash(nullptr);
    }

    static void __cdecl windows_OnInvalidParameter(
        [[maybe_unused]] const wchar_t* expression,
        [[maybe_unused]] const wchar_t* function,
        [[maybe_unused]] const wchar_t* file,
        [[maybe_unused]] unsigned int line,
        [[maybe_unused]] uintptr_t reserved)
    {
        HandleCrash(nullptr);
    }
}

namespace Anemone::Platform
{
    void InitializeStatic()
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

        // Set locale.
        (void)std::setlocale(LC_ALL, "en_US.UTF-8"); // NOLINT(concurrency-mt-unsafe); this is invoked in main thread.

        VerifyRequirements();

        // Setup crash handling callbacks.
        SetUnhandledExceptionFilter(windows_OnUnhandledExceptionFilter);
        AddVectoredExceptionHandler(0, windows_OnUnhandledExceptionVEH);

        _set_error_mode(_OUT_TO_STDERR);
        std::set_terminate(windows_OnTerminate);
        _set_purecall_handler(windows_OnPureCall);
        _set_invalid_parameter_handler(windows_OnInvalidParameter);
    }

    void ShutdownStatic()
    {
        // Finalize COM
        CoUninitialize();
    }
}
