#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Intrusive.hxx"
#include "AnemoneRuntime/Threading/ReaderWriterLock.hxx"
#include "AnemoneRuntime/Diagnostics/TraceLevel.hxx"

namespace Anemone::Diagnostics
{
    class RUNTIME_API TraceListener
        : private IntrusiveListNode<TraceListener>
    {
        friend struct IntrusiveList<TraceListener>;

    public:
        TraceListener() = default;
        TraceListener(TraceListener const&) = delete;
        TraceListener(TraceListener&&) = delete;
        TraceListener& operator=(TraceListener const&) = delete;
        TraceListener& operator=(TraceListener&&) = delete;
        virtual ~TraceListener() = default;

    public:
        virtual void Event(TraceLevel level, const char* message, size_t size) = 0;
        virtual void Flush() { }

        void LogEventFormatted(TraceLevel level, std::string_view format, fmt::format_args args);

        template <typename... Args>
        void LogEvent(TraceLevel level, const char* format, Args const&... args)
        {
            this->LogEventFormatted(level, format, fmt::make_format_args(args...));
        }


        template <typename... Args>
        void LogVerbose(const char* format, Args const&... args)
        {
            this->LogEventFormatted(TraceLevel::Verbose, format, fmt::make_format_args(args...));
        }

        template <typename... Args>
        void LogDebug(const char* format, Args const&... args)
        {
            this->LogEventFormatted(TraceLevel::Debug, format, fmt::make_format_args(args...));
        }

        template <typename... Args>
        void LogInformation(const char* format, Args const&... args)
        {
            this->LogEventFormatted(TraceLevel::Information, format, fmt::make_format_args(args...));
        }

        template <typename... Args>
        void LogWarning(const char* format, Args const&... args)
        {
            this->LogEventFormatted(TraceLevel::Warning, format, fmt::make_format_args(args...));
        }

        template <typename... Args>
        void LogError(const char* format, Args const&... args)
        {
            this->LogEventFormatted(TraceLevel::Error, format, fmt::make_format_args(args...));
        }

        template <typename... Args>
        void LogFatal(const char* format, Args const&... args)
        {
            this->LogEventFormatted(TraceLevel::Fatal, format, fmt::make_format_args(args...));
        }
    };
}
