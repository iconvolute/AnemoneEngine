#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#include <utility>

namespace Anemone::Diagnostic
{
    enum class LogLevel
    {
        None = 0,
        Fatal = 1,
        Error = 2,
        Warning = 3,
        Info = 4,
        Debug = 5,
    };

    inline constexpr LogLevel GDefaultLogLevel = []() -> LogLevel
    {
#if ANEMONE_BUILD_SHIPPING
        return LogLevel::Fatal;
#elif ANEMONE_CONFIG_DEBUG
        return LogLevel::Debug;
#else
        return LogLevel::Warning;
#endif
    }();

    constexpr bool CanDispatch(LogLevel level)
    {
        return std::to_underlying(level) <= std::to_underlying(GDefaultLogLevel);
    }
}
