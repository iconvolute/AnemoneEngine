#pragma once
#include "AnemoneInterop/Headers.hxx"

namespace Anemone
{
    class Module_Profiler
    {
    public:
        PROFILER_API static void Initialize();
        PROFILER_API static void Finalize();
    };
}
