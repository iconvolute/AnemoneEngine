#pragma once
#include "AnemoneInterop/Headers.hxx"
#include "AnemoneBase/Reference.hxx"

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
