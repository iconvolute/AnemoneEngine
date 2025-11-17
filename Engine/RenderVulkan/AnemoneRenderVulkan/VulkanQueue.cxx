#include "AnemoneRenderVulkan/VulkanQueue.hxx"
#include "AnemoneRenderVulkan/VulkanDevice.hxx"
#include "AnemoneRenderVulkan/VulkanCommandList.hxx"

namespace Anemone
{
    VulkanQueue::VulkanQueue(
        VulkanDevice& device,
        VulkanQueueType queueType,
        uint32_t familyIndex,
        VkQueueFlags queueFlags)
        : m_device{&device}
        , m_queueType{queueType}
        , m_familyIndex{familyIndex}
    {
        vkGetDeviceQueue(
            this->m_device->m_device,
            this->m_familyIndex,
            0, // NOTE: Do we need to support multiple queues per family?
            &this->m_queue);

        this->m_pipelineStageFlags =
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT |
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT |
            VK_PIPELINE_STAGE_HOST_BIT |
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

        this->m_accessFlags =
            VK_ACCESS_HOST_READ_BIT |
            VK_ACCESS_HOST_WRITE_BIT |
            VK_ACCESS_MEMORY_READ_BIT |
            VK_ACCESS_MEMORY_WRITE_BIT;

        if (queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            this->m_pipelineStageFlags |=
                VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT |
                VK_PIPELINE_STAGE_VERTEX_INPUT_BIT |
                VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT |
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                VK_PIPELINE_STAGE_TRANSFER_BIT |
                VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;

            this->m_accessFlags |=
                VK_ACCESS_TRANSFER_READ_BIT |
                VK_ACCESS_TRANSFER_WRITE_BIT |
                VK_ACCESS_INDIRECT_COMMAND_READ_BIT |
                VK_ACCESS_UNIFORM_READ_BIT |
                VK_ACCESS_SHADER_READ_BIT |
                VK_ACCESS_SHADER_WRITE_BIT |
                VK_ACCESS_INDEX_READ_BIT |
                VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT |
                VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
                VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            if (this->m_device->m_deviceFeatures.GeometryShader)
            {
                this->m_pipelineStageFlags |=
                    VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
            }

            if (this->m_device->m_deviceFeatures.FragmentShadingRate)
            {
                this->m_pipelineStageFlags |=
                    VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

                this->m_accessFlags |=
                    VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR;
            }

            if (this->m_device->m_deviceFeatures.FragmentDensityMap)
            {
                this->m_pipelineStageFlags |=
                    VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT;

                this->m_accessFlags |=
                    VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT;
            }
            if (this->m_device->m_deviceFeatures.MeshShader)
            {
                this->m_pipelineStageFlags |=
                    VK_PIPELINE_STAGE_TASK_SHADER_BIT_EXT |
                    VK_PIPELINE_STAGE_MESH_SHADER_BIT_EXT;
            }
        }

        if (queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            this->m_pipelineStageFlags |=
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT |
                VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT |
                VK_PIPELINE_STAGE_TRANSFER_BIT;

            this->m_accessFlags |=
                VK_ACCESS_TRANSFER_READ_BIT |
                VK_ACCESS_TRANSFER_WRITE_BIT |
                VK_ACCESS_INDIRECT_COMMAND_READ_BIT |
                VK_ACCESS_UNIFORM_READ_BIT |
                VK_ACCESS_SHADER_READ_BIT |
                VK_ACCESS_SHADER_WRITE_BIT;

            if (this->m_device->m_deviceFeatures.AccelerationStructure)
            {
                this->m_pipelineStageFlags |=
                    VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;

                this->m_accessFlags |=
                    VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR |
                    VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
            }

            if (this->m_device->m_deviceFeatures.RayTracingPipeline)
            {
                this->m_pipelineStageFlags |=
                    VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
            }
        }

        if (queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            this->m_pipelineStageFlags |= VK_PIPELINE_STAGE_TRANSFER_BIT;
            this->m_accessFlags |= VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
        }

        this->m_timelineSemaphore = MakeReference<VulkanSemaphore>(device, 0u);
    }

    VulkanQueue::~VulkanQueue()
    {
        while (VulkanCommandBufferPool* current = this->m_commandBufferPools[0].PopFront())
        {
            delete current;
        }

        while (VulkanCommandBufferPool* current = this->m_commandBufferPools[1].PopFront())
        {
            delete current;
        }
    }

    VulkanCommandBufferPool* VulkanQueue::AcquireCommandBufferPool(VulkanCommandBufferType commandBufferType)
    {
        IntrusiveList<VulkanCommandBufferPool>& list = this->m_commandBufferPools[static_cast<size_t>(commandBufferType)];

        UniqueLock scope{this->m_commandBufferPoolLock};

        VulkanCommandBufferPool* result = list.PopFront();

        if (!result)
        {
            result = new VulkanCommandBufferPool(
                this->m_device,
                this->m_familyIndex,
                commandBufferType);
        }

        return result;
    }

    void VulkanQueue::ReleaseCommandBufferPool(VulkanCommandBufferPool* commandBufferPool)
    {
        IntrusiveList<VulkanCommandBufferPool>& list = this->m_commandBufferPools[static_cast<size_t>(commandBufferPool->m_commandBufferType)];

        UniqueLock scope{this->m_commandBufferPoolLock};
        list.PushBack(commandBufferPool);
    }

    void VulkanQueue::Submit(VulkanCommandListTask& task)
    {
        task.m_timelineSemaphoreValue = this->m_timelineNextValue;

        VkSemaphoreSubmitInfo& timelineSemaphoreSubmitInfo = task.m_signalSemaphoreInfos.emplace_back();
        timelineSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        timelineSemaphoreSubmitInfo.pNext = nullptr;
        timelineSemaphoreSubmitInfo.semaphore = this->m_timelineSemaphore.Get()->GetHandle();
        timelineSemaphoreSubmitInfo.value = this->m_timelineNextValue;
        timelineSemaphoreSubmitInfo.stageMask = VK_PIPELINE_STAGE_2_NONE;
        timelineSemaphoreSubmitInfo.deviceIndex = 0;

        VkSubmitInfo2 submitInfo2{
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .pNext = nullptr,
            .flags = 0,
            .waitSemaphoreInfoCount = static_cast<uint32_t>(task.m_waitSemaphoreInfos.size()),
            .pWaitSemaphoreInfos = task.m_waitSemaphoreInfos.data(),
            .commandBufferInfoCount = static_cast<uint32_t>(task.m_commandBufferSubmitInfo.size()),
            .pCommandBufferInfos = task.m_commandBufferSubmitInfo.data(),
            .signalSemaphoreInfoCount = static_cast<uint32_t>(task.m_signalSemaphoreInfos.size()),
            .pSignalSemaphoreInfos = task.m_signalSemaphoreInfos.data(),
        };

        this->Submit(submitInfo2, nullptr);

        ++this->m_timelineNextValue;
    }

    void VulkanQueue::Submit(std::span<VkSubmitInfo2 const> submitInfo, VulkanFence* fence)
    {
        VkFence fenceHandle = fence ? fence->GetHandle() : VK_NULL_HANDLE;
        AE_VULKAN_ENSURE(vkQueueSubmit2(this->m_queue, static_cast<uint32_t>(submitInfo.size()), submitInfo.data(), fenceHandle));
    }

    void VulkanQueue::Submit(VkSubmitInfo2 const& submitInfo, VulkanFence* fence)
    {
        VkFence fenceHandle = fence ? fence->GetHandle() : VK_NULL_HANDLE;
        AE_VULKAN_ENSURE(vkQueueSubmit2(this->m_queue, 1, &submitInfo, fenceHandle));
    }
}
