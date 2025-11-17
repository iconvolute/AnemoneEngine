#pragma once
#include "AnemoneRenderVulkan/VulkanDevice.hxx"
#include "AnemoneRenderVulkan/VulkanHeaders.hxx"
#include "AnemoneRenderVulkan/VulkanCpuAllocator.hxx"
#include "AnemoneRenderVulkan/VulkanError.hxx"
#include "AnemoneThreading/CriticalSection.hxx"
#include "AnemoneBase/Intrusive.hxx"
#include "AnemoneBase/Reference.hxx"
#include "AnemoneBase/Instant.hxx"

namespace Anemone
{
    enum class VulkanCommandBufferType : uint8_t
    {
        Primary,
        Secondary,
    };

    enum class VulkanCommandBufferState : uint8_t
    {
        Disposed, // Resources are disposed
        Ready, // Ready to be recorded
        Recording, // Currently being recorded
        RecordingRenderPass, // Currently being recorded inside a render pass
        Finished, // Finished recording, ready to be submitted
        Submitted, // Submitted to the queue, waiting for execution
        PendingReset, // Waiting to be reset
    };

    class VulkanCommandBuffer;
    class VulkanBuffer;
    class VulkanQueue;
    class VulkanCommandListContext;

    class VulkanCommandBufferPool
        : private IntrusiveListNode<VulkanCommandBufferPool>
    {
        friend struct IntrusiveList<VulkanCommandBufferPool>;
        friend class VulkanCommandBuffer;
        friend class VulkanQueue;

    private:
        VulkanDevice* m_device{};
        VkCommandPool m_commandPool{};
        VulkanCommandBufferType m_commandBufferType{};

        CriticalSection m_lock{};

        IntrusiveList<VulkanCommandBuffer> m_acquiredCommandBufferList{};
        IntrusiveList<VulkanCommandBuffer> m_releasedCommandBufferList{};

    public:
        VulkanCommandBufferPool(VulkanDevice* device, uint32_t queueFamilyIndex, VulkanCommandBufferType commandBufferType);
        ~VulkanCommandBufferPool();

        VulkanCommandBuffer* AllocateCommandBuffer();
        void ReleaseCommandBuffer(VulkanCommandBuffer* commandBuffer);

        void ReleaseUnused(bool trim);
    };

    class VulkanCommandBuffer
        : private IntrusiveListNode<VulkanCommandBuffer>
    {
        friend struct IntrusiveList<VulkanCommandBuffer>;
        friend class VulkanCommandBufferPool;
        friend class VulkanCommandListContext;

    private:
        VulkanCommandBufferPool* m_commandBufferPool{};
        VkCommandBuffer m_commandBuffer{};
        CriticalSection m_lock{};
        VulkanCommandBufferState m_commandBufferState{VulkanCommandBufferState::Ready};
        std::vector<VkEvent> m_barrierEvents{};
        Instant m_submittedTimestamp{};

    public:
        constexpr VkCommandBuffer GetHandle() const
        {
            return this->m_commandBuffer;
        }

        constexpr VulkanCommandBufferType GetCommandBufferType() const
        {
            return this->m_commandBufferPool->m_commandBufferType;
        }

        void Reset();

        void SetMemoryBarrier(
            VkPipelineStageFlags2 sourceStages,
            VkPipelineStageFlags2 destinationStages)
        {
            VkMemoryBarrier2 memoryBarrier2{
                .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
                .pNext = nullptr,
                .srcStageMask = sourceStages,
                .srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
                .dstStageMask = destinationStages,
                .dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT,
            };
            VkDependencyInfo dependencyInfo{
                .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                .pNext = nullptr,
                .dependencyFlags = {},
                .memoryBarrierCount = 1,
                .pMemoryBarriers = &memoryBarrier2,
                .bufferMemoryBarrierCount = 0,
                .pBufferMemoryBarriers = nullptr,
                .imageMemoryBarrierCount = 0,
                .pImageMemoryBarriers = nullptr,
            };

            vkCmdPipelineBarrier2(this->m_commandBuffer, &dependencyInfo);
        }

        void SetImageMemoryBarrier(
            VkImage image,
            VkImageAspectFlags aspectFlags,
            VkImageLayout oldLayout,
            VkImageLayout newLayout,
            uint32_t baseMipLevel = 0,
            uint32_t mipLevelCount = VK_REMAINING_MIP_LEVELS,
            uint32_t baseArrayLayer = 0,
            uint32_t arrayLayerCount = VK_REMAINING_ARRAY_LAYERS)
        {
            constexpr VkPipelineStageFlags2 depthStageFlags =
                VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
                VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;

            constexpr VkPipelineStageFlags2 sampledStageFlags =
                VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT |
                VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT |
                VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;

            VkPipelineStageFlags2 srcStageFlags = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
            VkPipelineStageFlags2 dstStageFlags = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;

            VkAccessFlags2 srcAccessFlags = 0;
            VkAccessFlags2 dstAccessFlags = 0;

            switch (oldLayout) // NOLINT(clang-diagnostic-switch-enum)
            {
            case VK_IMAGE_LAYOUT_UNDEFINED:
                break;

            case VK_IMAGE_LAYOUT_GENERAL:
                srcStageFlags = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
                srcAccessFlags = VK_ACCESS_2_MEMORY_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
                srcStageFlags = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
                srcAccessFlags = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                srcStageFlags = depthStageFlags;
                srcAccessFlags = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
                srcStageFlags = depthStageFlags | sampledStageFlags;
                break;

            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
                srcStageFlags = sampledStageFlags;
                break;

            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
                srcStageFlags = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
                break;

            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                srcStageFlags = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
                srcAccessFlags = VK_ACCESS_TRANSFER_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_PREINITIALIZED:
                srcStageFlags = VK_PIPELINE_STAGE_2_HOST_BIT;
                srcAccessFlags = VK_ACCESS_2_HOST_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
                break;

            default:
                AE_PANIC("Image layout not supported.");
                break;
            }

            switch (newLayout) // NOLINT(clang-diagnostic-switch-enum)
            {
            case VK_IMAGE_LAYOUT_UNDEFINED:
                AE_PANIC("Cannot transition to VK_IMAGE_LAYOUT_UNDEFINED.");
                break;

            case VK_IMAGE_LAYOUT_GENERAL:
                dstStageFlags = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
                dstAccessFlags = VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
                dstStageFlags = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
                dstAccessFlags = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                dstStageFlags = depthStageFlags;
                dstAccessFlags = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
                dstStageFlags = depthStageFlags | sampledStageFlags;
                dstAccessFlags = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT;
                break;

            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
                dstStageFlags = sampledStageFlags;
                dstAccessFlags = VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT;
                break;

            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
                dstStageFlags = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
                dstAccessFlags = VK_ACCESS_2_TRANSFER_READ_BIT;
                break;

            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                dstStageFlags = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
                dstAccessFlags = VK_ACCESS_2_TRANSFER_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_PREINITIALIZED:
                AE_PANIC("Cannot transition to VK_IMAGE_LAYOUT_PREINITIALIZED.");
                break;

            case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
                break;

            default:
                AE_PANIC("Image layout not supported.");
            }

            VkImageMemoryBarrier2 imageMemoryBarrier2{
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                .pNext = nullptr,
                .srcStageMask = srcStageFlags,
                .srcAccessMask = srcAccessFlags,
                .dstStageMask = dstStageFlags,
                .dstAccessMask = dstAccessFlags,
                .oldLayout = oldLayout,
                .newLayout = newLayout,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = image,
                .subresourceRange = VkImageSubresourceRange{
                    .aspectMask = aspectFlags,
                    .baseMipLevel = baseMipLevel,
                    .levelCount = mipLevelCount,
                    .baseArrayLayer = baseArrayLayer,
                    .layerCount = arrayLayerCount,
                },
            };

            VkDependencyInfo dependencyInfo{
                .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                .pNext = nullptr,
                .memoryBarrierCount = 0,
                .pMemoryBarriers = nullptr,
                .bufferMemoryBarrierCount = 0,
                .pBufferMemoryBarriers = nullptr,
                .imageMemoryBarrierCount = 1,
                .pImageMemoryBarriers = &imageMemoryBarrier2,
            };

            vkCmdPipelineBarrier2(this->m_commandBuffer, &dependencyInfo);
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

        void BeginRecording();

        void EndRecording();

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

        void UploadBuffer(VulkanBuffer& buffer, void const* data, size_t size);

        void BeginSplitBarrier(VkEvent barrierEvent, VkDependencyInfo const& dependencyInfo);
        void EndSplitBarrier(VkEvent barrierEvent, VkDependencyInfo const& dependencyInfo);

    public:
        static VkCommandBufferLevel ToCommandBufferLevel(VulkanCommandBufferType type)
        {
            if (type == VulkanCommandBufferType::Primary)
            {
                return VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            }

            return VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        }

        VulkanCommandBuffer(VulkanCommandBufferPool& commandBufferPool)
            : m_commandBufferPool{&commandBufferPool}
        {
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
        }

        ~VulkanCommandBuffer()
        {        
            vkFreeCommandBuffers(
                this->m_commandBufferPool->m_device->m_device,
                this->m_commandBufferPool->m_commandPool,
                1, &this->m_commandBuffer);
        }
    };
}
