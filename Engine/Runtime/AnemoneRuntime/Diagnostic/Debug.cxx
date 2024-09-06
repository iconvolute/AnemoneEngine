#include "AnemoneRuntime/Diagnostic/Debug.hxx"
#include "AnemoneRuntime/Diagnostic/Trace.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"

#include <array>
#include <iterator>

namespace Anemone::Diagnostic::Debug
{
    AssertAction PlatformSpecificAssertionHandler(
        std::source_location const& location,
        std::string_view expression,
        std::string_view message);

    void PlatformSpecificPanicHandler();
}

namespace Anemone::Diagnostic::Debug
{
    struct DebugState
    {
        bool m_handling{};
        Threading::CriticalSection m_lock{};

        static DebugState& Get()
        {
            static DebugState instance{};
            return instance;
        }
    };

    AssertAction HandleAssertionFormatted(
        std::source_location const& location,
        std::string_view expression,
        std::string_view format,
        fmt::format_args args)
    {
        DebugState& debug = DebugState::Get();
        Threading::UniqueLock lock{debug.m_lock};

        fmt::memory_buffer message;
        fmt::vformat_to(std::back_inserter(message), format, args);
        std::string_view messageView{message.data(), message.size()};

        if (GTrace)
            [[likely]]
        {
            GTrace->WriteLine(TraceLevel::Error, "=== assertion ===");
            GTrace->WriteLine(TraceLevel::Error, "location: {}:{}", location.file_name(), location.line());
            GTrace->WriteLine(TraceLevel::Error, "expression: {}", expression);
            GTrace->WriteLine(TraceLevel::Error, "message: {}", messageView);
            GTrace->Flush();
        }

        AssertAction action = PlatformSpecificAssertionHandler(location, expression, messageView);

        if (action == AssertAction::Abort)
        {
            // Terminate process and create dump.
            Debug::Crash();
        }

        return action;
    }

    void HandlePanicFormatted(
        std::source_location const& location,
        std::string_view format,
        fmt::format_args args)
    {
        DebugState& debug = DebugState::Get();
        Threading::UniqueLock lock{debug.m_lock};

        fmt::memory_buffer message;
        fmt::vformat_to(std::back_inserter(message), format, args);
        std::string_view messageView{message.data(), message.size()};

        if (GTrace)
            [[likely]]
        {
            GTrace->WriteLine(TraceLevel::Error, "=== bugcheck ===");
            GTrace->WriteLine(TraceLevel::Error, "location: {}:{}", location.file_name(), location.line());
            GTrace->WriteLine(TraceLevel::Error, "message: {}", messageView);
            GTrace->Flush();
        }

#if !ANEMONE_BUILD_SHIPPING
        // Tell user about crash. We rely on external tool to actually report
        PlatformSpecificPanicHandler();
#endif

        // Terminate process and create dump.
        Debug::Crash();
    }

    void HandlePanic()
    {
        DebugState& debug = DebugState::Get();
        Threading::UniqueLock lock{debug.m_lock};

        if (GTrace)
            [[likely]]
        {
            GTrace->WriteLine(TraceLevel::Critical, "=== panic ===");
            GTrace->Flush();
        }

#if !ANEMONE_BUILD_SHIPPING
        // Tell user about crash. We rely on external tool to actually report
        PlatformSpecificPanicHandler();
#endif

        // Terminate process and create dump.
        Debug::Crash();
    }
}
