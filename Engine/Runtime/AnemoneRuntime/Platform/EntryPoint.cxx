#include "AnemoneRuntime/Interop/Headers.hxx"

extern "C" RUNTIME_API void AnemoneRuntimeInitialize(int argc, char** argv);
extern "C" RUNTIME_API void AnemoneRuntimeFinalize();

namespace Anemone::Internal
{
    extern void InitializeCommandLine(int argc, char** argv);
    extern void FinalizeCommandLine();

    extern void InitializeTrace();
    extern void FinalizeTrace();

    extern void InitializeDebugger();
    extern void FinalizeDebugger();

    extern void InitializeEnvironment();
    extern void FinalizeEnvironment();

    extern void InitializeProcessorProperties();
    extern void FinalizeProcessorProperties();

    extern void InitializeClipboard();
    extern void FinalizeClipboard();

    extern void InitializeApplicationResources();
    extern void FinalizeApplicationResources();

    extern void InitializeTaskScheduler();
    extern void FinalizeTaskScheduler();

}

void AnemoneRuntimeInitialize(int argc, char** argv)
{
    using namespace Anemone::Internal;

    InitializeCommandLine(argc, argv);
    InitializeTrace();
    InitializeDebugger();
    InitializeEnvironment();
    InitializeProcessorProperties();
    InitializeClipboard();
    InitializeApplicationResources();

#if ANEMONE_BUILD_PROFILING
    Anemone::Profiler::Initialize();
#endif

    InitializeTaskScheduler();
}

void AnemoneRuntimeFinalize()
{
    using namespace Anemone::Internal;

#if ANEMONE_BUILD_PROFILING
    Anemone::Profiler::Finalize();
#endif

    FinalizeApplicationResources();
    FinalizeClipboard();
    FinalizeProcessorProperties();
    FinalizeEnvironment();
    FinalizeDebugger();
    FinalizeTrace();
    FinalizeCommandLine();
}
