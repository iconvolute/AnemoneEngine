#include "AnemoneRuntime/Runtime.hxx"
#include "AnemoneRuntime/Platform/Application.hxx"
#include "AnemoneRuntime/CommandLine.hxx"
#include "AnemoneRuntime/Platform/Environment.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Platform/Debugger.hxx"
#include "AnemoneRuntime/Platform/ProcessorProperties.hxx"
#include "AnemoneRuntime/Platform/FileSystem.hxx"
#include "AnemoneRuntime/Platform/Dialogs.hxx"
#include "AnemoneRuntime/Platform/StackTrace.hxx"
#include "AnemoneRuntime/Profiler/Profiler.hxx"


namespace Anemone
{
    void Runtime::Initialize(int argc, char** argv)
    {
        CommandLine::Initialize(argc, argv);
        Trace::Initialize();
        Debugger::Initialize();
        StackTrace::Initialize();
        Environment::Initialize();
        ProcessorProperties::Initialize();
        FileSystem::Initialize();
        Application::Initialize();
        Dialogs::Initialize();

#if ANEMONE_BUILD_PROFILING
        Profiler::Initialize();
#endif
    }

    void Runtime::Finalize()
    {
#if ANEMONE_BUILD_PROFILING
        Profiler::Finalize();
#endif

        Dialogs::Finalize();
        Application::Finalize();
        FileSystem::Finalize();
        ProcessorProperties::Finalize();
        Environment::Finalize();
        StackTrace::Finalize();
        Debugger::Finalize();
        Trace::Finalize();
        CommandLine::Finalize();
    }
}
