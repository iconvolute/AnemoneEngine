#include "AnemoneProfiler/Profiler.hxx"

PROFILER_API void AnemoneProfiler_ModuleInitialize()
{
#if ANEMONE_BUILD_PROFILING
    Anemone::Profiler::Initialize();
#endif
}

PROFILER_API void AnemoneProfiler_ModuleFinalize()
{
#if ANEMONE_BUILD_PROFILING
    Anemone::Profiler::Finalize();
#endif
}
