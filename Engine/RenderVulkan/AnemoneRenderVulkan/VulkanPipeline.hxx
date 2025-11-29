#pragma once
#include "AnemoneRenderVulkan/VulkanHeaders.hxx"
#include "AnemoneBase/Reference.hxx"
#include "AnemoneBase/MemoryBuffer.hxx"
#include "AnemoneRender/Gpu.hxx"

namespace Anemone
{
    class VulkanDevice;

    class VulkanGraphicsPipeline : public GpuGraphicsPipeline
    {
    public:
        VulkanDevice* m_device{};
        VkPipeline m_pipeline{};

    public:
        VulkanGraphicsPipeline(VulkanDevice& device);
    };

    class VulkanComputePipeline : public GpuComputePipeline
    {
    public:
        VulkanDevice* m_device{};
        VkPipeline m_pipeline{};

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
