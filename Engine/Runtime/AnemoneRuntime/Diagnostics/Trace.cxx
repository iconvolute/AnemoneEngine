#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Diagnostics/TraceListener.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

#include <utility>

namespace Anemone::Diagnostics
{
    static UninitializedObject<TraceDispatcher> GTraceDispatcher{};

    TraceDispatcher& GetTraceDispatcher()
    {
        return *GTraceDispatcher;
    }
}

namespace Anemone::Internal
{
    extern void InitializeTrace()
    {
        ::Anemone::Diagnostics::GTraceDispatcher.Create();
    }

    extern void FinalizeTrace()
    {
        ::Anemone::Diagnostics::GTraceDispatcher.Destroy();
    }
}
