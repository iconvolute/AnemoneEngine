#include "AnemoneRuntime/Platform/Detect.hxx"

#if ANEMONE_BUILD_PROFILING
#include "AnemoneRuntime/Profiler/Profiler.hxx"
#include "AnemoneRuntime/Profiler/ProfilerNvidia.hxx"
#endif

namespace Anemone::Profiler
{
#if false
#if ANEMONE_BUILD_PROFILING
    Profiler* GProfiler = nullptr;
#endif

    void InitializeRuntime(RuntimeInitializeContext& context)
    {
        (void)context;

        // Initialize profiler
#if ANEMONE_BUILD_PROFILING
        GProfiler = new Anemone::Profiler::ProfilerNvidia();
#endif
    }

    void FinalizeRuntime(RuntimeFinalizeContext& context)
    {
        (void)context;

#if ANEMONE_BUILD_PROFILING
        delete GProfiler;
#endif
    }
#endif
}
