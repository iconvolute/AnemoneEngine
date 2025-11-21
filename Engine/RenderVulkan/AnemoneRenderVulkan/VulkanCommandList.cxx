#include "AnemoneRenderVulkan/VulkanCommandList.hxx"
#include "AnemoneRenderVulkan/VulkanQueue.hxx"
#include "AnemoneRenderVulkan/VulkanViewport.hxx"
#include "AnemoneRenderVulkan/VulkanDevice.hxx"
#include "AnemoneDiagnostics/Debug.hxx"

namespace Anemone
{
    VulkanCommandListContext::VulkanCommandListContext(
        VulkanDevice& device,
        VulkanQueue& queue,
        GpuPipeline pipeline,
        VulkanCommandListContext* immediate)
        : m_device{&device}
        , m_queue{&queue}
        , m_pipeline{pipeline}
        , m_immediate{immediate}
        , m_commandBufferPool{queue.AcquireCommandBufferPool(VulkanCommandBufferType::Primary)}
    {
    }

    VulkanCommandListContext::~VulkanCommandListContext()
    {
        AE_ASSERT(this->m_commandBuffer == nullptr);
        AE_ASSERT(this->m_currentViewport == nullptr);
        this->m_queue->ReleaseCommandBufferPool(this->m_commandBufferPool);
    }

    void VulkanCommandListContext::BeginFrame()
    {
        
    }

    void VulkanCommandListContext::EndFrame()
    {
    }

    void VulkanCommandListContext::BeginDrawingViewport(GpuViewport& viewport)
    {
        VulkanViewport& vulkanViewport = static_cast<VulkanViewport&>(viewport);
        AE_ASSERT(this->m_currentViewport == nullptr);
        this->m_currentViewport = &vulkanViewport;

        VulkanSemaphore* semaphore{};
        uint32_t imageIndex{};
        vulkanViewport.Acquire(imageIndex, semaphore);
    }

    void VulkanCommandListContext::EndDrawingViewport(GpuViewport& viewport)
    {
        VulkanViewport& vulkanViewport = static_cast<VulkanViewport&>(viewport);
        AE_ASSERT(this->m_currentViewport == &vulkanViewport);
        
        this->SubmitCommandListTasks();
        this->m_queue->m_timelineSemaphore->WaitForTimelineValue(
            this->m_queue->m_timelineNextValue - 1,
            UINT64_MAX);

        // TODO: Move this to proper place
        while (VulkanCommandListTask* task = this->m_commandListTasks.PopFront())
        {
            task->Completed();
            delete task;
        }

        this->m_commandBufferPool->CollectUnusedCommandBuffers(false);

        vulkanViewport.Present();

        this->m_currentViewport = nullptr;
    }

    void VulkanCommandListContext::BeginRecording()
    {
        // AE_ASSERT(this->m_currentPhase == VulkanCommandListContextPhase::Wait);

        this->m_commandBuffer = this->m_commandBufferPool->AllocateCommandBuffer();
        this->m_commandBuffer->BeginRecording();

        this->AddWaitSemaphore(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, this->m_currentViewport->m_imageAcquiredSemaphore[this->m_currentViewport->m_currentSemaphoreIndex].Get());

        VulkanCommandListTask* task = this->GetCommandListTask(VulkanCommandListContextPhase::Wait);
        task->m_commandBuffers.emplace_back(this->m_commandBuffer);

        this->m_currentPhase = VulkanCommandListContextPhase::Execute;

        // FOR NOW ONLY - test if cleaning swapchain works.
        this->m_commandBuffer->SetImageMemoryBarrier(
            this->m_currentViewport->m_image[this->m_currentViewport->m_currentImageIndex],
            VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);


        VkRenderingAttachmentInfo colorAttachmentInfo{
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .pNext = nullptr,
            .imageView = this->m_currentViewport->m_imageView[this->m_currentViewport->m_currentImageIndex],
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .resolveImageView = VK_NULL_HANDLE,
            .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue = VkClearValue{
                .color = VkClearColorValue{
                    .float32 = {1.0f, 0.0f, 1.0f, 1.0f},
                },
            },
        };

        VkRenderingInfo renderingInfo{
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .pNext = nullptr,
            .flags = {},
            .renderArea = VkRect2D{
                .offset = VkOffset2D{0, 0},
                .extent = this->m_currentViewport->m_swapchainExtent,
            },
            .layerCount = 1,
            .viewMask = {},
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachmentInfo,
            .pDepthAttachment = nullptr,
            .pStencilAttachment = nullptr,
        };
        this->m_commandBuffer->BeginRendering(renderingInfo);
        this->m_commandBuffer->EndRendering();

        this->m_commandBuffer->SetImageMemoryBarrier(
            this->m_currentViewport->m_image[this->m_currentViewport->m_currentImageIndex],
            VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        this->AddSignalSemaphore(this->m_currentViewport->m_imageReleasedSemaphore[this->m_currentViewport->m_currentSemaphoreIndex].Get());
    }

    void VulkanCommandListContext::EndRecording()
    {
        AE_ASSERT(this->m_commandBuffer != nullptr);

        this->m_commandBuffer->EndRecording();


        this->m_commandBuffer = nullptr;
    }

    void VulkanCommandListContext::AddWaitSemaphore(VkPipelineStageFlags2 stageMask, VulkanSemaphore* semaphore)
    {
        VulkanCommandListTask* task = this->GetCommandListTask(VulkanCommandListContextPhase::Wait);
        AE_ASSERT(task->m_signalSemaphores.empty());
        AE_ASSERT(task->m_commandBuffers.empty());
        task->m_waitSemaphores.emplace_back(semaphore, stageMask);
    }

    void VulkanCommandListContext::AddSignalSemaphore(VulkanSemaphore* semaphore)
    {
        VulkanCommandListTask* task = this->GetCommandListTask(VulkanCommandListContextPhase::Signal);
        task->m_signalSemaphores.emplace_back(semaphore);
    }

    void VulkanCommandListContext::FinishCommandListTask()
    {
    }

    void VulkanCommandListContext::SubmitCommandListTasks()
    {
        this->m_commandListTasks.ForEach([&](VulkanCommandListTask& task)
        {
            this->m_queue->Submit(task);
        });
    }

    VulkanCommandListTask* VulkanCommandListContext::CreateCommandTaskList()
    {
        this->FinishCommandListTask();
        VulkanCommandListTask* task = new VulkanCommandListTask(*this->m_queue);
        this->m_commandListTasks.PushBack(task);
        this->m_currentPhase = VulkanCommandListContextPhase::Wait;
        return task;
    }

    VulkanCommandListTask* VulkanCommandListContext::GetCommandListTask(VulkanCommandListContextPhase phase)
    {
        VulkanCommandListTask* task;

        if (this->m_commandListTasks.IsEmpty() || (phase < this->m_currentPhase))
        {
            this->FinishCommandListTask();
            task = new VulkanCommandListTask(*this->m_queue);
            this->m_commandListTasks.PushBack(task);
        }
        else
        {
            task = this->m_commandListTasks.PeekBack();
        }

        this->m_currentPhase = phase;
        return task;
    }
}

namespace Anemone
{
    VulkanCommandListTask::VulkanCommandListTask(VulkanQueue& queue)
        : m_queue{&queue}
    {
    }

    VulkanCommandListTask::~VulkanCommandListTask()
    {
    }

    void VulkanCommandListTask::Completed()
    {
        // Reset command buffers.
        for (auto& commandBuffer : this->m_commandBuffers)
        {
            commandBuffer.commandBuffer->Reset();
        }
    }
}
