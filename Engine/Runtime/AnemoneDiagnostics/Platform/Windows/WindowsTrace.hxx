#pragma once
#include "AnemoneDiagnostics/Trace.hxx"

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
