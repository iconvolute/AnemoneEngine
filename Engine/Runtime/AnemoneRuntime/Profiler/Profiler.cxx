#include "AnemoneRuntime/Profiler/Profiler.hxx"
#include "AnemoneRuntime/Hash/FNV.hxx"

#if ANEMONE_BUILD_PROFILING

namespace Anemone::Profiler
{
    static constexpr uint32_t ColorFromProfilerMarker(const char* name)
    {
        return Hash::FNV1A32{}.Update(name).Finalize();
    }

    ProfilerMarker::ProfilerMarker(const char* name, ProfilerLevel level)
        : m_name{name}
        , m_level{level}
        , m_color{ColorFromProfilerMarker(name)}
    {
        ProfilerMarkerRegistry::Get().Register(this);
    }

    ProfilerMarker::ProfilerMarker(const char* name)
        : m_name{name}
        , m_level{ProfilerLevel::Default}
        , m_color{ColorFromProfilerMarker(name)}
    {
        ProfilerMarkerRegistry::Get().Register(this);
    }

    ProfilerMarker::~ProfilerMarker()
    {
        ProfilerMarkerRegistry::Get().Unregister(this);
    }
}

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
