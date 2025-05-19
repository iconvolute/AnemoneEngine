#include "AnemoneRuntime/Diagnostics/Debugger.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Diagnostics/Internal/ConsoleTraceListener.hxx"

namespace Anemone::Diagnostics
{
    extern void PlatformInitializeDebugger()
    {
    }

    extern void PlatformFinalizeDebugger()
    {
    }
}

namespace Anemone::Diagnostics
{
    void Break()
    {
        anemone_debugbreak();
    }

    void Crash()
    {
#if !ANEMONE_BUILD_SHIPPING
        anemone_debugbreak();
#endif

        abort();
    }

    bool IsDebuggerAttached()
    {
        if (FILE* f = fopen("/proc/self/status", "r"))
        {
            int tracePid = 0;

            std::array<char, 256> line{};

            while (fgets(line.data(), line.size(), f))
            {
                if (sscanf(line.data(), "TracerPid:\t%d") == 1)
                {
                    return tracePid != 0;
                }
            }
        }

        return false;
    }

    void WaitForDebugger()
    {
    }

    bool AttachDebugger()
    {
        return false;
    }

    void ReportApplicationStop(std::string_view reason)
    {
        (void)fwrite(reason.data(), 1, reason.size(), stderr);
        (void)fflush(stderr);
        abort();
    }
}
