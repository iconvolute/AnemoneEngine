#pragma once
#include "AnemoneRuntime.RenderVulkan/VulkanHeaders.hxx"
#include "AnemoneRuntime.Base/Reference.hxx"

namespace Anemone
{
    class VulkanDevice;
    class VulkanCommandBuffer;

    class VulkanBuffer : public ThreadsafeReferenceCounted<VulkanBuffer>
    {
    public:
        VulkanDevice* m_device;
        VkBufferUsageFlags m_usage;
        VkDeviceSize m_size;
        VkBuffer m_buffer;
        VmaAllocation m_allocation;
        VmaAllocationInfo m_allocationInfo;

        ~VulkanBuffer();

    public:
        static VkResult Create(
            VulkanDevice& device,
            VkBufferUsageFlags bufferUsage,
            VkDeviceSize bufferSize,
            Reference<VulkanBuffer>& outBuffer);
    };
}
