#include "AnemoneRuntime.Diagnostics/Platform/Windows/WindowsTrace.hxx"
#include "AnemoneRuntime.Diagnostics/Platform/Windows/WindowsDebugTraceListener.hxx"
#include "AnemoneRuntime.Diagnostics/Platform/Windows/WindowsEtwTraceListener.hxx"
#include "AnemoneRuntime.Diagnostics/Debug.hxx"
#include "AnemoneRuntime.Diagnostics/Trace.hxx"
#include "AnemoneRuntime.Threading/CriticalSection.hxx"
#include "AnemoneRuntime.Interop/Windows/Debugger.hxx"
#include "AnemoneRuntime.Base/UninitializedObject.hxx"


namespace Anemone
{
    namespace
    {
#if !ANEMONE_BUILD_SHIPPING
        UninitializedObject<WindowsDebugTraceListener> gWindowsDebugTraceListener{};
#endif

        UninitializedObject<WindowsEtwTraceListener> gWindowsEtwTraceListener{};
    }

    void WindowsTrace::Initialize()
    {
#if !ANEMONE_BUILD_SHIPPING
        gWindowsDebugTraceListener.Create();
        Trace::Get().Register(*gWindowsDebugTraceListener);
#endif

        gWindowsEtwTraceListener.Create();
        Trace::Get().Register(*gWindowsEtwTraceListener);
    }

    void WindowsTrace::Finalize()
    {
        if (gWindowsEtwTraceListener)
        {
            Trace::Get().Unregister(*gWindowsEtwTraceListener);
            gWindowsEtwTraceListener.Destroy();
        }

#if !ANEMONE_BUILD_SHIPPING
        if (gWindowsDebugTraceListener)
        {
            Trace::Get().Unregister(*gWindowsDebugTraceListener);
            gWindowsDebugTraceListener.Destroy();
        }
#endif
    }
}
