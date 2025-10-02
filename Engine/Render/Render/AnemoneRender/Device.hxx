#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Base/Reference.hxx"

namespace Anemone
{
    class RENDER_API RenderDevice : public ReferenceCounted<RenderDevice>
    {
    protected:
        RenderDevice() = default;

    public:
        RenderDevice(RenderDevice const&) = delete;
        RenderDevice(RenderDevice&&) = delete;
        virtual ~RenderDevice();
        RenderDevice& operator=(RenderDevice const&) = delete;
        RenderDevice& operator=(RenderDevice&&) = delete;
    };
}
