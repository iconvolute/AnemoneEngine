#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"
#include "AnemoneRuntime/Diagnostics/Internal/ConsoleTraceListener.hxx"
#include "AnemoneRuntime/System/Environment.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Diagnostics/Platform/Windows/WindowsTrace.hxx"
#elif ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Diagnostics/Platform/Linux/LinuxTrace.hxx"
#else
#error Not implemented
#endif

namespace Anemone
{
    namespace
    {
        UninitializedObject<TraceDispatcher> gTraceDispatcher{};
        UninitializedObject<ConsoleTraceListener> gConsoleTraceListener{};
    }

    void Trace::Initialize()
    {
        gTraceDispatcher.Create();

        if (Environment::IsConsoleApplication())
        {
            gConsoleTraceListener.Create();
            gTraceDispatcher->Register(*gConsoleTraceListener);
        }

        PlatformTrace::Initialize();
    }

    void Trace::Finalize()
    {
        PlatformTrace::Finalize();

        if (gConsoleTraceListener)
        {
            gTraceDispatcher->Unregister(*gConsoleTraceListener);
            gConsoleTraceListener.Destroy();
        }

        gTraceDispatcher.Destroy();
    }

    TraceDispatcher& Trace::Get()
    {
        return *gTraceDispatcher;
    }
};
