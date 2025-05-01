#include "AnemoneRuntime/Diagnostics/Assert.hxx"
#include "AnemoneRuntime/Platform/StackTrace.hxx"
#include "AnemoneRuntime/System/Environment.hxx"
#include "AnemoneRuntime/Diagnostics/Debugger.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

#include <iterator>

namespace Anemone::Diagnostics
{
    bool ReportAssertionFormatted(
        std::source_location const& location,
        std::string_view expression,
        std::string_view format,
        fmt::format_args args)
    {
        // TODO: It would be nice to have tracing handle lack of trace listeners instead???


        fmt::memory_buffer message{};
        fmt::vformat_to(std::back_inserter(message), format, args);

        TraceMessage(TraceLevel::Fatal, "=== assertion failed ===");
        TraceMessage(TraceLevel::Fatal, "location: {}:{}", location.file_name(), location.line());
        TraceMessage(TraceLevel::Fatal, "expression: {}", expression);
        TraceMessage(TraceLevel::Fatal, "message: {}", std::string_view{message.data(), message.size()});

        StackTrace::Walk([&](void* address, std::string_view name)
        {
            TraceMessage(TraceLevel::Fatal, "{} {}", address, name);
        });

        FlushTraceListeners();

        // TODO: Re-enable once we will be sure that it also works on linux
#if false
        if (Debugger::Attach())
        {
            // Debugger is attached. Continue execution.
            return true;
        }
#endif

#if ANEMONE_BUILD_SHIPPING
        // Always execute breakpoint in shipping builds. It will propagate to the crash handler.
        return true;
#else
        // TODO: Add option to ignore assertions at runtime?
        return true;
#endif
    }

    void ReportPanicFormatted(
        std::source_location const& location,
        std::string_view format,
        fmt::format_args args)
    {
        fmt::memory_buffer message{};
        fmt::vformat_to(std::back_inserter(message), format, args);

        TraceMessage(TraceLevel::Fatal, "=== panic ===");
        TraceMessage(TraceLevel::Fatal, "location: {}:{}", location.file_name(), location.line());
        TraceMessage(TraceLevel::Fatal, "message: {}", std::string_view{message.data(), message.size()});

        StackTrace::Walk([&](void* address, std::string_view name)
        {
            TraceMessage(TraceLevel::Fatal, "{} {}", address, name);
        });

        FlushTraceListeners();

        // TODO: Re-enable once we will be sure that it also works on linux
#if false
        Debugger::Attach();
#endif
    }
}
