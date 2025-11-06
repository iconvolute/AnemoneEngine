#pragma once
#include "AnemoneRender/SwapChain.hxx"
#include "AnemoneRenderVulkan/VulkanHeaders.hxx"
#include "AnemoneApplication/HostWindow.hxx"

namespace Anemone
{
    class VulkanDevice;
    class VulkanQueue;

    class VulkanSwapChain final : public RenderSwapChain
    {
    public:
        VulkanSwapChain(
            VulkanDevice* device,
            Reference<HostWindow> hostWindow);

        VulkanSwapChain(VulkanSwapChain const&) = delete;
        VulkanSwapChain(VulkanSwapChain&&) = delete;
        ~VulkanSwapChain() override;
        VulkanSwapChain& operator=(VulkanSwapChain const&) = delete;
        VulkanSwapChain& operator=(VulkanSwapChain&&) = delete;

        static constexpr size_t MaxFramesInFlight = 3;

    public:
        void Start() override;
        void Present() override;

    private:
        VulkanDevice* _device{};
        VkSurfaceKHR _surface{};
        Reference<HostWindow> _hostWindow{};
        Reference<VulkanQueue> _presentQueue{};

        std::vector<VkImage> _swapChainImages{};
        std::vector<VkImageView> _swapChainImageViews{};
        std::vector<VkSemaphore> _imageAvailableSemaphores{};
        std::vector<VkSemaphore> _renderFinishedSemaphores{};
        std::vector<VkFence> _inFlightFences{};

        VkSurfaceFormatKHR _swapChainImageFormat{};
        VkExtent2D _swapChainExtent{};
        VkSwapchainKHR _swapChainHandle{};

        std::array<VkCommandPool, MaxFramesInFlight> commandPools{};
        std::array<VkCommandBuffer, MaxFramesInFlight> commandBuffers{};

        size_t currentSemaphoreIndex = 0;
        size_t currentFrameIndex = 0;
        uint32_t currentImageIndex = 0;

    private:
        void CreateSwapChain();
        void DestroySwapChain();

        VkExtent2D ChooseSwapExtent(VkSurfaceCapabilitiesKHR const& capabilities) const;

        static VkSurfaceFormatKHR ChooseSwapChainFormat(
            std::span<VkSurfaceFormatKHR const> availableFormats);

        static VkPresentModeKHR ChoosePresentMode(
            std::span<VkPresentModeKHR const> availablePresentModes);
    };
}
