#include "AnemoneRuntime/ErrorCode.hxx"

#include <cerrno>

namespace Anemone::System::Private
{
    ErrorCode ErrorCodeFromErrno(int error)
    {
        switch (error)
        {
        case EAFNOSUPPORT:
            return ErrorCode::AddressFamilyNotSupported;

        case EADDRINUSE:
            return ErrorCode::AddressInUse;

        case EADDRNOTAVAIL:
            return ErrorCode::AddressNotAvailable;

        case EISCONN:
            return ErrorCode::AlreadyConnected;

        case E2BIG:
            return ErrorCode::ArgumentListTooLong;

        case EDOM:
            return ErrorCode::ArgumentOutOfDomain;

        case EFAULT:
            return ErrorCode::BadAddress;

        case EBADF:
            return ErrorCode::BadFileDescriptor;

        case EBADMSG:
            return ErrorCode::BadMessage;

        case EPIPE:
            return ErrorCode::BrokenPipe;

        case ECONNABORTED:
            return ErrorCode::ConnectionAborted;

        case EALREADY:
            return ErrorCode::ConnectionAlreadyInProgress;

        case ECONNREFUSED:
            return ErrorCode::ConnectionRefused;

        case ECONNRESET:
            return ErrorCode::ConnectionReset;

        case EXDEV:
            return ErrorCode::CrossDeviceLink;

        case EDESTADDRREQ:
            return ErrorCode::DestinationAddressRequired;

        case EBUSY:
            return ErrorCode::DeviceOrResourceBusy;

        case ENOTEMPTY:
            return ErrorCode::DirectoryNotEmpty;

        case ENOEXEC:
            return ErrorCode::ExecutableFormatError;

        case EEXIST:
            return ErrorCode::FileExists;

        case EFBIG:
            return ErrorCode::FileTooLarge;

        case ENAMETOOLONG:
            return ErrorCode::FileNameTooLong;

        case ENOSYS:
            return ErrorCode::FunctionNotSupported;

        case EHOSTUNREACH:
            return ErrorCode::HostUnreachable;

        case EIDRM:
            return ErrorCode::IdentifierRemoved;

        case EILSEQ:
            return ErrorCode::IllegalByteSequence;

        case ENOTTY:
            return ErrorCode::InappropriateIoControlOperation;

        case EINTR:
            return ErrorCode::Interrupted;

        case EINVAL:
            return ErrorCode::InvalidArgument;

        case ESPIPE:
            return ErrorCode::InvalidSeek;

        case EIO:
            return ErrorCode::IoError;

        case EISDIR:
            return ErrorCode::IsADirectory;

        case EMSGSIZE:
            return ErrorCode::MessageSize;

        case ENETDOWN:
            return ErrorCode::NetworkDown;

        case ENETRESET:
            return ErrorCode::NetworkReset;

        case ENETUNREACH:
            return ErrorCode::NetworkUnreachable;

        case ENOBUFS:
            return ErrorCode::NoBufferSpace;

        case ECHILD:
            return ErrorCode::NoChildProcess;

        case ENOLINK:
            return ErrorCode::NoLink;

        case ENOLCK:
            return ErrorCode::NoLockAvailable;

#ifdef ENODATA
        case ENODATA:
            return ErrorCode::NoMessageAvailable;
#endif

        case ENOMSG:
            return ErrorCode::NoMessage;

        case ENOPROTOOPT:
            return ErrorCode::NoProtocolOption;

        case ENOSPC:
            return ErrorCode::NoSpaceOnDevice;

#ifdef ENOSR
        case ENOSR:
            return ErrorCode::NoStreamResources;
#endif

        case ENODEV:
            return ErrorCode::NoSuchDevice;

        case ENXIO:
            return ErrorCode::NoSuchDeviceOrAddress;

        case ENOENT:
            return ErrorCode::NoSuchFileOrDirectory;

        case ESRCH:
            return ErrorCode::NoSuchProcess;

        case ENOTDIR:
            return ErrorCode::NotADirectory;

        case ENOTSOCK:
            return ErrorCode::NotASocket;
#ifdef ENOSTR
        case ENOSTR:
            return ErrorCode::NotAStream;
#endif

        case ENOTCONN:
            return ErrorCode::NotConnected;

        case ENOMEM:
            return ErrorCode::NotEnoughMemory;

        case ENOTSUP:
            return ErrorCode::NotSupported;

        case ECANCELED:
            return ErrorCode::OperationCanceled;

        case EINPROGRESS:
            return ErrorCode::OperationInProgress;

        case EPERM:
            return ErrorCode::OperationNotPermitted;

#if defined(EOPNOTSUPP) && (EOPNOTSUPP != ENOTSUP)
        case EOPNOTSUPP:
            return ErrorCode::OperationNotSupported;
#endif

#if defined(WOULDBLOCK) && (WOULDBLOCK != EAGAIN)
        case EWOULDBLOCK:
            return ErrorCode::OperationWouldBlock;
#endif

        case EOWNERDEAD:
            return ErrorCode::OwnerDead;

        case EACCES:
            return ErrorCode::PermissionDenied;

        case EPROTO:
            return ErrorCode::ProtocolError;

        case EPROTONOSUPPORT:
            return ErrorCode::ProtocolNotSupported;

        case EROFS:
            return ErrorCode::ReadOnlyFileSystem;

        case EDEADLK:
            return ErrorCode::ResourceDeadlockWouldOccur;

        case EAGAIN:
            return ErrorCode::ResourceUnavailableTryAgain;

        case ERANGE:
            return ErrorCode::ResultOutOfRange;

        case ENOTRECOVERABLE:
            return ErrorCode::StateNotRecoverable;

#ifdef ETIME
        case ETIME:
            return ErrorCode::StreamTimeout;
#endif

        case ETXTBSY:
            return ErrorCode::TextFileBusy;

        case ETIMEDOUT:
            return ErrorCode::TimedOut;

        case EMFILE:
            return ErrorCode::TooManyFilesOpen;

        case ENFILE:
            return ErrorCode::TooManyFilesOpenInSystem;

        case EMLINK:
            return ErrorCode::TooManyLinks;

        case ELOOP:
            return ErrorCode::TooManySymbolicLinkLevels;

        case EOVERFLOW:
            return ErrorCode::ValueTooLarge;

        case EPROTOTYPE:
            return ErrorCode::WrongProtocolType;

        default:
            break;
        }

        return ErrorCode::Unknown;
    }
}
