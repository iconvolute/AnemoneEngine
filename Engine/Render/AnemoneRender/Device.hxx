#pragma once
#include "AnemoneInterop/Headers.hxx"
#include "AnemoneBase/Reference.hxx"
#include "AnemoneApplication/HostWindow.hxx"

namespace Anemone
{
    class RenderSwapChain;

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

    public:
        virtual Reference<RenderSwapChain> CreateSwapChain(Reference<HostWindow> window) = 0;
    };
}
