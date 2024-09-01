#include "AnemoneRuntime/Initialize.hxx"
#include "AnemoneRuntime/Platform/Static.hxx"
#include "AnemoneRuntime/Diagnostic/Static.hxx"
#include "AnemoneRuntime/Profiler/Static.hxx"
#include "AnemoneRuntime/System/Static.hxx"
#include "AnemoneRuntime/Network/Static.hxx"
#include "AnemoneRuntime/Tasks/Static.hxx"

namespace Anemone
{
    void InitializeRuntime(int argc, char* argv[])
    {
        (void)argc;
        (void)argv;
        Platform::PlatformStatic::Initialize();
        Diagnostic::DiagnosticStatic::Initialize();
        Network::NetworkStatic::Initialize();
        System::SystemStatic::Initialize();
        Profiler::ProfilerStatic::Initialize();
        Tasks::TasksStatic::Initialize();
    }

    void ShutdownRuntime()
    {
        Tasks::TasksStatic::Finalize();
        Profiler::ProfilerStatic::Finalize();
        System::SystemStatic::Finalize();
        Network::NetworkStatic::Finalize();
        Diagnostic::DiagnosticStatic::Finalize();
        Platform::PlatformStatic::Finalize();
    }
}
