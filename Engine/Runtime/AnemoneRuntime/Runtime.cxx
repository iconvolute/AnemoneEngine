#include "AnemoneRuntime/Runtime.hxx"
#include "AnemoneRuntime/Platform/Application.hxx"
#include "AnemoneRuntime/CommandLine.hxx"
#include "AnemoneRuntime/Platform/Environment.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Diagnostics/Platform/Debugger.hxx"
#include "AnemoneRuntime/Platform/ProcessorProperties.hxx"
#include "AnemoneRuntime/Platform/FileSystem.hxx"
#include "AnemoneRuntime/System/Platform/Dialogs.hxx"
#include "AnemoneRuntime/Platform/StackTrace.hxx"
#include "AnemoneRuntime/Profiler/Profiler.hxx"
#include "AnemoneRuntime/Tasks/Private/TaskScheduler.hxx"


namespace Anemone
{
    void Runtime::Initialize(int argc, char** argv)
    {
        CommandLine::Initialize(argc, argv);
        Trace::Initialize();
        Private::GDebuggerStatics.Create();
        StackTrace::Initialize();
        Environment::Initialize();
        ProcessorProperties::Initialize();
        FileSystem::Initialize();
        Application::Initialize();
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
        Application::Finalize();
        FileSystem::Finalize();
        ProcessorProperties::Finalize();
        Environment::Finalize();
        StackTrace::Finalize();
        Private::GDebuggerStatics.Destroy();
        Trace::Finalize();
        CommandLine::Finalize();
    }
}
