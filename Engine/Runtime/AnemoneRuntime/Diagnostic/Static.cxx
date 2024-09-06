#include "AnemoneRuntime/Diagnostic/Static.hxx"
#include "AnemoneRuntime/Diagnostic/Trace.hxx"
#include "AnemoneRuntime/Diagnostic/StandardOutputTraceListener.hxx"

namespace Anemone::Diagnostic
{
    static UninitializedObject<StandardOutputTraceListener> GStandardOutputTraceListener{};

    void GenericDiagnosticStatic::Initialize()
    {
        GTrace.Create();

        GStandardOutputTraceListener.Create();
    }

    void GenericDiagnosticStatic::Finalize()
    {
        GStandardOutputTraceListener.Destroy();

        GTrace.Destroy();
    }
}
