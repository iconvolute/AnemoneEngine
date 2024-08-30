#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#include <string_view>

namespace Anemone::Diagnostic
{
    RUNTIME_API void Abort(std::string_view message);

    RUNTIME_API bool IsDebuggerAttached();

    RUNTIME_API bool AttachDebugger();
}
