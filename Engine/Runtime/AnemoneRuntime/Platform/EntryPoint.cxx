#include "AnemoneRuntime/Platform/FileSystem.hxx"
#include "AnemoneRuntime/Platform/StackTrace.hxx"
#include "AnemoneRuntime/Profiler/Profiler.hxx"

#include "AnemoneRuntime/Tasks/Internal/TaskScheduler.hxx"
#include "AnemoneRuntime/Diagnostics/Internal/TraceStatics.hxx"
#include "AnemoneRuntime/Diagnostics/Platform/Debugger.hxx"
#include "AnemoneRuntime/System/Platform/Application.hxx"
#include "AnemoneRuntime/System/Platform/Dialogs.hxx"
#include "AnemoneRuntime/System/Platform/Clipboard.hxx"
#include "AnemoneRuntime/System/Platform/ProcessorProperties.hxx"
#include "AnemoneRuntime/System/Platform/Environment.hxx"
#include "AnemoneRuntime/System/Internal/CommandLineStatics.hxx"

extern "C" RUNTIME_API void AnemoneRuntimeInitialize(int argc, char** argv);
extern "C" RUNTIME_API void AnemoneRuntimeFinalize();

void AnemoneRuntimeInitialize(int argc, char** argv)
{
    Anemone::Internal::GCommandLineStatics.Create(argc, argv);
    Anemone::Internal::GTraceStatics.Create();
    Anemone::Internal::GDebuggerStatics.Create();
    Anemone::StackTrace::Initialize();
    Anemone::Internal::GEnvironmentStatics.Create();
    Anemone::Internal::GClipboardStatics.Create();
    Anemone::Internal::GProcessorProperties.Create();
    Anemone::FileSystem::Initialize();
    Anemone::Internal::GApplicationStatics.Create();
    Anemone::Internal::GDialogsStatics.Create();

#if ANEMONE_BUILD_PROFILING
    Anemone::Profiler::Initialize();
#endif

    Anemone::Internal::GTaskSchedulerStatics.Create();
}

void AnemoneRuntimeFinalize()
{
    Anemone::Internal::GTaskSchedulerStatics.Destroy();

#if ANEMONE_BUILD_PROFILING
    Anemone::Profiler::Finalize();
#endif

    Anemone::Internal::GDialogsStatics.Destroy();
    Anemone::Internal::GApplicationStatics.Destroy();
    Anemone::FileSystem::Finalize();
    Anemone::Internal::GProcessorProperties.Destroy();
    Anemone::Internal::GClipboardStatics.Destroy();
    Anemone::Internal::GEnvironmentStatics.Destroy();
    Anemone::StackTrace::Finalize();
    Anemone::Internal::GDebuggerStatics.Destroy();
    Anemone::Internal::GTraceStatics.Destroy();
    Anemone::Internal::GCommandLineStatics.Destroy();
}
