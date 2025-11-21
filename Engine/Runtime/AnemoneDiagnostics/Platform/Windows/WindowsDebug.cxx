#include "AnemoneDiagnostics/Platform/Windows/WindowsDebug.hxx"
#include "AnemoneInterop/Windows/Text.hxx"
#include "AnemoneInterop/Windows/MemoryMappedFile.hxx"
#include "AnemoneDiagnostics/Trace.hxx"
#include "AnemoneBase/UninitializedObject.hxx"

#include <format>

#define ENABLE_HEAP_CORRUPTION_CRASHES false

namespace Anemone
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

        UninitializedObject<WindowsDebug> gWindowsDebug{};
    }
}

// WindowsDebug implementation

namespace Anemone
{
    void WindowsDebug::Initialize()
    {
        gWindowsDebug.Create();
    }

    void WindowsDebug::Finalize()
    {
        gWindowsDebug.Destroy();
    }

    WindowsDebug::WindowsDebug()
    {
        // Initialize exception handlers
        this->topLevelExceptionFilter = SetUnhandledExceptionFilter(UnhandledExceptionFilter);

#if defined(_MSC_VER)
        this->pureCallHandler = _set_purecall_handler(PureCallHandler);
        this->invalidParameterHandler = _set_invalid_parameter_handler(InvalidParameterHandler);
#endif

        this->terminateHandler = std::set_terminate(TerminateHandler);

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
        AddVectoredExceptionHandler(0, UnhandledExceptionVEH);
#endif
    }

    WindowsDebug::~WindowsDebug()
    {
        // Restore exception handlers.
#if ENABLE_HEAP_CORRUPTION_CRASHES
        RemoveVectoredExceptionHandler(WindowsDebug__OnUnhandledExceptionVEH);
#endif

#if ANEMONE_COMPILER_MSVC
        _set_purecall_handler(this->pureCallHandler);
        _set_invalid_parameter_handler(this->invalidParameterHandler);
#endif

        std::set_terminate(this->terminateHandler);

        SetUnhandledExceptionFilter(this->topLevelExceptionFilter);
    }

    void WindowsDebug::ReportErrorWin32(DWORD error, std::source_location const& location)
    {
#if ANEMONE_FEATURE_PLATFORM_READABLE_ERROR_MESSAGES
        // https://learn.microsoft.com/en-us/windows/win32/seccrypto/retrieving-error-messages

        WCHAR szMessageBuffer[512];

        DWORD dwChars = FormatMessageW(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            error,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
            szMessageBuffer,
            512,
            nullptr);

        if (0 == dwChars)
        {
            HINSTANCE hInst = LoadLibraryW(L"Ntdsbmsg.dll");

            if (hInst)
            {
                dwChars = FormatMessageW(
                    FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
                    hInst,
                    error,
                    0,
                    szMessageBuffer,
                    512,
                    nullptr);

                // Free the library.
                FreeLibrary(hInst);
            }
        }

        // Remove trailing newlines and periods.
        if (dwChars > 3)
        {
            if (szMessageBuffer[dwChars - 1] == L'\n')
            {
                --dwChars;
            }

            if (szMessageBuffer[dwChars - 1] == L'\r')
            {
                --dwChars;
            }

            if (szMessageBuffer[dwChars - 1] == L'.')
            {
                --dwChars;
            }

            szMessageBuffer[dwChars] = L'\0';
        }

        Interop::string_buffer<char, 128> szMessage;
        Interop::Windows::NarrowString(szMessage, std::wstring_view{szMessageBuffer, dwChars});

        Trace::Get().TraceError("{}:({}): tid: {}, error: 0x{:08x}, message: '{}'",
            location.file_name(),
            location.line(),
            // location.function_name(),
            GetCurrentThreadId(),
            error,
            szMessage.c_str());
#else

        Trace::Get().TraceError("{}:({}): tid: {}, error: 0x{:08x}",
            location.file_name(),
            location.line(),
            // location.function_name(),
            GetCurrentThreadId(),
            error);

#endif
    }

    void WindowsDebug::ReportErrorHRESULT(HRESULT hresult, std::source_location const& location)
    {
        //
        // Forwarding to Win32 version as it supports HRESULT as well.
        //

        ReportErrorWin32(static_cast<DWORD>(hresult), location);
    }

    Error WindowsDebug::TranslateErrorCodeHRESULT(HRESULT hr)
    {
        if (SUCCEEDED(hr))
        {
            return Error::Success;
        }

        switch (hr)
        {
        case __HRESULT_FROM_WIN32(ERROR_SUCCESS):
            return Error::Success;

        case E_INVALIDARG:
        case E_POINTER:
            // case __HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER):
            return Error::InvalidArgument;

        case E_ILLEGAL_METHOD_CALL:
        case E_FAIL:
        case E_ABORT:
            return Error::InvalidOperation;

        case E_NOTIMPL:
            return Error::NotImplemented;

        case __HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY):
        case E_OUTOFMEMORY:
            // case __HRESULT_FROM_WIN32(ERROR_OUTOFMEMORY):
            return Error::NotEnoughMemory;

        case E_PENDING:
            return Error::InProgress;

        case __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE):
        case __HRESULT_FROM_WIN32(ERROR_BAD_PATHNAME):
        case __HRESULT_FROM_WIN32(ERROR_INVALID_NAME):
        case __HRESULT_FROM_WIN32(ERROR_HANDLE_DISK_FULL):
        case __HRESULT_FROM_WIN32(ERROR_FILE_READ_ONLY):
            return Error::InvalidArgument;

        case __HRESULT_FROM_WIN32(ERROR_INVALID_DATA):
        case __HRESULT_FROM_WIN32(ERROR_INVALID_DRIVE):
            return Error::InvalidData;

        case __HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND):
            return Error::FileNotFound;

        case __HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND):
        case __HRESULT_FROM_WIN32(ERROR_NOT_FOUND):
        case __HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND):
            return Error::NotFound;

        case __HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS):
            return Error::AlreadyExists;

        case STG_E_FILEALREADYEXISTS:
        case __HRESULT_FROM_WIN32(ERROR_FILE_EXISTS):
            return Error::FileExists;

        case __HRESULT_FROM_WIN32(ERROR_HANDLE_EOF):
            return Error::EndOfFile;

        case E_ACCESSDENIED:
        case __HRESULT_FROM_WIN32(ERROR_LOCK_VIOLATION):
        case __HRESULT_FROM_WIN32(ERROR_CANNOT_BREAK_OPLOCK):
        case __HRESULT_FROM_WIN32(ERROR_WRITE_PROTECT):
        case __HRESULT_FROM_WIN32(ERROR_INVALID_ACCESS):
            // case __HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED): // E_ACCESSDENIED
            return Error::AccessDenied;

        case __HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE):
            return Error::InvalidHandle;

        case __HRESULT_FROM_WIN32(ERROR_DIRECTORY):
            return Error::InvalidDirectory;

        case __HRESULT_FROM_WIN32(ERROR_READ_FAULT):
        case __HRESULT_FROM_WIN32(ERROR_WRITE_FAULT):
            return Error::IoError;

        case __HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION):
            return Error::SharingViolation;

        case __HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED):
            return Error::NotSupported;

        case __HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND):
        case __HRESULT_FROM_WIN32(ERROR_BAD_NETPATH):
            return Error::InvalidPath;

        case __HRESULT_FROM_WIN32(ERROR_TIMEOUT):
            return Error::Timeout;

        case __HRESULT_FROM_WIN32(ERROR_DIR_NOT_EMPTY):
            return Error::DirectoryNotEmpty;
        }

        return Error::Failure;
    }

    Error WindowsDebug::TranslateErrorCodeWin32(DWORD error)
    {
        return TranslateErrorCodeHRESULT(__HRESULT_FROM_WIN32(error));
    }

    void WindowsDebug::CrashHandler(LPEXCEPTION_POINTERS pExceptionPointers)
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

    LONG CALLBACK WindowsDebug::UnhandledExceptionFilter(LPEXCEPTION_POINTERS lpExceptionPointers)
    {
        if (lpExceptionPointers->ExceptionRecord->ExceptionCode == DBG_PRINTEXCEPTION_C)
        {
            return EXCEPTION_CONTINUE_EXECUTION;
        }

        WindowsDebug::CrashHandler(lpExceptionPointers);
        return EXCEPTION_CONTINUE_SEARCH;
    }

    LONG CALLBACK WindowsDebug::UnhandledExceptionVEH(LPEXCEPTION_POINTERS lpExceptionPointers)
    {
        if (lpExceptionPointers->ExceptionRecord->ExceptionCode == STATUS_HEAP_CORRUPTION)
        {
            CrashHandler(lpExceptionPointers);
        }

        return EXCEPTION_EXECUTE_HANDLER;
    }

    void __cdecl WindowsDebug::PureCallHandler()
    {
        AE_PANIC("pure-call");
    }

    void __cdecl WindowsDebug::InvalidParameterHandler(
        wchar_t const*,
        wchar_t const*,
        wchar_t const*,
        unsigned int,
        uintptr_t)
    {
        AE_PANIC("Invalid parameter");
    }

    void __CRTDECL WindowsDebug::TerminateHandler()
    {
        AE_PANIC("Terminate handler");
    }
}

// Debug class implementation

namespace Anemone
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
        Interop::string_buffer<wchar_t, 512> buffer{};
        Interop::Windows::WidenString(buffer, reason);

        MessageBoxW(
            nullptr,
            buffer,
            L"Application Stop",
            MB_OK | MB_ICONERROR);
    }
}
