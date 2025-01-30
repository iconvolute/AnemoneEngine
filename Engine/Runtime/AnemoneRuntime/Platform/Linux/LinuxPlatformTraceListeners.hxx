#pragma once
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

namespace Anemone
{
    struct LinuxPlatformTraceListeners final
    {
        RUNTIME_API static void Initialize() { }
        RUNTIME_API static void Finalize() { }
    };

    using PlatformTraceListeners = LinuxPlatformTraceListeners;
}

