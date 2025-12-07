#pragma once
#include "AnemoneRuntime.RenderVulkan/VulkanHeaders.hxx"
#include "AnemoneRuntime.Base/Reference.hxx"
#include "AnemoneRuntime.Base/MemoryBuffer.hxx"
#include "AnemoneRuntime.Render/Gpu.hxx"

namespace Anemone
{
    // Maximum number of descriptor set layouts in a pipeline layout.
    inline constexpr size_t VulkanMaxPipelineLayoutDescriptorSetLayouts = 8uz;

    class VulkanDevice;

    class VulkanGraphicsPipeline : public GpuGraphicsPipeline
    {
    public:
        VulkanDevice* m_device{};
        VkPipeline m_pipeline{};

        // TODO: Cache layout based on used descriptor sets and push constants.
        VkPipelineLayout m_pipelineLayout{};

    public:
        VulkanGraphicsPipeline(VulkanDevice& device);
    };

    class VulkanComputePipeline : public GpuComputePipeline
    {
    public:
        VulkanDevice* m_device{};
        VkPipeline m_pipeline{};

        // TODO: Cache layout based on used descriptor sets and push constants.
        VkPipelineLayout m_pipelineLayout{};

    public:
        VulkanComputePipeline(VulkanDevice& device);
    };

    class VulkanPipelineCache : public ThreadsafeReferenceCounted<VulkanPipelineCache>
    {
    public:
        VulkanDevice* m_device{};
        VkPipelineCache m_pipelineCache{};

        Reference<MemoryBuffer> Save() const;
    };
}
