#include "AnemoneRuntime/Profiler.hxx"

#if ANEMONE_BUILD_PROFILING

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <nvtx3/nvToolsExt.h>

ANEMONE_EXTERNAL_HEADERS_END

namespace Anemone::Profiler
{
    RUNTIME_API void BeginFrame()
    {
        nvtxRangePushA("Frame");
    }

    RUNTIME_API void EndFrame()
    {
        nvtxRangePop();
    }

    constexpr uint32_t ColorFromAddress(uintptr_t address)
    {
        uint64_t const c = 0xcbf29ce484222325 + (address * 0x00000100000001B3);
        return static_cast<uint32_t>((c ^ (c >> 32)) & 0xFFFFFFFF);
    }

    RUNTIME_API void BeginMarker(ProfilerMarker& marker)
    {
        nvtxEventAttributes_t attributes = {0};
        attributes.version = NVTX_VERSION;
        attributes.size = NVTX_EVENT_ATTRIB_STRUCT_SIZE;
        attributes.category = 0;
        attributes.colorType = NVTX_COLOR_ARGB;
        attributes.color = ColorFromAddress(reinterpret_cast<uintptr_t>(&marker));
        attributes.messageType = NVTX_MESSAGE_TYPE_ASCII;
        attributes.message.ascii = marker.Name();
        nvtxRangePushEx(&attributes);
    }

    RUNTIME_API void EndMarker(ProfilerMarker& marker)
    {
        (void)marker;
        nvtxRangePop();
    }

    RUNTIME_API void Event(const char* name)
    {
        nvtxEventAttributes_t attributes = {0};
        attributes.version = NVTX_VERSION;
        attributes.size = NVTX_EVENT_ATTRIB_STRUCT_SIZE;
        attributes.category = 0;
        attributes.colorType = NVTX_COLOR_ARGB;
        attributes.color = ColorFromAddress(reinterpret_cast<uintptr_t>(name));
        attributes.messageType = NVTX_MESSAGE_TYPE_ASCII;
        attributes.message.ascii = name;
        nvtxMarkEx(&attributes);
    }
}

#endif
