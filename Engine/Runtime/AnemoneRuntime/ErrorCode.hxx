#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include <cstdint>

#include <fmt/format.h>

namespace Anemone
{
    enum class ErrorCode : uint16_t
    {
        Success,
        AddressFamilyNotSupported,
        AddressInUse,
        AddressNotAvailable,
        AlreadyConnected,
        ArgumentListTooLong,
        ArgumentOutOfDomain,
        BadAddress,
        BadFileDescriptor,
        BadMessage,
        BrokenPipe,
        ConnectionAborted,
        ConnectionAlreadyInProgress,
        ConnectionRefused,
        ConnectionReset,
        CrossDeviceLink,
        DestinationAddressRequired,
        DeviceOrResourceBusy,
        DirectoryNotEmpty,
        ExecutableFormatError,
        FileExists,
        FileTooLarge,
        FileNameTooLong,
        FunctionNotSupported,
        HostUnreachable,
        IdentifierRemoved,
        IllegalByteSequence,
        InappropriateIoControlOperation,
        Interrupted,
        InvalidArgument,
        InvalidSeek,
        IoError,
        IsADirectory,
        MessageSize,
        NetworkDown,
        NetworkReset,
        NetworkUnreachable,
        NoBufferSpace,
        NoChildProcess,
        NoLink,
        NoLockAvailable,
        NoMessageAvailable,
        NoMessage,
        NoProtocolOption,
        NoSpaceOnDevice,
        NoStreamResources,
        NoSuchDeviceOrAddress,
        NoSuchDevice,
        NoSuchFileOrDirectory,
        NoSuchProcess,
        NotADirectory,
        NotASocket,
        NotAStream,
        NotConnected,
        NotEnoughMemory,
        NotSupported,
        OperationCanceled,
        OperationInProgress,
        OperationNotPermitted,
        OperationNotSupported,
        OperationWouldBlock,
        OwnerDead,
        PermissionDenied,
        ProtocolError,
        ProtocolNotSupported,
        ReadOnlyFileSystem,
        ResourceDeadlockWouldOccur,
        ResourceUnavailableTryAgain,
        ResultOutOfRange,
        StateNotRecoverable,
        StreamTimeout,
        TextFileBusy,
        TimedOut,
        TooManyFilesOpenInSystem,
        TooManyFilesOpen,
        TooManyLinks,
        TooManySymbolicLinkLevels,
        ValueTooLarge,
        WrongProtocolType,
        EndOfFile,
        Unknown,
    };
}

namespace Anemone::System::Private
{
    RUNTIME_API ErrorCode ErrorCodeFromErrno(int error);

    RUNTIME_API ErrorCode ErrorCodeFromWin32Error(uint32_t error);
}

template <>
struct fmt::formatter<Anemone::ErrorCode> : fmt::formatter<uint16_t>
{
    constexpr auto parse(auto& context)
    {
        return context.begin();
    }

    constexpr auto format(Anemone::ErrorCode const& value, auto& context) const
    {
        return fmt::format_to(context.out(), "{}", static_cast<uint16_t>(value));
    }
};
