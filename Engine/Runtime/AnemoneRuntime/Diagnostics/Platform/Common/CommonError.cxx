#include "AnemoneRuntime/Diagnostics/Platform/Common/CommonError.hxx"
#include "AnemoneRuntime/Base/ErrorCode.hxx"
#include "AnemoneRuntime/Threading/CurrentThread.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

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
        case 0:
            return ErrorCode::Success;

        case ENOENT:
        case ESRCH:
        case ENXIO:
        case ECHILD:
        case ENODEV:
            return ErrorCode::NotFound;

        case EBADF:
        case EINVAL:
        case EDOM:
        case ERANGE:
        case EILSEQ:
        case EOVERFLOW:
        case EDESTADDRREQ:
            return ErrorCode::InvalidArgument;

        case E2BIG:
        case EFBIG:
        case EMSGSIZE:
        case ENAMETOOLONG:
            return ErrorCode::InvalidRange;

        case EPERM:
        case EACCES:
            return ErrorCode::AccessDenied;

        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case EMFILE:
        case EMLINK:
        case ENFILE:
            return ErrorCode::NotEnoughResources;

        case ENOMEM:
            return ErrorCode::NotEnoughMemory;

        case EEXIST:
        case EISCONN:
            return ErrorCode::AlreadyExists;

        case ETXTBSY:
        case ETIME:
        case ETIMEDOUT:
            return ErrorCode::OperationTimeout;

        case EINTR:
        case EAGAIN:
        case EDEADLOCK:
            return ErrorCode::OperationInterrupted;

        case EAFNOSUPPORT:
        case ENOEXEC:
#if defined(ENOTBLK)
        case ENOTBLK:
#endif
            return ErrorCode::NotSupported;

        case EALREADY:
        case EBUSY:
        case EINPROGRESS:
            return ErrorCode::OperationInProgress;

        case ECONNABORTED:
            return ErrorCode::OperationAborted;

        case ECONNREFUSED:
            return ErrorCode::OperationRefused;

        case EFAULT:
        case EHOSTUNREACH:
        case ELOOP:
            return ErrorCode::InvalidOperation;

        case EBADMSG:
            return ErrorCode::InvalidData;

        case EIDRM:
        case ENETDOWN:
            return ErrorCode::InvalidRequest;

        case ECANCELED:
        case ECONNRESET:
        case ENETRESET:
            return ErrorCode::OperationCanceled;

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
