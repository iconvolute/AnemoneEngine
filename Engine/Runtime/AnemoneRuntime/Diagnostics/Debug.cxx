#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

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

    extern AssertAction PlatformHandleAssertion(
        std::source_location const& location,
        std::string_view expression,
        std::string_view message);

    extern void PlatformHandlePanic();

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

        Trace::WriteLine(TraceLevel::Error, "=== assertion ===");
        Trace::WriteLine(TraceLevel::Error, "location: {}:{}", location.file_name(), location.line());
        Trace::WriteLine(TraceLevel::Error, "expression: {}", expression);
        Trace::WriteLine(TraceLevel::Error, "message: {}", view);
        Trace::Flush();

        AssertAction const action = PlatformHandleAssertion(location, expression, view);

        if (action == AssertAction::Abort)
        {
            Debug::Crash();
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

        Trace::WriteLine(TraceLevel::Critical, "=== panic ===");
        Trace::WriteLine(TraceLevel::Critical, "location: {}:{}", location.file_name(), location.line());
        Trace::WriteLine(TraceLevel::Critical, "message: {}", view);
        Trace::Flush();

#if !ANEMONE_BUILD_SHIPPING
        PlatformHandlePanic();
#endif

        Debug::Crash();
    }

    void Debug::HandlePanic()
    {
        DebugImpl& self = DebugImpl::Get();
        UniqueLock _{self.Lock};

        Trace::WriteLine(TraceLevel::Critical, "=== panic ===");
        Trace::Flush();

#if !ANEMONE_BUILD_SHIPPING
        PlatformHandlePanic();
#endif

        Debug::Crash();
    }
}
