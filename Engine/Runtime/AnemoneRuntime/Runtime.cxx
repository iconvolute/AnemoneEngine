#include "AnemoneRuntime/Runtime.hxx"
#include "AnemoneRuntime/Platform/FileSystem.hxx"
#include "AnemoneRuntime/Platform/StackTrace.hxx"
#include "AnemoneRuntime/Profiler/Profiler.hxx"

#include "AnemoneRuntime/Tasks/Private/TaskScheduler.hxx"
#include "AnemoneRuntime/Diagnostics/Private/TraceStatics.hxx"
#include "AnemoneRuntime/Diagnostics/Platform/Debugger.hxx"
#include "AnemoneRuntime/System/Platform/Application.hxx"
#include "AnemoneRuntime/System/Platform/Dialogs.hxx"
#include "AnemoneRuntime/System/Platform/Clipboard.hxx"
#include "AnemoneRuntime/System/Platform/ProcessorProperties.hxx"
#include "AnemoneRuntime/System/Platform/Environment.hxx"
#include "AnemoneRuntime/System/Private/CommandLineStatics.hxx"

namespace Anemone
{
    void Runtime::Initialize(int argc, char** argv)
    {
        Private::GCommandLineStatics.Create(argc, argv);
        Private::GTraceStatics.Create();
        Private::GDebuggerStatics.Create();
        StackTrace::Initialize();
        Private::GEnvironmentStatics.Create();
        Private::GClipboardStatics.Create();
        Private::GProcessorProperties.Create();
        FileSystem::Initialize();
        Private::GApplicationStatics.Create();
        Private::GDialogsStatics.Create();

#if ANEMONE_BUILD_PROFILING
        Profiler::Initialize();
#endif

        Private::GTaskSchedulerStatics.Create();
    }

    void Runtime::Finalize()
    {
        Private::GTaskSchedulerStatics.Destroy();

#if ANEMONE_BUILD_PROFILING
        Profiler::Finalize();
#endif

        Private::GDialogsStatics.Destroy();
        Private::GApplicationStatics.Destroy();
        FileSystem::Finalize();
        Private::GProcessorProperties.Destroy();
        Private::GClipboardStatics.Destroy();
        Private::GEnvironmentStatics.Destroy();
        StackTrace::Finalize();
        Private::GDebuggerStatics.Destroy();
        Private::GTraceStatics.Destroy();
        Private::GCommandLineStatics.Destroy();
    }
}
