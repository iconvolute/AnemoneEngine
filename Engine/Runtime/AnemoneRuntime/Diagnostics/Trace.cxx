#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"
#include "AnemoneRuntime/Diagnostics/Internal/ConsoleTraceListener.hxx"
#include "AnemoneRuntime/System/Environment.hxx"

namespace Anemone::Diagnostics
{
    namespace
    {
        UninitializedObject<Trace> GTrace{};
        UninitializedObject<ConsoleTraceListener> GConsoleTraceListener{};
    }

    Trace& Trace::Get()
    {
        return *GTrace;
    }
};

namespace Anemone::Internal
{
    extern void PlatformInitializeTrace();
    extern void PlatformFinalizeTrace();

    extern void InitializeTrace()
    {
        Diagnostics::GTrace.Create();

        PlatformInitializeTrace();

        if (Environment::IsConsoleApplication())
        {
            Diagnostics::GConsoleTraceListener.Create();
            Diagnostics::Trace::Get().Register(*Diagnostics::GConsoleTraceListener);
        }
    }

    extern void FinalizeTrace()
    {
        if (Diagnostics::GConsoleTraceListener)
        {
            Diagnostics::Trace::Get().Unregister(*Diagnostics::GConsoleTraceListener);
            Diagnostics::GConsoleTraceListener.Destroy();
        }

        PlatformFinalizeTrace();

        Diagnostics::GTrace.Destroy();
    }
}
