#include "AnemoneDiagnostics/Platform/Windows/WindowsTrace.hxx"
#include "AnemoneDiagnostics/Platform/Windows/WindowsDebugTraceListener.hxx"
#include "AnemoneDiagnostics/Platform/Windows/WindowsEtwTraceListener.hxx"
#include "AnemoneDiagnostics/Debug.hxx"
#include "AnemoneDiagnostics/Trace.hxx"
#include "AnemoneThreading/CriticalSection.hxx"
#include "AnemoneInterop/Windows/Debugger.hxx"
#include "AnemoneBase/UninitializedObject.hxx"


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
