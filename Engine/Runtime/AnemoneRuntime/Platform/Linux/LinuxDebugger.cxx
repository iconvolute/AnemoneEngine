#include "AnemoneRuntime/Platform/Debugger.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"

#include <iterator>

namespace Anemone
{
    void Debugger::Break()
    {
        anemone_debugbreak();
    }

    void Debugger::Crash()
    {
#if !ANEMONE_BUILD_SHIPPING
        anemone_debugbreak();
#endif

        abort();
    }

    bool Debugger::IsAttached()
    {
        return false;
    }

    void Debugger::Wait()
    {
    }

    bool Debugger::Attach()
    {
        return false;
    }

    void Debugger::ReportApplicationStop(std::string_view reason)
    {
        fwrite(reason.data(), 1, reason.size(), stderr);
        fflush(stderr);
        abort();
    }
}
