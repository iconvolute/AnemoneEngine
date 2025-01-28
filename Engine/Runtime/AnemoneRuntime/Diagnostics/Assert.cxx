#include "AnemoneRuntime/Diagnostics/Assert.hxx"
#include "AnemoneRuntime/Platform/Platform.hxx"
#include "AnemoneRuntime/Platform/StackTrace.hxx"
#include "AnemoneRuntime/Platform/Environment.hxx"
#include "AnemoneRuntime/Platform/Debugger.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

#include <iterator>

namespace Anemone::Diagnostics
{
    bool ReportAssertionFormatted(
        std::source_location const& location,
        std::string_view expression,
        std::string_view format,
        fmt::format_args args)
    //! Reports assertion message.
    {
        // TODO: It would be nice to have tracing handle lack of trace listeners instead???


        fmt::memory_buffer message{};
        fmt::vformat_to(std::back_inserter(message), format, args);

        Trace::TraceMessage(TraceLevel::Fatal, "=== assertion failed ===");
        Trace::TraceMessage(TraceLevel::Fatal, "location: {}:{}", location.file_name(), location.line());
        Trace::TraceMessage(TraceLevel::Fatal, "expression: {}", expression);
        Trace::TraceMessage(TraceLevel::Fatal, "message: {}", std::string_view{message.data(), message.size()});

        StackTrace::Walk([&](void* address, std::string_view name)
        {
            Trace::TraceMessage(TraceLevel::Fatal, "{} {}", address, name);
        });

        // TODO: Flush any log messages.
        if (Debugger::Attach())
        {
            // Debugger is attached. Continue execution.
            return true;
        }

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

        Trace::TraceMessage(TraceLevel::Fatal, "=== panic ===");
        Trace::TraceMessage(TraceLevel::Fatal, "location: {}:{}", location.file_name(), location.line());
        Trace::TraceMessage(TraceLevel::Fatal, "message: {}", std::string_view{message.data(), message.size()});

        StackTrace::Walk([&](void* address, std::string_view name)
        {
            Trace::TraceMessage(TraceLevel::Fatal, "{} {}", address, name);
        });

        Debugger::Attach();
    }
}
