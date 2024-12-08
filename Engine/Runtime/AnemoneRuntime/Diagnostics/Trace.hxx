#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Intrusive.hxx"
#include "AnemoneRuntime/Threading/ReaderWriterLock.hxx"

#include <source_location>
#include <string_view>
#include <fmt/format.h>


namespace Anemone
{
    class Trace;

    enum class TraceLevel
    {
        Verbose,
        Info,
        Warning,
        Error,
        Critical,
        None,
    };

    class TraceListener : private IntrusiveListNode<TraceListener, Trace>
    {
        friend struct IntrusiveList<TraceListener, Trace>;

    public:
        virtual ~TraceListener() = default;

        virtual void WriteLine(TraceLevel level, std::string_view message) = 0;

        virtual void Flush()
        {
        }
    };

    class Trace final
    {
    public:
        static constexpr TraceLevel DefaultTraceLevel = []() -> TraceLevel
        {
#if ANEMONE_BUILD_SHIPPING
            return TraceLevel::Critical;
#elif ANEMONE_CONFIG_DEBUG
            return TraceLevel::Verbose;
#else
            return TraceLevel::Warning;
#endif
        }();

        static constexpr bool CanDispatch(TraceLevel level)
        {
            return level >= DefaultTraceLevel;
        }

    public:
        Trace() = delete;

    private:
        RUNTIME_API static void WriteLineFormatted(TraceLevel level, std::string_view format, fmt::format_args args);

    public:
        template <typename... Args>
        static void WriteLine(TraceLevel level, std::string_view format, Args const&... args)
        {
            WriteLineFormatted(level, format, fmt::make_format_args(args...));
        }

        RUNTIME_API static void Flush();

    public:
        RUNTIME_API static void AddListener(TraceListener& listener);
        RUNTIME_API static void RemoveListener(TraceListener& listener);
    };
}

#define AE_TRACE(level, format, ...) \
    do \
    { \
        if constexpr (::Anemone::Trace::CanDispatch(::Anemone::TraceLevel::level)) \
        { \
            ::Anemone::Trace::WriteLine(::Anemone::TraceLevel::level, format __VA_OPT__(, ) __VA_ARGS__); \
        } \
    } while (false)
