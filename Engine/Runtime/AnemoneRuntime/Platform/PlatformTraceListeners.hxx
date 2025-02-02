#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

namespace Anemone
{
    struct PlatformTraceListeners final
    {
        RUNTIME_API static void Initialize();
        RUNTIME_API static void Finalize();
    };
}
