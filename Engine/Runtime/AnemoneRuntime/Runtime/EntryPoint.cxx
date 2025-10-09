#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Storage/FileSystem.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/System/Clipboard.hxx"
#include "AnemoneRuntime/Tasks/TaskScheduler.hxx"
#include "AnemoneRuntime/Profiler/Profiler.hxx"
#include "AnemoneRuntime/System/ProcessorProperties.hxx"
#include "AnemoneRuntime/System/Environment.hxx"

namespace Anemone::Internal
{
    extern int GCommandLineArgC;
    extern char** GCommandLineArgV;
}

extern "C" RUNTIME_API void AnemoneRuntimeInitialize(int argc, char** argv, bool console)
{
    Anemone::Internal::GCommandLineArgC = argc;
    Anemone::Internal::GCommandLineArgV = argv;

    Anemone::Trace::Initialize();
    Anemone::Debug::Initialize();
    Anemone::Environment::Initialize(console);
    Anemone::ProcessorProperties::Initialize();
    Anemone::FileSystem::Initialize();
    Anemone::Clipboard::Initialize();

    Anemone::TaskScheduler::Initialize();

#if ANEMONE_BUILD_PROFILING
    Anemone::Profiler::Initialize();
#endif
}

extern "C" RUNTIME_API void AnemoneRuntimeFinalize()
{
#if ANEMONE_BUILD_PROFILING
    Anemone::Profiler::Finalize();
#endif

    Anemone::TaskScheduler::Finalize();

    Anemone::Clipboard::Finalize();
    Anemone::FileSystem::Finalize();
    Anemone::ProcessorProperties::Finalize();
    Anemone::Environment::Finalize();
    Anemone::Debug::Finalize();
    Anemone::Trace::Finalize();
}
