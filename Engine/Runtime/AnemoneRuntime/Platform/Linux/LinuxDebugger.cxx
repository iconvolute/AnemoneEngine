#include "AnemoneRuntime/Platform/Linux/LinuxDebugger.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"

#include <iterator>

namespace Anemone
{
    void LinuxDebugger::Break()
    {
        anemone_debugbreak();
    }

    void LinuxDebugger::Crash()
    {
#if !ANEMONE_BUILD_SHIPPING
        anemone_debugbreak();
#endif

        abort();
    }

    bool LinuxDebugger::IsAttached()
    {
        return false;
    }

    void LinuxDebugger::Wait()
    {
    }

    bool LinuxDebugger::Attach()
    {
        return false;
    }

    void LinuxDebugger::ReportApplicationStop(std::string_view reason)
    {
        fwrite(reason.data(), 1, reason.size(), stderr);
        fflush(stderr);
        abort();
    }
}
