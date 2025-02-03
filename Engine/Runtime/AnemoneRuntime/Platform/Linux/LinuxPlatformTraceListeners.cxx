#include "AnemoneRuntime/Platform/PlatformTraceListeners.hxx"
#include "AnemoneRuntime/Diagnostics/ConsoleTraceListener.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

namespace Anemone
{
    static UninitializedObject<ConsoleTraceListener> GConsoleTraceListener;

    void PlatformTraceListeners::Initialize()
    {
        GConsoleTraceListener.Create();
        Trace::AddListener(*GConsoleTraceListener);
    }

    void PlatformTraceListeners::Finalize()
    {
        Trace::RemoveListener(*GConsoleTraceListener);
        GConsoleTraceListener.Destroy();
    }
}
