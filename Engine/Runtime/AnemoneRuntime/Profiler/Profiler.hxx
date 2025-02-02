#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
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

    class RUNTIME_API ProfilerMarker final
        : private IntrusiveListNode<ProfilerMarker, class ProfilerMarkerRegistry>
    {
        friend struct IntrusiveList<ProfilerMarker, ProfilerMarkerRegistry>;

    private:
        const char* m_name{};
        ProfilerLevel m_level{};
        uint32_t m_color{};
        void* m_handle{}; // additional handle for profiler-specific data.

    public:
        explicit ProfilerMarker(const char* name, ProfilerLevel level);
        explicit ProfilerMarker(const char* name);
        ProfilerMarker(ProfilerMarker const&) = delete;
        ProfilerMarker(ProfilerMarker&&) = delete;
        ProfilerMarker& operator=(ProfilerMarker const&) = delete;
        ProfilerMarker& operator=(ProfilerMarker&&) = delete;
        ~ProfilerMarker();

        const char* Name() const
        {
            return this->m_name;
        }

        ProfilerLevel Level() const
        {
            return this->m_level;
        }
        uint32_t Color() const
        {
            return this->m_color;
        }
    };

    // Profiler system design:
    //  - development tool for performance analysis
    //    - not for production
    //    - compiled out from final build
    //    - may be initialized lazily (singleton)
    //    - used to initialize profiling handle sections at runtime

    class Profiler
    {
    protected:
        Profiler() = default;

    public:
        Profiler(Profiler const&) = delete;
        Profiler(Profiler&&) = delete;
        Profiler& operator=(Profiler const&) = delete;
        Profiler& operator=(Profiler&&) = delete;
        virtual ~Profiler() = default;

    public:
        virtual void BeginMarker(ProfilerMarker& marker) = 0;
        virtual void EndMarker(ProfilerMarker& marker) = 0;
        virtual void Event(ProfilerMarker& marker) = 0;
    };

    extern Profiler* GProfiler;

    class ProfilerScope final
    {
    private:
        ProfilerMarker* m_marker;

    public:
        ProfilerScope(ProfilerMarker& marker)
            : m_marker{&marker}
        {
            if (GProfiler)
            {
                GProfiler->BeginMarker(*this->m_marker);
            }
        }

        ProfilerScope(ProfilerScope const&) = delete;
        ProfilerScope(ProfilerScope&&) = delete;
        ProfilerScope& operator=(ProfilerScope const&) = delete;
        ProfilerScope& operator=(ProfilerScope&&) = delete;

        ~ProfilerScope()
        {
            if (GProfiler)
            {
                GProfiler->EndMarker(*this->m_marker);
            }
        }
    };

}

#endif

#if ANEMONE_BUILD_PROFILING

#define AE_DECLARE_PROFILE(name) \
    Anemone::Profiler::ProfilerMarker GProfilerMarker_##name { #name }

#define AE_PROFILE_SCOPE(name) \
    Anemone::Profiler::ProfilerScope local_profiler_scope_##name { GProfilerMarker_##name }

#define AE_PROFILE_EVENT(name) \
    Anemone::Profiler::GProfiler->Event(GProfilerMarker_##name);

#else

#define AE_DECLARE_PROFILE(name)
#define AE_PROFILE_SCOPE(name)
#define AE_PROFILE_EVENT(name)

#endif

// - intel
//   - __itt_task_begin
//   - __itt_task_end
//   - __itt_marker + __itt_marker_scope_thread
//   - __itt_string_handle_createA
//   - __itt_domain, __itt_string_handle
