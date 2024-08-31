#include "AnemoneRuntime/Diagnostic/Static.hxx"
#include "AnemoneRuntime/Diagnostic/Trace.hxx"
#include "AnemoneRuntime/Diagnostic/StandardOutputTraceListener.hxx"

namespace Anemone::Diagnostic
{
    UninitializedObject<DiagnosticStatic> GDiagnosticStatic{};
}

namespace Anemone::Diagnostic
{
    static UninitializedObject<StandardOutputTraceListener> GStandardOutputTraceListener{};

    void InitializePlatformStatic();
    void ShutdownPlatformStatic();

    void InitializeStatic()
    {
        GDiagnosticStatic.Create();
        GTrace.Create();

        GStandardOutputTraceListener.Create();

        InitializePlatformStatic();
    }

    void ShutdownStatic()
    {
        ShutdownPlatformStatic();

        GStandardOutputTraceListener.Destroy();

        GTrace.Destroy();
        GDiagnosticStatic.Destroy();
    }
}
