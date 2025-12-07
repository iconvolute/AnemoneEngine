#include "AnemoneRuntime.RenderVulkan/VulkanPipeline.hxx"
#include "AnemoneRuntime.RenderVulkan/VulkanShader.hxx"
#include "AnemoneRuntime.RenderVulkan/VulkanCpuAllocator.hxx"
#include "AnemoneRuntime.RenderVulkan/VulkanError.hxx"
#include "AnemoneRuntime.RenderVulkan/VulkanDevice.hxx"
#include "AnemoneRuntime.RenderVulkan/VulkanVertexDeclaration.hxx"

namespace Anemone
{
    inline VkShaderStageFlagBits TranslateGpuShaderStage(GpuShaderStage stage)
    {
        switch (stage)
        {
        case GpuShaderStage::Vertex:
            return VK_SHADER_STAGE_VERTEX_BIT;

        case GpuShaderStage::TessellationControl:
            return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;

        case GpuShaderStage::TessellationEvaluation:
            return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

        case GpuShaderStage::Geometry:
            return VK_SHADER_STAGE_GEOMETRY_BIT;

        case GpuShaderStage::Fragment:
            return VK_SHADER_STAGE_FRAGMENT_BIT;

        case GpuShaderStage::Compute:
            return VK_SHADER_STAGE_COMPUTE_BIT;

        case GpuShaderStage::Task:
            return VK_SHADER_STAGE_TASK_BIT_EXT;

        case GpuShaderStage::Mesh:
            return VK_SHADER_STAGE_MESH_BIT_EXT;
        }

        AE_PANIC("Unknown GPU shader stage");
        std::unreachable();
    }

    inline VkPrimitiveTopology TranslateGpuPrimitiveTopology(GpuPrimitiveTopology value)
    {
        switch (value)
        {
        case GpuPrimitiveTopology::TriangleList:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        case GpuPrimitiveTopology::TriangleStrip:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        }

        AE_PANIC("Unknown GPU primitive topology");
        std::unreachable();
    }

    inline VkPolygonMode TranslateGpuPolygonMode(GpuPolygonMode value)
    {
        switch (value)
        {
        case GpuPolygonMode::Fill:
            return VK_POLYGON_MODE_FILL;

        case GpuPolygonMode::Line:
            return VK_POLYGON_MODE_LINE;

        case GpuPolygonMode::Point:
            return VK_POLYGON_MODE_POINT;
        }

        AE_PANIC("Unknown GPU polygon mode");
        std::unreachable();
    }

    inline VkCullModeFlags TranslateGpuCullMode(GpuCullMode value)
    {
        switch (value)
        {
        case GpuCullMode::None:
            return VK_CULL_MODE_NONE;

        case GpuCullMode::Front:
            return VK_CULL_MODE_FRONT_BIT;

        case GpuCullMode::Back:
            return VK_CULL_MODE_BACK_BIT;

        case GpuCullMode::FrontAndBack:
            return VK_CULL_MODE_FRONT_AND_BACK;
        }

        AE_PANIC("Unknown GPU cull mode");
        std::unreachable();
    }

    inline VkFrontFace TranslateGpuFrontFace(GpuFrontFace value)
    {
        switch (value)
        {
        case GpuFrontFace::CounterClockwise:
            return VK_FRONT_FACE_COUNTER_CLOCKWISE;

        case GpuFrontFace::Clockwise:
            return VK_FRONT_FACE_CLOCKWISE;
        }

        AE_PANIC("Unknown GPU front face");
        std::unreachable();
    }

    inline VkCompareOp TranslateGpuCompareOperation(GpuCompareOperation value)
    {
        switch (value)
        {
        case GpuCompareOperation::Never:
            return VK_COMPARE_OP_NEVER;

        case GpuCompareOperation::Less:
            return VK_COMPARE_OP_LESS;

        case GpuCompareOperation::Equal:
            return VK_COMPARE_OP_EQUAL;

        case GpuCompareOperation::LessOrEqual:
            return VK_COMPARE_OP_LESS_OR_EQUAL;

        case GpuCompareOperation::Greater:
            return VK_COMPARE_OP_GREATER;

        case GpuCompareOperation::NotEqual:
            return VK_COMPARE_OP_NOT_EQUAL;

        case GpuCompareOperation::GreaterOrEqual:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;

        case GpuCompareOperation::Always:
            return VK_COMPARE_OP_ALWAYS;
        }

        AE_PANIC("Unknown GPU compare operation");
        std::unreachable();
    }

    inline VkStencilOp TranslateGpuStencilOperation(GpuStencilOperation value)
    {
        switch (value)
        {
        case GpuStencilOperation::Keep:
            return VK_STENCIL_OP_KEEP;

        case GpuStencilOperation::Zero:
            return VK_STENCIL_OP_ZERO;

        case GpuStencilOperation::Replace:
            return VK_STENCIL_OP_REPLACE;

        case GpuStencilOperation::IncrementAndClamp:
            return VK_STENCIL_OP_INCREMENT_AND_CLAMP;

        case GpuStencilOperation::DecrementAndClamp:
            return VK_STENCIL_OP_DECREMENT_AND_CLAMP;

        case GpuStencilOperation::Invert:
            return VK_STENCIL_OP_INVERT;

        case GpuStencilOperation::IncrementAndWrap:
            return VK_STENCIL_OP_INCREMENT_AND_WRAP;

        case GpuStencilOperation::DecrementAndWrap:
            return VK_STENCIL_OP_DECREMENT_AND_WRAP;
        }

        AE_PANIC("Unknown GPU stencil operation");
        std::unreachable();
    }

    inline VkBlendFactor TranslateGpuBlendFactor(GpuBlendFactor value)
    {
        switch (value)
        {
        case GpuBlendFactor::Zero:
            return VK_BLEND_FACTOR_ZERO;

        case GpuBlendFactor::One:
            return VK_BLEND_FACTOR_ONE;

        case GpuBlendFactor::SrcColor:
            return VK_BLEND_FACTOR_SRC_COLOR;

        case GpuBlendFactor::OneMinusSrcColor:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;

        case GpuBlendFactor::DstColor:
            return VK_BLEND_FACTOR_DST_COLOR;

        case GpuBlendFactor::OneMinusDstColor:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;

        case GpuBlendFactor::SrcAlpha:
            return VK_BLEND_FACTOR_SRC_ALPHA;

        case GpuBlendFactor::OneMinusSrcAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

        case GpuBlendFactor::DstAlpha:
            return VK_BLEND_FACTOR_DST_ALPHA;

        case GpuBlendFactor::OneMinusDstAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;

        case GpuBlendFactor::ConstantColor:
            return VK_BLEND_FACTOR_CONSTANT_COLOR;

        case GpuBlendFactor::OneMinusConstantColor:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;

        case GpuBlendFactor::ConstantAlpha:
            return VK_BLEND_FACTOR_CONSTANT_ALPHA;

        case GpuBlendFactor::OneMinusConstantAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;

        case GpuBlendFactor::SrcAlphaSaturate:
            return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;

        case GpuBlendFactor::Src1Color:
            return VK_BLEND_FACTOR_SRC1_COLOR;

        case GpuBlendFactor::OneMinusSrc1Color:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;

        case GpuBlendFactor::Src1Alpha:
            return VK_BLEND_FACTOR_SRC1_ALPHA;

        case GpuBlendFactor::OneMinusSrc1Alpha:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
        }

        AE_PANIC("Unknown GPU blend factor");
        std::unreachable();
    }

    inline VkBlendOp TranslateGpuBlendOperation(GpuBlendOperation value)
    {
        switch (value)
        {
        case GpuBlendOperation::Add:
            return VK_BLEND_OP_ADD;

        case GpuBlendOperation::Subtract:
            return VK_BLEND_OP_SUBTRACT;

        case GpuBlendOperation::ReverseSubtract:
            return VK_BLEND_OP_REVERSE_SUBTRACT;

        case GpuBlendOperation::Min:
            return VK_BLEND_OP_MIN;

        case GpuBlendOperation::Max:
            return VK_BLEND_OP_MAX;
        }

        AE_PANIC("Unknown GPU blend operation");
        std::unreachable();
    }

    inline VkLogicOp TranslateGpuLogicOperation(GpuLogicOperation value)
    {
        switch (value)
        {
        case GpuLogicOperation::Clear:
            return VK_LOGIC_OP_CLEAR;

        case GpuLogicOperation::And:
            return VK_LOGIC_OP_AND;

        case GpuLogicOperation::AndReverse:
            return VK_LOGIC_OP_AND_REVERSE;

        case GpuLogicOperation::Copy:
            return VK_LOGIC_OP_COPY;

        case GpuLogicOperation::AndInverted:
            return VK_LOGIC_OP_AND_INVERTED;

        case GpuLogicOperation::Noop:
            return VK_LOGIC_OP_NO_OP;

        case GpuLogicOperation::Xor:
            return VK_LOGIC_OP_XOR;

        case GpuLogicOperation::Or:
            return VK_LOGIC_OP_OR;

        case GpuLogicOperation::Nor:
            return VK_LOGIC_OP_NOR;

        case GpuLogicOperation::Equivalent:
            return VK_LOGIC_OP_EQUIVALENT;

        case GpuLogicOperation::Invert:
            return VK_LOGIC_OP_INVERT;

        case GpuLogicOperation::OrReverse:
            return VK_LOGIC_OP_OR_REVERSE;

        case GpuLogicOperation::CopyInverted:
            return VK_LOGIC_OP_COPY_INVERTED;

        case GpuLogicOperation::OrInverted:
            return VK_LOGIC_OP_OR_INVERTED;

        case GpuLogicOperation::Nand:
            return VK_LOGIC_OP_NAND;

        case GpuLogicOperation::Set:
            return VK_LOGIC_OP_SET;
        }

        AE_PANIC("Unknown GPU logic operation");
        std::unreachable();
    }

    inline VkColorComponentFlags TranslateGpuColorComponentFlags(GpuColorComponents value)
    {
        // Types are compatible.
        return static_cast<VkColorComponentFlags>(value);
    }

    VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanDevice& device)
        : m_device{&device}
    {
        // const VkPipelineShaderStageCreateInfo* pStages;
        // const VkPipelineVertexInputStateCreateInfo* pVertexInputState;
        // const VkPipelineInputAssemblyStateCreateInfo* pInputAssemblyState;
        // const VkPipelineTessellationStateCreateInfo* pTessellationState;
        // const VkPipelineViewportStateCreateInfo* pViewportState;
        // const VkPipelineRasterizationStateCreateInfo* pRasterizationState;
        // const VkPipelineMultisampleStateCreateInfo* pMultisampleState;
        // const VkPipelineDepthStencilStateCreateInfo* pDepthStencilState;
        // const VkPipelineColorBlendStateCreateInfo* pColorBlendState;
        // const VkPipelineDynamicStateCreateInfo* pDynamicState;
#if false
        VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo[1]{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
            .module = nullptr,
            .pName = "vertMain",
            .pSpecializationInfo = nullptr,
        };

        /*VkVertexInputBindingDescription vertexInputBindings[]
        {
            {
                .binding = 0, .stride = sizeof(float), .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
            }
        }*/

        VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .vertexBindingDescriptionCount = ,
            .pVertexBindingDescriptions = ,
            .vertexAttributeDescriptionCount = ,
            .pVertexAttributeDescriptions = 
        };

        VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stageCount = ,
        .pStages = ,
        .pVertexInputState = ,
        .pInputAssemblyState = ,
        .pTessellationState = ,
        .pViewportState = ,
        .pRasterizationState = ,
        .pMultisampleState = ,
        .pDepthStencilState = ,
        .pColorBlendState = ,
        .pDynamicState = ,
        .layout = ,
        .renderPass = ,
        .subpass = ,
        .basePipelineHandle = ,
        .basePipelineIndex = 
        };

        AE_VULKAN_ENSURE(vkCreateGraphicsPipelines(
            this->m_device->m_device,
            VK_NULL_HANDLE,
            1,
            &graphicsPipelineCreateInfo,
            VulkanCpuAllocator,
            &this->m_pipeline));
#endif
    }
}

namespace Anemone
{
    VulkanComputePipeline::VulkanComputePipeline(VulkanDevice& device)
        : m_device{&device}
    {
    }
}

namespace Anemone
{
    Reference<MemoryBuffer> VulkanPipelineCache::Save() const
    {
        Reference<MemoryBuffer> buffer{};

        if (auto data = MemoryBuffer::Create(sizeof(VkPipelineCacheHeaderVersionOne)))
        {
            buffer = std::move(*data);

            VkResult result;
            size_t size = 0;

            do
            {
                result = vkGetPipelineCacheData(
                    this->m_device->m_device,
                    this->m_pipelineCache,
                    &size,
                    nullptr);

                if (result == VK_SUCCESS)
                {
                    if (buffer->Resize(size))
                    {
                        result = vkGetPipelineCacheData(
                            this->m_device->m_device,
                            this->m_pipelineCache,
                            &size,
                            buffer->GetData());
                    }
                    else
                    {
                        // Unable to resize buffer.
                        break;
                    }
                }
            } while (result == VK_INCOMPLETE);
        }

        return buffer;
    }
}

namespace Anemone
{
    GpuGraphicsPipelineRef VulkanDevice::CreateGraphicsPipeline(GpuGraphicsPipelineInitializer const& initializer)
    {
        VulkanVertexDeclaration const* vertexDeclaration = static_cast<VulkanVertexDeclaration const*>(initializer.vertexDeclaration);

        Reference<VulkanGraphicsPipeline> result = MakeReference<VulkanGraphicsPipeline>(*this);

        VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .vertexBindingDescriptionCount = vertexDeclaration->m_bindingCount,
            .pVertexBindingDescriptions = vertexDeclaration->m_bindings,
            .vertexAttributeDescriptionCount = vertexDeclaration->m_attributeCount,
            .pVertexAttributeDescriptions = vertexDeclaration->m_attributes,
        };

        uint32_t pipelineShaderStagesCount = 0;
        VkPipelineShaderStageCreateInfo pipelineShaderStages[GpuMaxGraphicsShaderStages];

        if (VulkanVertexShader* shader = static_cast<VulkanVertexShader*>(initializer.vertexShader))
        {
            pipelineShaderStages[pipelineShaderStagesCount++] = VkPipelineShaderStageCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = shader->m_handle,
                .pName = "main",
                .pSpecializationInfo = nullptr,
            };
        }

        if (VulkanTessellationControlShader* shader = static_cast<VulkanTessellationControlShader*>(initializer.tessellationControlShader))
        {
            pipelineShaderStages[pipelineShaderStagesCount++] = VkPipelineShaderStageCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
                .module = shader->m_handle,
                .pName = "main",
                .pSpecializationInfo = nullptr,
            };
        }

        if (VulkanTessellationEvaluationShader* shader = static_cast<VulkanTessellationEvaluationShader*>(initializer.tessellationEvaluationShader))
        {
            pipelineShaderStages[pipelineShaderStagesCount++] = VkPipelineShaderStageCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
                .module = shader->m_handle,
                .pName = "main",
                .pSpecializationInfo = nullptr,
            };
        }

        if (VulkanGeometryShader* shader = static_cast<VulkanGeometryShader*>(initializer.geometryShader))
        {
            pipelineShaderStages[pipelineShaderStagesCount++] = VkPipelineShaderStageCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .stage = VK_SHADER_STAGE_GEOMETRY_BIT,
                .module = shader->m_handle,
                .pName = "main",
                .pSpecializationInfo = nullptr,
            };
        }

        if (VulkanFragmentShader* shader = static_cast<VulkanFragmentShader*>(initializer.fragmentShader))
        {
            pipelineShaderStages[pipelineShaderStagesCount++] = VkPipelineShaderStageCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = shader->m_handle,
                .pName = "main",
                .pSpecializationInfo = nullptr,
            };
        }

        if (VulkanMeshShader* shader = static_cast<VulkanMeshShader*>(initializer.meshShader))
        {
            pipelineShaderStages[pipelineShaderStagesCount++] = VkPipelineShaderStageCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .stage = VK_SHADER_STAGE_MESH_BIT_EXT,
                .module = shader->m_handle,
                .pName = "main",
                .pSpecializationInfo = nullptr,
            };
        }

        if (VulkanTaskShader* shader = static_cast<VulkanTaskShader*>(initializer.taskShader))
        {
            pipelineShaderStages[pipelineShaderStagesCount++] = VkPipelineShaderStageCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .stage = VK_SHADER_STAGE_TASK_BIT_EXT,
                .module = shader->m_handle,
                .pName = "main",
                .pSpecializationInfo = nullptr,
            };
        }

        VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .topology = TranslateGpuPrimitiveTopology(initializer.inputAssemblyState.topology),
            .primitiveRestartEnable = initializer.inputAssemblyState.primitiveRestartEnable ? VK_TRUE : VK_FALSE,
        };

        VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .patchControlPoints = initializer.tessellationState.patchControlPoints,
        };

        VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .viewportCount = 1,
            .pViewports = nullptr,
            .scissorCount = 1,
            .pScissors = nullptr,
        };

        VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthClampEnable = initializer.rasterizationState.depthClampEnable ? VK_TRUE : VK_FALSE,
            .rasterizerDiscardEnable = initializer.rasterizationState.rasterizerDiscardEnable ? VK_TRUE : VK_FALSE,
            .polygonMode = TranslateGpuPolygonMode(initializer.rasterizationState.polygonMode),
            .cullMode = TranslateGpuCullMode(initializer.rasterizationState.cullMode),
            .frontFace = TranslateGpuFrontFace(initializer.rasterizationState.frontFace),
            .depthBiasEnable = initializer.rasterizationState.depthBiasEnable ? VK_TRUE : VK_FALSE,
            .depthBiasConstantFactor = initializer.rasterizationState.depthBiasConstantFactor,
            .depthBiasClamp = initializer.rasterizationState.depthBiasClamp,
            .depthBiasSlopeFactor = initializer.rasterizationState.depthBiasSlopeFactor,
            .lineWidth = initializer.rasterizationState.lineWidth,
        };

        VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .rasterizationSamples = static_cast<VkSampleCountFlagBits>(std::max<uint32_t>(1u, initializer.multisampleState.rasterizationSamples)),
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 0.0f,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = initializer.multisampleState.alphaToCoverageEnable ? VK_TRUE : VK_FALSE,
            .alphaToOneEnable = VK_FALSE,
        };

        VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthTestEnable = initializer.depthStencilState.depthTestEnable ? VK_TRUE : VK_FALSE,
            .depthWriteEnable = initializer.depthStencilState.depthWriteEnable ? VK_TRUE : VK_FALSE,
            .depthCompareOp = TranslateGpuCompareOperation(initializer.depthStencilState.depthCompareOperation),
            .depthBoundsTestEnable = initializer.depthStencilState.depthBoundsTestEnable ? VK_TRUE : VK_FALSE,
            .stencilTestEnable = initializer.depthStencilState.stencilTestEnable ? VK_TRUE : VK_FALSE,
            .front = VkStencilOpState{
                .failOp = TranslateGpuStencilOperation(initializer.depthStencilState.front.failOperation),
                .passOp = TranslateGpuStencilOperation(initializer.depthStencilState.front.passOperation),
                .depthFailOp = TranslateGpuStencilOperation(initializer.depthStencilState.front.depthFailOperation),
                .compareOp = TranslateGpuCompareOperation(initializer.depthStencilState.front.compareOperation),
                .compareMask = initializer.depthStencilState.front.compareMask,
                .writeMask = initializer.depthStencilState.front.writeMask,
                .reference = initializer.depthStencilState.front.reference,
            },
            .back = VkStencilOpState{
                .failOp = TranslateGpuStencilOperation(initializer.depthStencilState.back.failOperation),
                .passOp = TranslateGpuStencilOperation(initializer.depthStencilState.back.passOperation),
                .depthFailOp = TranslateGpuStencilOperation(initializer.depthStencilState.back.depthFailOperation),
                .compareOp = TranslateGpuCompareOperation(initializer.depthStencilState.back.compareOperation),
                .compareMask = initializer.depthStencilState.back.compareMask,
                .writeMask = initializer.depthStencilState.back.writeMask,
                .reference = initializer.depthStencilState.back.reference,
            },
            .minDepthBounds = initializer.depthStencilState.minDepthBounds,
            .maxDepthBounds = initializer.depthStencilState.maxDepthBounds,
        };

        // TODO: can we get which render targets are used by fragment shader?
        VkPipelineColorBlendAttachmentState colorBlendAttachments[GpuMaxRenderTargets];
        AE_ASSERT(initializer.blendState.attachmentCount < GpuMaxRenderTargets);

        for (uint32_t i = 0; i < initializer.blendState.attachmentCount; ++i)
        {
            colorBlendAttachments[i] = VkPipelineColorBlendAttachmentState{
                .blendEnable = initializer.blendState.attachments[i].blendEnable ? VK_TRUE : VK_FALSE,
                .srcColorBlendFactor = TranslateGpuBlendFactor(initializer.blendState.attachments[i].srcColorBlendFactor),
                .dstColorBlendFactor = TranslateGpuBlendFactor(initializer.blendState.attachments[i].dstColorBlendFactor),
                .colorBlendOp = TranslateGpuBlendOperation(initializer.blendState.attachments[i].colorBlendOperation),
                .srcAlphaBlendFactor = TranslateGpuBlendFactor(initializer.blendState.attachments[i].srcAlphaBlendFactor),
                .dstAlphaBlendFactor = TranslateGpuBlendFactor(initializer.blendState.attachments[i].dstAlphaBlendFactor),
                .alphaBlendOp = TranslateGpuBlendOperation(initializer.blendState.attachments[i].alphaBlendOperation),
                .colorWriteMask = TranslateGpuColorComponentFlags(initializer.blendState.attachments[i].colorWriteMask),
            };
        }

        VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .logicOpEnable = initializer.blendState.logicOpEnable ? VK_TRUE : VK_FALSE,
            .logicOp = TranslateGpuLogicOperation(initializer.blendState.logicOperation),
            .attachmentCount = initializer.blendState.attachmentCount,
            .pAttachments = colorBlendAttachments,
            .blendConstants = {
                initializer.blendState.blendConstants[0],
                initializer.blendState.blendConstants[1],
                initializer.blendState.blendConstants[2],
                initializer.blendState.blendConstants[3],
            },
        };

        uint32_t dynamicStateCount = 0;
        VkDynamicState dynamicStates[8];
        dynamicStates[dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
        dynamicStates[dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;
        dynamicStates[dynamicStateCount++] = VK_DYNAMIC_STATE_STENCIL_REFERENCE;
        dynamicStates[dynamicStateCount++] = VK_DYNAMIC_STATE_DEPTH_BOUNDS;

        VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .dynamicStateCount = dynamicStateCount,
            .pDynamicStates = dynamicStates,
        };

        uint32_t pipelineDescriptorSetLayoutCount = 0;
        VkDescriptorSetLayout pipelineDescriptorSetLayouts[VulkanMaxPipelineLayoutDescriptorSetLayouts];
        // TODO: populate descriptor set layouts from initializer.

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = pipelineDescriptorSetLayoutCount,
            .pSetLayouts = pipelineDescriptorSetLayouts,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr,
        };

        AE_VULKAN_ENSURE(vkCreatePipelineLayout(
            this->m_device,
            &pipelineLayoutCreateInfo,
            VulkanCpuAllocator,
            &result->m_pipelineLayout));

        VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stageCount = pipelineShaderStagesCount,
            .pStages = pipelineShaderStages,
            .pVertexInputState = &pipelineVertexInputStateCreateInfo,
            .pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo,
            .pTessellationState = &pipelineTessellationStateCreateInfo,
            .pViewportState = &pipelineViewportStateCreateInfo,
            .pRasterizationState = &pipelineRasterizationStateCreateInfo,
            .pMultisampleState = &pipelineMultisampleStateCreateInfo,
            .pDepthStencilState = &pipelineDepthStencilStateCreateInfo,
            .pColorBlendState = &pipelineColorBlendStateCreateInfo,
            .pDynamicState = &pipelineDynamicStateCreateInfo,
            .layout = result->m_pipelineLayout,
            .renderPass = VK_NULL_HANDLE,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = 0,
        };

        AE_VULKAN_ENSURE(vkCreateGraphicsPipelines(
            this->m_device,
            VK_NULL_HANDLE,
            1,
            &graphicsPipelineCreateInfo,
            VulkanCpuAllocator,
            &result->m_pipeline));

        return result;
    }

    GpuComputePipelineRef VulkanDevice::CreateComputePipeline(GpuComputePipelineInitializer const& initializer)
    {
        AE_ASSERT(initializer.computeShader != nullptr);

        VulkanComputeShader const* vulkanComputeShader = static_cast<VulkanComputeShader const*>(initializer.computeShader);

        uint32_t pipelineDescriptorSetLayoutCount = 0;
        VkDescriptorSetLayout pipelineDescriptorSetLayouts[VulkanMaxPipelineLayoutDescriptorSetLayouts];
        // TODO: populate descriptor set layouts from initializer.

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = pipelineDescriptorSetLayoutCount,
            .pSetLayouts = pipelineDescriptorSetLayouts,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr,
        };

        Reference<VulkanComputePipeline> result = MakeReference<VulkanComputePipeline>(*this);

        AE_VULKAN_ENSURE(vkCreatePipelineLayout(
            this->m_device,
            &pipelineLayoutCreateInfo,
            VulkanCpuAllocator,
            &result->m_pipelineLayout));

        VkComputePipelineCreateInfo computePipelineCreateInfo{
            .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = VkPipelineShaderStageCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .stage = VK_SHADER_STAGE_COMPUTE_BIT,
                .module = vulkanComputeShader->m_handle,
                .pName = "main",
                .pSpecializationInfo = nullptr,
            },
            .layout = result->m_pipelineLayout,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = 0,
        };

        AE_VULKAN_ENSURE(vkCreateComputePipelines(
            this->m_device,
            VK_NULL_HANDLE,
            1,
            &computePipelineCreateInfo,
            VulkanCpuAllocator,
            &result->m_pipeline));

        return result;
    }
}
