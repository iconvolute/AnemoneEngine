#include "AnemoneRuntime/Diagnostic/Runtime.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"
#include "AnemoneRuntime/Diagnostic/StandardOutputTraceListener.hxx"

namespace Anemone::Diagnostic
{
    static UninitializedObject<Diagnostics::DebugOutputTraceListener> GDebugOutputTraceListener{};
    static UninitializedObject<Diagnostics::StandardOutputTraceListener> GStandardOutputTraceListener{};
    // static UninitializedObject<EtwLogListener> GEtwLogListener{};

    void InitializeRuntime()
    {
        // if (context.UseDebugOutput)
        {
            GDebugOutputTraceListener.Create();
            Diagnostics::GTrace->AddListener(GDebugOutputTraceListener.Get());
        }

        // if (context.UseStandardOutput)
        {
            GStandardOutputTraceListener.Create();
            Diagnostics::GTrace->AddListener(GStandardOutputTraceListener.Get());
        }

        // GEtwLogListener.Create();
    }

    void FinalizeRuntime()
    {
        // GEtwLogListener.Destroy();

        // if (context.UseStandardOutput)
        {
            Diagnostics::GTrace->RemoveListener(GStandardOutputTraceListener.Get());
            GStandardOutputTraceListener.Destroy();
        }

        // if (context.UseDebugOutput)
        {
            Diagnostics::GTrace->RemoveListener(GDebugOutputTraceListener.Get());
            GDebugOutputTraceListener.Destroy();
        }
    }
}
