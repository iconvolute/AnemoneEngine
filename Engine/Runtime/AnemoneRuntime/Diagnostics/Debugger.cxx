#include "AnemoneRuntime/Diagnostics/Debugger.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Diagnostics/Internal/ConsoleTraceListener.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"

namespace Anemone::Diagnostics
{
    namespace
    {
        UninitializedObject<ConsoleTraceListener> GConsoleTraceListener{};
    }

    extern void PlatformInitializeDebugger();
    extern void PlatformFinalizeDebugger();

    extern void InitializeDebugger(bool console)
    {
        PlatformInitializeDebugger();

        // TODO: Common trace listener initialization path
        //
        // * similarly, initialize file handle trace listener

        if (console)
        {
            GConsoleTraceListener.Create();
            GetTraceDispatcher().Register(*GConsoleTraceListener);
        }
    }

    extern void FinalizeDebugger()
    {
        if (GConsoleTraceListener)
        {
            GetTraceDispatcher().Unregister(*GConsoleTraceListener);
            GConsoleTraceListener.Destroy();
        }

        PlatformFinalizeDebugger();
    }
}
