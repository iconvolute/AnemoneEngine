#include "AnemoneRuntime/Diagnostic/Static.hxx"
#include "AnemoneRuntime/Diagnostic/Trace.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Diagnostic/StandardOutputTraceListener.hxx"

namespace Anemone::Diagnostic
{
    static UninitializedObject<StandardOutputTraceListener> GStandardOutputTraceListener{};

    void InitializeStatic()
    {
        GStandardOutputTraceListener.Create();
    }

    void ShutdownStatic()
    {
        GStandardOutputTraceListener.Destroy();
    }
}
