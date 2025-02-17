#include "AnemoneRuntime/Platform/Base/BaseError.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"
#include "AnemoneRuntime/Threading/CurrentThread.hxx"

#include <cerrno>

namespace Anemone::Internal
{
    void ReportErrorErrno(int error, std::source_location const& location)
    {
#if ANEMONE_FEATURE_PLATFORM_READABLE_ERROR_MESSAGES

#if ANEMONE_PLATFORM_WINDOWS
        char message[512];
        if (strerror_s(message, error) != 0)
        {
            message[0] = '\0';
        }
#else
        char const* message = strerror(error);
#endif

        AE_TRACE(Error, "{}:({}): caller: {}, tid: {}, error: {:#08x}, message: '{}'",
            location.file_name(),
            location.line(),
            location.function_name(),
            CurrentThread::Id(),
            error,
            message);
#else
        AE_TRACE(Error, "{}:({}): caller: {}, tid: {}, error: {:#08x}",
            location.file_name(),
            location.line(),
            location.function_name(),
            CurrentThread::Id(),
            error);
#endif
    }

    ErrorCode TranslateErrorCodeErrno(int error)
    {
        switch (error)
        {
        case E2BIG:
        case EDOM:
        case EFBIG:
        case EMSGSIZE:
        case ENAMETOOLONG:
            return ErrorCode::InvalidRange;

        case EACCES:
            return ErrorCode::AccessDenied;

        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case EMFILE:
        case EMLINK:
        case ENFILE:
            return ErrorCode::NotEnoughResources;

        case EAFNOSUPPORT:
            return ErrorCode::NotSupported;

        case EALREADY:
        case EBUSY:
        case EINPROGRESS:
            return ErrorCode::OperationInProgress;

        case ECONNABORTED:
            return ErrorCode::OperationAborted;

        case ECONNREFUSED:
            return ErrorCode::OperationRefused;

        case EISCONN:
            return ErrorCode::AlreadyExists;

        case EFAULT:
        case EHOSTUNREACH:
        case ELOOP:
            return ErrorCode::InvalidOperation;

        case EBADF:
            return ErrorCode::InvalidHandle;

        case EBADMSG:
        case EILSEQ:
            return ErrorCode::InvalidData;

        case EIDRM:
        case ENETDOWN:
            return ErrorCode::InvalidRequest;

        case ECANCELED:
        case ECONNRESET:
        case ENETRESET:
            return ErrorCode::OperationCanceled;

        case ECHILD:
            return ErrorCode::NotFound;

        case EAGAIN:
        case EDEADLOCK:
        case EINTR:
            return ErrorCode::OperationInterrupted;

        case EDESTADDRREQ:
        case EINVAL:
            return ErrorCode::InvalidArgument;

        case EEXIST:
            return ErrorCode::FileExists;

        case EISDIR:
            return ErrorCode::DirectoryExists;

        case EIO:
            return ErrorCode::IoError;

        case ENETUNREACH:
            return ErrorCode::NotConnected;

        default:
            break;
        }

        return ErrorCode::Unknown;
    }
}
