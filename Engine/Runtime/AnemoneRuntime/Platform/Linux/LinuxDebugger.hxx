#pragma once
#include "AnemoneRuntime/Platform/Base/BaseDebugger.hxx"

#include <string_view>
#include <fmt/format.h>

namespace Anemone
{
    struct LinuxDebugger final
    {
        static RUNTIME_API void Break();
        static RUNTIME_API void Crash();
        static RUNTIME_API bool IsAttached();
        static RUNTIME_API void Wait();
        static RUNTIME_API bool Attach();

        static RUNTIME_API void ReportApplicationStop(std::string_view reason);
    };

    using Debugger = LinuxDebugger;
}
