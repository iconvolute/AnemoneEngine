#include "AnemoneProfiler/Module.hxx"
#include "AnemoneProfiler/Profiler.hxx"

namespace Anemone
{
    void Module_Profiler::Initialize()
    {
#if ANEMONE_BUILD_PROFILING
        Profiler::Initialize();
#endif
    }

    void Module_Profiler::Finalize()
    {
#if ANEMONE_BUILD_PROFILING
        Profiler::Finalize();
#endif
    }
}
