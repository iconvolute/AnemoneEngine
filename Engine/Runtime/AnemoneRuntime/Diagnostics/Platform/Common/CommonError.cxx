#include "AnemoneRuntime/Diagnostics/Platform/Common/CommonError.hxx"
#include "AnemoneRuntime/Diagnostics/Status.hxx"
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

    Status TranslateErrorCodeErrno(int error)
    {
        switch (error)
        {
        case 0:
            return Status::Success;

        case ENOENT:
        case ESRCH:
        case ENXIO:
        case ECHILD:
        case ENODEV:
            return Status::NotFound;

        case EBADF:
        case EINVAL:
        case EDOM:
        case ERANGE:
        case EILSEQ:
        case EOVERFLOW:
        case EDESTADDRREQ:
            return Status::InvalidArgument;

        case E2BIG:
        case EFBIG:
        case EMSGSIZE:
        case ENAMETOOLONG:
            return Status::InvalidRange;

        case EPERM:
        case EACCES:
            return Status::AccessDenied;

        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case EMFILE:
        case EMLINK:
        case ENFILE:
            return Status::NotEnoughResources;

        case ENOMEM:
            return Status::NotEnoughMemory;

        case EEXIST:
        case EISCONN:
            return Status::AlreadyExists;

        case ETXTBSY:
        case ETIME:
        case ETIMEDOUT:
            return Status::OperationTimeout;

        case EINTR:
        case EAGAIN:
        case EDEADLOCK:
            return Status::OperationInterrupted;

        case EAFNOSUPPORT:
        case ENOEXEC:
#if defined(ENOTBLK)
        case ENOTBLK:
#endif
            return Status::NotSupported;

        case EALREADY:
        case EBUSY:
        case EINPROGRESS:
            return Status::OperationInProgress;

        case ECONNABORTED:
            return Status::OperationAborted;

        case ECONNREFUSED:
            return Status::OperationRefused;

        case EFAULT:
        case EHOSTUNREACH:
        case ELOOP:
            return Status::InvalidOperation;

        case EBADMSG:
            return Status::InvalidData;

        case EIDRM:
        case ENETDOWN:
            return Status::InvalidRequest;

        case ECANCELED:
        case ECONNRESET:
        case ENETRESET:
            return Status::OperationCanceled;

        case EISDIR:
            return Status::DirectoryExists;

        case EIO:
            return Status::IoError;

        case ENETUNREACH:
            return Status::NotConnected;

        default:
            break;
        }

        return Status::Unknown;
    }
}
