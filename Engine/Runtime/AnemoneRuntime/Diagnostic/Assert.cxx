#include "AnemoneRuntime/Diagnostic/Assert.hxx"
#include "AnemoneRuntime/Diagnostic/Static.hxx"
#include "AnemoneRuntime/Diagnostic/Trace.hxx"

#include <iterator>

namespace Anemone::Diagnostic
{
    void SetErrorReportingMode(ErrorReportingMode mode)
    {
        GDiagnosticStatic->ReportingMode = mode;
    }

    ErrorReportingMode GetErrorReportingMode()
    {
        return GDiagnosticStatic->ReportingMode;
    }
}

namespace Anemone::Diagnostic
{
    void BugCheckCore(std::source_location const& location, std::string_view format, fmt::format_args args)
    {
        Threading::UniqueLock lock{GDiagnosticStatic->ErrorLock};

        fmt::memory_buffer message;
        fmt::vformat_to(std::back_inserter(message), format, args);

        GTrace->WriteLine(TraceLevel::Critical, "=== bugcheck ===");
        GTrace->WriteLine(TraceLevel::Critical, "location: {}:{} at {}", location.file_name(), location.line(), location.function_name());
        GTrace->WriteLine(TraceLevel::Critical, "message: {}", std::string_view{message.data(), message.size()});
        Abort("BugCheck");
        abort();
    }
}

namespace Anemone::Diagnostic
{
    bool AssertCore(std::source_location const& location, std::string_view condition, std::string_view format, fmt::format_args args)
    {
        Threading::UniqueLock lock{GDiagnosticStatic->ErrorLock};

        fmt::memory_buffer message;
        fmt::vformat_to(std::back_inserter(message), format, args);

        GTrace->WriteLine(TraceLevel::Critical, "=== assert ===");
        GTrace->WriteLine(TraceLevel::Critical, "location: {}:{} at {}", location.file_name(), location.line(), location.function_name());
        GTrace->WriteLine(TraceLevel::Critical, "condition: {}", condition);
        GTrace->WriteLine(TraceLevel::Critical, "message: {}", std::string_view{message.data(), message.size()});

        return true;
    }
}
