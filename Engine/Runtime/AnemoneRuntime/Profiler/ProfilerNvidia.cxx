#include "AnemoneRuntime/Profiler/ProfilerNvidia.hxx"

#if ANEMONE_BUILD_PROFILING

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <nvtx3/nvToolsExt.h>

ANEMONE_EXTERNAL_HEADERS_END

namespace Anemone::Profiler
{
    void ProfilerNvidia::BeginMarker(ProfilerMarker& marker)
    {
        nvtxEventAttributes_t attributes{};
        attributes.version = NVTX_VERSION;
        attributes.size = NVTX_EVENT_ATTRIB_STRUCT_SIZE;
        attributes.category = 0;
        attributes.colorType = NVTX_COLOR_ARGB;
        attributes.color = marker.Color();
        attributes.messageType = NVTX_MESSAGE_TYPE_ASCII;
        attributes.message.ascii = marker.Name();
        nvtxRangePushEx(&attributes);
    }

    void ProfilerNvidia::EndMarker(ProfilerMarker& marker)
    {
        (void)marker;
        nvtxRangePop();
    }

    void ProfilerNvidia::Event(ProfilerMarker& marker)
    {
        nvtxEventAttributes_t attributes{};
        attributes.version = NVTX_VERSION;
        attributes.size = NVTX_EVENT_ATTRIB_STRUCT_SIZE;
        attributes.category = 0;
        attributes.colorType = NVTX_COLOR_ARGB;
        attributes.color = marker.Color();
        attributes.messageType = NVTX_MESSAGE_TYPE_ASCII;
        attributes.message.ascii = marker.Name();
        nvtxMarkEx(&attributes);
    }
}

#endif
