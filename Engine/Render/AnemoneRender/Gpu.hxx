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

    class GpuVertexDeclaration;

    class GpuGraphicsPipeline;
    class GpuComputePipeline;

    class GpuShader;

    class GpuVertexShader;
    class GpuTessellationControlShader;
    class GpuTessellationEvaluationShader;
    class GpuGeometryShader;
    class GpuFragmentShader;
    class GpuMeshShader;
    class GpuTaskShader;

    class GpuComputeShader;

    // class GpuRayTracingShader;

    inline constexpr size_t GpuMaxGraphicsShaderStages = 7uz;
    inline constexpr size_t GpuMaxComputeShaderStages = 1uz;

    enum class GpuShaderStage
    {
        // Graphics
        Vertex = 0,
        TessellationControl = 1,
        TessellationEvaluation = 2,
        Geometry = 3,
        Fragment = 4,

        // Meshlet support
        Task = 5,
        Mesh = 6,

        // Compute
        Compute = 7,


        // TODO: raytracing
        // RayGen,
        // RayMiss,
        // RayHitGroup,
        // RayCallable,
    };

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

    enum class GpuTextureDimension : uint8_t
    {
        Texture2D,
        Texture2DArray,
        Texture3D,
        TextureCube,
        TextureCubeArray,
    };

    enum class GpuVertexElementFormat : uint8_t
    {
        None,
        Float1,
        Float2,
        Float3,
        Float4,
        Half2,
        Half4,
    };

    enum class GpuVertexElementFrequency : uint8_t
    {
        PerVertex,
        PerInstance,
    };

    inline constexpr size_t GpuMaxVertexElementCount = 16uz;
    inline constexpr size_t GpuMaxRenderTargets = 8uz;

    struct GpuVertexAttribute
    {
        // Vertex buffer binding
        uint8_t binding = 0;

        // Attribute index in the shader
        uint8_t attribute = 0;

        // Offset of the attribute in the vertex data.
        uint16_t offset = 0;

        // Format of the vertex attribute
        GpuVertexElementFormat format = GpuVertexElementFormat::None;

        // Frequency of the vertex attribute.
        GpuVertexElementFrequency frequency = GpuVertexElementFrequency::PerVertex;

        // Size of the vertex data
        uint16_t stride = 0;
    };

    struct GpuVertexDeclarationInitializer final
    {
        std::vector<GpuVertexAttribute> attributes;
    };

    struct GpuComputePipelineInitializer final
    {
        GpuComputeShader* computeShader{};
    };

    enum class GpuPrimitiveTopology : uint8_t
    {
        TriangleList,
        TriangleStrip,
    };


    struct GpuShadingRate
    {
    };

    struct GpuInputAssemblyState final
    {
        GpuPrimitiveTopology topology;
        bool primitiveRestartEnable;
    };

    struct GpuTessellationState final
    {
        uint32_t patchControlPoints;
    };

    enum class GpuPolygonMode : uint8_t
    {
        Fill,
        Line,
        Point,
    };

    enum class GpuCullMode : uint8_t
    {
        None,
        Front,
        Back,
        FrontAndBack,
    };

    enum class GpuFrontFace : uint8_t
    {
        CounterClockwise,
        Clockwise,
    };

    struct GpuRasterizationState final
    {
        bool depthClampEnable;
        bool rasterizerDiscardEnable;
        GpuPolygonMode polygonMode;
        GpuCullMode cullMode;
        GpuFrontFace frontFace;
        bool depthBiasEnable;
        float depthBiasConstantFactor;
        float depthBiasClamp;
        float depthBiasSlopeFactor;
        float lineWidth;
    };

    struct GpuMultisampleState final
    {
        uint32_t rasterizationSamples;
        bool alphaToCoverageEnable;
    };

    enum class GpuCompareOperation : uint8_t
    {
        Never,
        Less,
        Equal,
        LessOrEqual,
        Greater,
        NotEqual,
        GreaterOrEqual,
        Always,
    };

    enum class GpuStencilOperation : uint8_t
    {
        Keep,
        Zero,
        Replace,
        IncrementAndClamp,
        DecrementAndClamp,
        Invert,
        IncrementAndWrap,
        DecrementAndWrap,
    };

    struct GpuStencilOperationState final
    {
        GpuStencilOperation failOperation;
        GpuStencilOperation passOperation;
        GpuStencilOperation depthFailOperation;
        GpuCompareOperation compareOperation;
        uint32_t compareMask;
        uint32_t writeMask;
        uint32_t reference;
    };

    struct GpuDepthStencilState final
    {
        // VkPipelineDepthStencilStateCreateFlags flags;
        bool depthTestEnable;
        bool depthWriteEnable;
        GpuCompareOperation depthCompareOperation;
        bool depthBoundsTestEnable;
        bool stencilTestEnable;
        GpuStencilOperationState front;
        GpuStencilOperationState back;
        float minDepthBounds;
        float maxDepthBounds;
    };

    enum class GpuLogicOperation : uint8_t
    {
        Clear = 0, // 0
        And = 1, // s & d
        AndReverse = 2, // s & ~d
        Copy = 3, // s
        AndInverted = 4, // ~s & d
        Noop = 5, // no-op
        Xor = 6, // s ^ d
        Or = 7, // s | d
        Nor = 8, // ~(s | d)
        Equivalent = 9, // ~(s ^ d)
        Invert = 10, // ~d
        OrReverse = 11, // s | ~d
        CopyInverted = 12, // ~s
        OrInverted = 13, // ~s | d
        Nand = 14, // ~(s & d)
        Set = 15, // 1

    };

    enum class GpuBlendFactor : uint8_t
    {
        Zero,
        One,
        SrcColor,
        OneMinusSrcColor,
        DstColor,
        OneMinusDstColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstAlpha,
        OneMinusDstAlpha,
        ConstantColor,
        OneMinusConstantColor,
        ConstantAlpha,
        OneMinusConstantAlpha,
        SrcAlphaSaturate,
        Src1Color,
        OneMinusSrc1Color,
        Src1Alpha,
        OneMinusSrc1Alpha,
    };

    enum class GpuBlendOperation : uint8_t
    {
        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max,
    };

    enum class GpuColorComponents : uint8_t
    {
        R = 0x1,
        G = 0x2,
        B = 0x4,
        A = 0x8,
    };

    struct GpuColorBlendState final
    {
        bool blendEnable;
        GpuBlendFactor srcColorBlendFactor;
        GpuBlendFactor dstColorBlendFactor;
        GpuBlendOperation colorBlendOperation;
        GpuBlendFactor srcAlphaBlendFactor;
        GpuBlendFactor dstAlphaBlendFactor;
        GpuBlendOperation alphaBlendOperation;
        GpuColorComponents colorWriteMask;
    };

    struct GpuBlendState final
    {
        bool logicOpEnable;
        GpuLogicOperation logicOperation;
        uint32_t attachmentCount;
        GpuColorBlendState attachments[GpuMaxRenderTargets];
        float blendConstants[4];
    };

    struct GpuGraphicsPipelineInitializer final
    {
        GpuVertexDeclaration* vertexDeclaration{};

        GpuVertexShader* vertexShader;
        GpuTessellationControlShader* tessellationControlShader;
        GpuTessellationEvaluationShader* tessellationEvaluationShader;
        GpuGeometryShader* geometryShader;
        GpuFragmentShader* fragmentShader;
        GpuMeshShader* meshShader;
        GpuTaskShader* taskShader;

        GpuInputAssemblyState inputAssemblyState;
        GpuTessellationState tessellationState;
        GpuRasterizationState rasterizationState;
        GpuDepthStencilState depthStencilState;
        GpuMultisampleState multisampleState;
        GpuBlendState blendState;
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
    using GpuVertexDeclarationRef = Reference<GpuVertexDeclaration>;
    using GpuGraphicsPipelineRef = Reference<GpuGraphicsPipeline>;
    using GpuComputePipelineRef = Reference<GpuComputePipeline>;

    using GpuVertexShaderRef = Reference<GpuVertexShader>;
    using GpuTessellationControlShaderRef = Reference<GpuTessellationControlShader>;
    using GpuTessellationEvaluationShaderRef = Reference<GpuTessellationEvaluationShader>;
    using GpuGeometryShaderRef = Reference<GpuGeometryShader>;
    using GpuFragmentShaderRef = Reference<GpuFragmentShader>;
    using GpuMeshShaderRef = Reference<GpuMeshShader>;
    using GpuTaskShaderRef = Reference<GpuTaskShader>;
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
        virtual GpuViewportRef CreateViewport(Reference<HostWindow> window) = 0;
        virtual GpuVertexDeclarationRef CreateVertexDeclaration(GpuVertexDeclarationInitializer const& initializer) = 0;
        virtual GpuGraphicsPipelineRef CreateGraphicsPipeline(GpuGraphicsPipelineInitializer const& initializer) = 0;
        virtual GpuComputePipelineRef CreateComputePipeline(GpuComputePipelineInitializer const& initializer) = 0;
        virtual GpuCommandListContextRef GetImmediateContext() = 0;
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
        ~GpuTexture() override = default;
    };

    class GpuBuffer : public GpuResource
    {
    public:
        ~GpuBuffer() override = default;
    };

    class GpuVertexDeclaration : public GpuResource
    {
    public:
        ~GpuVertexDeclaration() override = default;
    };

    class GpuGraphicsPipeline : public GpuResource
    {
    public:
        ~GpuGraphicsPipeline() override = default;
    };

    class GpuComputePipeline : public GpuResource
    {
    public:
        ~GpuComputePipeline() override = default;
    };

    class GpuShader : public GpuResource
    {
    public:
        ~GpuShader() override = default;
    };

    class GpuVertexShader : public GpuShader
    {
    public:
        ~GpuVertexShader() override = default;
    };

    class GpuTessellationControlShader : public GpuShader
    {
    public:
        ~GpuTessellationControlShader() override = default;
    };

    class GpuTessellationEvaluationShader : public GpuShader
    {
    public:
        ~GpuTessellationEvaluationShader() override = default;
    };

    class GpuGeometryShader : public GpuShader
    {
    public:
        ~GpuGeometryShader() override = default;
    };

    class GpuFragmentShader : public GpuShader
    {
    public:
        ~GpuFragmentShader() override = default;
    };

    class GpuMeshShader : public GpuShader
    {
    public:
        ~GpuMeshShader() override = default;
    };

    class GpuTaskShader : public GpuShader
    {
    public:
        ~GpuTaskShader() override = default;
    };
}
