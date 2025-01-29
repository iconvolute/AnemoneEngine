#pragma once
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

namespace Anemone
{
    struct WindowsPlatformTraceListeners final
    {
        RUNTIME_API static void Initialize();
        RUNTIME_API static void Finalize();
    };

    using PlatformTraceListeners = WindowsPlatformTraceListeners;
}
