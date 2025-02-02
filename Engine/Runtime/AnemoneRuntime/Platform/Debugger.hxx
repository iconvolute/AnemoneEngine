#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#include <string_view>

namespace Anemone
{
    struct Debugger final
    {
        static RUNTIME_API void Break();
        static RUNTIME_API void Crash();
        static RUNTIME_API bool IsAttached();
        static RUNTIME_API void Wait();
        static RUNTIME_API bool Attach();

        static RUNTIME_API void ReportApplicationStop(std::string_view reason);
    };
}
