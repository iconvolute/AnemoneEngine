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

    public:
        constexpr VkCommandBuffer GetHandle() const
        {
            return this->m_commandBuffer;
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
        }

        ~VulkanCommandBuffer()
        {
            AE_ASSERT(this->m_commandPool->m_commandBuffers.Contains(this));
            this->m_commandPool->m_commandBuffers.Remove(this);

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
