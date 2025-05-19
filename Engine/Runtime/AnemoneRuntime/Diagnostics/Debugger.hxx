#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Base/FunctionRef.hxx"

#include <string_view>

namespace Anemone::Diagnostics
{
    RUNTIME_API void Break();

    RUNTIME_API void Crash();

    RUNTIME_API bool IsDebuggerAttached();

    RUNTIME_API void WaitForDebugger();

    RUNTIME_API bool AttachDebugger();

    RUNTIME_API void ReportApplicationStop(
        std::string_view reason);

    RUNTIME_API void GetCurrentStackTrace(
        FunctionRef<void(void* address)> callback);

    RUNTIME_API void GetCurrentStackTrace(
        FunctionRef<void(void* address, std::string_view functionName)> callback);
}
