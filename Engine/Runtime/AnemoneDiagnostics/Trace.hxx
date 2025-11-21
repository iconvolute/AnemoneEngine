#pragma once
#include "AnemoneInterop/Headers.hxx"
#include "AnemoneDiagnostics/TraceDispatcher.hxx"

namespace Anemone
{
    class Trace final
    {
    public:
        RUNTIME_API static void Initialize();

        RUNTIME_API static void Finalize();

        RUNTIME_API static TraceDispatcher& Get();
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
