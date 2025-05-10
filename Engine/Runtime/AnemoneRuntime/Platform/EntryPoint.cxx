#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Profiler/Profiler.hxx"

namespace Anemone::Environment
{
    extern void Initialize();
    extern void Finalize();
}

namespace Anemone::Clipboard
{
    extern void Initialize();
    extern void Finalize();
}

namespace Anemone::CommandLine
{
    extern void Initialize(int argc, char** argv);
    extern void Finalize();
}

namespace Anemone::Diagnostics
{
    extern void InitializeTrace();
    extern void FinalizeTrace();

    extern void InitializeDebugger();
    extern void FinalizeDebugger();
}

namespace Anemone::Input
{
    extern void Initialize();
    extern void Finalize();
}

namespace Anemone::Threading
{
    extern void InitializeTaskScheduler();
    extern void FinalizeTaskScheduler();
}

extern "C" RUNTIME_API void AnemoneRuntimeInitialize(int argc, char** argv);
extern "C" RUNTIME_API void AnemoneRuntimeFinalize();

void AnemoneRuntimeInitialize(int argc, char** argv)
{
    Anemone::CommandLine::Initialize(argc, argv);
    Anemone::Diagnostics::InitializeTrace();
    Anemone::Diagnostics::InitializeDebugger();

    Anemone::Environment::Initialize();
    Anemone::Input::Initialize();
    Anemone::Clipboard::Initialize();

#if ANEMONE_BUILD_PROFILING
    Anemone::Profiler::Initialize();
#endif

    Anemone::Threading::InitializeTaskScheduler();
}

void AnemoneRuntimeFinalize()
{
#if ANEMONE_BUILD_PROFILING
    Anemone::Profiler::Finalize();
#endif

    Anemone::Threading::FinalizeTaskScheduler();
    Anemone::Clipboard::Finalize();
    Anemone::Input::Finalize();
    Anemone::Environment::Finalize();
    Anemone::Diagnostics::FinalizeDebugger();
    Anemone::Diagnostics::FinalizeTrace();
    Anemone::CommandLine::Finalize();
}
