#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#include <string_view>

namespace Anemone
{
    struct Debugger final
    {
        static void Initialize();
        static void Finalize();

        RUNTIME_API static void Break();
        RUNTIME_API static void Crash();
        RUNTIME_API static bool IsAttached();
        RUNTIME_API static void Wait();
        RUNTIME_API static bool Attach();
        RUNTIME_API static void ReportApplicationStop(std::string_view reason);
    };
}
