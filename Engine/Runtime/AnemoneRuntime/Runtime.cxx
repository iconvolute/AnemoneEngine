#include "AnemoneRuntime/Runtime.hxx"
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

namespace Anemone
{
    void Runtime::Initialize(int argc, char** argv)
    {
        Internal::GCommandLineStatics.Create(argc, argv);
        Internal::GTraceStatics.Create();
        Internal::GDebuggerStatics.Create();
        StackTrace::Initialize();
        Internal::GEnvironmentStatics.Create();
        Internal::GClipboardStatics.Create();
        Internal::GProcessorProperties.Create();
        FileSystem::Initialize();
        Internal::GApplicationStatics.Create();
        Internal::GDialogsStatics.Create();

#if ANEMONE_BUILD_PROFILING
        Profiler::Initialize();
#endif

        Internal::GTaskSchedulerStatics.Create();
    }

    void Runtime::Finalize()
    {
        Internal::GTaskSchedulerStatics.Destroy();

#if ANEMONE_BUILD_PROFILING
        Profiler::Finalize();
#endif

        Internal::GDialogsStatics.Destroy();
        Internal::GApplicationStatics.Destroy();
        FileSystem::Finalize();
        Internal::GProcessorProperties.Destroy();
        Internal::GClipboardStatics.Destroy();
        Internal::GEnvironmentStatics.Destroy();
        StackTrace::Finalize();
        Internal::GDebuggerStatics.Destroy();
        Internal::GTraceStatics.Destroy();
        Internal::GCommandLineStatics.Destroy();
    }
}
