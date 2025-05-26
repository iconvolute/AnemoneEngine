#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Diagnostics/TraceDispatcher.hxx"

namespace Anemone::Diagnostics
{
    class Trace : public TraceDispatcher
    {
    public:
        RUNTIME_API static Trace& Get();
    };
}

#define AE_TRACE(level, format, ...) \
    do \
    { \
        if constexpr (::Anemone::Diagnostics::TraceLevel::level >= ::Anemone::Diagnostics::TraceLevel::Default) \
        { \
            ::Anemone::Diagnostics::Trace::Get().Trace##level(format __VA_OPT__(, ) __VA_ARGS__); \
        } \
    } while (false)
