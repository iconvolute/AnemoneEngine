#include "AnemoneRuntime.RenderVulkan/VulkanQueue.hxx"
#include "AnemoneRuntime.RenderVulkan/VulkanDevice.hxx"
#include "AnemoneRuntime.RenderVulkan/VulkanCommandList.hxx"

#include <memory>

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
        size_t const waitSemaphoresCount = task.m_waitSemaphores.size();
        size_t const signalSemaphoresCount = task.m_signalSemaphores.size();
        size_t const totalSemaphoresCount = waitSemaphoresCount + signalSemaphoresCount + 1uz; // +1 for timeline semaphore
        size_t const totalCommandBuffersCount = task.m_commandBuffers.size();
        
        std::unique_ptr<VkSemaphoreSubmitInfo[]> semaphores = std::make_unique_for_overwrite<VkSemaphoreSubmitInfo[]>(totalSemaphoresCount);

        VkSemaphoreSubmitInfo* semaphoreSubmitInfos = semaphores.get();

        VkSemaphoreSubmitInfo* waitSemaphores = semaphoreSubmitInfos;
        VkSemaphoreSubmitInfo* signalSemaphores = semaphoreSubmitInfos + waitSemaphoresCount;

        for (size_t i = 0; i < waitSemaphoresCount; ++i)
        {
            VulkanCommandListTask::WaitSemaphoreInfo const& waitInfo = task.m_waitSemaphores[i];

            (*semaphoreSubmitInfos++) = VkSemaphoreSubmitInfo{
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                .pNext = nullptr,
                .semaphore = waitInfo.semaphore->GetHandle(),
                .value = 0,
                .stageMask = waitInfo.stageMask,
                .deviceIndex = 0,
            };
        }

        for (size_t i = 0; i < signalSemaphoresCount; ++i)
        {
            VulkanCommandListTask::SignalSemaphoreInfo const& signalInfo = task.m_signalSemaphores[i];

            (*semaphoreSubmitInfos++) = VkSemaphoreSubmitInfo{
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                .pNext = nullptr,
                .semaphore = signalInfo.semaphore->GetHandle(),
                .value = 0,
                .stageMask = VK_PIPELINE_STAGE_2_NONE,
                .deviceIndex = 0,
            };
        }

        (*semaphoreSubmitInfos) = VkSemaphoreSubmitInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .pNext = nullptr,
            .semaphore = this->m_timelineSemaphore.Get()->GetHandle(),
            .value = this->m_timelineNextValue,
            .stageMask = VK_PIPELINE_STAGE_2_NONE,
            .deviceIndex = 0,
        };

        std::unique_ptr<VkCommandBufferSubmitInfo[]> commandBuffers = std::make_unique_for_overwrite<VkCommandBufferSubmitInfo[]>(task.m_commandBuffers.size());

        for (size_t i = 0; i < totalCommandBuffersCount; ++i)
        {
            VulkanCommandBuffer* commandBufferInfo = task.m_commandBuffers[i].commandBuffer;

            commandBuffers[i] = VkCommandBufferSubmitInfo{
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
                .pNext = nullptr,
                .commandBuffer = commandBufferInfo->m_commandBuffer,
                .deviceMask = 0,
            };

            commandBufferInfo->Submitted();
        }

        // TODO: Submission should happen on single thread. Move that code to receive end of the queue.
        // m_timelineNextValue is not thread safe currently.
        task.m_timelineSemaphoreValue = this->m_timelineNextValue;

        VkSubmitInfo2 submitInfo2{
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .pNext = nullptr,
            .flags = 0,
            .waitSemaphoreInfoCount = static_cast<uint32_t>(waitSemaphoresCount),
            .pWaitSemaphoreInfos = waitSemaphores,
            .commandBufferInfoCount = static_cast<uint32_t>(totalCommandBuffersCount),
            .pCommandBufferInfos = commandBuffers.get(),
            .signalSemaphoreInfoCount = static_cast<uint32_t>(signalSemaphoresCount + 1uz),
            .pSignalSemaphoreInfos = signalSemaphores,
        };

        this->Submit(submitInfo2, nullptr);

        ++this->m_timelineNextValue;

        //this->m_submittedTasks.PushBack(&task);
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

    void VulkanQueue::FlushSubmittedTasks()
    {
        UniqueLock scope{this->m_commandBufferPoolLock};

        //this->m_submittedTasks.ForEach([&](VulkanCommandListTask& task)
        //{
        //    task.m_commandBufferSubmitInfo
        //});
    }
}
