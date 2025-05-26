#include "AnemoneRuntime/Interop/Windows/Text.hxx"
#include "AnemoneRuntime/Diagnostics/Platform/Windows/Debug.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

namespace Anemone::Diagnostics
{
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

        Trace::Get().TraceError("{}:({}): caller: {}, tid: {}, error: {:#08x}, message: '{}'",
            location.file_name(),
            location.line(),
            location.function_name(),
            GetCurrentThreadId(),
            error,
            szMessage.c_str());
#else

        Trace::Get().TraceError("{}:({}): caller: {}, tid: {}, error: {:#08x}",
            location.file_name(),
            location.line(),
            location.function_name(),
            GetCurrentThreadId(),
            error);

#endif
        anemone_debugbreak();
    }

    void WindowsDebug::ReportErrorHRESULT(HRESULT hresult, std::source_location const& location)
    {
        //
        // Forwarding to Win32 version as it supports HRESULT as well.
        //

        ReportErrorWin32(static_cast<DWORD>(hresult), location);
    }

    Status WindowsDebug::TranslateErrorCodeHRESULT(HRESULT hr)
    {
        if (SUCCEEDED(hr))
        {
            return Status::Success;
        }
        else
        {
            switch (hr)
            {
            case __HRESULT_FROM_WIN32(ERROR_SUCCESS):
                return Status::Success;

            case E_INVALIDARG:
            case E_POINTER:
                // case __HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER):
                return Status::InvalidArgument;

            case E_ILLEGAL_METHOD_CALL:
            case E_FAIL:
            case E_ABORT:
                return Status::InvalidOperation;

            case E_NOTIMPL:
                return Status::NotImplemented;

            case __HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY):
            case E_OUTOFMEMORY:
                // case __HRESULT_FROM_WIN32(ERROR_OUTOFMEMORY):
                return Status::NotEnoughMemory;

            case E_PENDING:
                return Status::OperationInProgress;

            case __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE):
            case __HRESULT_FROM_WIN32(ERROR_BAD_PATHNAME):
            case __HRESULT_FROM_WIN32(ERROR_INVALID_NAME):
            case __HRESULT_FROM_WIN32(ERROR_HANDLE_DISK_FULL):
            case __HRESULT_FROM_WIN32(ERROR_FILE_READ_ONLY):
                return Status::InvalidArgument;

            case __HRESULT_FROM_WIN32(ERROR_INVALID_DATA):
            case __HRESULT_FROM_WIN32(ERROR_INVALID_DRIVE):
                return Status::InvalidData;

            case __HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND):
                return Status::FileNotFound;

            case __HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND):
            case __HRESULT_FROM_WIN32(ERROR_NOT_FOUND):
            case __HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND):
                return Status::NotFound;

            case __HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS):
                return Status::AlreadyExists;

            case STG_E_FILEALREADYEXISTS:
            case __HRESULT_FROM_WIN32(ERROR_FILE_EXISTS):
                return Status::FileExists;

            case __HRESULT_FROM_WIN32(ERROR_HANDLE_EOF):
                return Status::EndOfFile;

            case E_ACCESSDENIED:
            case __HRESULT_FROM_WIN32(ERROR_LOCK_VIOLATION):
            case __HRESULT_FROM_WIN32(ERROR_CANNOT_BREAK_OPLOCK):
            case __HRESULT_FROM_WIN32(ERROR_WRITE_PROTECT):
            case __HRESULT_FROM_WIN32(ERROR_INVALID_ACCESS):
                // case __HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED): // E_ACCESSDENIED
                return Status::AccessDenied;

            case __HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE):
                return Status::InvalidHandle;

            case __HRESULT_FROM_WIN32(ERROR_DIRECTORY):
                return Status::InvalidDirectory;

            case __HRESULT_FROM_WIN32(ERROR_READ_FAULT):
            case __HRESULT_FROM_WIN32(ERROR_WRITE_FAULT):
                return Status::IoError;

            case __HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION):
                return Status::SharingViolation;

            case __HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED):
                return Status::NotSupported;

            case __HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND):
            case __HRESULT_FROM_WIN32(ERROR_BAD_NETPATH):
                return Status::InvalidPath;

            case __HRESULT_FROM_WIN32(ERROR_TIMEOUT):
                return Status::OperationTimeout;

            case __HRESULT_FROM_WIN32(ERROR_DIR_NOT_EMPTY):
                return Status::DirectoryNotEmpty;

            default:
                AE_TRACE(Error, "Unhandled HRESULT 0x{:08X}", static_cast<DWORD>(hr));
                break;
            }
        }

        return Status::Unknown;
    }

    Status WindowsDebug::TranslateErrorCodeWin32(DWORD error)
    {
        return TranslateErrorCodeHRESULT(__HRESULT_FROM_WIN32(error));
    }
}
