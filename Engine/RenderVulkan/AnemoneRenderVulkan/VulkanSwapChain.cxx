#include "AnemoneRenderVulkan/VulkanSwapChain.hxx"
#include "AnemoneRenderVulkan/VulkanQueue.hxx"
#include "AnemoneRenderVulkan/VulkanCpuAllocator.hxx"
#include "AnemoneRenderVulkan/VulkanDevice.hxx"
#include "AnemoneRenderVulkan/VulkanError.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneApplication/Platform/Windows/WindowsHostWindow.hxx"
#endif

namespace Anemone
{
    VulkanSwapChain::VulkanSwapChain(
        VulkanDevice* device,
        Reference<HostWindow> hostWindow)
        : _device{device}
        , _hostWindow{std::move(hostWindow)}
    {
        this->CreateSwapChain();
    }

    VulkanSwapChain::~VulkanSwapChain()
    {
        this->DestroySwapChain();
    }

    void VulkanSwapChain::Start()
    {
        VkFence currentFence = this->m_commandBuffers[this->currentFrameIndex]->GetFence();

        while (vkWaitForFences(
                   this->_device->_logicalDevice,
                   1,
                   &currentFence,
                   VK_TRUE,
                   UINT64_MAX) == VK_TIMEOUT)
        {
            // busy wait
        }

        VkResult result = vkAcquireNextImageKHR(
            this->_device->_logicalDevice,
            this->_swapChainHandle,
            UINT64_MAX,
            this->_imageAvailableSemaphores[this->currentSemaphoreIndex],
            VK_NULL_HANDLE,
            &this->currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            this->DestroySwapChain();
            this->CreateSwapChain();
        }
        else if ((result != VK_SUCCESS) && (result != VK_SUBOPTIMAL_KHR))
        {
            AE_PANIC("Failed to acquire swap chain image");
        }

        this->m_commandBuffers[this->currentFrameIndex]->Reset();
        this->m_commandBuffers[this->currentFrameIndex]->BeginRecording();

        // AE_VK_CALL(vkResetFences(
        //     this->_device->_logicalDevice,
        //     1,
        //     &currentFence));
        //
        // AE_VK_CALL(vkResetCommandBuffer(currentCommandBuffer, 0));

        /*VkCommandBufferBeginInfo commandBufferBeginInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pInheritanceInfo = nullptr,
        };
        AE_VK_CALL(vkBeginCommandBuffer(currentCommandBuffer, &commandBufferBeginInfo));*/

        this->m_commandBuffers[this->currentFrameIndex]->ImageTransition(
            this->_swapChainImages[this->currentImageIndex],
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            {},
            VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_IMAGE_ASPECT_COLOR_BIT);


        // THIS IS RENDERING NOW
        VkRenderingAttachmentInfo colorAttachmentInfo{
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .pNext = nullptr,
            .imageView = this->_swapChainImageViews[this->currentImageIndex],
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
                .extent = this->_swapChainExtent,
            },
            .layerCount = 1,
            .viewMask = {},
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachmentInfo,
            .pDepthAttachment = nullptr,
            .pStencilAttachment = nullptr,
        };

        this->m_commandBuffers[this->currentFrameIndex]->BeginRendering(renderingInfo);
    }

    void VulkanSwapChain::Present()
    {
        VkCommandBuffer currentCommandBuffer = this->m_commandBuffers[this->currentFrameIndex]->GetHandle();
        VkFence currentFence = this->m_commandBuffers[this->currentFrameIndex]->GetFence();

        this->m_commandBuffers[this->currentFrameIndex]->EndRendering();
        // vkCmdEndRendering(currentCommandBuffer);

        this->m_commandBuffers[this->currentFrameIndex]->ImageTransition(
            this->_swapChainImages[this->currentImageIndex],
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            {},
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
            VK_IMAGE_ASPECT_COLOR_BIT);

        this->m_commandBuffers[this->currentFrameIndex]->EndRecording();
        // AE_VK_CALL(vkEndCommandBuffer(currentCommandBuffer));

        VkPipelineStageFlags waitDestinationStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        VkSubmitInfo submitInfo{
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &this->_imageAvailableSemaphores[this->currentSemaphoreIndex],
            .pWaitDstStageMask = &waitDestinationStageMask,
            .commandBufferCount = 1,
            .pCommandBuffers = &currentCommandBuffer,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &this->_renderFinishedSemaphores[this->currentImageIndex],
        };

        AE_VK_CALL(vkQueueSubmit(
            this->_device->_graphicsQueue->_queue,
            1,
            &submitInfo,
            currentFence));

        VkPresentInfoKHR presentInfo{
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &this->_renderFinishedSemaphores[this->currentImageIndex],
            .swapchainCount = 1,
            .pSwapchains = &this->_swapChainHandle,
            .pImageIndices = &this->currentImageIndex,
            .pResults = nullptr,
        };

        VkResult result = vkQueuePresentKHR(
            this->_presentQueue->_queue,
            &presentInfo);

        this->currentSemaphoreIndex = (this->currentSemaphoreIndex + 1) % this->_imageAvailableSemaphores.size();
        this->currentFrameIndex = (this->currentFrameIndex + 1) % MaxFramesInFlight;

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            this->DestroySwapChain();
            this->CreateSwapChain();
        }
    }

    void VulkanSwapChain::CreateSwapChain()
    {
#if ANEMONE_PLATFORM_WINDOWS
        WindowsHostWindow& nativeWindow = static_cast<WindowsHostWindow&>(*this->_hostWindow); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)

        if (!nativeWindow.GetHandle())
        {
            // Window was destroyed.
            return;
        }

        VkWin32SurfaceCreateInfoKHR win32SurfaceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .hinstance = GetModuleHandleW(nullptr),
            .hwnd = nativeWindow.GetHandle(),
        };

        AE_VK_CALL(vkCreateWin32SurfaceKHR(this->_device->_instance, &win32SurfaceCreateInfo, &VulkanCpuAllocator, &this->_surface));

#elif ANEMONE_PLATFORM_LINUX
        // nothing right now.
#else
#error "Not implemented"
#endif

        std::vector<VkSurfaceFormatKHR> surfaceFormats{};
        AE_VK_CALL(VulkanGraphics::EnumeratePhysicalDeviceSurfaceFormats(
            this->_device->_physicalDevice,
            this->_surface,
            surfaceFormats));

        std::vector<VkPresentModeKHR> presentModes{};
        AE_VK_CALL(VulkanGraphics::EnumerateSurfacePresentModes(
            this->_device->_physicalDevice,
            this->_surface,
            presentModes));

        VkBool32 presentSupport = VK_FALSE;
        AE_VK_CALL(vkGetPhysicalDeviceSurfaceSupportKHR(
            this->_device->_physicalDevice,
            this->_device->_graphicsQueue->_familyIndex,
            this->_surface,
            &presentSupport));

        if (presentSupport != VK_FALSE)
        {
            this->_presentQueue = this->_device->_graphicsQueue;
        }
        else
        {
            AE_PANIC("Default Graphics Queue doesn't support presenting to the surface");
        }

        VkSurfaceCapabilitiesKHR surfaceCapabilities{};
        AE_VK_CALL(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            this->_device->_physicalDevice,
            this->_surface,
            &surfaceCapabilities));

        this->_swapChainImageFormat = ChooseSwapChainFormat(surfaceFormats);

        this->_swapChainExtent = ChooseSwapExtent(surfaceCapabilities);

        uint32_t minImageCount = std::clamp<uint32_t>(3u, surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount);

        VkSwapchainCreateInfoKHR swapChainCreateInfo{
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .surface = this->_surface,
            .minImageCount = minImageCount,
            .imageFormat = this->_swapChainImageFormat.format,
            .imageColorSpace = this->_swapChainImageFormat.colorSpace,
            .imageExtent = this->_swapChainExtent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .preTransform = surfaceCapabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = ChoosePresentMode(presentModes),
            .clipped = true,
            .oldSwapchain = nullptr,
        };

#if defined(VK_EXT_full_screen_exclusive)
        VkSurfaceFullScreenExclusiveInfoEXT surfaceFullScreenExclusiveInfo;
        if (this->_device->m_physicalDeviceExtensions.FullscreenExclusive)
        {
            surfaceFullScreenExclusiveInfo.sType = VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_INFO_EXT;
            surfaceFullScreenExclusiveInfo.pNext = const_cast<void*>(swapChainCreateInfo.pNext);
            surfaceFullScreenExclusiveInfo.fullScreenExclusive = this->_fullscreen
                ? VK_FULL_SCREEN_EXCLUSIVE_ALLOWED_EXT
                : VK_FULL_SCREEN_EXCLUSIVE_DISALLOWED_EXT;
            swapChainCreateInfo.pNext = &surfaceFullScreenExclusiveInfo;
        }
#endif

        VkResult result = vkCreateSwapchainKHR(
            this->_device->_logicalDevice,
            &swapChainCreateInfo,
            &VulkanCpuAllocator,
            &this->_swapChainHandle);

        if ((this->_device->m_physicalDeviceExtensions.FullscreenExclusive) and (result == VK_ERROR_INITIALIZATION_FAILED))
        {
            // Failed to create swap chain with full screen info, try without it
            swapChainCreateInfo.pNext = nullptr;

            result = vkCreateSwapchainKHR(
                this->_device->_logicalDevice,
                &swapChainCreateInfo,
                &VulkanCpuAllocator,
                &this->_swapChainHandle);
        }

        AE_VK_CALL(result);

        uint32_t imagesCount = 0;
        AE_VK_CALL(vkGetSwapchainImagesKHR(this->_device->_logicalDevice, this->_swapChainHandle, &imagesCount, nullptr));

        this->_swapChainImages.resize(imagesCount);
        AE_VK_CALL(vkGetSwapchainImagesKHR(this->_device->_logicalDevice, this->_swapChainHandle, &imagesCount, this->_swapChainImages.data()));

        this->_swapChainImageViews.resize(imagesCount);

        for (uint32_t i = 0; i < imagesCount; ++i)
        {
            VkImageViewCreateInfo imageViewCreateInfo{
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .image = this->_swapChainImages[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = this->_swapChainImageFormat.format,
                .components = VkComponentMapping{
                    .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .a = VK_COMPONENT_SWIZZLE_IDENTITY,
                },
                .subresourceRange = VkImageSubresourceRange{
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
            };

            AE_VK_CALL(vkCreateImageView(
                this->_device->_logicalDevice,
                &imageViewCreateInfo,
                &VulkanCpuAllocator,
                &this->_swapChainImageViews[i]));
        }

        VkSemaphoreCreateInfo const semaphoreCreateInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
        };

        this->_imageAvailableSemaphores.resize(imagesCount);
        this->_renderFinishedSemaphores.resize(imagesCount);

        for (uint32_t i = 0; i < imagesCount; ++i)
        {
            AE_VK_CALL(vkCreateSemaphore(
                this->_device->_logicalDevice,
                &semaphoreCreateInfo,
                &VulkanCpuAllocator,
                &this->_imageAvailableSemaphores[i]));

            AE_VK_CALL(vkCreateSemaphore(
                this->_device->_logicalDevice,
                &semaphoreCreateInfo,
                &VulkanCpuAllocator,
                &this->_renderFinishedSemaphores[i]));
        }

        this->m_commandPool = MakeReference<VulkanCommandPool>(
            this->_device,
            this->_device->_graphicsQueue->_familyIndex);

        for (size_t i = 0; i < MaxFramesInFlight; ++i)
        {
            this->m_commandBuffers[i] = this->m_commandPool->CreateCommandBuffer();
        }
    }

    void VulkanSwapChain::DestroySwapChain()
    {
        // Before releasing resources, wait for all semaphores and fences to be signaled.
        AE_VK_CALL(vkDeviceWaitIdle(this->_device->_logicalDevice));

        for (VkSemaphore const semaphore : this->_imageAvailableSemaphores)
        {
            vkDestroySemaphore(this->_device->_logicalDevice, semaphore, &VulkanCpuAllocator);
        }

        this->_imageAvailableSemaphores.clear();

        for (VkSemaphore const semaphore : this->_renderFinishedSemaphores)
        {
            vkDestroySemaphore(this->_device->_logicalDevice, semaphore, &VulkanCpuAllocator);
        }

        this->_renderFinishedSemaphores.clear();

        for (VkImageView const imageView : this->_swapChainImageViews)
        {
            vkDestroyImageView(this->_device->_logicalDevice, imageView, &VulkanCpuAllocator);
        }

        this->_swapChainImageViews.clear();

        this->_swapChainImages.clear();

        for (size_t i = 0; i < MaxFramesInFlight; ++i)
        {
            this->m_commandBuffers[i] = {};
        }

        this->m_commandPool = {};

        if (this->_swapChainHandle)
        {
            vkDestroySwapchainKHR(this->_device->_logicalDevice, this->_swapChainHandle, &VulkanCpuAllocator);
            this->_swapChainHandle = {};
        }

        if (this->_surface)
        {
            vkDestroySurfaceKHR(this->_device->_instance, this->_surface, &VulkanCpuAllocator);
            this->_surface = {};
        }
    }

    VkExtent2D VulkanSwapChain::ChooseSwapExtent(VkSurfaceCapabilitiesKHR const& capabilities) const
    {
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            return capabilities.currentExtent;
        }

        RectF const clientBounds = this->_hostWindow->GetClientBounds();

        VkExtent2D actualExtent{
            .width = static_cast<uint32_t>(clientBounds.Width),
            .height = static_cast<uint32_t>(clientBounds.Height),
        };

        actualExtent.width = std::clamp(
            actualExtent.width,
            capabilities.minImageExtent.width,
            capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(
            actualExtent.height,
            capabilities.minImageExtent.height,
            capabilities.maxImageExtent.height);
        return actualExtent;
    }


    VkSurfaceFormatKHR VulkanSwapChain::ChooseSwapChainFormat(
        std::span<VkSurfaceFormatKHR const> availableFormats)
    {
        for (VkSurfaceFormatKHR const& format : availableFormats)
        {
            if ((format.format == VK_FORMAT_B8G8R8A8_SRGB) and
                (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR))
            {
                return format;
            }
        }

        // Fallback to the first available format.
        return availableFormats.front();
    }

    VkPresentModeKHR VulkanSwapChain::ChoosePresentMode(
        std::span<VkPresentModeKHR const> availablePresentModes)
    {
        for (VkPresentModeKHR presentMode : availablePresentModes)
        {
            if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return presentMode;
            }
        }

        // FIFO is guaranteed to be available.
        return VK_PRESENT_MODE_FIFO_KHR;
    }
}
