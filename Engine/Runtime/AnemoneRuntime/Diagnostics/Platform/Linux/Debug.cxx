#include "AnemoneRuntime/Diagnostics/Debug.hxx"

#include <format>

namespace Anemone::Diagnostics
{
    void Debug::Break()
    {
        anemone_debugbreak();
    }

    void Debug::Crash()
    {
#if !ANEMONE_BUILD_SHIPPING
        anemone_debugbreak();
#endif

        abort();
    }

    bool Debug::IsDebuggerAttached()
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

    void Debug::WaitForDebugger()
    {
    }

    bool Debug::AttachDebugger()
    {
        return false;
    }

    void Debug::ReportApplicationStop(std::string_view reason)
    {
        (void)fwrite(reason.data(), 1, reason.size(), stderr);
        (void)fflush(stderr);
        abort();
    }
}

namespace Anemone::Internal
{
    extern void PlatformInitializeDebug()
    {
    }

    extern void PlatformFinalizeDebug()
    {
    }
}
