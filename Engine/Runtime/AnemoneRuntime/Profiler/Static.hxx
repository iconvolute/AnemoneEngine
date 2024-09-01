#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

namespace Anemone::Profiler
{
    struct ProfilerStatic
    {
        static void Initialize();
        static void Finalize();
    };
}
