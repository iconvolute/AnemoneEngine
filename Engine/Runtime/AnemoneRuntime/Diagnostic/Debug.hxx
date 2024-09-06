#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#include <source_location>
#include <string_view>
#include <fmt/format.h>

namespace Anemone::Diagnostic
{
    // Rationale:
    // - UE5 uses `FDebug` class
    // - O3DE
    // - POCO uses `Bugcheck` and `Debugger` classes with static methods
    // - Diligent Core - static class
    // - Flax Engine - static classes
    // - wicked engine - namespaces + free functions
    // - Stroika - namespaces
    enum class AssertAction : uint8_t
    {
        Abort,
        Debug,
        Continue,
    };
}


namespace Anemone::Diagnostic::Debug
{
    RUNTIME_API void Break();

    [[noreturn]] RUNTIME_API void Crash();

    RUNTIME_API bool IsDebuggerAttached();

    RUNTIME_API void WaitForDebugger();

    RUNTIME_API bool AttachDebugger();

    RUNTIME_API AssertAction HandleAssertionFormatted(
        std::source_location const& location,
        std::string_view expression,
        std::string_view format,
        fmt::format_args args);

    inline AssertAction HandleAssertion(
        std::source_location const& location,
        std::string_view expression)
    {
        return HandleAssertionFormatted(
            location,
            expression,
            {},
            {});
    }

    inline AssertAction HandleAssertion(
        std::source_location const& location,
        std::string_view expression,
        std::string_view format)
    {
        return HandleAssertionFormatted(
            location,
            expression,
            format,
            {});
    }

    template <typename... Args>
    inline AssertAction HandleAssertion(
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

    [[noreturn]] RUNTIME_API void HandlePanicFormatted(
        std::source_location const& location,
        std::string_view format,
        fmt::format_args args);

    [[noreturn]] RUNTIME_API void HandlePanic();

    [[noreturn]] inline void HandlePanic(
        std::source_location const& location)
    {
        Debug::HandlePanicFormatted(location, {}, {});
    }

    [[noreturn]] inline void HandlePanic(
        std::source_location const& location,
        std::string_view format)
    {
        Debug::HandlePanicFormatted(location, format, {});
    }

    template <typename... Args>
    [[noreturn]] inline void HandlePanic(
        std::source_location const& location,
        std::string_view format,
        Args const&... args)
    {
        Debug::HandlePanicFormatted(location, format, fmt::make_format_args(args...));
    }
}


// AE_VERIFY

#if ANEMONE_FEATURE_ASSERTIONS

#define AE_VERIFY(expression, ...) \
    do \
    { \
        if (!(expression)) \
            [[unlikely]] \
        { \
            if (::Anemone::Diagnostic::Debug::HandleAssertion(std::source_location::current(), #expression __VA_OPT__(, ) __VA_ARGS__) == ::Anemone::Diagnostic::AssertAction::Debug) \
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
            if (::Anemone::Diagnostic::Debug::HandleAssertion(std::source_location::current(), #expression __VA_OPT__(, ) __VA_ARGS__) == ::Anemone::Diagnostic::AssertAction::Debug) \
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
            ::Anemone::Diagnostic::Debug::HandlePanic(std::source_location::current(), #expression __VA_OPT__(, ) __VA_ARGS__); \
        } \
    } while (false)

#else

#define AE_ENSURE(expression, ...) \
    do \
    { \
        if (!(expression)) \
            [[unlikely]] \
        { \
            ::Anemone::Diagnostic::Debug::HandlePanic(); \
        } \
    } while (false)
#endif

// AE_PANIC

#if ANEMONE_FEATURE_ASSERTIONS

#define AE_PANIC(...) \
    do \
    { \
        ::Anemone::Diagnostic::Debug::HandlePanic(std::source_location::current() __VA_OPT__(, ) __VA_ARGS__); \
    } while (false)

#else

#define AE_PANIC(...) \
    do \
    { \
        ::Anemone::Diagnostic::Debug::HandlePanic(); \
    } while (false)

#endif
