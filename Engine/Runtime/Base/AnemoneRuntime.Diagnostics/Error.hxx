#pragma once
#include "AnemoneRuntime.Interop/Headers.hxx"
#include <cstdint>

#include <fmt/format.h>

namespace Anemone
{
    enum class Error : uint32_t
    {
        Success,

        // Generic failure
        Failure,

        Abandoned,
        AccessDenied,
        Alerted,
        AlreadyConnected,
        AlreadyExists,
        AlreadyInitialized,
        ArithmeticOverflow,
        Break,
        BufferOverflow,
        Canceled,
        CircularReference,
        Continue,
        DeviceNotFound,
        DirectoryExists,
        DirectoryNotEmpty,
        DirectoryNotFound,
        EndOfFile,
        FileExists,
        FileNotFound,
        Filtered,
        InProgress,
        Interrupted,
        InUse,
        InvalidAddress,
        InvalidArgument,
        InvalidAttribute,
        InvalidBlock,
        InvalidBuffer,
        InvalidChecksum,
        InvalidConversion,
        InvalidData,
        InvalidDescriptor,
        InvalidDigest,
        InvalidDirectory,
        InvalidFile,
        InvalidFlags,
        InvalidFormat,
        InvalidFunction,
        InvalidHandle,
        InvalidHeader,
        InvalidId,
        InvalidMetadata,
        InvalidModule,
        InvalidObject,
        InvalidOperation,
        InvalidOption,
        InvalidPath,
        InvalidPointer,
        InvalidRange,
        InvalidRequest,
        InvalidResource,
        InvalidSeek,
        InvalidSignature,
        InvalidSize,
        InvalidSocket,
        InvalidState,
        InvalidStream,
        InvalidSyntax,
        InvalidThread,
        InvalidTransition,
        InvalidType,
        InvalidValue,
        IoError,
        NotAllowed,
        NotAvailable,
        NotConnected,
        NotEnoughMemory,
        NotEnoughResources,
        NotEnoughSpace,
        NotExpected,
        NotFound,
        NotImplemented,
        NotInitialized,
        NotPermitted,
        NotReady,
        NotRecoverable,
        NotRegistered,
        NotResumable,
        NotSigned,
        NotSupported,
        PathNotFound,
        Pending,
        PermissionDenied,
        Recursion,
        Refused,
        Reset,
        SharingViolation,
        Timeout,
        TypeNotFound,
        Unreachable,
        Unsafe,
        WouldBlock,
    };
}

template <>
struct fmt::formatter<Anemone::Error> : fmt::formatter<uint32_t>
{
    constexpr auto parse(auto& context)
    {
        return context.begin();
    }

    constexpr auto format(Anemone::Error const& value, auto& context) const
    {
        return fmt::format_to(context.out(), "{:08x}", static_cast<uint32_t>(value));
    }
};
