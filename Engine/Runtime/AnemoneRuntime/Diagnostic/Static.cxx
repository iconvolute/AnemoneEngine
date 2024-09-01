#include "AnemoneRuntime/Diagnostic/Static.hxx"
#include "AnemoneRuntime/Diagnostic/Trace.hxx"
#include "AnemoneRuntime/Diagnostic/StandardOutputTraceListener.hxx"

namespace Anemone::Diagnostic
{
    UninitializedObject<ErrorHandling> GErrorHandling{};
}

namespace Anemone::Diagnostic
{
    static UninitializedObject<StandardOutputTraceListener> GStandardOutputTraceListener{};

    void GenericDiagnosticStatic::Initialize()
    {
        GErrorHandling.Create();
        GTrace.Create();

        GStandardOutputTraceListener.Create();
    }

    void GenericDiagnosticStatic::Finalize()
    {
        GStandardOutputTraceListener.Destroy();

        GTrace.Destroy();
        GErrorHandling.Destroy();
    }
}
