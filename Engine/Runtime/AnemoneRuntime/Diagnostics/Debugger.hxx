#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include <string_view>

namespace Anemone::Diagnostics
{
    RUNTIME_API void Break();
    RUNTIME_API void Crash();
    RUNTIME_API bool IsDebuggerAttached();
    RUNTIME_API void WaitForDebugger();
    RUNTIME_API bool AttachDebugger();
    RUNTIME_API void ReportApplicationStop(std::string_view reason);
}
