#include "AnemoneRenderVulkan/VulkanBuffer.hxx"
#include "AnemoneRenderVulkan/VulkanDevice.hxx"

namespace Anemone
{
    VulkanBuffer::~VulkanBuffer()
    {
        vmaDestroyBuffer(this->m_device->_allocator, this->m_buffer, this->m_allocation);
    }

    VkResult VulkanBuffer::Create(
        VulkanDevice& device,
        VkBufferUsageFlags bufferUsage,
        VkDeviceSize bufferSize,
        Reference<VulkanBuffer>& outBuffer)
    {
        //VkBufferUsageFlags bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

        VkBufferCreateInfo bufferCreateInfo{
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .size = bufferSize,
            .usage = bufferUsage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
        };

        VmaAllocationCreateInfo allocationCreateInfo{
            .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
                VMA_ALLOCATION_CREATE_MAPPED_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO,
            .requiredFlags = 0,
            .preferredFlags = 0,
            .memoryTypeBits = 0,
            .pool = VK_NULL_HANDLE,
            .pUserData = nullptr,
            .priority = 0.0f,
        };

        VkBuffer buffer;
        VmaAllocation allocation;
        VmaAllocationInfo allocationInfo;

        VkResult result = vmaCreateBuffer(
            device._allocator,
            &bufferCreateInfo,
            &allocationCreateInfo,
            &buffer,
            &allocation,
            &allocationInfo);

        if (result == VK_SUCCESS)
        {
            outBuffer = MakeReference<VulkanBuffer>();
            if (!outBuffer)
            {
                return VK_ERROR_OUT_OF_HOST_MEMORY;
            }

            outBuffer->m_device = &device;
            outBuffer->m_usage = bufferUsage;
            outBuffer->m_size = bufferSize;
            outBuffer->m_buffer = buffer;
            outBuffer->m_allocation = allocation;
            outBuffer->m_allocationInfo = allocationInfo;
        }
        return result;
    }

}
