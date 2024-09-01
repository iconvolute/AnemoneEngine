#include "AnemoneRuntime/Profiler/Static.hxx"

#if ANEMONE_BUILD_PROFILING
#include "AnemoneRuntime/Profiler/Profiler.hxx"
#include "AnemoneRuntime/Profiler/ProfilerNvidia.hxx"
#endif

namespace Anemone::Profiler
{
#if ANEMONE_BUILD_PROFILING
    Profiler* GProfiler = nullptr;
#endif

    void ProfilerStatic::Initialize()
    {
        // Initialize profiler
#if ANEMONE_BUILD_PROFILING
        GProfiler = new Anemone::Profiler::ProfilerNvidia();
#endif
    }

    void ProfilerStatic::Finalize()
    {
#if ANEMONE_BUILD_PROFILING
        delete GProfiler;
#endif
    }
}
