#pragma once
#include "AnemoneApplication/HostWindow.hxx"
#include "AnemoneRender/Gpu.hxx"
#include "AnemoneRenderVulkan/VulkanHeaders.hxx"
#include "AnemoneRenderVulkan/VulkanSemaphore.hxx"

namespace Anemone
{
    class VulkanDevice;

    class VulkanViewport : public GpuViewport
    {
    public:
        static constexpr size_t MaxFramesCount = 2;
        static constexpr size_t MaxImagesCount = 8;

    public:
        VulkanDevice* m_device;
        Reference<HostWindow> m_window{};

        VkSurfaceKHR m_surface{};
        VkSwapchainKHR m_swapchain{};
        VkExtent2D m_swapchainExtent{};

        std::vector<VkImage> m_image{};
        std::vector<VkImageView> m_imageView{};

        std::vector<Reference<VulkanFence>> m_imageAcquiredFence{};
        std::vector<Reference<VulkanSemaphore>> m_imageAcquiredSemaphore{};
        std::vector<Reference<VulkanSemaphore>> m_imageReleasedSemaphore{};

        VkQueue m_presentQueue{};
        uint32_t m_currentSemaphoreIndex{};

        uint32_t m_imagesCount{};
        uint32_t m_currentImageIndex{UINT32_MAX};

    public:
        VulkanViewport(VulkanDevice& device, Reference<HostWindow> window);
        ~VulkanViewport() override;

        void Resize(uint32_t width, uint32_t height) override;


    public:
        VkResult Create();
        VkResult Destroy();

        VkResult Acquire(uint32_t& outImageIndex, VulkanSemaphore*& outSemaphore);
        VkResult Present();
        VkSurfaceFormatKHR ChooseSurfaceFormat() const;
        VkPresentModeKHR ChoosePresentMode() const;
        VkExtent2D ChooseExtents(VkSurfaceCapabilitiesKHR const& capabilities) const;

        static VkResult EnumeratePhysicalDeviceSurfaceFormats(
            VkPhysicalDevice physicalDevice,
            VkSurfaceKHR surface,
            std::vector<VkSurfaceFormatKHR>& outFormats);

        static VkResult EnumerateSurfacePresentModes(
            VkPhysicalDevice physicalDevice,
            VkSurfaceKHR surface,
            std::vector<VkPresentModeKHR>& outPresentModes);
    };
}
