#pragma once
#include "AnemoneRender/Gpu.hxx"
#include "AnemoneRenderVulkan/VulkanHeaders.hxx"
#include "AnemoneRenderVulkan/VulkanError.hxx"

namespace Anemone
{
    class VulkanSemaphore;
    class VulkanViewport;
    class VulkanDevice;
    class VulkanQueue;
    class VulkanCommandBufferPool;
    class VulkanCommandBuffer;
    class VulkanCommandListTask;
    class VulkanFence;

    enum class VulkanCommandListContextPhase : uint8_t
    {
        Wait,
        Execute,
        Signal,
    };

    class VulkanCommandListContext : public GpuCommandListContext
    {
    private:
        VulkanDevice* m_device{};
        VulkanQueue* m_queue{};
        VulkanViewport* m_currentViewport{};
        GpuPipeline m_pipeline{};
        VulkanCommandListContext* m_immediate{};

        VulkanCommandBufferPool* m_commandBufferPool{};
        VulkanCommandBuffer* m_commandBuffer{};
        IntrusiveList<VulkanCommandListTask> m_commandListTasks{};
        VulkanCommandListContextPhase m_currentPhase{VulkanCommandListContextPhase::Wait};

    public:
        VulkanCommandListContext(
            VulkanDevice& device,
            VulkanQueue& queue,
            GpuPipeline pipeline,
            VulkanCommandListContext* immediate);

        ~VulkanCommandListContext() override;

        void BeginFrame() override;
        void EndFrame() override;

        void BeginDrawingViewport(GpuViewport& viewport) override;
        void EndDrawingViewport(GpuViewport& viewport) override;

        void BeginRecording() override;
        void EndRecording() override;

    public:
        void AddWaitSemaphore(VkPipelineStageFlags2 stageMask, VulkanSemaphore* semaphore);
        void AddSignalSemaphore(VulkanSemaphore* semaphore);

    private:
        void FinishCommandListTask();
        void SubmitCommandListTasks();
        VulkanCommandListTask* CreateCommandTaskList();
        VulkanCommandListTask* GetCommandListTask(VulkanCommandListContextPhase phase);
    };

    class VulkanCommandListTask final : private IntrusiveListNode<VulkanCommandListTask>
    {
        friend class VulkanQueue;
        friend class VulkanCommandListContext;
        friend struct IntrusiveList<VulkanCommandListTask>;

    private:
        struct SignalSemaphoreInfo
        {
            VulkanSemaphore* semaphore;
        };

        struct WaitSemaphoreInfo
        {
            VulkanSemaphore* semaphore;
            VkPipelineStageFlags2 stageMask;
        };

        struct CommandBufferInfo
        {
            VulkanCommandBuffer* commandBuffer;
        };

        VulkanQueue* m_queue{};
        std::vector<WaitSemaphoreInfo> m_waitSemaphores{};
        std::vector<SignalSemaphoreInfo> m_signalSemaphores{};
        std::vector<CommandBufferInfo> m_commandBuffers{};
        uint64_t m_timelineSemaphoreValue{};

    public:
        explicit VulkanCommandListTask(VulkanQueue& queue);

        ~VulkanCommandListTask();
        void Completed();
    };
}
