#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Intrusive.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"

namespace Anemone::Diagnostics
{
    enum class LogLevel
    {
        Trace = 0,
        Debug = 1,
        Information = 2,
        Warning = 3,
        Error = 4,
        Critical = 5,
        None = 6,
    };

    class LogListener : private IntrusiveListNode<LogListener>
    {
        friend struct IntrusiveList<LogListener>;

    public:
        virtual ~LogListener() = default;

        virtual void Flush()
        {
        }

        virtual void Log(LogLevel level, std::string_view message) = 0;
    };

    class Logger final
    {
    public:
        void LogFormatted(LogLevel level, std::string_view format, fmt::format_args args);

        template <typename... Args>
        void LogTrace(std::string_view format, Args const&... args)
        {
            this->LogFormatted(LogLevel::Trace, format, fmt::make_format_args(args...));
        }

        template <typename... Args>
        void LogDebug(std::string_view format, Args const&... args)
        {
            this->LogFormatted(LogLevel::Debug, format, fmt::make_format_args(args...));
        }

        template <typename... Args>
        void LogInformation(std::string_view format, Args const&... args)
        {
            this->LogFormatted(LogLevel::Information, format, fmt::make_format_args(args...));
        }

        template <typename... Args>
        void LogWarning(std::string_view format, Args const&... args)
        {
            this->LogFormatted(LogLevel::Warning, format, fmt::make_format_args(args...));
        }

        template <typename... Args>
        void LogError(std::string_view format, Args const&... args)
        {
            this->LogFormatted(LogLevel::Error, format, fmt::make_format_args(args...));
        }

        template <typename... Args>
        void LogCritical(std::string_view format, Args const&... args)
        {
            this->LogFormatted(LogLevel::Critical, format, fmt::make_format_args(args...));
        }

        void Flush();
    };

    extern Logger* GLogger;
}

// LogXxx statement -> LogFormatted()

#define AE_LOG_TRACE(format, ...) \
    do \
    { \
        Anemone::Diagnostics::GLogger->LogTrace(format, __VA_ARGS__); \
    } while (false)

#define AE_LOG_DEBUG
#define AE_LOG_INFORMATION
#define AE_LOG_WARNING
#define AE_LOG_ERROR
#define AE_LOG_CRITICAL
#define AE_LOG_NONE
