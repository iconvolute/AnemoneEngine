#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Diagnostic/Debugger.hxx"

#include <fmt/format.h>
#include <source_location>
#include <string_view>

namespace Anemone::Diagnostic
{
    enum class ErrorReportingMode
    {
        Interactive,
        Unattended,
    };

    RUNTIME_API void SetErrorReportingMode(ErrorReportingMode mode);
    RUNTIME_API ErrorReportingMode GetErrorReportingMode();
}

namespace Anemone::Diagnostic
{
    [[noreturn]] RUNTIME_API void BugCheckCore(std::source_location const& location, std::string_view format, fmt::format_args args);

    [[noreturn]] inline void BugCheck(std::source_location const& location)
    {
        BugCheckCore(location, "", fmt::make_format_args());
    }

    [[noreturn]] inline void BugCheck(std::source_location const& location, std::string_view format)
    {
        BugCheckCore(location, format, fmt::make_format_args());
    }

    template <typename... Args>
    [[noreturn]] void BugCheck(std::source_location const& location, std::string_view format, Args const&... args)
    {
        BugCheckCore(location, format, fmt::make_format_args(args...));
    }
}

namespace Anemone::Diagnostic
{
    [[nodiscard]] RUNTIME_API bool AssertCore(std::source_location const& location, std::string_view condition, std::string_view format, fmt::format_args args);

    [[nodiscard]] inline bool Assert(std::source_location const& location, std::string_view condition, std::string_view message)
    {
        return AssertCore(location, condition, message, fmt::make_format_args());
    }

    [[nodiscard]] inline bool Assert(std::source_location const& location, std::string_view condition)
    {
        return AssertCore(location, condition, "", fmt::make_format_args());
    }

    template <typename... Args>
    [[nodiscard]] bool Assert(std::source_location const& location, std::string_view condition, std::string_view format, Args const&... args)
    {
        return AssertCore(location, condition, format, fmt::make_format_args(args...));
    }
}

#if ANEMONE_FEATURE_ASSERTIONS

#define AE_BUGCHECK(...) \
    do \
    { \
        ::Anemone::Diagnostic::BugCheck(std::source_location::current() __VA_OPT__(, ) __VA_ARGS__); \
    } while (false)

#else
#define AE_BUGCHECK(...) \
    abort()
#endif

#if ANEMONE_FEATURE_ASSERTIONS

#define AE_ASSERT(condition, ...) \
    do \
    { \
        if (!(condition)) \
        { \
            if (::Anemone::Diagnostic::Assert(std::source_location::current(), #condition __VA_OPT__(, ) __VA_ARGS__)) \
            { \
                anemone_debugbreak(); \
            } \
        } \
    } while (false)

#define AE_ENSURE(condition, ...) \
    do \
    { \
        if (!(condition)) \
        { \
            AE_BUGCHECK(__VA_ARGS__); \
        } \
    } while (false)

#else

#define AE_ASSERT(condition, ...) \
    do \
    { \
    } while (false)

#define AE_ENSURE(condition, ...) \
    (condition)

#endif

#define AE_FATAL(condition, ...) \
    do \
    { \
        if (!(condition)) \
        { \
            AE_BUGCHECK(__VA_ARGS__); \
        } \
    } while (false)
