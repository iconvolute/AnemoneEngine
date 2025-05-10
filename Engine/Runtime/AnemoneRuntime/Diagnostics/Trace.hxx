#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Diagnostics/TraceDispatcher.hxx"

namespace Anemone::Diagnostics
{
    RUNTIME_API TraceDispatcher& GetTraceDispatcher();
}

#define AE_TRACE(level, format, ...) \
    do \
    { \
        if constexpr (::Anemone::Diagnostics::TraceLevel::level >= ::Anemone::Diagnostics::TraceLevel::Default) \
        { \
            ::Anemone::Diagnostics::GetTraceDispatcher().Trace##level(format __VA_OPT__(, ) __VA_ARGS__); \
        } \
    } while (false)
