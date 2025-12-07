#pragma once
#include "AnemoneRuntime.Interop/Headers.hxx"
#include "AnemoneRuntime.Diagnostics/TraceDispatcher.hxx"

namespace Anemone
{
    class Trace final
    {
    public:
        ANEMONE_RUNTIME_BASE_API static void Initialize();

        ANEMONE_RUNTIME_BASE_API static void Finalize();

        ANEMONE_RUNTIME_BASE_API static TraceDispatcher& Get();
    };
}

#define AE_TRACE(level, format, ...) \
    do \
    { \
        if constexpr (::Anemone::TraceLevel::level >= ::Anemone::TraceLevel::Default) \
        { \
            using namespace std::string_view_literals; \
            ::Anemone::Trace::Get().Trace##level(format##sv __VA_OPT__(, ) __VA_ARGS__); \
        } \
    } while (false)
