#pragma once
#include "AnemoneDiagnostics/Debug.hxx"

namespace Anemone
{
    class LinuxDebug final
    {
    public:
        static void Initialize();
        static void Finalize();
    };

    using PlatformDebug = LinuxDebug;
}
