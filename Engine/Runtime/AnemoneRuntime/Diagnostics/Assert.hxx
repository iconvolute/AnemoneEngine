#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#include <string_view>
#include <source_location>
#include <fmt/format.h>

namespace Anemone
{
    enum class AssertAction : uint8_t
    {
        Abort,
        Debug,
        Continue,
    };

    struct Debug
    {
        Debug() = delete;

        // Assertions
        RUNTIME_API static AssertAction HandleAssertionFormatted(
            std::source_location const& location,
            std::string_view expression,
            std::string_view format,
            fmt::format_args args);

        static AssertAction HandleAssertion(
            std::source_location const& location,
            std::string_view expression)
        {
            return HandleAssertionFormatted(
                location,
                expression,
                {},
                {});
        }

        template <typename... Args>
        static AssertAction HandleAssertion(
            std::source_location const& location,
            std::string_view expression,
            std::string_view format,
            Args const&... args)
        {
            return HandleAssertionFormatted(
                location,
                expression,
                fmt::format(format, std::forward<Args>(args)...));
        }

        // Panics
        [[noreturn]] RUNTIME_API static void HandlePanicFormatted(
            std::source_location const& location,
            std::string_view format,
            fmt::format_args args);

        [[noreturn]] RUNTIME_API static void HandlePanic();

        [[noreturn]] static void HandlePanic(
            std::source_location const& location)
        {
            HandlePanicFormatted(
                location,
                {},
                {});
        }

        template <typename... Args>
        static void HandlePanic(
            std::source_location const& location,
            std::string_view format,
            Args const&... args)
        {
            HandlePanicFormatted(
                location,
                fmt::format(format, std::forward<Args>(args)...));
        }
    };
}
