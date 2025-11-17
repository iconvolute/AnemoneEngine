#pragma once
#include "AnemoneBase/Intrusive.hxx"
#include "AnemoneBase/Reference.hxx"

namespace Anemone
{
    class GpuResource;
    class GpuDevice;
    class GpuViewport;

    class GpuTexture;
    class GpuBuffer;
    class GpuUniformBuffer;

    // class GpuIndexBuffer;
    // class GpuVertexBuffer;
    class GpuCommandList;
    class GpuCommandListContext;

    // Represents all mesh data stored on the GPU. This includes all buffers and other related resources
    // class GpuMesh;

    // Represents a shader program, textures and graphics pipeline state
    // class GpuMaterial;

    // Note: we hide some details about data passed to renderer. This is a first iteration, so we can refine it later.
    // For now, we hard-code:
    //     * uniform buffers for per-frame and per-draw data
    //       * some of them are per-drawcall, per-object, per-material
    //     * vertex formats
    //     * descriptor sets layout - our deferred renderer will have a predefined set of resources accessible for each draw call.
    //     * synchronization - it's handled internally

    // Represents a render target texture stored on the GPU.
    // This can be used as a render target or as a texture for sampling in shaders.
    // It can represent a color or depth/stencil buffer.
    // Swap chain returns one for the current back buffer.
    class GpuRenderTarget;

    enum class GpuVendor : uint32_t
    {
        Unknown = 0xFFFFFFFFu,
        Amd = 0x1002u,
        Nvidia = 0x10DEu,
        Intel = 0x8086u,
        Qualcomm = 0x5143u,
    };

    enum class GpuPipeline : uint8_t
    {
        Graphics,
        Compute,
        Transfer,
    };
}

namespace Anemone
{
    using GpuResourceRef = Reference<GpuResource>;
    using GpuDeviceRef = Reference<GpuDevice>;
    using GpuCommandListRef = Reference<GpuCommandList>;
    using GpuViewportRef = Reference<GpuViewport>;
    using GpuTextureRef = Reference<GpuTexture>;
    using GpuBufferRef = Reference<GpuBuffer>;
    using GpuCommandListContextRef = Reference<GpuCommandListContext>;
}

namespace Anemone
{
    class HostWindow;

    class GpuResource : public ThreadsafeReferenceCounted<GpuResource>
    {
    public:
        virtual ~GpuResource() = default;
    };

    class GpuDevice : public ThreadsafeReferenceCounted<GpuDevice>
    {
    public:
        virtual ~GpuDevice() = default;
        virtual Reference<GpuViewport> CreateViewport(Reference<HostWindow> window) = 0;
        virtual Reference<GpuCommandListContext> GetImmediateContext() = 0;
    };

    class GpuViewport : public ThreadsafeReferenceCounted<GpuViewport>
    {
    public:
        virtual ~GpuViewport() = default;
        virtual void Resize(uint32_t width, uint32_t height) = 0;
    };

    class GpuCommandListContext : public ThreadsafeReferenceCounted<GpuCommandListContext>
    {
    public:
        virtual ~GpuCommandListContext() = default;

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual void BeginDrawingViewport(GpuViewport& viewport) = 0;
        virtual void EndDrawingViewport(GpuViewport& viewport) = 0;

        virtual void BeginRecording() = 0;
        virtual void EndRecording() = 0;
    };

    class GpuTexture : public GpuResource
    {
    public:
        virtual ~GpuTexture() = default;
    };

    class GpuBuffer : public GpuResource
    {
        virtual ~GpuBuffer() = default;
    };
}
