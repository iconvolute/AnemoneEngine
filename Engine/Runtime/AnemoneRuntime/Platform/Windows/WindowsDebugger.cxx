#include "AnemoneRuntime/Platform/Windows/WindowsDebugger.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsCrashHandler.hxx"

#include <iterator>

namespace Anemone
{
    void Debugger::Break()
    {
        anemone_debugbreak();
    }

    void Debugger::Crash()
    {
#if !ANEMONE_BUILD_SHIPPING
        anemone_debugbreak();
#endif

        __fastfail(FAST_FAIL_FATAL_APP_EXIT);
    }

    bool Debugger::IsAttached()
    {
#if ANEMONE_BUILD_SHIPPING
        return false;
#else
        return IsDebuggerPresent() != FALSE;
#endif
    }

    void Debugger::Wait()
    {
        while (not IsDebuggerPresent())
        {
            SleepEx(100, TRUE);
        }
    }

    bool Debugger::Attach()
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

    void Debugger::ReportApplicationStop(std::string_view reason)
    {
        Interop::string_buffer<wchar_t, 128> buffer{};
        Interop::win32_WidenString(buffer, reason);

        MessageBoxW(
            nullptr,
            buffer,
            L"Application Stop",
            MB_OK | MB_ICONERROR);

        ExitProcess(static_cast<UINT>(-1));
    }
}

namespace Anemone
{
    bool WindowsDebugger::IsProcessEmulated()
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

    void WindowsDebugger::HandleCrash(PEXCEPTION_POINTERS pExceptionPointers)
    {
        static SRWLOCK lock = SRWLOCK_INIT;

        AcquireSRWLockExclusive(&lock);

        Debugger::Attach();

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

#if ANEMONE_ARCHITECTURE_X64
            crashDetails.XmmRegisters = LocateXStateFeature(pExceptionPointers->ContextRecord, XSTATE_LEGACY_SSE, &crashDetails.XmmRegistersLength);
            crashDetails.YmmRegisters = LocateXStateFeature(pExceptionPointers->ContextRecord, XSTATE_AVX, &crashDetails.YmmRegistersLength);
            crashDetails.ZmmRegisters = LocateXStateFeature(pExceptionPointers->ContextRecord, XSTATE_AVX512_ZMM, &crashDetails.ZmmRegistersLength);
            crashDetails.ZmmhRegisters = LocateXStateFeature(pExceptionPointers->ContextRecord, XSTATE_AVX512_ZMM_H, &crashDetails.ZmmhRegistersLength);
#endif
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
                Debugger::ReportApplicationStop("Could not format command line");
            }

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
                Debugger::ReportApplicationStop("Could not start AnemoneCrashReporter");
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

    void WindowsDebugger::PrintRaw(const char* message)
    {
        OutputDebugStringA(message);
    }

    void WindowsDebugger::PrintFormatted(std::string_view format, fmt::format_args args)
    {
        fmt::memory_buffer buffer{};
        std::back_insert_iterator out{buffer};
        out = fmt::vformat_to(out, format, args);

        size_t const length = buffer.size();

        (*out) = '\0';

        // Raise exception directly to the debugger.
        ULONG_PTR const ptrs[2]{
            static_cast<ULONG_PTR>(length),
            reinterpret_cast<ULONG_PTR>(buffer.data()),
        };

        RaiseException(DBG_PRINTEXCEPTION_C, 0, 2, ptrs);
    }
}
