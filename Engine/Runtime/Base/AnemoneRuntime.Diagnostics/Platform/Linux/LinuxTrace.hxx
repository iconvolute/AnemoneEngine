#pragma once
#include "AnemoneRuntime.Diagnostics/Trace.hxx"

namespace Anemone
{
    class LinuxTrace final
    {
    public:
        ANEMONE_RUNTIME_BASE_API static void Initialize();
        ANEMONE_RUNTIME_BASE_API static void Finalize();
    };

    using PlatformTrace = LinuxTrace;
}
