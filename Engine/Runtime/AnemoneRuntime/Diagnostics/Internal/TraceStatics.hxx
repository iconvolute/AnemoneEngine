#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Threading/ReaderWriterLock.hxx"
#include "AnemoneRuntime/Intrusive.hxx"

#include <array>

namespace Anemone::Internal
{
    struct TraceStatics final
    {
        ReaderWriterLock Lock{};
        IntrusiveList<TraceListener, Trace> Listeners{};

        static constexpr std::array TraceLevelMarks = []()
        {
            return std::array{
                'V',
                'D',
                'I',
                'W',
                'E',
                'F',
                'N',
            };
        }();

        static constexpr char GetCharacter(TraceLevel level)
        {
            auto const index = std::to_underlying(level);
            if (index < TraceLevelMarks.size())
            {
                return TraceLevelMarks[index];
            }

            return 'N';
        }
    };

    extern UninitializedObject<TraceStatics> GTraceStatics;
}
