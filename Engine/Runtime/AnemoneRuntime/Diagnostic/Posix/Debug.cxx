#include "AnemoneRuntime/Diagnostic/Debug.hxx"

#include <iterator>
#include <array>
#include <exception>

namespace Anemone::Diagnostic::Debug
{
    AssertAction PlatformSpecificAssertionHandler(
        std::source_location const& location,
        std::string_view expression,
        std::string_view message)
    {
        (void)location;
        (void)expression;
        (void)message;
        return AssertAction::Abort;
    }

    void PlatformSpecificPanicHandler()
    {
    }

    void Break()
    {
        anemone_debugbreak();
    }

    void Crash()
    {
        // Break into debugger. This will also generate a crash if no debugger is attached.
        anemone_debugbreak();

        // But even so, if we're still here, terminate the process.
        std::terminate();
    }

    bool IsDebuggerAttached()
    {
        return false;
    }

    void WaitForDebugger()
    {
        anemone_debugbreak();
    }

    bool AttachDebugger()
    {
        return false;
    }
}
