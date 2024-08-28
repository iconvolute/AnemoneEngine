#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Intrusive.hxx"
#include "AnemoneRuntime/FunctionRef.hxx"

#if ANEMONE_BUILD_PROFILING

namespace Anemone::Profiler
{
    enum class ProfilerLevel
    {
        Disabled = 0,
        Minimal = 1,
        Default = 2,
        Detailed = 3,
    };

    class ProfilerMarker;

    class ProfilerMarkerRegistry final
    {
    private:
        IntrusiveList<ProfilerMarker, ProfilerMarkerRegistry> m_markers{};

    public:
        ProfilerMarkerRegistry() = default;
        ProfilerMarkerRegistry(ProfilerMarkerRegistry const&) = delete;
        ProfilerMarkerRegistry(ProfilerMarkerRegistry&&) = delete;
        ProfilerMarkerRegistry& operator=(ProfilerMarkerRegistry const&) = delete;
        ProfilerMarkerRegistry& operator=(ProfilerMarkerRegistry&&) = delete;
        ~ProfilerMarkerRegistry() = default;

    public:
        RUNTIME_API void Register(ProfilerMarker* marker);

        RUNTIME_API void Unregister(ProfilerMarker* marker);

        RUNTIME_API void Enumerate(FunctionRef<void(ProfilerMarker&)> callback);

        RUNTIME_API static ProfilerMarkerRegistry& Get();
    };

    class ProfilerMarker final
        : private IntrusiveListNode<ProfilerMarker, class ProfilerMarkerRegistry>
    {
        friend struct IntrusiveList<ProfilerMarker, ProfilerMarkerRegistry>;

    private:
        const char* m_name{};
        ProfilerLevel m_level{};

    public:
        explicit ProfilerMarker(const char* name, ProfilerLevel level) noexcept
            : m_name{name}
            , m_level{level}
        {
            ProfilerMarkerRegistry::Get().Register(this);
        }

        explicit ProfilerMarker(const char* name) noexcept
            : m_name{name}
            , m_level{ProfilerLevel::Default}
        {
            ProfilerMarkerRegistry::Get().Register(this);
        }

        ~ProfilerMarker()
        {
            ProfilerMarkerRegistry::Get().Unregister(this);
        }

        const char* Name() const noexcept
        {
            return m_name;
        }

        ProfilerLevel Level() const noexcept
        {
            return m_level;
        }
    };

    // Profiler system design:
    //  - development tool for performance analysis
    //    - not for production
    //    - compiled out from final build
    //    - may be initialized lazily (singleton)
    //    - used to initialize profiling handle sections at runtime

    RUNTIME_API void BeginFrame();
    RUNTIME_API void EndFrame();

    RUNTIME_API void BeginMarker(ProfilerMarker& marker);
    RUNTIME_API void EndMarker(ProfilerMarker& marker);
    RUNTIME_API void Event(const char* name);

    class ProfilerScope final
    {
    private:
        ProfilerMarker& m_marker;

    public:
        ProfilerScope(ProfilerMarker& marker)
            : m_marker{marker}
        {
            BeginMarker(this->m_marker);
        }

        ~ProfilerScope()
        {
            EndMarker(this->m_marker);
        }
    };

}

#endif

#if ANEMONE_BUILD_PROFILING

#define AE_DECLARE_PROFILE(name) \
    Anemone::Profiler::ProfilerMarker GProfilerMarker_##name { #name }

#define AE_PROFILE_SCOPE(name) \
    Anemone::Profiler::ProfilerScope local_profiler_scope_##name { GProfilerMarker_##name }

#else

#define AE_DECLARE_PROFILE(name)
#define AE_PROFILE_SCOPE(name)

#endif

// - intel
//   - __itt_task_begin
//   - __itt_task_end
//   - __itt_marker + __itt_marker_scope_thread
//   - __itt_string_handle_createA
//   - __itt_domain, __itt_string_handle
