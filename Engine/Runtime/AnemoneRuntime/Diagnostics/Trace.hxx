#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Intrusive.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

#include <source_location>
#include <string_view>
#include <fmt/format.h>

namespace Anemone::Diagnostics
{
    enum class TraceLevel : uint8_t
    {
        Verbose,
        Info,
        Warning,
        Error,
        Critical,
        None,
    };

    class TraceListener : private IntrusiveListNode<TraceListener>
    {
        friend struct IntrusiveList<TraceListener>;

    public:
        virtual ~TraceListener() = default;

        virtual void WriteLine(TraceLevel level, std::string_view message) = 0;

        virtual void Flush()
        {
        }
    };

    class RUNTIME_API Trace final
    {
    private:
        CriticalSection m_lock;
        IntrusiveList<TraceListener> m_listeners;

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
        Trace() = default;

        void WriteLineFormatted(TraceLevel level, std::string_view format, fmt::format_args args);

        template <typename... Args>
        void WriteLine(TraceLevel level, std::string_view format, Args const&... args)
        {
            this->WriteLineFormatted(level, format, fmt::make_format_args(args...));
        }

        void Flush();

        void AddListener(TraceListener& listener);

        void RemoveListener(TraceListener& listener);
    };

    RUNTIME_API extern UninitializedObject<Trace> GTrace;
}

#define AE_TRACE(level, format, ...) \
    do \
    { \
        if constexpr (::Anemone::Diagnostics::Trace::CanDispatch(::Anemone::Diagnostics::TraceLevel::level)) \
        { \
            ::Anemone::Diagnostics::GTrace->WriteLine(::Anemone::Diagnostics::TraceLevel::level, format __VA_OPT__(, ) __VA_ARGS__); \
        } \
    } while (false)


class RUNTIME_API XTrace
{
public:
    size_t m_count{};

public:
    anemone_noinline void Formatted(Anemone::Diagnostics::TraceLevel level, std::string_view format, fmt::format_args args);

    template <typename... Args>
    void Log(Anemone::Diagnostics::TraceLevel level, std::string_view format, Args const&... args)
    {
        this->Formatted(level, format, fmt::make_format_args(args...));
    }
};

#include "AnemoneRuntime/UninitializedObject.hxx"

RUNTIME_API extern Anemone::UninitializedObject<XTrace> GXtrace;
