#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Diagnostic/TraceListener.hxx"
#include "AnemoneRuntime/Threading/ReaderWriterLock.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

#include <fmt/format.h>

namespace Anemone::Diagnostic
{
    inline constexpr TraceLevel GDefaultLogLevel = []() -> TraceLevel
    {
#if ANEMONE_BUILD_SHIPPING
        return TraceLevel::Critical;
#elif ANEMONE_CONFIG_DEBUG
        return TraceLevel::Verbose;
#else
        return TraceLevel::Warning;
#endif
    }();

    constexpr bool CanDispatch(TraceLevel level)
    {
        return std::to_underlying(level) >= std::to_underlying(GDefaultLogLevel);
    }
}

namespace Anemone::Diagnostic
{
    class RUNTIME_API Trace final
    {
    private:
        IntrusiveList<TraceListener, Trace> m_listeners{};
        Threading::ReaderWriterLock m_lock{};

    private:
        void WriteLineCore(TraceLevel level, std::string_view format, fmt::format_args args);

    public:
        void WriteLine(TraceLevel level, std::string_view format)
        {
            this->WriteLineCore(level, format, {});
        }

        template <typename... Args>
        void WriteLine(TraceLevel level, std::string_view format, Args const&... args)
        {
            this->WriteLineCore(level, format, fmt::make_format_args(args...));
        }

        void Flush();

    public:
        void AddListener(TraceListener& listener);
        void RemoveListener(TraceListener& listener);
    };

    RUNTIME_API extern UninitializedObject<Trace> GTrace;
}

#define AE_TRACE(level, format, ...) \
    do \
    { \
        if constexpr (::Anemone::Diagnostic::CanDispatch(::Anemone::Diagnostic::TraceLevel::level)) \
        { \
            ::Anemone::Diagnostic::GTrace->WriteLine(::Anemone::Diagnostic::TraceLevel::level, format __VA_OPT__(, ) __VA_ARGS__); \
        } \
    } while (false)
