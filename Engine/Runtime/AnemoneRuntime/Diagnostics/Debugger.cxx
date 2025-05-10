#include "AnemoneRuntime/Diagnostics/Debugger.hxx"
#include "AnemoneRuntime/Diagnostics/Internal/ConsoleTraceListener.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"

namespace Anemone::Diagnostics
{
    namespace
    {
    }

    extern void PlatformInitializeDebugger();
    extern void PlatformFinalizeDebugger();

    extern void InitializeDebugger()
    {
        PlatformInitializeDebugger();

        // TODO: Common trace listener initialization path
        //
        // * get way to determine if we should enable console trace listener
        // * similarly, initialize file handle trace listener
    }

    extern void FinalizeDebugger()
    {
        PlatformFinalizeDebugger();
    }
}
