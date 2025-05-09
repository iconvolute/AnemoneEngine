#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Diagnostics/TraceLevel.hxx"
#include "AnemoneRuntime/Diagnostics/TraceListener.hxx"

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
    inline constexpr TraceLevel DefaultTraceLevel = ANEMONE_DEFAULT_TRACE_LEVEL;

    class RUNTIME_API TraceDispatcher final : public TraceListener
    {
    private:
        ReaderWriterLock _lock{};
        IntrusiveList<TraceListener> _listeners{};

    public:
        void TraceEvent(TraceLevel level, const char* message, size_t size) override;

        void Flush() override;

        void Register(TraceListener& listener);

        void Unregister(TraceListener& listener);
    };

    RUNTIME_API TraceDispatcher& GetTraceDispatcher();
}

#define AE_TRACE(level, format, ...) \
    do \
    { \
        if constexpr (::Anemone::Diagnostics::TraceLevel::level >= ANEMONE_DEFAULT_TRACE_LEVEL) \
        { \
            ::Anemone::Diagnostics::GetTraceDispatcher().Trace##level(format __VA_OPT__(, ) __VA_ARGS__); \
        } \
    } while (false)
