#pragma once
#include "AnemoneRuntime.Interop/Headers.hxx"
#include "AnemoneRuntime.Base/Intrusive.hxx"
#include "AnemoneRuntime.Threading/ReaderWriterLock.hxx"

#include <string_view>
#include <fmt/format.h>

namespace Anemone
{
    enum class TraceLevel : uint8_t
    {
        Verbose,
        Debug,
        Information,
        Warning,
        Error,
        Fatal,
        None,

#if ANEMONE_BUILD_SHIPPING || ANEMONE_BUILD_PROFILING
        Default = Error,
#elif ANEMONE_BUILD_DEVELOPER
        Default = Information,
#else
        Default = Verbose,
#endif
    };

    class ANEMONE_RUNTIME_BASE_API TraceListener
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
        virtual void TraceEvent(TraceLevel level, const char* message, size_t size) = 0;
        virtual void Flush() { }

        void TraceFormatted(TraceLevel level, std::string_view format, fmt::format_args args);

        template <typename... Args>
        void Trace(TraceLevel level, const char* format, Args const&... args)
        {
            this->TraceFormatted(level, format, fmt::make_format_args(args...));
        }


        template <typename... Args>
        void TraceVerbose(std::string_view format, Args const&... args)
        {
            this->TraceFormatted(TraceLevel::Verbose, format, fmt::make_format_args(args...));
        }

        template <typename... Args>
        void TraceDebug(std::string_view format, Args const&... args)
        {
            this->TraceFormatted(TraceLevel::Debug, format, fmt::make_format_args(args...));
        }

        template <typename... Args>
        void TraceInformation(std::string_view format, Args const&... args)
        {
            this->TraceFormatted(TraceLevel::Information, format, fmt::make_format_args(args...));
        }

        template <typename... Args>
        void TraceWarning(std::string_view format, Args const&... args)
        {
            this->TraceFormatted(TraceLevel::Warning, format, fmt::make_format_args(args...));
        }

        template <typename... Args>
        void TraceError(std::string_view format, Args const&... args)
        {
            this->TraceFormatted(TraceLevel::Error, format, fmt::make_format_args(args...));
        }

        template <typename... Args>
        void TraceFatal(std::string_view format, Args const&... args)
        {
            this->TraceFormatted(TraceLevel::Fatal, format, fmt::make_format_args(args...));
        }
    };
}
