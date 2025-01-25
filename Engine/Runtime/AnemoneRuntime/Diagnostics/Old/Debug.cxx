#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Platform/Platform.hxx"

#include <iterator>

namespace Anemone
{
    struct DebugImpl
    {
        bool Handling{};
        CriticalSection Lock{};

        static DebugImpl& Get()
        {
            static DebugImpl instance{};
            return instance;
        }
    };

    AssertAction Debug::HandleAssertionFormatted(
        std::source_location const& location,
        std::string_view expression,
        std::string_view format,
        fmt::format_args args)
    {
        DebugImpl& self = DebugImpl::Get();
        UniqueLock _{self.Lock};

        fmt::memory_buffer buffer{};
        fmt::vformat_to(std::back_inserter(buffer), format, args);
        std::string_view view{buffer.data(), buffer.size()};

        Diagnostics::GTrace->WriteLine(Diagnostics::TraceLevel::Error, "=== assertion ===");
        Diagnostics::GTrace->WriteLine(Diagnostics::TraceLevel::Error, "location: {}:{}", location.file_name(), location.line());
        Diagnostics::GTrace->WriteLine(Diagnostics::TraceLevel::Error, "expression: {}", expression);
        Diagnostics::GTrace->WriteLine(Diagnostics::TraceLevel::Error, "message: {}", view);
        Diagnostics::GTrace->Flush();

        AssertAction const action = Platform::HandleAssertion(location, expression, view);

        if (action == AssertAction::Abort)
        {
            Platform::Crash();
        }

        return action;
    }

    void Debug::HandlePanicFormatted(
        std::source_location const& location,
        std::string_view format,
        fmt::format_args args)
    {
        DebugImpl& self = DebugImpl::Get();
        UniqueLock _{self.Lock};

        fmt::memory_buffer buffer{};
        fmt::vformat_to(std::back_inserter(buffer), format, args);
        std::string_view view{buffer.data(), buffer.size()};

        Diagnostics::GTrace->WriteLine(Diagnostics::TraceLevel::Critical, "=== panic ===");
        Diagnostics::GTrace->WriteLine(Diagnostics::TraceLevel::Critical, "location: {}:{}", location.file_name(), location.line());
        Diagnostics::GTrace->WriteLine(Diagnostics::TraceLevel::Critical, "message: {}", view);
        Diagnostics::GTrace->Flush();

#if !ANEMONE_BUILD_SHIPPING
        Platform::HandlePanic();
#endif

        Platform::Crash();
    }

    void Debug::HandlePanic()
    {
        DebugImpl& self = DebugImpl::Get();
        UniqueLock _{self.Lock};

        Diagnostics::GTrace->WriteLine(Diagnostics::TraceLevel::Critical, "=== panic ===");
        Diagnostics::GTrace->Flush();

#if !ANEMONE_BUILD_SHIPPING
        Platform::HandlePanic();
#endif

        Platform::Crash();
    }
}
