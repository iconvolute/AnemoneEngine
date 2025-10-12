#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"

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
