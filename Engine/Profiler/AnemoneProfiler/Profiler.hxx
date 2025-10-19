#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Base/Intrusive.hxx"
#include "AnemoneRuntime/Base/FunctionRef.hxx"

#if ANEMONE_BUILD_PROFILING

// Profiler system design:
//  - development tool for performance analysis
//    - not for production
//    - compiled out from final build
//    - may be initialized lazily (singleton)
//    - used to initialize profiling handle sections at runtime

namespace Anemone
{
    enum class ProfilerLevel
    {
        Disabled = 0,
        Minimal = 1,
        Default = 2,
        Detailed = 3,
    };

    class ProfilerMarker;

    class IProfilerBackend
    {
    public:
        virtual ~IProfilerBackend() = default;

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual void BeginMarker(ProfilerMarker& marker) = 0;
        virtual void EndMarker(ProfilerMarker& marker) = 0;

        virtual void EventMarker(ProfilerMarker& marker) = 0;
    };

    struct Profiler final
    {
        RUNTIME_API static void Initialize();
        RUNTIME_API static void Finalize();

        RUNTIME_API static void RegisterMarker(ProfilerMarker& marker);
        RUNTIME_API static void UnregisterMarker(ProfilerMarker& marker);
        RUNTIME_API static void EnumerateMarkers(FunctionRef<void(ProfilerMarker& marker)> callback);

        RUNTIME_API static void BeginMarker(ProfilerMarker& marker);
        RUNTIME_API static void EndMarker(ProfilerMarker& marker);
        RUNTIME_API static void EventMarker(ProfilerMarker& marker);
    };

    class RUNTIME_API ProfilerMarker final
        : private IntrusiveListNode<ProfilerMarker>
    {
        friend struct IntrusiveList<ProfilerMarker>;

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

    class ProfilerScope final
    {
    private:
        ProfilerMarker* m_marker;

    public:
        ProfilerScope(ProfilerMarker& marker)
            : m_marker{&marker}
        {
            Profiler::BeginMarker(*this->m_marker);
        }

        ProfilerScope(ProfilerScope const&) = delete;
        ProfilerScope(ProfilerScope&&) = delete;
        ProfilerScope& operator=(ProfilerScope const&) = delete;
        ProfilerScope& operator=(ProfilerScope&&) = delete;

        ~ProfilerScope()
        {
            Profiler::EndMarker(*this->m_marker);
        }
    };

}

#endif

#if ANEMONE_BUILD_PROFILING

#define AE_DECLARE_PROFILE(name) \
    Anemone::ProfilerMarker GProfilerMarker_##name { #name }

#define AE_PROFILE_SCOPE(name) \
    Anemone::ProfilerScope local_profiler_scope_##name { GProfilerMarker_##name }

#define AE_PROFILE_EVENT(name) \
    Anemone::Profiler::EventMarker(GProfilerMarker_##name);

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
