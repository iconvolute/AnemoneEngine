#include "AnemoneRuntime/Runtime.hxx"
#include "AnemoneRuntime/Platform/Runtime.hxx"
#include "AnemoneRuntime/Diagnostic/Runtime.hxx"
#include "AnemoneRuntime/Profiler/Runtime.hxx"
#include "AnemoneRuntime/System/Runtime.hxx"
#include "AnemoneRuntime/Network/Runtime.hxx"
#include "AnemoneRuntime/Tasks/Runtime.hxx"
#include "AnemoneRuntime/App/Runtime.hxx"

namespace Anemone
{
    struct RuntimeInitializeContext{ };
    struct RuntimeFinalizeContext{ };

    void InitializeRuntime(int argc, char* argv[])
    {
        RuntimeInitializeContext context{};

        (void)argc;
        (void)argv;
        Platform::InitializeRuntime(context);
        Diagnostic::InitializeRuntime(context);
        Network::InitializeRuntime(context);
        System::InitializeRuntime(context);
        App::InitializeRuntime(context);
        Profiler::InitializeRuntime(context);
        Tasks::InitializeRuntime(context);
    }

    void ShutdownRuntime()
    {
        RuntimeFinalizeContext context{};

        Tasks::FinalizeRuntime(context);
        Profiler::FinalizeRuntime(context);
        App::FinalizeRuntime(context);
        System::FinalizeRuntime(context);
        Network::FinalizeRuntime(context);
        Diagnostic::FinalizeRuntime(context);
        Platform::FinalizeRuntime(context);
    }
}
