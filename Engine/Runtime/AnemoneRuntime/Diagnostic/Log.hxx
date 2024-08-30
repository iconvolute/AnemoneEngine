#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Diagnostic/LogLevel.hxx"
#include "AnemoneRuntime/Intrusive.hxx"

#include <string_view>
#include <fmt/format.h>

namespace Anemone::Diagnostic
{
    class LogListener
        : private IntrusiveListNode<LogListener>
    {
        friend struct IntrusiveList<LogListener>;

    public:
        virtual void Log(LogLevel level, std::string_view message) = 0;
        virtual void Flush() = 0;
        virtual ~LogListener() = default;
    };
}

namespace Anemone::Diagnostic
{
    RUNTIME_API void AddLogListener(LogListener* listener);
    RUNTIME_API void RemoveLogListener(LogListener* listener);
    RUNTIME_API void FlushLog();
    RUNTIME_API void LogMessageCore(LogLevel level, std::string_view format, fmt::format_args args);

    inline void LogMessage(LogLevel level, std::string_view message)
    {
        LogMessageCore(level, message, fmt::make_format_args());
    }

    template <typename... Args>
    inline void Log(LogLevel level, std::string_view format, Args const&... args)
    {
        LogMessageCore(level, format, fmt::make_format_args(args...));
    }

}

#define AE_LOG(level, format, ...) \
    if (false) \
    { \
    } \
    else \
    { \
        ::Anemone::Diagnostic::Log(::Anemone::Diagnostic::LogLevel::level,format __VA_OPT__(,) __VA_ARGS__); \
    }
