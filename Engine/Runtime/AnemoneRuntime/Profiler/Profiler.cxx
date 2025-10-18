#include "AnemoneRuntime/Profiler/Profiler.hxx"
#include "AnemoneRuntime/Hash/FNV.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"

namespace Anemone::Internal
{
    extern void InitializeProfiling()
    {
    }

    extern void FinalizeProfiling()
    {
    }
}

#if ANEMONE_BUILD_PROFILING

namespace Anemone
{
    struct ProfilerStatics final
    {
        IProfilerBackend* Backend = nullptr;

        static constexpr uint32_t ColorFromProfilerMarker(const char* name)
        {
            return FNV1A32{}.Update(name).Finalize();
        }
    };

    static ProfilerStatics GProfilerStatics{};

    void Profiler::Initialize()
    {
    }

    void Profiler::Finalize()
    {
    }

    struct ProfilerMarkerRegistry final
    {
        CriticalSection Lock{};
        IntrusiveList<ProfilerMarker> Markers{};

        static ProfilerMarkerRegistry& Get()
        {
            static ProfilerMarkerRegistry registry{};
            return registry;
        }
    };

    void Profiler::RegisterMarker(ProfilerMarker& marker)
    {
        ProfilerMarkerRegistry& registry = ProfilerMarkerRegistry::Get();

        UniqueLock scope{registry.Lock};
        registry.Markers.PushBack(&marker);
    }

    void Profiler::UnregisterMarker(ProfilerMarker& marker)
    {
        ProfilerMarkerRegistry& registry = ProfilerMarkerRegistry::Get();

        UniqueLock scope{registry.Lock};
        registry.Markers.Remove(&marker);
    }

    void Profiler::EnumerateMarkers(FunctionRef<void(ProfilerMarker& marker)> callback)
    {
        ProfilerMarkerRegistry& registry = ProfilerMarkerRegistry::Get();

        UniqueLock lock{registry.Lock};
        registry.Markers.ForEach(callback);
    }

    void Profiler::BeginMarker(ProfilerMarker& marker)
    {
        if (IProfilerBackend* backend = GProfilerStatics.Backend)
        {
            backend->BeginMarker(marker);
        }
    }

    void Profiler::EndMarker(ProfilerMarker& marker)
    {
        if (IProfilerBackend* backend = GProfilerStatics.Backend)
        {
            backend->EndMarker(marker);
        }
    }

    void Profiler::EventMarker(ProfilerMarker& marker)
    {
        if (IProfilerBackend* backend = GProfilerStatics.Backend)
        {
            backend->EventMarker(marker);
        }
    }
}

namespace Anemone
{
    ProfilerMarker::ProfilerMarker(const char* name, ProfilerLevel level)
        : m_name{name}
        , m_level{level}
        , m_color{ProfilerStatics::ColorFromProfilerMarker(name)}
    {
        Profiler::RegisterMarker(*this);
    }

    ProfilerMarker::ProfilerMarker(const char* name)
        : m_name{name}
        , m_level{ProfilerLevel::Default}
        , m_color{ProfilerStatics::ColorFromProfilerMarker(name)}
    {
        Profiler::RegisterMarker(*this);
    }

    ProfilerMarker::~ProfilerMarker()
    {
        Profiler::UnregisterMarker(*this);
    }
}

#endif
