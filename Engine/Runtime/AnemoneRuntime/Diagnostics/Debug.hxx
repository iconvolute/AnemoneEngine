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

    class Debug
    {
    public:
        Debug() = delete;

    public:
        RUNTIME_API static void Breakpoint();

        [[noreturn]] RUNTIME_API static void Crash();

        RUNTIME_API static bool IsDebuggerAttached();

        RUNTIME_API static void WaitForDebugger();

        RUNTIME_API static bool AttachDebugger();

    public: // Assertions
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
                format,
                fmt::make_format_args(args...));
        }

    public: // Panics
        [[noreturn]] RUNTIME_API static void HandlePanicFormatted(
            std::source_location const& location,
            std::string_view format,
            fmt::format_args args);

        [[noreturn]] RUNTIME_API static void HandlePanic();

        [[noreturn]] static void HandlePanic(
            std::source_location const& location)
        {
            HandlePanicFormatted(location, {}, {});
        }

        template <typename... Args>
        [[noreturn]] static void HandlePanic(
            std::source_location const& location,
            std::string_view format,
            Args const&... args)
        {
            HandlePanicFormatted(
                location,
                format,
                fmt::make_format_args(args...));
        }
    };
}

// AE_VERIFY

#if ANEMONE_FEATURE_ASSERTIONS

#define AE_VERIFY(expression, ...) \
    do \
    { \
        if (!(expression)) \
            [[unlikely]] \
        { \
            if (::Anemone::Debug::HandleAssertion(std::source_location::current(), #expression __VA_OPT__(, ) __VA_ARGS__) == ::Anemone::AssertAction::Debug) \
            { \
                anemone_debugbreak(); \
            } \
        } \
    } while (false)

#else

#define AE_VERIFY(expression, ...) \
    do \
    { \
        (void)(expression); \
    } while (false)

#endif


// AE_ASSERT

#if ANEMONE_FEATURE_ASSERTIONS

#define AE_ASSERT(expression, ...) \
    do \
    { \
        if (!(expression)) \
            [[unlikely]] \
        { \
            if (::Anemone::Debug::HandleAssertion(std::source_location::current(), #expression __VA_OPT__(, ) __VA_ARGS__) == ::Anemone::AssertAction::Debug) \
            { \
                anemone_debugbreak(); \
            } \
        } \
    } while (false)

#else

#define AE_ASSERT(expression, ...) \
    do \
    { \
    } while (false)

#endif

// AE_ENSURE

#if ANEMONE_FEATURE_ASSERTIONS

#define AE_ENSURE(expression, ...) \
    do \
    { \
        if (!(expression)) \
            [[unlikely]] \
        { \
            ::Anemone::Debug::HandlePanic(std::source_location::current(), #expression __VA_OPT__(, ) __VA_ARGS__); \
        } \
    } while (false)

#else

#define AE_ENSURE(expression, ...) \
    do \
    { \
        if (!(expression)) \
            [[unlikely]] \
        { \
            ::Anemone::Debug::HandlePanic(); \
        } \
    } while (false)
#endif

// AE_PANIC

#if ANEMONE_FEATURE_ASSERTIONS

#define AE_PANIC(...) \
    do \
    { \
        ::Anemone::Debug::HandlePanic(std::source_location::current() __VA_OPT__(, ) __VA_ARGS__); \
    } while (false)

#else

#define AE_PANIC(...) \
    do \
    { \
        ::Anemone::Debug::HandlePanic(); \
    } while (false)

#endif
