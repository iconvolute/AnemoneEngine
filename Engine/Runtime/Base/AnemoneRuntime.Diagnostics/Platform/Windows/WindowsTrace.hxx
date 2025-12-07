#pragma once
#include "AnemoneRuntime.Diagnostics/Trace.hxx"

namespace Anemone
{
    class WindowsTrace final
    {
    public:
        static void Initialize();
        static void Finalize();
    };

    using PlatformTrace = WindowsTrace;
}
