#pragma once
#include "AnemoneRuntime.Render/Device.hxx"
#include "AnemoneRuntime.RenderVulkan/VulkanHeaders.hxx"
#include "AnemoneRuntime.RenderVulkan/VulkanSemaphore.hxx"
#include "AnemoneRuntime.Threading/CriticalSection.hxx"
#include "AnemoneRuntime.RenderVulkan/VulkanCommandBuffer.hxx"

#include <span>

namespace Anemone
{
    class VulkanDevice;
    class VulkanCommandBuffer;
    class VulkanCommandListTask;

    enum class VulkanQueueType
    {
        Graphics,
        Compute,
        Transfer,
    };

    class VulkanQueue : public ReferenceCounted<VulkanQueue>
    {
    public:
        VulkanQueue(
            VulkanDevice& device,
            VulkanQueueType queueType,
            uint32_t familyIndex,
            VkQueueFlags queueFlags);

        ~VulkanQueue();

    public:
        VulkanDevice* m_device{};
        VulkanQueueType m_queueType{};
        VkQueue m_queue{};
        uint32_t m_familyIndex{};
        VkAccessFlags m_accessFlags{};
        VkPipelineStageFlags m_pipelineStageFlags{};

        CriticalSection m_commandBufferPoolLock{};
        IntrusiveList<VulkanCommandBufferPool> m_commandBufferPools[2];
        IntrusiveList<VulkanCommandListTask> m_submittedTasks{};
        Reference<VulkanSemaphore> m_timelineSemaphore{};
        uint64_t m_timelineNextValue{1};

    public:
        VulkanCommandBufferPool* AcquireCommandBufferPool(
            VulkanCommandBufferType commandBufferType);

        void ReleaseCommandBufferPool(
            VulkanCommandBufferPool* commandBufferPool);

    public:
        void Submit(VulkanCommandListTask& task);
        void Submit(std::span<VkSubmitInfo2 const> submitInfo, VulkanFence* fence);
        void Submit(VkSubmitInfo2 const& submitInfo, VulkanFence* fence);
        void FlushSubmittedTasks();
    };
}
