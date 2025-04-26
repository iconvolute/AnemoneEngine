#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#include <string_view>
#include <source_location>
#include <fmt/format.h>

namespace Anemone::Diagnostics
{
    RUNTIME_API bool ReportAssertionFormatted(
        std::source_location const& location,
        std::string_view expression,
        std::string_view format,
        fmt::format_args args);

    template <typename... Args>
    bool ReportAssertion(
        std::source_location const& location,
        std::string_view expression,
        std::string_view format,
        Args const&... args)
    {
        return ReportAssertionFormatted(
            location,
            expression,
            format,
            fmt::make_format_args(args...));
    }

    inline bool ReportAssertion(
        std::source_location const& location,
        std::string_view expression)
    {
        return ReportAssertionFormatted(
            location,
            expression,
            {},
            {});
    }

    RUNTIME_API void ReportPanicFormatted(
        std::source_location const& location,
        std::string_view format,
        fmt::format_args args);

    template <typename... Args>
    void ReportPanic(
        std::source_location const& location,
        std::string_view format,
        Args const&... args)
    {
        ReportPanicFormatted(
            location,
            format,
            fmt::make_format_args(args...));
    }

    inline void ReportPanic(
        std::source_location const& location)
    {
        ReportPanicFormatted(
            location,
            {},
            {});
    }
}

#if ANEMONE_FEATURE_ASSERTIONS

#define AE_ASSERT(expression, ...) \
    do \
    { \
        if (!(expression)) \
            [[unlikely]] \
        { \
            if (::Anemone::Diagnostics::ReportAssertion(::std::source_location::current(), #expression __VA_OPT__(, ) __VA_ARGS__)) \
            { \
                anemone_debugbreak(); \
            } \
        } \
    } while (false)

#define AE_VERIFY AE_ASSERT
#define AE_ENSURE AE_ASSERT
#define AE_PANIC(...) \
    do \
    { \
        ::Anemone::Diagnostics::ReportPanic(::std::source_location::current() __VA_OPT__(, ) __VA_ARGS__); \
        anemone_debugbreak(); \
    } while (false)

#else

// Assertions are compiled out.
#define AE_ASSERT(expression, ...)

// Verification is executed without checking.
#define AE_VERIFY(expression, ...) \
    do \
    { \
        if (!(expression)) \
            [[unlikely]] \
        { \
        } \
    } while (false)

// Ensure breaks even in shipping builds.
#define AE_ENSURE(expression, ...) \
    do \
    { \
        if (!(expression)) \
            [[unlikely]] \
        { \
            anemone_debugbreak(); \
        } \
    } while (false)

// Panic breaks unconditionally.
#define AE_PANIC(...) \
    anemone_debugbreak()

#endif
