#pragma once
#include "AnemoneRuntime.Interop/Headers.hxx"
#include "AnemoneRuntime.Base/Reference.hxx"
#include "AnemoneRuntime.HostApplication/HostWindow.hxx"

namespace Anemone
{
    class RenderSwapChain;

    class ANEMONE_RUNTIME_RENDER_API RenderDevice : public ReferenceCounted<RenderDevice>
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
