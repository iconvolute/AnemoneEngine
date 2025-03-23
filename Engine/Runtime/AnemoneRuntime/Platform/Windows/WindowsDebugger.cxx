#include "AnemoneRuntime/Platform/Windows/WindowsDebugger.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsCrashHandler.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Diagnostics/ConsoleTraceListener.hxx"

#include <iterator>

#include <winmeta.h>
#include <TraceLoggingProvider.h>

namespace Anemone
{
    class WindowsDebugTraceListener final : public TraceListener
    {
    public:
        void TraceEvent(TraceLevel level, const char* message, size_t size) override
        {
            (void)level;

            Interop::win32_OutputDebugString(message, size + 1);
            Interop::win32_OutputDebugString("\r\n", 3);
        }
    };
    static UninitializedObject<WindowsDebugTraceListener> GWindowsDebugTraceListener;

    // C0B8BB2A-7E1E-5A0B-0ACD-3EE6187895ED
    // https://learn.microsoft.com/en-us/windows/win32/api/traceloggingprovider/
    TRACELOGGING_DEFINE_PROVIDER(
        GWindowsEtwTraceProvider,
        "AnemoneEngine.Runtime",
        (0xC0B8BB2A, 0x7E1E, 0x5A0B, 0x0A, 0xCD, 0x3E, 0xE6, 0x18, 0x78, 0x95, 0xED));


    class WindowsEtwTraceListener final : public TraceListener
    {
    public:
        WindowsEtwTraceListener()
        {
            TraceLoggingRegister(GWindowsEtwTraceProvider);
        }

        ~WindowsEtwTraceListener() override
        {
            TraceLoggingUnregister(GWindowsEtwTraceProvider);
        }

        void TraceEvent(TraceLevel level, const char* message, size_t size) override
        {
            switch (level)
            {
            case TraceLevel::Verbose:
            case TraceLevel::Debug:
                TraceLoggingWrite(
                    GWindowsEtwTraceProvider,
                    "Log",
                    TraceLoggingLevel(WINEVENT_LEVEL_VERBOSE),
                    TraceLoggingCountedUtf8String(message, static_cast<ULONG>(size), "Message"));
                break;

            case TraceLevel::Information:
                TraceLoggingWrite(
                    GWindowsEtwTraceProvider,
                    "Log",
                    TraceLoggingLevel(WINEVENT_LEVEL_INFO),
                    TraceLoggingCountedUtf8String(message, static_cast<ULONG>(size), "Message"));
                break;

            case TraceLevel::Warning:
                TraceLoggingWrite(
                    GWindowsEtwTraceProvider,
                    "Log",
                    TraceLoggingLevel(WINEVENT_LEVEL_WARNING),
                    TraceLoggingCountedUtf8String(message, static_cast<ULONG>(size), "Message"));

                break;
            case TraceLevel::Error:
                TraceLoggingWrite(
                    GWindowsEtwTraceProvider,
                    "Log",
                    TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
                    TraceLoggingCountedUtf8String(message, static_cast<ULONG>(size), "Message"));
                break;

            case TraceLevel::Fatal:
                TraceLoggingWrite(
                    GWindowsEtwTraceProvider,
                    "Log",
                    TraceLoggingLevel(WINEVENT_LEVEL_CRITICAL),
                    TraceLoggingCountedUtf8String(message, static_cast<ULONG>(size), "Message"));
                break;

            case TraceLevel::None:
                break;
            }
        }
    };

    static UninitializedObject<WindowsEtwTraceListener> GWindowsEtwTraceListener;

    static UninitializedObject<ConsoleTraceListener> GConsoleTraceListener;

    void Debugger::Initialize()
    {
        GWindowsEtwTraceListener.Create();
        Trace::AddListener(*GWindowsEtwTraceListener);

        GWindowsDebugTraceListener.Create();
        Trace::AddListener(*GWindowsDebugTraceListener);

        // TODO: Move this to application setup. It should be common to all platforms and configurable.
        if (Interop::win32_IsConsoleApplication(GetModuleHandleW(nullptr)))
        {
            GConsoleTraceListener.Create();
            Trace::AddListener(*GConsoleTraceListener);
        }
    }

    void Debugger::Finalize()
    {
        if (Interop::win32_IsConsoleApplication(GetModuleHandleW(nullptr)))
        {
            Trace::RemoveListener(*GConsoleTraceListener);
            GConsoleTraceListener.Destroy();
        }

        Trace::RemoveListener(*GWindowsDebugTraceListener);
        GWindowsDebugTraceListener.Destroy();

        Trace::RemoveListener(*GWindowsEtwTraceListener);
        GWindowsEtwTraceListener.Destroy();
    }

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
            return true;
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
    void WindowsDebugger::HandleCrash(PEXCEPTION_POINTERS pExceptionPointers)
    {
        static SRWLOCK lock = SRWLOCK_INIT;

        AcquireSRWLockExclusive(&lock);

        if (Debugger::Attach())
        {
            // Debugger attached, let it handle the crash.
            ReleaseSRWLockExclusive(&lock);
            return;
        }

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
