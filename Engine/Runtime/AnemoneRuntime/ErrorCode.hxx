#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include <cstdint>

#include <fmt/format.h>

namespace Anemone
{
    enum class ErrorCode : uint32_t
    {
        Success,
        
        InvalidArgument,
        InvalidOperation,
        InvalidHandle,
        InvalidFormat,
        InvalidPath,
        NotSupported,
        NotImplemented,
        NotFound,
        OutOfMemory,
        EndOfFile,
        IoError,
        OperationCanceled,
        OperationTimeout,
        OperationInProgress,
        Unknown,
    };
}

namespace Anemone::System::Private
{
    RUNTIME_API ErrorCode ErrorCodeFromErrno(int error);

    RUNTIME_API ErrorCode ErrorCodeFromWin32Error(uint32_t error);
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
