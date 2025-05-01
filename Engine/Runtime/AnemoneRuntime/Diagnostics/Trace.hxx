#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Diagnostics/TraceLevel.hxx"

#include <string_view>
#include <fmt/format.h>

#if ANEMONE_BUILD_SHIPPING
#define ANEMONE_DEFAULT_TRACE_LEVEL ::Anemone::Diagnostics::TraceLevel::Error
#elif ANEMONE_CONFIG_DEBUG
#define ANEMONE_DEFAULT_TRACE_LEVEL ::Anemone::Diagnostics::TraceLevel::Debug
#else
#define ANEMONE_DEFAULT_TRACE_LEVEL ::Anemone::Diagnostics::TraceLevel::Warning
#endif

namespace Anemone::Diagnostics
{
    class TraceListener;

    inline constexpr TraceLevel DefaultTraceLevel = ANEMONE_DEFAULT_TRACE_LEVEL;

    RUNTIME_API void RegisterGlobalTraceListener(TraceListener& listener);

    RUNTIME_API void UnregisterGlobalTraceListener(TraceListener& listener);

    RUNTIME_API TraceListener& GetGlobalTraceListener();

    RUNTIME_API void FlushTraceListeners();

    RUNTIME_API void TraceMessageFormatted(TraceLevel level, std::string_view format, fmt::format_args args);

    RUNTIME_API void TraceMessageFormatted(TraceListener& listener, TraceLevel level, std::string_view format, fmt::format_args args);

    template <typename... Args>
    void TraceMessage(TraceLevel level, std::string_view format, Args const&... args)
    {
        TraceMessageFormatted(level, format, fmt::make_format_args(args...));
    }
}

#define AE_TRACE(level, format, ...) \
    do \
    { \
        if constexpr (::Anemone::Diagnostics::TraceLevel::level >= ANEMONE_DEFAULT_TRACE_LEVEL) \
        { \
            ::Anemone::Diagnostics::TraceMessage(::Anemone::Diagnostics::TraceLevel::level, format __VA_OPT__(, ) __VA_ARGS__); \
        } \
    } while (false)
