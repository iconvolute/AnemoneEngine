#pragma once
#include "AnemoneProfiler/Profiler.hxx"

#if ANEMONE_BUILD_PROFILING

namespace Anemone
{
    class NvidiaProfilerBackend final : public IProfilerBackend
    {
    public:
        NvidiaProfilerBackend() = default;
        ~NvidiaProfilerBackend() override = default;

    public:
        void BeginFrame() override;

        void EndFrame() override;

        void BeginMarker(ProfilerMarker& marker) override;

        void EndMarker(ProfilerMarker& marker) override;

        void EventMarker(ProfilerMarker& marker) override;
    };
}

#endif
