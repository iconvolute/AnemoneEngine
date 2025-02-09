#include "AnemoneRuntime/Platform/Windows/WindowsError.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

namespace Anemone::Internal
{
    void ReportErrorWin32(DWORD error, std::source_location const& location)
    {
#if ANEMONE_FEATURE_PLATFORM_READABLE_ERROR_MESSAGES
        // https://learn.microsoft.com/en-us/windows/win32/seccrypto/retrieving-error-messages

        CHAR szMessageBuffer[512];

        DWORD dwChars = FormatMessageA(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            error,
            0,
            szMessageBuffer,
            512,
            nullptr);

        if (0 == dwChars)
        {
            HINSTANCE hInst = LoadLibraryW(L"Ntdsbmsg.dll");

            if (hInst)
            {
                dwChars = FormatMessageA(
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

        if (dwChars)
        {
            // Remove white space at end of message.
            while (dwChars && isspace(szMessageBuffer[dwChars - 1]))
            {
                szMessageBuffer[--dwChars] = 0;
            }

            // Remove the trailing dot if exists.
            if (dwChars && (szMessageBuffer[dwChars - 1] == '.'))
            {
                szMessageBuffer[--dwChars] = 0;
            }
        }

        Trace::TraceMessage(TraceLevel::Error, "{}:({}): caller: {}, tid: {}, error: {:#08x}, message: '{}'",
            location.file_name(),
            location.line(),
            location.function_name(),
            GetCurrentThreadId(),
            error,
            dwChars ? szMessageBuffer : "Error Message not found");
#else

        Trace::TraceMessage(TraceLevel::Error, "{}:({}): caller: {}, tid: {}, error: {:#08x}",
            location.file_name(),
            location.line(),
            location.function_name(),
            GetCurrentThreadId(),
            error);

#endif
        anemone_debugbreak();
    }

    void ReportErrorHRESULT(HRESULT hresult, std::source_location const& location)
    {
        //
        // Forwarding to Win32 version as it supports HRESULT as well.
        //

        ReportErrorWin32(static_cast<DWORD>(hresult), location);
    }

    ErrorCode TranslateErrorCodeHRESULT(HRESULT hr)
    {
        ErrorCode result = ErrorCode::Unknown;

        if (SUCCEEDED(hr))
        {
            result = ErrorCode::Success;
        }
        else
        {
            switch (hr)
            {
            case E_INVALIDARG:
            case E_POINTER:
                result = ErrorCode::InvalidArgument;
                break;

            case E_ILLEGAL_METHOD_CALL:
            case E_FAIL:
            case E_ABORT:
                result = ErrorCode::InvalidOperation;
                break;

            case E_NOTIMPL:
                result = ErrorCode::NotImplemented;
                break;

            case E_OUTOFMEMORY:
                result = ErrorCode::NotEnoughMemory;
                break;

            case E_PENDING:
                result = ErrorCode::OperationInProgress;
                break;

            case __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE):
            case __HRESULT_FROM_WIN32(ERROR_BAD_PATHNAME):
            case __HRESULT_FROM_WIN32(ERROR_BAD_NETPATH):
            case __HRESULT_FROM_WIN32(ERROR_INVALID_NAME):
            case __HRESULT_FROM_WIN32(ERROR_HANDLE_DISK_FULL):
            case __HRESULT_FROM_WIN32(ERROR_DIR_NOT_EMPTY):
            case __HRESULT_FROM_WIN32(ERROR_FILE_READ_ONLY):
            case __HRESULT_FROM_WIN32(ERROR_WRITE_PROTECT):
                result = ErrorCode::InvalidArgument;
                break;

            case __HRESULT_FROM_WIN32(ERROR_INVALID_DATA):
                result = ErrorCode::InvalidData;
                break;

            case __HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND):
            case __HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND):
            case __HRESULT_FROM_WIN32(ERROR_NOT_FOUND):
                result = ErrorCode::FileNotFound;
                break;

            case STG_E_FILEALREADYEXISTS:
            case __HRESULT_FROM_WIN32(ERROR_FILE_EXISTS):
            case __HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS):
                result = ErrorCode::AlreadyExists;
                break;

            case E_ACCESSDENIED:
            case __HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION):
            case __HRESULT_FROM_WIN32(ERROR_LOCK_VIOLATION):
            case __HRESULT_FROM_WIN32(ERROR_CANNOT_BREAK_OPLOCK):
                result = ErrorCode::AccessDenied;
                break;

            case __HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE):
                result = ErrorCode::InvalidHandle;
                break;

            default:
                AE_TRACE(Error, "Unhandled HRESULT 0x{:08X}", static_cast<DWORD>(hr));
                result = ErrorCode::Unknown;
                break;
            }
        }

        return result;
    }

    ErrorCode TranslateErrorCodeWin32(DWORD error)
    {
        return TranslateErrorCodeHRESULT(__HRESULT_FROM_WIN32(error));
    }
}
