#include "AnemoneRuntime/Profiler.hxx"

#if ANEMONE_BUILD_PROFILING

namespace Anemone::Profiler
{
    void ProfilerMarkerRegistry::Register(ProfilerMarker* marker)
    {
        this->m_markers.PushBack(marker);
    }

    void ProfilerMarkerRegistry::Unregister(ProfilerMarker* marker)
    {
        this->m_markers.Remove(marker);
    }

    void ProfilerMarkerRegistry::Enumerate(FunctionRef<void(ProfilerMarker&)> callback)
    {
        this->m_markers.ForEach(callback);
    }

    ProfilerMarkerRegistry& ProfilerMarkerRegistry::Get()
    {
        static ProfilerMarkerRegistry instance;
        return instance;
    }
}

#endif
