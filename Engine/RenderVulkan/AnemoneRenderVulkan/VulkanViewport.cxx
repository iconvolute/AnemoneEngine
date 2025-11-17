#include "AnemoneRenderVulkan/VulkanViewport.hxx"
#include "AnemoneRenderVulkan/VulkanQueue.hxx"
#include "AnemoneRenderVulkan/VulkanCommandList.hxx"
#include "AnemoneRenderVulkan/VulkanCpuAllocator.hxx"
#include "AnemoneRenderVulkan/VulkanDevice.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneApplication/Platform/Windows/WindowsHostWindow.hxx"
#endif

namespace Anemone
{
    VulkanViewport::VulkanViewport(VulkanDevice& device, Reference<HostWindow> window)
        : m_device{&device}
        , m_window{std::move(window)}
    {
        this->Create();
    }

    VulkanViewport::~VulkanViewport()
    {
        this->Destroy();
    }

    void VulkanViewport::Resize(uint32_t width, uint32_t height)
    {
        this->m_window->SetBounds(RectF{0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)});
    }

    VkResult VulkanViewport::Create()
    {
        if (vkDeviceWaitIdle(this->m_device->m_device) != VK_SUCCESS)
        {
            return VK_ERROR_UNKNOWN;
        }

#if ANEMONE_PLATFORM_WINDOWS
        //
        // Windows specific surface creation
        //

        WindowsHostWindow& nativeWindow = static_cast<WindowsHostWindow&>(*this->m_window); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)

        if (!nativeWindow.GetHandle())
        {
            return VK_ERROR_UNKNOWN;
        }

        VkWin32SurfaceCreateInfoKHR win32SurfaceCreateInfoKHR{
            .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = {},
            .hwnd = nativeWindow.GetHandle(),
        };

        if (vkCreateWin32SurfaceKHR(
                this->m_device->m_instance,
                &win32SurfaceCreateInfoKHR,
                VulkanCpuAllocator,
                &this->m_surface) != VK_SUCCESS)
        {
            return VK_ERROR_UNKNOWN;
        }
#endif


        //
        // Choose surface format and present mode
        //

        VkSurfaceFormatKHR const surfaceFormat = this->ChooseSurfaceFormat();
        VkPresentModeKHR const presentMode = this->ChoosePresentMode();


        //
        // Choose present queue.
        //

        VkBool32 presentSupport = VK_FALSE;
        AE_VULKAN_ENSURE(vkGetPhysicalDeviceSurfaceSupportKHR(
            this->m_device->m_physicalDevice,
            this->m_device->m_graphicsQueue->m_familyIndex,
            this->m_surface,
            &presentSupport));

        if (presentSupport != VK_FALSE)
        {
            this->m_presentQueue = this->m_device->m_graphicsQueue->m_queue;
        }
        else
        {
            AE_PANIC("Graphics queue doesn't support presenting to the surface");
        }


        //
        // Get surface capabilities
        //

        VkSurfaceCapabilitiesKHR surfaceCapabilitiesKHR{};
        AE_VULKAN_ENSURE(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            this->m_device->m_physicalDevice,
            this->m_surface,
            &surfaceCapabilitiesKHR));

        //
        // Choose images count.
        //

        this->m_imagesCount = std::clamp<uint32_t>(3, surfaceCapabilitiesKHR.minImageCount, surfaceCapabilitiesKHR.maxImageCount);
        AE_ASSERT(this->m_imagesCount <= MaxImagesCount);

        //
        // Create swap chain
        //

        this->m_swapchainExtent = this->ChooseExtents(surfaceCapabilitiesKHR);

        VkSwapchainCreateInfoKHR swapchainCreateInfoKHR{
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .surface = this->m_surface,
            .minImageCount = this->m_imagesCount,
            .imageFormat = surfaceFormat.format,
            .imageColorSpace = surfaceFormat.colorSpace,
            .imageExtent = this->m_swapchainExtent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .preTransform = surfaceCapabilitiesKHR.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = presentMode,
            .clipped = VK_TRUE,
            .oldSwapchain = VK_NULL_HANDLE,
        };

#if defined(VK_EXT_full_screen_exclusive)
        VkSurfaceFullScreenExclusiveInfoEXT surfaceFullScreenExclusiveInfoEXT{
            .sType = VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_INFO_EXT,
            .pNext = nullptr,
            .fullScreenExclusive = VK_FULL_SCREEN_EXCLUSIVE_ALLOWED_EXT,
        };

        if (this->m_device->m_physicalDeviceExtensions.EXT_full_screen_exclusive)
        {
            RHIVulkanPushStruct(swapchainCreateInfoKHR, surfaceFullScreenExclusiveInfoEXT);
        }
#endif

        AE_VULKAN_ENSURE(vkCreateSwapchainKHR(
            this->m_device->m_device,
            &swapchainCreateInfoKHR,
            VulkanCpuAllocator,
            &this->m_swapchain));


        //
        // Get swap chain images.
        //
        // TODO: Change this to use proper texture object with state tracking.
        //

        AE_VULKAN_ENSURE(vkGetSwapchainImagesKHR(
            this->m_device->m_device,
            this->m_swapchain,
            &this->m_imagesCount,
            nullptr));

        this->m_image.resize(this->m_imagesCount);
        this->m_imageView.resize(this->m_imagesCount);

        AE_VULKAN_ENSURE(vkGetSwapchainImagesKHR(
            this->m_device->m_device,
            this->m_swapchain,
            &this->m_imagesCount,
            this->m_image.data()));

        //
        // Create image views
        //

        for (size_t i = 0; i < this->m_image.size(); ++i)
        {
            VkImageViewCreateInfo imageViewCreateInfo{
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .image = this->m_image[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = surfaceFormat.format,
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

            AE_VULKAN_ENSURE(vkCreateImageView(
                this->m_device->m_device,
                &imageViewCreateInfo,
                VulkanCpuAllocator,
                &this->m_imageView[i]));
        }

        // Create synchronization primitives.
        this->m_imageAcquiredFence.resize(this->m_imagesCount);
        for (Reference<VulkanFence>& fence : this->m_imageAcquiredFence)
        {
            fence = MakeReference<VulkanFence>(*this->m_device);
        }

        this->m_imageAcquiredSemaphore.resize(this->m_imagesCount);
        for (Reference<VulkanSemaphore>& semaphore : this->m_imageAcquiredSemaphore)
        {
            semaphore = MakeReference<VulkanSemaphore>(*this->m_device);
        }

        this->m_imageReleasedSemaphore.resize(this->m_imagesCount);
        for (Reference<VulkanSemaphore>& semaphore : this->m_imageReleasedSemaphore)
        {
            semaphore = MakeReference<VulkanSemaphore>(*this->m_device);
        }

        return VK_SUCCESS;
    }

    VkResult VulkanViewport::Destroy()
    {
        //
        // Wait for device to be idle before destroying resources.
        //

        AE_VULKAN_ENSURE(vkDeviceWaitIdle(this->m_device->m_device));

        //
        // Destroy swap chain.
        //

        vkDestroySwapchainKHR(
            this->m_device->m_device,
            this->m_swapchain,
            VulkanCpuAllocator);

        this->m_swapchain = VK_NULL_HANDLE;

        //
        // Destroy surface.
        //

        vkDestroySurfaceKHR(
            this->m_device->m_instance,
            this->m_surface,
            VulkanCpuAllocator);

        this->m_surface = VK_NULL_HANDLE;

        //
        // Release synchronization primitives
        //

        this->m_imageAcquiredFence.clear();
        this->m_imageAcquiredSemaphore.clear();
        this->m_imageReleasedSemaphore.clear();

        //
        // Release images and image views
        //

        for (VkImageView const imageView : this->m_imageView)
        {
            (void)imageView;
            vkDestroyImageView(
                this->m_device->m_device,
                imageView,
                VulkanCpuAllocator);
        }
        this->m_imageView.clear();
        this->m_image.clear();

        return VK_SUCCESS;
    }

    VkResult VulkanViewport::Acquire(uint32_t& outImageIndex, VulkanSemaphore*& outSemaphore)
    {
        AE_ASSERT(this->m_currentImageIndex == UINT32_MAX);

        uint32_t imageIndex = UINT32_MAX;
        uint32_t previousSemaphoreIndex = this->m_currentSemaphoreIndex;
        this->m_currentSemaphoreIndex = (this->m_currentSemaphoreIndex + 1) % this->m_imagesCount;

        VulkanFence& acquireFence = *this->m_imageAcquiredFence[this->m_currentSemaphoreIndex];
        acquireFence.Reset();

        VkResult result = vkAcquireNextImageKHR(
            this->m_device->m_device,
            this->m_swapchain,
            UINT64_MAX,
            this->m_imageAcquiredSemaphore[this->m_currentSemaphoreIndex]->GetHandle(),
            acquireFence.GetHandle(),
            &imageIndex);

        if ((result == VK_ERROR_OUT_OF_DATE_KHR) || (result == VK_ERROR_SURFACE_LOST_KHR))
        {
            outSemaphore = nullptr;
            this->m_currentSemaphoreIndex = previousSemaphoreIndex;
            return result;
        }

        AE_ASSERT((result == VK_SUCCESS) || (result == VK_SUBOPTIMAL_KHR));

        outSemaphore = this->m_imageAcquiredSemaphore[this->m_currentSemaphoreIndex].Get();

        this->m_currentImageIndex = imageIndex;
        outImageIndex = imageIndex;

        this->m_imageAcquiredFence[this->m_currentSemaphoreIndex]->Wait();

        return result;
    }

    VkResult VulkanViewport::Present()
    {
        AE_ASSERT(this->m_currentImageIndex != UINT32_MAX);

        VkSemaphore imageReleasedSemaphore = this->m_imageReleasedSemaphore[this->m_currentSemaphoreIndex]->GetHandle();

        VkPresentInfoKHR presentInfoKHR{
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &imageReleasedSemaphore,
            .swapchainCount = 1,
            .pSwapchains = &this->m_swapchain,
            .pImageIndices = &this->m_currentImageIndex,
            .pResults = nullptr,
        };

        VkResult result = vkQueuePresentKHR(
            this->m_presentQueue,
            &presentInfoKHR);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            this->Destroy();
            this->Create();
        }

        this->m_currentImageIndex = UINT32_MAX;
        return VK_SUCCESS;
    }

    VkSurfaceFormatKHR VulkanViewport::ChooseSurfaceFormat() const
    {
        std::vector<VkSurfaceFormatKHR> surfaceFormats{};
        AE_VULKAN_ENSURE(EnumeratePhysicalDeviceSurfaceFormats(
            this->m_device->m_physicalDevice,
            this->m_surface,
            surfaceFormats));

        for (VkSurfaceFormatKHR const& format : surfaceFormats)
        {
            if ((format.format == VK_FORMAT_B8G8R8A8_SRGB) &&
                (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR))
            {
                return format;
            }
        }

        // For now, fallback to the first available format.
        return surfaceFormats.front();
    }

    VkPresentModeKHR VulkanViewport::ChoosePresentMode() const
    {
        std::vector<VkPresentModeKHR> presentModes{};
        AE_VULKAN_ENSURE(EnumerateSurfacePresentModes(
            this->m_device->m_physicalDevice,
            this->m_surface,
            presentModes));

        for (VkPresentModeKHR const& presentMode : presentModes)
        {
            if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return presentMode;
            }
        }

        return presentModes.front();
    }

    VkExtent2D VulkanViewport::ChooseExtents(VkSurfaceCapabilitiesKHR const& capabilities) const
    {
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            return capabilities.currentExtent;
        }

        RectF const clientBounds = this->m_window->GetClientBounds();

        VkExtent2D extent2D{
            .width = static_cast<uint32_t>(clientBounds.Width),
            .height = static_cast<uint32_t>(clientBounds.Height),
        };

        extent2D.width = std::clamp(extent2D.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        extent2D.height = std::clamp(extent2D.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        return extent2D;
    }

    VkResult VulkanViewport::EnumeratePhysicalDeviceSurfaceFormats(
        VkPhysicalDevice physicalDevice,
        VkSurfaceKHR surface,
        std::vector<VkSurfaceFormatKHR>& outFormats)
    {
        VkResult result;
        uint32_t count = 0;

        do
        {
            result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, nullptr);

            if (result == VK_SUCCESS)
            {
                outFormats.resize(count);
                result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, outFormats.data());
            }
        } while (result == VK_INCOMPLETE);

        if (count < outFormats.size())
        {
            outFormats.resize(count);
        }

        return result;
    }

    VkResult VulkanViewport::EnumerateSurfacePresentModes(
        VkPhysicalDevice physicalDevice,
        VkSurfaceKHR surface,
        std::vector<VkPresentModeKHR>& outPresentModes)
    {
        VkResult result;
        uint32_t count = 0;

        do
        {
            result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, nullptr);

            if (result == VK_SUCCESS)
            {
                outPresentModes.resize(count);
                result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, outPresentModes.data());
            }
        } while (result == VK_INCOMPLETE);

        if (count < outPresentModes.size())
        {
            outPresentModes.resize(count);
        }

        return result;
    }
}

namespace Anemone
{
    Reference<GpuViewport> VulkanDevice::CreateViewport(Reference<HostWindow> window)
    {
        return MakeReference<VulkanViewport>(*this, std::move(window));
    }

    Reference<GpuCommandListContext> VulkanDevice::GetImmediateContext()
    {
        return this->m_immediateContext;
    }
}

#if false
    // Submit
    VkSubmitInfo submitInfo{};
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};

    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // Present
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores; // use renderFinishedSemaphores[currentFrame]
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &imageIndex;

    VkResult presentResult = vkQueuePresentKHR(presentQueue, &presentInfo);
    if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR)
    {
        // handle swapchain recreation
    }
    else if (presentResult != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    VkResult GpuViewportVulkan::Acquire()
    {
        VkAcquireNextImageInfoKHR acquireNextImageInfoKHR{
            .sType = VK_STRUCTURE_TYPE_ACQUIRE_NEXT_IMAGE_INFO_KHR,
            .pNext = nullptr,
            .swapchain = this->m_swapchain,
            .timeout = UINT64_MAX,
            .semaphore = this->m_backBuffers[this->m_currentBackBuffer].m_acquireSemaphore,
            .fence = VK_NULL_HANDLE,
            .deviceMask = 0,
        };

        return vkAcquireNextImage2KHR(
            this->m_device->m_device,
            &acquireNextImageInfoKHR,
            &this->m_currentBackBuffer);
    }

    VkResult GpuViewportVulkan::Present()
    {
        VkPresentInfoKHR presentInfo{
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &this->m_backBuffers[this->m_currentBackBuffer].m_releaseSemaphore,
            .swapchainCount = 1,
            .pSwapchains = &this->m_swapchain,
            .pImageIndices = &this->m_currentBackBuffer,
            .pResults = nullptr};

        // return vkQueuePresentKHR(this->m_device->m_device, &presentInfo);
        return VK_SUCCESS;
    }
}
#endif
