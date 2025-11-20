#include "AnemoneRenderVulkan/VulkanCommandBuffer.hxx"
#include "AnemoneRenderVulkan/VulkanBuffer.hxx"

namespace Anemone
{
    VulkanCommandBufferPool::VulkanCommandBufferPool(VulkanDevice* device, uint32_t queueFamilyIndex, VulkanCommandBufferType commandBufferType)
        : m_device{device}
        , m_commandBufferType{commandBufferType}
    {
        VkCommandPoolCreateInfo commandPoolCreateInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = queueFamilyIndex,
        };

        (void)commandPoolCreateInfo;
        AE_VULKAN_ENSURE(vkCreateCommandPool(
            this->m_device->m_device,
            &commandPoolCreateInfo,
            VulkanCpuAllocator,
            &this->m_commandPool));
    }

    VulkanCommandBufferPool::~VulkanCommandBufferPool()
    {
        while (VulkanCommandBuffer* current = this->m_acquiredCommandBufferList.PopFront())
        {
            delete current;
        }

        while (VulkanCommandBuffer* current = this->m_releasedCommandBufferList.PopFront())
        {
            delete current;
        }

        vkDestroyCommandPool(
            this->m_device->m_device,
            this->m_commandPool,
            VulkanCpuAllocator);
        this->m_commandPool = VK_NULL_HANDLE;
    }

    VulkanCommandBuffer* VulkanCommandBufferPool::AllocateCommandBuffer()
    {
        UniqueLock lock{this->m_lock};

        VulkanCommandBuffer* commandBuffer = this->m_releasedCommandBufferList.PopFront();

        if (commandBuffer)
        {
            commandBuffer->Reset();
        }
        else
        {
            commandBuffer = new VulkanCommandBuffer(*this);
        }

        if (commandBuffer->m_commandBufferState == VulkanCommandBufferState::Disposed)
        {
            commandBuffer->AllocateResources();
        }

        this->m_acquiredCommandBufferList.PushBack(commandBuffer);
        return commandBuffer;
    }

    void VulkanCommandBufferPool::ReleaseCommandBuffer(VulkanCommandBuffer* commandBuffer)
    {
        UniqueLock lock{this->m_lock};

        AE_ASSERT(this->m_acquiredCommandBufferList.Contains(commandBuffer));
        this->m_acquiredCommandBufferList.Remove(commandBuffer);
        this->m_releasedCommandBufferList.PushBack(commandBuffer);
    }

    void VulkanCommandBufferPool::CollectUnusedCommandBuffers(bool trim)
    {
        UniqueLock scope{this->m_lock};

        if (trim)
        {
            vkTrimCommandPool(this->m_device->m_device, this->m_commandPool, 0);
        }
        else
        {
            Instant const currentTimestamp = Instant::Now();

            static constexpr Duration Timeout = Duration::FromSeconds(10);

            this->m_acquiredCommandBufferList.ForEach([&](VulkanCommandBuffer& commandBuffer)
            {
                if ((commandBuffer.m_commandBufferState == VulkanCommandBufferState::Ready) ||
                    (commandBuffer.m_commandBufferState == VulkanCommandBufferState::PendingReset))
                {
                    Duration const timeout = currentTimestamp - commandBuffer.m_submittedTimestamp;

                    if (timeout > Timeout)
                    {
                        commandBuffer.ReleaseResources();
                        this->m_acquiredCommandBufferList.Remove(&commandBuffer);
                        this->m_releasedCommandBufferList.PushBack(&commandBuffer);
                    }
                }
            });
        }
    }
}

namespace Anemone
{
    void VulkanCommandBuffer::AllocateResources()
    {
        AE_ASSERT(this->m_commandBufferState == VulkanCommandBufferState::Disposed);

        VkCommandBufferAllocateInfo commandBufferAllocateInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = this->m_commandBufferPool->m_commandPool,
            .level = ToCommandBufferLevel(this->m_commandBufferPool->m_commandBufferType),
            .commandBufferCount = 1,
        };

        AE_VULKAN_ENSURE(vkAllocateCommandBuffers(
            this->m_commandBufferPool->m_device->m_device,
            &commandBufferAllocateInfo,
            &this->m_commandBuffer));

        this->m_commandBufferState = VulkanCommandBufferState::Ready;
    }

    void VulkanCommandBuffer::ReleaseResources()
    {
        AE_ASSERT(this->m_commandBufferState != VulkanCommandBufferState::Disposed);

        vkFreeCommandBuffers(
            this->m_commandBufferPool->m_device->m_device,
            this->m_commandBufferPool->m_commandPool,
            1, &this->m_commandBuffer);

        this->m_commandBufferState = VulkanCommandBufferState::Disposed;
    }

    void VulkanCommandBuffer::Reset()
    {
        UniqueLock scope{this->m_lock};
        if (this->m_commandBufferState == VulkanCommandBufferState::Submitted)
        {
            this->m_commandBufferPool->m_device->ReleaseBarrierEvents(this->m_barrierEvents);
            this->m_barrierEvents.clear();

            this->m_commandBufferState = VulkanCommandBufferState::PendingReset;
        }
    }

    void VulkanCommandBuffer::BeginRecording()
    {
        {
            UniqueLock scope{this->m_lock};

            if (this->m_commandBufferState == VulkanCommandBufferState::PendingReset)
            {
                AE_VULKAN_ENSURE(vkResetCommandBuffer(this->m_commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT));
            }
            else
            {
                AE_ASSERT(this->m_commandBufferState == VulkanCommandBufferState::Ready);
            }

            this->m_commandBufferState = VulkanCommandBufferState::Recording;
        }

        VkCommandBufferBeginInfo commandBufferBeginInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = nullptr,
        };
        AE_VULKAN_ENSURE(vkBeginCommandBuffer(this->m_commandBuffer, &commandBufferBeginInfo));
    }

    void VulkanCommandBuffer::EndRecording()
    {
        AE_ASSERT(this->m_commandBufferState == VulkanCommandBufferState::Recording);

        for (VkEvent barrierEvent : this->m_barrierEvents)
        {
            vkCmdResetEvent2(this->m_commandBuffer, barrierEvent, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
        }

        AE_VULKAN_ENSURE(vkEndCommandBuffer(this->m_commandBuffer));

        this->m_commandBufferState = VulkanCommandBufferState::Finished;
    }

    void VulkanCommandBuffer::UploadBuffer(VulkanBuffer& buffer, void const* data, size_t size)
    {
        // Where changes are visible
        constexpr VkPipelineStageFlags2 dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
        // What kind of access is needed
        constexpr VkAccessFlags2 dstAccessMask = VK_ACCESS_2_UNIFORM_READ_BIT;

        // TODO: Use one time submit command buffer for this
        VkMemoryPropertyFlags memoryPropertyFlags;

        vmaGetAllocationMemoryProperties(
            this->m_commandBufferPool->m_device->m_allocator,
            buffer.m_allocation, &memoryPropertyFlags);

        if (memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
        {
            // FIXME: Maybe it is better to allocate staging buffer anyway?
            AE_VULKAN_ENSURE(vmaCopyMemoryToAllocation(
                this->m_commandBufferPool->m_device->m_allocator,
                data,
                buffer.m_allocation,
                0,
                size));

            VkBufferMemoryBarrier2 bufferMemoryBarrier2{
                .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                .pNext = nullptr,
                .srcStageMask = VK_PIPELINE_STAGE_2_HOST_BIT,
                .srcAccessMask = VK_ACCESS_2_HOST_WRITE_BIT,
                .dstStageMask = dstStageMask,
                .dstAccessMask = dstAccessMask,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .buffer = buffer.m_buffer,
                .offset = 0,
                .size = VK_WHOLE_SIZE};

            VkDependencyInfo dependencyInfo{
                .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                .pNext = nullptr,
                .dependencyFlags = {},
                .memoryBarrierCount = 0,
                .pMemoryBarriers = nullptr,
                .bufferMemoryBarrierCount = 1,
                .pBufferMemoryBarriers = &bufferMemoryBarrier2,
                .imageMemoryBarrierCount = 0,
                .pImageMemoryBarriers = nullptr,
            };

            vkCmdPipelineBarrier2(this->m_commandBuffer, &dependencyInfo);
        }
        else
        {
            VkBufferCreateInfo stagingBufferCreateInfo{
                .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .size = buffer.m_size,
                .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                .queueFamilyIndexCount = 0,
                .pQueueFamilyIndices = nullptr,
            };

            VmaAllocationCreateInfo stagingAllocationCreateInfo{
                .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                    VMA_ALLOCATION_CREATE_MAPPED_BIT,
                .usage = VMA_MEMORY_USAGE_AUTO,
                .requiredFlags = 0,
                .preferredFlags = 0,
                .memoryTypeBits = 0,
                .pool = VK_NULL_HANDLE,
                .pUserData = nullptr,
                .priority = 0.0f,
            };

            VkBuffer stagingBuffer;
            VmaAllocation stagingAllocation;
            VmaAllocationInfo stagingAllocationInfo;

            AE_VULKAN_ENSURE(vmaCreateBuffer(
                this->m_commandBufferPool->m_device->m_allocator,
                &stagingBufferCreateInfo,
                &stagingAllocationCreateInfo,
                &stagingBuffer,
                &stagingAllocation,
                &stagingAllocationInfo));

            AE_VULKAN_ENSURE(vmaCopyMemoryToAllocation(
                this->m_commandBufferPool->m_device->m_allocator,
                data,
                stagingAllocation,
                0,
                size));

            // Host-device barrier
            {
                VkBufferMemoryBarrier2 bufferMemoryBarrier2{
                    .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                    .pNext = nullptr,
                    .srcStageMask = VK_PIPELINE_STAGE_2_HOST_BIT,
                    .srcAccessMask = VK_ACCESS_2_HOST_WRITE_BIT,
                    .dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                    .dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT,
                    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .buffer = stagingBuffer,
                    .offset = 0,
                    .size = VK_WHOLE_SIZE,
                };

                VkDependencyInfo dependencyInfo{
                    .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                    .pNext = nullptr,
                    .dependencyFlags = {},
                    .memoryBarrierCount = 0,
                    .pMemoryBarriers = nullptr,
                    .bufferMemoryBarrierCount = 1,
                    .pBufferMemoryBarriers = &bufferMemoryBarrier2,
                    .imageMemoryBarrierCount = 0,
                    .pImageMemoryBarriers = nullptr,
                };

                vkCmdPipelineBarrier2(this->m_commandBuffer, &dependencyInfo);
            }

            // staging -> buffer copy
            {
                VkBufferCopy2 bufferCopyRegion{
                    .sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2,
                    .pNext = nullptr,
                    .srcOffset = 0,
                    .dstOffset = 0,
                    .size = size,
                };

                VkCopyBufferInfo2 copyBufferInfo{
                    .sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
                    .pNext = 0,
                    .srcBuffer = stagingBuffer,
                    .dstBuffer = buffer.m_buffer,
                    .regionCount = 1,
                    .pRegions = &bufferCopyRegion,
                };
                vkCmdCopyBuffer2(this->m_commandBuffer, &copyBufferInfo);
            }

            // transfer barrier
            {
                VkBufferMemoryBarrier2 bufferMemoryBarrier2{
                    .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                    .pNext = nullptr,
                    .srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                    .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
                    .dstStageMask = dstStageMask,
                    .dstAccessMask = dstAccessMask,
                    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .buffer = stagingBuffer,
                    .offset = 0,
                    .size = VK_WHOLE_SIZE,
                };

                VkDependencyInfo dependencyInfo{
                    .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                    .pNext = nullptr,
                    .dependencyFlags = {},
                    .memoryBarrierCount = 0,
                    .pMemoryBarriers = nullptr,
                    .bufferMemoryBarrierCount = 1,
                    .pBufferMemoryBarriers = &bufferMemoryBarrier2,
                    .imageMemoryBarrierCount = 0,
                    .pImageMemoryBarriers = nullptr,
                };

                vkCmdPipelineBarrier2(this->m_commandBuffer, &dependencyInfo);
            }
        }
    }

    void VulkanCommandBuffer::BeginSplitBarrier(VkEvent barrierEvent, VkDependencyInfo const& dependencyInfo)
    {
        vkCmdSetEvent2(this->m_commandBuffer, barrierEvent, &dependencyInfo);
    }

    void VulkanCommandBuffer::EndSplitBarrier(VkEvent barrierEvent, VkDependencyInfo const& dependencyInfo)
    {
        vkCmdWaitEvents2(this->m_commandBuffer, 1, &barrierEvent, &dependencyInfo);
        this->m_barrierEvents.push_back(barrierEvent);
    }

    void VulkanCommandBuffer::Submitted()
    {
        UniqueLock scope{this->m_commandBufferPool->m_lock};
        this->m_commandBufferState = VulkanCommandBufferState::Submitted;
        this->m_submittedTimestamp = Instant::Now();
    }

    VulkanCommandBuffer::VulkanCommandBuffer(VulkanCommandBufferPool& commandBufferPool)
        : m_commandBufferPool{&commandBufferPool}
    {
        this->AllocateResources();
    }

    VulkanCommandBuffer::~VulkanCommandBuffer()
    {
        if (this->m_commandBufferState != VulkanCommandBufferState::Disposed)
        {
            this->ReleaseResources();
        }
    }
}
