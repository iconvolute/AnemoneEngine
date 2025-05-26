#include "AnemoneRuntime/Interop/Headers.hxx"

namespace Anemone::Internal
{
    extern void InitializeTrace();
    extern void FinalizeTrace();

    extern void InitializeDebug();
    extern void FinalizeDebug();

    extern void InitializeDiagnostics()
    {
        InitializeTrace();
        InitializeDebug();
    }

    extern void FinalizeDiagnostics()
    {
        FinalizeDebug();
        FinalizeTrace();
    }
}
