#pragma once
#include "AnemoneRenderVulkan/VulkanDevice.hxx"
#include "AnemoneRenderVulkan/VulkanHeaders.hxx"
#include "AnemoneRenderVulkan/VulkanCpuAllocator.hxx"
#include "AnemoneRenderVulkan/VulkanError.hxx"
#include "AnemoneBase/Intrusive.hxx"
#include "AnemoneBase/Reference.hxx"

namespace Anemone
{
    class VulkanCommandBuffer;

    class VulkanCommandPool
        : public ReferenceCounted<VulkanCommandPool>
        , private IntrusiveListNode<VulkanCommandPool>
    {
        friend struct IntrusiveList<VulkanCommandPool>;
        friend class VulkanCommandBuffer;

    private:
        VulkanDevice* m_device{};
        VkCommandPool m_commandPool{};

        IntrusiveList<VulkanCommandBuffer> m_commandBuffers{};

    public:
        VulkanCommandPool(VulkanDevice* device, uint32_t queueFamilyIndex)
            : m_device{device}
        {
            VkCommandPoolCreateInfo commandPoolCreateInfo{
                .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                .pNext = nullptr,
                .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                .queueFamilyIndex = queueFamilyIndex,
            };

            AE_VK_CALL(vkCreateCommandPool(this->m_device->_logicalDevice, &commandPoolCreateInfo, &VulkanCpuAllocator, &this->m_commandPool));
        }

        ~VulkanCommandPool()
        {
            AE_ASSERT(this->m_commandBuffers.IsEmpty());
            vkDestroyCommandPool(this->m_device->_logicalDevice, this->m_commandPool, &VulkanCpuAllocator);
        }

        Reference<VulkanCommandBuffer> CreateCommandBuffer();
    };

    class VulkanCommandBuffer
        : public ReferenceCounted<VulkanCommandBuffer>
        , private IntrusiveListNode<VulkanCommandBuffer>
    {
        friend struct IntrusiveList<VulkanCommandBuffer>;

    private:
        VulkanCommandPool* m_commandPool{};
        VkCommandBuffer m_commandBuffer{};
        VkFence m_fence{};

    public:
        constexpr VkCommandBuffer GetHandle() const
        {
            return this->m_commandBuffer;
        }

        constexpr VkFence GetFence() const
        {
            return this->m_fence;
        }

        void Reset()
        {
            vkResetFences(this->m_commandPool->m_device->_logicalDevice, 1, &this->m_fence);
            AE_VK_CALL(vkResetCommandBuffer(this->m_commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT));
        }

        void ImageTransition(
            VkImage image,
            VkImageLayout oldLayout,
            VkImageLayout newLayout,
            VkAccessFlags2 srcAccessMask,
            VkAccessFlags2 dstAccessMask,
            VkPipelineStageFlags2 srcStageMask,
            VkPipelineStageFlags2 dstStageMask,
            VkImageAspectFlags aspectMask)
        {

            VkImageMemoryBarrier2 barrier = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                .pNext = nullptr,
                .srcStageMask = srcStageMask,
                .srcAccessMask = srcAccessMask,
                .dstStageMask = dstStageMask,
                .dstAccessMask = dstAccessMask,
                .oldLayout = oldLayout,
                .newLayout = newLayout,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = image,
                .subresourceRange = VkImageSubresourceRange{
                    .aspectMask = aspectMask,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
            };

            VkDependencyInfo dependencyInfo{
                .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                .pNext = nullptr,
                .dependencyFlags = {},
                .memoryBarrierCount = 0,
                .pMemoryBarriers = nullptr,
                .bufferMemoryBarrierCount = 0,
                .pBufferMemoryBarriers = nullptr,
                .imageMemoryBarrierCount = 1,
                .pImageMemoryBarriers = &barrier,
            };

            vkCmdPipelineBarrier2(this->m_commandBuffer, &dependencyInfo);
        }

        void BeginRecording()
        {
            VkCommandBufferBeginInfo commandBufferBeginInfo{
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext = nullptr,
                .flags = 0,
                .pInheritanceInfo = nullptr,
            };
            AE_VK_CALL(vkBeginCommandBuffer(this->m_commandBuffer, &commandBufferBeginInfo));
        }

        void EndRecording()
        {
            AE_VK_CALL(vkEndCommandBuffer(this->m_commandBuffer));
        }

        // IDEA
        // - two kinds of rendering targets
        //   - swap chain - color attachment without any depth/stencil
        //   - offscreen framebuffer - 
        void BeginRendering(VkRenderingInfo const& renderingInfo)
        {
            vkCmdBeginRendering(this->m_commandBuffer, &renderingInfo);
        }

        void EndRendering()
        {
            vkCmdEndRendering(this->m_commandBuffer);
        }

    public:
        VulkanCommandBuffer(VulkanCommandPool* commandPool)
            : m_commandPool{commandPool}
        {
            VkCommandBufferAllocateInfo commandBufferAllocateInfo{
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .pNext = nullptr,
                .commandPool = this->m_commandPool->m_commandPool,
                .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandBufferCount = 1,
            };

            AE_VK_CALL(vkAllocateCommandBuffers(
                this->m_commandPool->m_device->_logicalDevice,
                &commandBufferAllocateInfo,
                &this->m_commandBuffer));

            AE_ASSERT(!this->m_commandPool->m_commandBuffers.Contains(this));
            this->m_commandPool->m_commandBuffers.PushBack(this);

            VkFenceCreateInfo fenceCreateInfo{
                .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                .pNext = nullptr,
                .flags = VK_FENCE_CREATE_SIGNALED_BIT,
            };

            AE_VK_CALL(vkCreateFence(
                this->m_commandPool->m_device->_logicalDevice,
                &fenceCreateInfo,
                &VulkanCpuAllocator,
                &this->m_fence));
        }

        ~VulkanCommandBuffer()
        {
            AE_ASSERT(this->m_commandPool->m_commandBuffers.Contains(this));
            this->m_commandPool->m_commandBuffers.Remove(this);

            AE_VK_CALL(vkWaitForFences(
                this->m_commandPool->m_device->_logicalDevice,
                1,
                &this->m_fence,
                VK_TRUE,
                UINT64_MAX));

            vkDestroyFence(
                this->m_commandPool->m_device->_logicalDevice,
                this->m_fence,
                &VulkanCpuAllocator);

            vkFreeCommandBuffers(
                this->m_commandPool->m_device->_logicalDevice,
                this->m_commandPool->m_commandPool,
                1, &this->m_commandBuffer);
        }
    };

    inline Reference<VulkanCommandBuffer> VulkanCommandPool::CreateCommandBuffer()
    {
        return MakeReference<VulkanCommandBuffer>(this);
    }

}
