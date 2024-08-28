#pragma once
#include "AnemoneRuntime/Profiler/Profiler.hxx"

#if ANEMONE_BUILD_PROFILING

namespace Anemone::Profiler
{
    class ProfilerNvidia : public Profiler
    {
    public:
        ProfilerNvidia() = default;
        virtual ~ProfilerNvidia() = default;

    public:
        void BeginMarker(ProfilerMarker& marker) override;
        void EndMarker(ProfilerMarker& marker) override;
        void Event(ProfilerMarker& marker) override;
    };
}

#endif
