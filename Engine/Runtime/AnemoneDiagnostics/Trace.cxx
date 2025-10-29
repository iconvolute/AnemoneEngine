#include "AnemoneDiagnostics/Trace.hxx"
#include "AnemoneBase/UninitializedObject.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneDiagnostics/Platform/Windows/WindowsTrace.hxx"
#elif ANEMONE_PLATFORM_LINUX
#include "AnemoneDiagnostics/Platform/Linux/LinuxTrace.hxx"
#else
#error Not implemented
#endif

namespace Anemone
{
    namespace
    {
        UninitializedObject<TraceDispatcher> gTraceDispatcher{};
    }

    void Trace::Initialize()
    {
        gTraceDispatcher.Create();

        PlatformTrace::Initialize();
    }

    void Trace::Finalize()
    {
        PlatformTrace::Finalize();

        gTraceDispatcher.Destroy();
    }

    TraceDispatcher& Trace::Get()
    {
        return *gTraceDispatcher;
    }
};
