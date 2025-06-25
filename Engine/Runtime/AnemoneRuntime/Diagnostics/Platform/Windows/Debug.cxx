#include "AnemoneRuntime/Diagnostics/Platform/Windows/Debug.hxx"
#include "AnemoneRuntime/Interop/Windows/Text.hxx"
#include "AnemoneRuntime/Interop/Windows/MemoryMappedFile.hxx"

#include <format>

namespace Anemone::Diagnostics
{
    namespace
    {
        struct CrashDetails
        {
            CHAR BuildId[64];
            DWORD ProcessId;
            DWORD ThreadId;
            EXCEPTION_RECORD ExceptionRecord;
            CONTEXT Context;
            PVOID XmmRegisters;
            PVOID YmmRegisters;
            PVOID ZmmRegisters;
            PVOID ZmmhRegisters;
            DWORD XmmRegistersLength;
            DWORD YmmRegistersLength;
            DWORD ZmmRegistersLength;
            DWORD ZmmhRegistersLength;
        };
    }
}

namespace Anemone::Diagnostics
{
    namespace
    {
        void HandleCrash(PEXCEPTION_POINTERS pExceptionPointers)
        {
            static SRWLOCK lock = SRWLOCK_INIT;

            AcquireSRWLockExclusive(&lock);

            if (IsDebuggerPresent())
            {
                // Debugger attached, let it handle the crash.
                ReleaseSRWLockExclusive(&lock);
                return;
            }

            HANDLE const hProcess = GetCurrentProcess();
            HANDLE const hThread = GetCurrentThread();

            DWORD const dwProcessId = GetProcessId(hProcess);
            DWORD const dwThreadId = GetThreadId(hThread);

            CrashDetails crashDetails{};
            std::string_view{ANEMONE_ENGINE_BUILD_ID}.copy(crashDetails.BuildId, sizeof(crashDetails.BuildId) - 1);

            crashDetails.ProcessId = dwProcessId;
            crashDetails.ThreadId = dwThreadId;

            if (pExceptionPointers != nullptr)
            {
                crashDetails.ExceptionRecord = *pExceptionPointers->ExceptionRecord;
                crashDetails.Context = *pExceptionPointers->ContextRecord;

#if ANEMONE_ARCHITECTURE_X64
                crashDetails.XmmRegisters = LocateXStateFeature(pExceptionPointers->ContextRecord, XSTATE_LEGACY_SSE, &crashDetails.XmmRegistersLength);
                crashDetails.YmmRegisters = LocateXStateFeature(pExceptionPointers->ContextRecord, XSTATE_AVX, &crashDetails.YmmRegistersLength);
                crashDetails.ZmmRegisters = LocateXStateFeature(pExceptionPointers->ContextRecord, XSTATE_AVX512_ZMM, &crashDetails.ZmmRegistersLength);
                crashDetails.ZmmhRegisters = LocateXStateFeature(pExceptionPointers->ContextRecord, XSTATE_AVX512_ZMM_H, &crashDetails.ZmmhRegistersLength);
#endif
            }

            {
                wchar_t name[MAX_PATH + 1];
                auto nameFormatResult = std::format_to_n(
                    name,
                    MAX_PATH,
                    L"AnemoneCrashDetails_pid{}_tid{}",
                    dwProcessId,
                    dwThreadId);
                (*nameFormatResult.out) = L'\0';

                Interop::Windows::SafeMemoryMappedFileHandle fhMapping = Interop::Windows::OpenMemoryMappedFile(
                    Interop::Windows::SafeFileHandle{},
                    name,
                    sizeof(CrashDetails),
                    Interop::Windows::MemoryMappedFileAccess::ReadWrite);

                if (not fhMapping)
                {
                    Debug::ReportApplicationStop("Could not open memory mapped file");
                    return;
                }

                Interop::Windows::SafeMemoryMappedViewHandle fvMapping = Interop::Windows::CreateMemoryMappedView(
                    fhMapping,
                    Interop::Windows::MemoryMappedFileAccess::ReadWrite);

                if (not fvMapping)
                {
                    Debug::ReportApplicationStop("Could not create memory mapped view");
                    return;
                }

                CopyMemory(fvMapping.GetData(), &crashDetails, sizeof(CrashDetails));

                Interop::Windows::FlushMemoryMappedView(fvMapping);

                wchar_t commandLine[MAX_PATH + 1];
                auto commandLineFormatResult = std::format_to_n(
                    commandLine,
                    MAX_PATH,
                    L"AnemoneCrashReporter.exe --pid {} --tid {}",
                    dwProcessId,
                    dwThreadId);
                (*commandLineFormatResult.out) = L'\0';

                STARTUPINFOW startupInfo{};
                startupInfo.cb = sizeof(startupInfo);
                startupInfo.dwX = static_cast<DWORD>(CW_USEDEFAULT);
                startupInfo.dwY = static_cast<DWORD>(CW_USEDEFAULT);
                startupInfo.dwXSize = static_cast<DWORD>(CW_USEDEFAULT);
                startupInfo.dwYSize = static_cast<DWORD>(CW_USEDEFAULT);

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
                    Debug::ReportApplicationStop("Could not start AnemoneCrashReporter");
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
            TerminateProcess(hProcess, 66602137u);
        }
    }
}

#define ENABLE_HEAP_CORRUPTION_CRASHES false

namespace Anemone::Diagnostics
{
    namespace
    {
        PTOP_LEVEL_EXCEPTION_FILTER GPreviousExceptionFilter{};

        static LONG CALLBACK OnUnhandledExceptionFilter(LPEXCEPTION_POINTERS lpExceptionPointers)
        {
            if (lpExceptionPointers->ExceptionRecord->ExceptionCode == DBG_PRINTEXCEPTION_C)
            {
                return EXCEPTION_CONTINUE_EXECUTION;
            }

            HandleCrash(lpExceptionPointers);
            return EXCEPTION_CONTINUE_SEARCH;
        }

#if ENABLE_HEAP_CORRUPTION_CRASHES
        static LONG CALLBACK OnUnhandledExceptionVEH(LPEXCEPTION_POINTERS lpExceptionPointers)
        {
            if (lpExceptionPointers->ExceptionRecord->ExceptionCode == STATUS_HEAP_CORRUPTION)
            {
                HandleCrash(lpExceptionPointers);
            }

            return EXCEPTION_EXECUTE_HANDLER;
        }
#endif

        _purecall_handler GPureCallHandler{};

        void __cdecl OnPureCallHandler(void)
        {
            AE_PANIC("pure-call");
        }

        _invalid_parameter_handler GInvalidParameterHandler{};

        void __cdecl OnInvalidParameterHandler(
            wchar_t const*,
            wchar_t const*,
            wchar_t const*,
            unsigned int,
            uintptr_t)
        {
            AE_PANIC("Invalid parameter");
        }


        std::terminate_handler GTerminateHandler{};

        void __CRTDECL OnTerminateHandler(void)
        {
            AE_PANIC("Terminate handler");
        }
    }
}


namespace Anemone::Diagnostics
{
    void Debug::Break()
    {
        anemone_debugbreak();
    }

    void Debug::Crash()
    {
#if !ANEMONE_BUILD_SHIPPING
        anemone_debugbreak();
#endif

        __fastfail(FAST_FAIL_FATAL_APP_EXIT);
    }

    bool Debug::IsDebuggerAttached()
    {
#if ANEMONE_BUILD_SHIPPING
        return false;
#else
        return IsDebuggerPresent() != FALSE;
#endif
    }

    void Debug::WaitForDebugger()
    {
        while (not IsDebuggerPresent())
        {
            SleepEx(100, TRUE);
        }
    }

    bool Debug::AttachDebugger()
    {
#if ANEMONE_BUILD_SHIPPING
        return false;
#elif ANEMONE_FEATURE_JITDEBUGGER
        if (::IsDebuggerPresent())
        {
            return true;
        }

        wchar_t commandLine[MAX_PATH + 1];
        auto commandLineFormatResult = std::format_to_n(
            commandLine,
            MAX_PATH,
            L"vsjitdebugger.exe -p {}", GetCurrentProcessId());
        (*commandLineFormatResult.out) = L'\0';

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
            DWORD dwExit{};
            WaitForSingleObject(pi.hProcess, INFINITE);
            GetExitCodeProcess(pi.hProcess, &dwExit);

            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);

            return dwExit == 0;
        }

        return false;
#else
        return false;
#endif
    }

    void Debug::ReportApplicationStop(std::string_view reason)
    {
        Interop::string_buffer<wchar_t, 128> buffer{};
        Interop::Windows::WidenString(buffer, reason);

        MessageBoxW(
            nullptr,
            buffer,
            L"Application Stop",
            MB_OK | MB_ICONERROR);

        ExitProcess(static_cast<UINT>(-1));
    }
}

namespace Anemone::Internal
{
    extern void PlatformInitializeDebug()
    {
        // Initialize exception handlers
        Diagnostics::GPreviousExceptionFilter = SetUnhandledExceptionFilter(Diagnostics::OnUnhandledExceptionFilter);

#if defined(_MSC_VER)
        Diagnostics::GPureCallHandler = _set_purecall_handler(Diagnostics::OnPureCallHandler);
        Diagnostics::GInvalidParameterHandler = _set_invalid_parameter_handler(Diagnostics::OnInvalidParameterHandler);
#endif

        Diagnostics::GTerminateHandler = std::set_terminate(Diagnostics::OnTerminateHandler);

#if !defined(NDEBUG) && defined(_MSC_VER)
        //
        // Enable CRT debugging facilities.
        //

        auto crtdbg = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
        crtdbg |= _CRTDBG_ALLOC_MEM_DF;
        crtdbg |= _CRTDBG_CHECK_ALWAYS_DF;
        crtdbg |= _CRTDBG_CHECK_CRT_DF;
        crtdbg |= _CRTDBG_DELAY_FREE_MEM_DF;
        crtdbg |= _CRTDBG_LEAK_CHECK_DF;

        if constexpr (true)
        {
            // Enable checking each 1024 allocations
            crtdbg &= ~_CRTDBG_CHECK_ALWAYS_DF;
            crtdbg &= 0x0000FFFF;
            crtdbg |= _CRTDBG_CHECK_EVERY_1024_DF;
        }

        _CrtSetDbgFlag(crtdbg);

        _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
        _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
#endif

#if ENABLE_HEAP_CORRUPTION_CRASHES
        AddVectoredExceptionHandler(0, Diagnostics::OnUnhandledExceptionVEH);
#endif
    }

    extern void PlatformFinalizeDebug()
    {
        // Restore exception handlers.
#if ENABLE_HEAP_CORRUPTION_CRASHES
        RemoveVectoredExceptionHandler(OnUnhandledExceptionVEH);
#endif

#if ANEMONE_COMPILER_MSVC
        _set_purecall_handler(Diagnostics::GPureCallHandler);
        _set_invalid_parameter_handler(Diagnostics::GInvalidParameterHandler);
#endif

        std::set_terminate(Diagnostics::GTerminateHandler);

        SetUnhandledExceptionFilter(Diagnostics::GPreviousExceptionFilter);
    }
}
