#pragma once
#include "AnemoneRuntime.Interop/Headers.hxx"
#include "AnemoneRuntime.Base/Reference.hxx"

namespace Anemone
{
    class ANEMONE_RUNTIME_RENDER_API RenderSwapChain : public ReferenceCounted<RenderSwapChain>
    {
    protected:
        RenderSwapChain() = default;

    public:
        RenderSwapChain(RenderSwapChain const&) = delete;
        RenderSwapChain(RenderSwapChain&&) = delete;
        virtual ~RenderSwapChain();
        RenderSwapChain& operator=(RenderSwapChain const&) = delete;
        RenderSwapChain& operator=(RenderSwapChain&&) = delete;

    public:
        virtual void Start() = 0;
        virtual void Present() = 0;
    };
}
