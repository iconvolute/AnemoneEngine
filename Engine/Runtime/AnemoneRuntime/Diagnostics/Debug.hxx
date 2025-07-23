#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Base/FunctionRef.hxx"
#include "AnemoneRuntime/Diagnostics/Status.hxx"

#include <string_view>
#include <source_location>
#include <fmt/format.h>

namespace Anemone::Diagnostics
{
    class Debug
    {
    public:
        RUNTIME_API static bool ReportAssertionFormatted(
            std::source_location const& location,
            std::string_view expression,
            std::string_view format,
            fmt::format_args args);

        template <typename... Args>
        static bool ReportAssertion(
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

        static bool ReportAssertion(
            std::source_location const& location,
            std::string_view expression)
        {
            return ReportAssertionFormatted(
                location,
                expression,
                {},
                {});
        }

        RUNTIME_API static void ReportPanicFormatted(
            std::source_location const& location,
            std::string_view format,
            fmt::format_args args);

        template <typename... Args>
        static void ReportPanic(
            std::source_location const& location,
            std::string_view format,
            Args const&... args)
        {
            ReportPanicFormatted(
                location,
                format,
                fmt::make_format_args(args...));
        }

        static void ReportPanic(
            std::source_location const& location)
        {
            ReportPanicFormatted(
                location,
                {},
                {});
        }

        RUNTIME_API static void Break();

        RUNTIME_API static void Crash();

        RUNTIME_API static bool IsDebuggerAttached();

        RUNTIME_API static void WaitForDebugger();

        RUNTIME_API static bool AttachDebugger();

        RUNTIME_API static void ReportApplicationStop(std::string_view reason);

        RUNTIME_API static void GetCurrentStackTrace(FunctionRef<void(void* address)> callback);

        RUNTIME_API static void GetCurrentStackTrace(FunctionRef<void(void* address, std::string_view symbol)> callback);

        RUNTIME_API static void ReportErrorErrno(int error, std::source_location const& location);

        RUNTIME_API static Status TranslateErrorCodeErrno(int error);
    };
}


#if ANEMONE_FEATURE_ASSERTIONS

#define AE_ASSERT(expression, ...) \
    do \
    { \
        if (!(expression)) \
            [[unlikely]] \
        { \
            if (::Anemone::Diagnostics::Debug::ReportAssertion(::std::source_location::current(), #expression __VA_OPT__(, ) __VA_ARGS__)) \
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
        ::Anemone::Diagnostics::Debug::ReportPanic(::std::source_location::current() __VA_OPT__(, ) __VA_ARGS__); \
        anemone_debugbreak(); \
    } while (false)


#define AE_VERIFY_ERRNO(error) \
    do \
    { \
        ::Anemone::Diagnostics::Debug::ReportErrorErrno(error, std::source_location::current()); \
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

#define AE_VERIFY_ERRNO(error)

#endif
