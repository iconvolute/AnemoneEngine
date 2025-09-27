#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Storage/FileSystem.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/System/Clipboard.hxx"
#include "AnemoneRuntime/Tasks/TaskScheduler.hxx"
#include "AnemoneRuntime/Profiler/Profiler.hxx"

namespace Anemone::Internal
{
    extern int GCommandLineArgC;
    extern char** GCommandLineArgV;
    extern bool GIsConsoleApplication;

    extern void InitializeEnvironment();
    extern void FinalizeEnvironment();
}

extern "C" RUNTIME_API void AnemoneRuntimeInitialize(int argc, char** argv, bool console)
{
    Anemone::Internal::GCommandLineArgC = argc;
    Anemone::Internal::GCommandLineArgV = argv;
    Anemone::Internal::GIsConsoleApplication = console;

    Anemone::Trace::Initialize();
    Anemone::Debug::Initialize();
    Anemone::Internal::InitializeEnvironment();
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

    Anemone::Internal::FinalizeEnvironment();
    Anemone::Debug::Finalize();
    Anemone::Trace::Finalize();
}
