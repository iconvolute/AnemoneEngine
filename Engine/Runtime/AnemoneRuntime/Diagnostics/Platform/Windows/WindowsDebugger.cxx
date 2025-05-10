#include "AnemoneRuntime/Diagnostics/Debugger.hxx"
#include "AnemoneRuntime/Diagnostics/Platform/Windows/WindowsDebugger.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Diagnostics/TraceListener.hxx"
#include "AnemoneRuntime/Diagnostics/Internal/ConsoleTraceListener.hxx"
#include "AnemoneRuntime/Interop/Windows/Process.hxx"
#include "AnemoneRuntime/Interop/Windows/Debugger.hxx"
#include "AnemoneRuntime/Interop/Windows/Text.hxx"
#include "AnemoneRuntime/Interop/Windows/MemoryMappedFile.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"

#include <winmeta.h>
#include <TraceLoggingProvider.h>

#include <iterator>
#include <format>

namespace Anemone::Internal
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


#define ENABLE_HEAP_CORRUPTION_CRASHES false

namespace Anemone::Internal
{
    class WindowsDebugTraceListener final : public Diagnostics::TraceListener
    {
    private:
        CriticalSection m_lock{};
    public:
        void TraceEvent(Diagnostics::TraceLevel level, const char* message, size_t size) override
        {
            (void)level;

            UniqueLock scope{this->m_lock};
            Interop::Windows::OutputDebugStringLengthA(message, size + 1);
            Interop::Windows::OutputDebugStringLengthA("\r\n", 3);
        }
    };
}

namespace Anemone::Internal
{
    // C0B8BB2A-7E1E-5A0B-0ACD-3EE6187895ED
    // https://learn.microsoft.com/en-us/windows/win32/api/traceloggingprovider/
    TRACELOGGING_DEFINE_PROVIDER(
        GWindowsEtwTraceProvider,
        "AnemoneEngine.Runtime",
        (0xC0B8BB2A, 0x7E1E, 0x5A0B, 0x0A, 0xCD, 0x3E, 0xE6, 0x18, 0x78, 0x95, 0xED));


    class WindowsEtwTraceListener final : public Diagnostics::TraceListener
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

        void TraceEvent(Diagnostics::TraceLevel level, const char* message, size_t size) override
        {
            switch (level)
            {
            case Diagnostics::TraceLevel::Verbose:
            case Diagnostics::TraceLevel::Debug:
                TraceLoggingWrite(
                    GWindowsEtwTraceProvider,
                    "Log",
                    TraceLoggingLevel(WINEVENT_LEVEL_VERBOSE),
                    TraceLoggingCountedUtf8String(message, static_cast<ULONG>(size), "Message"));
                break;

            case Diagnostics::TraceLevel::Information:
                TraceLoggingWrite(
                    GWindowsEtwTraceProvider,
                    "Log",
                    TraceLoggingLevel(WINEVENT_LEVEL_INFO),
                    TraceLoggingCountedUtf8String(message, static_cast<ULONG>(size), "Message"));
                break;

            case Diagnostics::TraceLevel::Warning:
                TraceLoggingWrite(
                    GWindowsEtwTraceProvider,
                    "Log",
                    TraceLoggingLevel(WINEVENT_LEVEL_WARNING),
                    TraceLoggingCountedUtf8String(message, static_cast<ULONG>(size), "Message"));

                break;
            case Diagnostics::TraceLevel::Error:
                TraceLoggingWrite(
                    GWindowsEtwTraceProvider,
                    "Log",
                    TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
                    TraceLoggingCountedUtf8String(message, static_cast<ULONG>(size), "Message"));
                break;

            case Diagnostics::TraceLevel::Fatal:
                TraceLoggingWrite(
                    GWindowsEtwTraceProvider,
                    "Log",
                    TraceLoggingLevel(WINEVENT_LEVEL_CRITICAL),
                    TraceLoggingCountedUtf8String(message, static_cast<ULONG>(size), "Message"));
                break;

            case Diagnostics::TraceLevel::None:
                break;
            }
        }
    };

}

namespace Anemone::Internal
{
    static PTOP_LEVEL_EXCEPTION_FILTER GPreviousExceptionFilter{};

    static UninitializedObject<Diagnostics::ConsoleTraceListener> GConsoleTraceListener{};
    static UninitializedObject<WindowsDebugTraceListener> GWindowsDebugTraceListener{};
    static UninitializedObject<WindowsEtwTraceListener> GWindowsEtwTraceListener{};

    static LONG CALLBACK OnUnhandledExceptionFilter(LPEXCEPTION_POINTERS lpExceptionPointers)
    {
        if (lpExceptionPointers->ExceptionRecord->ExceptionCode == DBG_PRINTEXCEPTION_C)
        {
            return EXCEPTION_CONTINUE_EXECUTION;
        }

        Windows::HandleCrash(lpExceptionPointers);
        return EXCEPTION_CONTINUE_SEARCH;
    }

#if ENABLE_HEAP_CORRUPTION_CRASHES
    static LONG CALLBACK OnUnhandledExceptionVEH(LPEXCEPTION_POINTERS lpExceptionPointers)
    {
        if (lpExceptionPointers->ExceptionRecord->ExceptionCode == STATUS_HEAP_CORRUPTION)
        {
            Windows::HandleCrash::HandleCrash(lpExceptionPointers);
        }

        return EXCEPTION_EXECUTE_HANDLER;
    }
#endif

    extern void InitializeDebugger()
    {
        // Initialize exception handlers
        GPreviousExceptionFilter = SetUnhandledExceptionFilter(OnUnhandledExceptionFilter);

#if ENABLE_HEAP_CORRUPTION_CRASHES
        AddVectoredExceptionHandler(0, OnUnhandledExceptionVEH);
#endif

        // Register console trace listener.
        if (Interop::Windows::IsConsoleApplication(GetModuleHandleW(nullptr)))
        {
            GConsoleTraceListener.Create();
            Diagnostics::GetTraceDispatcher().Register(*GConsoleTraceListener);
        }

#if !ANEMONE_BUILD_SHIPPING
        //
        // Debug trace listener is available only on non-shipping builds.
        //

        GWindowsDebugTraceListener.Create();
        Diagnostics::GetTraceDispatcher().Register(*GWindowsDebugTraceListener);
#endif

        GWindowsEtwTraceListener.Create();
        Diagnostics::GetTraceDispatcher().Register(*GWindowsEtwTraceListener);
    }

    extern void FinalizeDebugger()
    {
        if (GWindowsEtwTraceListener)
        {
            Diagnostics::GetTraceDispatcher().Unregister(*GWindowsEtwTraceListener);
            GWindowsEtwTraceListener.Destroy();
        }

        if (GWindowsDebugTraceListener)
        {
            Diagnostics::GetTraceDispatcher().Unregister(*GWindowsDebugTraceListener);
            GWindowsDebugTraceListener.Destroy();
        }

        if (GConsoleTraceListener)
        {
            Diagnostics::GetTraceDispatcher().Unregister(*GConsoleTraceListener);
            GConsoleTraceListener.Destroy();
        }

        // Restore exception handlers.
#if ENABLE_HEAP_CORRUPTION_CRASHES
        RemoveVectoredExceptionHandler(OnUnhandledExceptionVEH);
#endif

        SetUnhandledExceptionFilter(GPreviousExceptionFilter);
    }
}

namespace Anemone
{
    void Diagnostics::Break()
    {
        anemone_debugbreak();
    }

    void Diagnostics::Crash()
    {
#if !ANEMONE_BUILD_SHIPPING
        anemone_debugbreak();
#endif

        __fastfail(FAST_FAIL_FATAL_APP_EXIT);
    }

    bool Diagnostics::IsDebuggerAttached()
    {
#if ANEMONE_BUILD_SHIPPING
        return false;
#else
        return IsDebuggerPresent() != FALSE;
#endif
    }

    void Diagnostics::WaitForDebugger()
    {
        while (not IsDebuggerPresent())
        {
            SleepEx(100, TRUE);
        }
    }

    bool Diagnostics::AttachDebugger()
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

    void Diagnostics::ReportApplicationStop(std::string_view reason)
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

namespace Anemone::Internal::Windows
{
    void HandleCrash(PEXCEPTION_POINTERS pExceptionPointers)
    {
        static SRWLOCK lock = SRWLOCK_INIT;

        AcquireSRWLockExclusive(&lock);

        if (Diagnostics::AttachDebugger())
        {
            // Debugger attached, let it handle the crash.
            ReleaseSRWLockExclusive(&lock);
            return;
        }

        HANDLE const hProcess = GetCurrentProcess();
        HANDLE const hThread = GetCurrentThread();

        DWORD const dwProcessId = GetProcessId(hProcess);
        DWORD const dwThreadId = GetThreadId(hThread);

        Internal::CrashDetails crashDetails{};
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
                sizeof(Internal::CrashDetails),
                Interop::Windows::MemoryMappedFileAccess::ReadWrite);

            if (not fhMapping)
            {
                Diagnostics::ReportApplicationStop("Could not open memory mapped file");
                return;
            }

            Interop::Windows::SafeMemoryMappedViewHandle fvMapping = Interop::Windows::CreateMemoryMappedView(
                fhMapping,
                Interop::Windows::MemoryMappedFileAccess::ReadWrite);

            if (not fvMapping)
            {
                Diagnostics::ReportApplicationStop("Could not create memory mapped view");
                return;
            }

            CopyMemory(fvMapping.GetData(), &crashDetails, sizeof(Internal::CrashDetails));

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
                Diagnostics::ReportApplicationStop("Could not start AnemoneCrashReporter");
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
