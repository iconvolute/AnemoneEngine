#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include <cstdint>

#include <fmt/format.h>

namespace Anemone
{
    enum class Status : uint32_t
    {
        Success,
        Filtered,
        Continue,
        Break,

        Failed,
        InvalidAttribute,
        InvalidBlock,
        InvalidArgument,
        InvalidAddress,
        InvalidOperation,
        InvalidChecksum,
        InvalidDirectory,
        InvalidHandle,
        InvalidFlags,
        InvalidDescriptor,
        InvalidFormat,
        InvalidPath,
        InvalidFunction,
        InvalidData,
        InvalidConversion,
        InvalidRange,
        InvalidSyntax,
        InvalidType,
        InvalidTransition,
        InvalidState,
        InvalidBuffer,
        InvalidRequest,
        InvalidObject,
        InvalidModule,
        ArithmeticOverflow,
        BufferOverflow,
        NotSupported,
        NotImplemented,
        NotFound,
        NotConnected,
        NotInitialized,
        NotReady,
        NotAvailable,
        NotEnoughMemory,
        NotEnoughResources,
        NotEnoughSpace,
        NoInterface,
        EndOfFile,
        IoError,
        OperationCanceled,
        OperationTimeout,
        OperationInProgress,
        OperationInterrupted,
        OperationAborted,
        OperationRefused,
        OperationPending,
        FileNotFound,
        PathNotFound,
        TypeNotFound,
        AccessDenied,
        SharingViolation,
        FileExists,
        DirectoryExists,
        AlreadyExists,
        CircularReference,
        DirectoryNotEmpty,
        OperationUnsafe,
        Recursion,
        InvalidValue,
        InvalidPointer,
        DifferentThread,
        AlreadyInitialized,
        NotRegistered,
        NotSigned,
        NotAllowed,
        NotResumable,
        Unexpected,
        InvalidMetadata,
        InvalidSignature,
        InvalidHeader,
        InvalidId,
        InvalidDigest,
        PossibleDeadlock,
        InvalidSize,
        
        Unknown,
    };
}

template <>
struct fmt::formatter<Anemone::Status> : fmt::formatter<uint32_t>
{
    constexpr auto parse(auto& context)
    {
        return context.begin();
    }

    constexpr auto format(Anemone::Status const& value, auto& context) const
    {
        return fmt::format_to(context.out(), "{:08x}", static_cast<uint32_t>(value));
    }
};
