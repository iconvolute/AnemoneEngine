#pragma once
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

namespace Anemone
{
    class LinuxTrace final
    {
    public:
        RUNTIME_API static void Initialize();
        RUNTIME_API static void Finalize();
    };

    using PlatformTrace = LinuxTrace;
}
