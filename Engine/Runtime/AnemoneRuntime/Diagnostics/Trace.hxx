#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Diagnostics/TraceLevel.hxx"

#include <string_view>
#include <fmt/format.h>

#if ANEMONE_BUILD_SHIPPING
#define ANEMONE_DEFAULT_TRACE_LEVEL ::Anemone::TraceLevel::Error
#elif ANEMONE_CONFIG_DEBUG
#define ANEMONE_DEFAULT_TRACE_LEVEL ::Anemone::TraceLevel::Debug
#else
#define ANEMONE_DEFAULT_TRACE_LEVEL ::Anemone::TraceLevel::Warning
#endif


namespace Anemone
{
    class TraceListener;

    class Trace final
    {
    public:
        RUNTIME_API static void AddListener(TraceListener& listener);
        RUNTIME_API static void RemoveListener(TraceListener& listener);

        RUNTIME_API static void TraceMessageFormatted(TraceLevel level, std::string_view format, fmt::format_args args);

        template <typename... Args>
        static void TraceMessage(TraceLevel level, std::string_view format, Args const&... args)
        {
            TraceMessageFormatted(level, format, fmt::make_format_args(args...));
        }

        RUNTIME_API static void Flush();

    public:
        static constexpr TraceLevel DefaultLevel = ANEMONE_DEFAULT_TRACE_LEVEL;
    };
}

#define AE_TRACE(level, format, ...) \
    do \
    { \
        if constexpr (::Anemone::TraceLevel::level >= ANEMONE_DEFAULT_TRACE_LEVEL) \
        { \
            ::Anemone::Trace::TraceMessage(::Anemone::TraceLevel::level, format __VA_OPT__(, ) __VA_ARGS__); \
        } \
    } while (false)
