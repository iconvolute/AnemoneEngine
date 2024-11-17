#include "AnemoneRuntime/Diagnostics/Debug.hxx"

#include <string_view>
#include <source_location>
#include <cstdlib>

namespace Anemone
{
    AssertAction PlatformHandleAssertion(
        std::source_location const& location,
        std::string_view expression,
        std::string_view message)
    {
        (void)location;
        (void)expression;
        (void)message;
        return AssertAction::Abort;
    }

    void PlatformHandlePanic()
    {
    }
}

namespace Anemone
{
    void Debug::Breakpoint()
    {
        anemone_debugbreak();
    }

    void Debug::Crash()
    {
        anemone_debugbreak();
        abort();
    }

    bool Debug::IsDebuggerAttached()
    {
        return false;
    }

    void Debug::WaitForDebugger()
    {
        anemone_debugbreak();
    }

    bool Debug::AttachDebugger()
    {
        return false;
    }
}
