#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include <cstdint>

#include <fmt/format.h>

namespace Anemone
{
    enum class ErrorCode : uint32_t
    {
        Success,

        InvalidArgument,
        InvalidAddress,
        InvalidOperation,
        InvalidChecksum,
        InvalidDirectory,
        InvalidHandle,
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
        EndOfFile,
        IoError,
        OperationCanceled,
        OperationTimeout,
        OperationInProgress,
        OperationInterrupted,
        OperationAborted,
        OperationRefused,
        FileNotFound,
        PathNotFound,
        AccessDenied,
        SharingViolation,
        FileExists,
        DirectoryExists,
        AlreadyExists,
        CircularReference,
        DirectoryNotEmpty,
        Unknown,
    };
}

template <>
struct fmt::formatter<Anemone::ErrorCode> : fmt::formatter<uint32_t>
{
    constexpr auto parse(auto& context)
    {
        return context.begin();
    }

    constexpr auto format(Anemone::ErrorCode const& value, auto& context) const
    {
        return fmt::format_to(context.out(), "{:08x}", static_cast<uint32_t>(value));
    }
};
