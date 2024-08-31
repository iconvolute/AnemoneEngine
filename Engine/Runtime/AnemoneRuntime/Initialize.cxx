#include "AnemoneRuntime/Initialize.hxx"
#include "AnemoneRuntime/Platform/Static.hxx"
#include "AnemoneRuntime/Diagnostic/Static.hxx"
#include "AnemoneRuntime/Profiler/Static.hxx"
#include "AnemoneRuntime/System/Static.hxx"
#include "AnemoneRuntime/Network/Static.hxx"

namespace Anemone
{
    void InitializeRuntime(int argc, char* argv[])
    {
        (void)argc;
        (void)argv;
        Platform::InitializeStatic();
        Diagnostic::InitializeStatic();
        Network::InitializeStatic();
        System::InitializeStatic();
        Profiler::InitializeStatic();
    }

    void ShutdownRuntime()
    {
        Profiler::ShutdownStatic();
        System::ShutdownStatic();
        Network::ShutdownStatic();
        Diagnostic::ShutdownStatic();
        Platform::ShutdownStatic();
    }
}
