#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Diagnostics/TraceListener.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"

#include <utility>

namespace Anemone::Diagnostics
{
    namespace
    {
        UninitializedObject<TraceDispatcher> GTraceDispatcher{};
    }

    extern void InitializeTrace()
    {
        GTraceDispatcher.Create();
    }

    extern void FinalizeTrace()
    {
        GTraceDispatcher.Destroy();
    }

    TraceDispatcher& GetTraceDispatcher()
    {
        return *GTraceDispatcher;
    }
}
