#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"
#include "AnemoneRuntime/Platform/Base/BaseStackTrace.hxx"
#include "AnemoneRuntime/FunctionRef.hxx"

#include <string_view>

namespace Anemone
{
    struct LinuxStackTrace final
    {
        static RUNTIME_API void Walk(FunctionRef<void(void* address)> callback);
        static RUNTIME_API void Walk(FunctionRef<void(void* address, std::string_view symbol)> callback);
    };

    using StackTrace = LinuxStackTrace;
}
