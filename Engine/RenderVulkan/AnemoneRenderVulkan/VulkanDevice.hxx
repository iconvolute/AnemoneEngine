#pragma once
#include "AnemoneRender/Device.hxx"
#include "AnemoneRenderVulkan/VulkanHeaders.hxx"
#include "AnemoneRenderVulkan/VulkanUtils.hxx"


namespace Anemone
{
    class VulkanQueue;
    class VulkanSwapChain;

    class VulkanDevice : public RenderDevice
    {
        friend class VulkanQueue;
        friend class VulkanSwapChain;

    public:
        VkInstance _instance{};

#if ANEMONE_VULKAN_VALIDATION
        VkDebugUtilsMessengerEXT _debugMessenger{};
#endif

        VkPhysicalDevice _physicalDevice{};
        VkDevice _logicalDevice{};
        VmaAllocator _allocator{};

        std::vector<uint32_t> _activeQueueFamilies{};
        std::vector<VkQueueFamilyProperties> m_queueFamilyProperties{};

        Reference<VulkanQueue> _graphicsQueue{};
        Reference<VulkanQueue> _computeQueue{};
        Reference<VulkanQueue> _transferQueue{};

        VulkanGraphics::PhysicalDeviceExtensions m_physicalDeviceExtensions{};

        VulkanGraphics::InstanceLayers m_instanceLayers{};
        VulkanGraphics::InstanceExtensions m_instanceExtensions{};

    public:
        VulkanDevice();
        VulkanDevice(VulkanDevice const&) = delete;
        VulkanDevice(VulkanDevice&&) = delete;
        ~VulkanDevice() override;
        VulkanDevice& operator=(VulkanDevice const&) = delete;
        VulkanDevice& operator=(VulkanDevice&&) = delete;

    public:
        Reference<RenderSwapChain> CreateSwapChain(Reference<HostWindow> window) override;

    private:
        VkResult CreateInstance(
            VkInstance& outInstance);

        VkResult CreateLogicalDevice(
            VkInstance instance,
            VkPhysicalDevice physicalDevice,
            VkDevice& outLogicalDevice,
            std::vector<const char*> const& extensionNames,
            std::vector<VkQueueFamilyProperties> const& queueFamilyProperties);

        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessengerCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT severity,
            VkDebugUtilsMessageTypeFlagsEXT type,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData);
    };

    RENDER_VULKAN_API Reference<RenderDevice> CreateRenderDevice();
}
