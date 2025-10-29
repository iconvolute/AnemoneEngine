#include "AnemoneDiagnostics/Debug.hxx"
#include "AnemoneThreading/CurrentThread.hxx"
#include "AnemoneDiagnostics/Trace.hxx"

#include <cerrno>

namespace Anemone
{
    void Debug::ReportErrorErrno(int error, std::source_location const& location)
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

    Error Debug::TranslateErrorCodeErrno(int error)
    {
        switch (error)
        {
        case 0:
            return Error::Success;

        case ENOENT:
        case ESRCH:
        case ENXIO:
        case ECHILD:
        case ENODEV:
            return Error::NotFound;

        case EBADF:
            return Error::InvalidHandle;

        case EINVAL:
        case EDOM:
        case ERANGE:
        case EILSEQ:
        case EDESTADDRREQ:
            return Error::InvalidArgument;

        case EOVERFLOW:
            return Error::ArithmeticOverflow;

        case E2BIG:
        case EFBIG:
        case EMSGSIZE:
        case ENAMETOOLONG:
            return Error::InvalidRange;

        case EPERM:
        case EACCES:
            return Error::AccessDenied;

        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case EMFILE:
        case EMLINK:
        case ENFILE:
            return Error::NotEnoughResources;

        case ENOMEM:
            return Error::NotEnoughMemory;

        case EEXIST:
            return Error::AlreadyExists;

        case EISCONN:
            return Error::AlreadyConnected;

        case ETXTBSY:
        case ETIME:
        case ETIMEDOUT:
            return Error::Timeout;

        case EINTR:
        case EAGAIN:
        case EDEADLOCK:
            return Error::Interrupted;

        case EAFNOSUPPORT:
        case ENOEXEC:
#if defined(ENOTBLK)
        case ENOTBLK:
#endif
            return Error::NotSupported;

        case EALREADY:
        case EBUSY:
        case EINPROGRESS:
            return Error::InProgress;

        case ECONNABORTED:
            return Error::Canceled;

        case ECONNREFUSED:
            return Error::Refused;

        case EFAULT:
        case EHOSTUNREACH:
        case ELOOP:
            return Error::InvalidOperation;

        case EBADMSG:
            return Error::InvalidData;

        case EIDRM:
        case ENETDOWN:
            return Error::InvalidRequest;

        case ECANCELED:
        case ECONNRESET:
        case ENETRESET:
            return Error::Canceled;

        case EISDIR:
            return Error::DirectoryExists;

        case EIO:
            return Error::IoError;

        case ENETUNREACH:
            return Error::NotConnected;

        case EPIPE:
            return Error::InvalidResource;

        case EXDEV:
            return Error::InvalidResource;

        case ENOTEMPTY:
            return Error::DirectoryNotEmpty;

        case ENOSYS:
            return Error::NotSupported;

        case ENOTTY:
            return Error::InvalidOperation;

        case ESPIPE:
            return Error::InvalidSeek;

        case ENOBUFS:
            return Error::NotEnoughSpace;

        case ENOLINK:
            return Error::InvalidResource;

        case ENOLCK:
            return Error::InvalidOperation;

#ifdef ENODATA
        case ENODATA:
            return Error::InvalidData;
#endif

        case ENOMSG:
            return Error::InvalidData;

        case ENOPROTOOPT:
            return Error::InvalidOption;

        case ENOSPC:
            return Error::NotEnoughSpace;

#ifdef ENOSR
        case ENOSR:
            return Error::NotEnoughResources;
#endif

        case ENOTDIR:
            return Error::InvalidDirectory;

        case ENOTSOCK:
            return Error::InvalidSocket;
#ifdef ENOSTR
        case ENOSTR:
            return Error::InvalidStream;
#endif

        case ENOTCONN:
            return Error::NotConnected;

        case ENOTSUP:
            return Error::NotSupported;

#if defined(EOPNOTSUPP) && (EOPNOTSUPP != ENOTSUP)
        case EOPNOTSUPP:
            return Error::NotSupported;
#endif

        case EOWNERDEAD:
            return Error::InvalidOperation;

        case EPROTO:
            return Error::NotSupported;

        case EPROTONOSUPPORT:
            return Error::NotSupported;

        case EROFS:
            return Error::AccessDenied;

        case ENOTRECOVERABLE:
            return Error::NotRecoverable;

        case EPROTOTYPE:
            return Error::InvalidType;

        default:
            break;
        }

        return Error::Failure;
    }
}
