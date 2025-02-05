#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include "AnemoneRuntime/Diagnostics/TraceLevel.hxx"

#include <string_view>
#include <fmt/format.h>

namespace Anemone
{
    class TraceListener;

    class Trace final
    {
    public:
        static constexpr bool CanDispatch(TraceLevel level)
        {
#if ANEMONE_BUILD_SHIPPING
            return level >= TraceLevel::Error;
#elif ANEMONE_CONFIG_DEBUG
            return level >= TraceLevel::Debug;
#else
            return level >= TraceLevel::Warning;
#endif
        }

        RUNTIME_API static void AddListener(TraceListener& listener);
        RUNTIME_API static void RemoveListener(TraceListener& listener);

        RUNTIME_API static void TraceMessageFormatted(TraceLevel level, std::string_view format, fmt::format_args args);

        template <typename... Args>
        static void TraceMessage(TraceLevel level, std::string_view format, Args const&... args)
        {
            TraceMessageFormatted(level, format, fmt::make_format_args(args...));
        }

        RUNTIME_API static void Flush();
    };
}

#define AE_TRACE(level, format, ...) \
    do \
    { \
        if constexpr (Anemone::Trace::CanDispatch(Anemone::TraceLevel::level)) \
        { \
            Anemone::Trace::TraceMessage(Anemone::TraceLevel::level, format __VA_OPT__(, ) __VA_ARGS__); \
        } \
    } while (false)
