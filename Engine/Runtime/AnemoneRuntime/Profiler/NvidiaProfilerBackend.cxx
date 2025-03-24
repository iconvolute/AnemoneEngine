#include "AnemoneRuntime/Profiler/NvidiaProfilerBackend.hxx"

#if ANEMONE_BUILD_PROFILING

#include <nvtx3/nvToolsExt.h>

namespace Anemone
{
    void NvidiaProfilerBackend::BeginFrame()
    {
        nvtxEventAttributes_t attributes{};
        attributes.version = NVTX_VERSION;
        attributes.size = NVTX_EVENT_ATTRIB_STRUCT_SIZE;
        attributes.category = 0;
        attributes.colorType = NVTX_COLOR_UNKNOWN;
        attributes.color = 0;
        attributes.messageType = NVTX_MESSAGE_TYPE_ASCII;
        attributes.message.ascii = "Frame";
        nvtxRangePushEx(&attributes);
    }

    void NvidiaProfilerBackend::EndFrame()
    {
        nvtxRangePop();
    }

    void NvidiaProfilerBackend::BeginMarker(ProfilerMarker& marker)
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

    void NvidiaProfilerBackend::EndMarker(ProfilerMarker& marker)
    {
        (void)marker;
        nvtxRangePop();
    }

    void NvidiaProfilerBackend::EventMarker(ProfilerMarker& marker)
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
