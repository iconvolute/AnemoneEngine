#pragma once
#include "AnemoneRender/Device.hxx"
#include "AnemoneRenderVulkan/VulkanHeaders.hxx"

namespace Anemone
{
    class VulkanQueue;
    class VulkanSwapChain;

    struct VulkanDeviceExtensions
    {
        bool HasSeparateDepthStencilLayouts : 1;
        bool HasKHRSynchronization2 : 1;
        bool HasKHRRenderPass2 : 1;
        bool HasGeometryShader : 1;
        bool HasKHRFragmentShadingRate : 1;
        bool HasEXTFragmentDensityMap : 1;
        bool HasEXTMeshShader : 1;
        bool HasAccelerationStructure : 1;
        bool HasRayTracingPipeline : 1;
        bool HasEXTFullscreenExclusive : 1;
    };

    class VulkanDevice : public RenderDevice
    {
        friend class VulkanQueue;
        friend class VulkanSwapChain;

    public:
        VkInstance _instance{};
        VkDebugUtilsMessengerEXT _debugMessenger{};
        VkPhysicalDevice _physicalDevice{};
        VkDevice _logicalDevice{};
        VmaAllocator _allocator{};

        std::vector<uint32_t> _activeQueueFamilies{};
        std::vector<VkQueueFamilyProperties> _queueFamilyProperties{};

        Reference<VulkanQueue> _graphicsQueue{};
        Reference<VulkanQueue> _computeQueue{};
        Reference<VulkanQueue> _transferQueue{};

        VulkanDeviceExtensions _deviceExtensions{};

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
        static VkResult EnumerateLayerProperties(
            std::vector<VkLayerProperties>& outProperties);

        static VkResult EnumerateInstanceExtensionProperties(
            std::vector<VkExtensionProperties>& outProperties);

        static VkResult EnumeratePhysicalDevices(
            VkInstance instance,
            std::vector<VkPhysicalDevice>& outPhysicalDevices);

        static VkResult EnumeratePhysicalDeviceQueueFamilyProperties(
            VkPhysicalDevice device,
            std::vector<VkQueueFamilyProperties>& outProperties);

        static VkResult EnumeratePhysicalDeviceExtensionProperties(
            std::vector<VkExtensionProperties>& outProperties,
            VkPhysicalDevice device,
            const char* layerName);

        static VkResult EnumeratePhysicalDeviceSurfaceFormats(
            VkPhysicalDevice physicalDevice,
            VkSurfaceKHR surface,
            std::vector<VkSurfaceFormatKHR>& outFormats);

        static VkResult EnumerateSurfacePresentModes(
            VkPhysicalDevice physicalDevice,
            VkSurfaceKHR surface,
            std::vector<VkPresentModeKHR>& outPresentModes);

    private:
        VkResult CreateInstance(
            VkInstance& outInstance);

        VkResult CreateDebugUtilsMessenger(
            VkInstance instance,
            VkDebugUtilsMessengerEXT& messenger);

        VkResult ChoosePhysicalDevice(
            VkInstance instance,
            VkPhysicalDevice& outPhysicalDevice);

        bool IsPhysicalDeviceSuitable(VkPhysicalDevice device, VulkanDeviceExtensions& deviceExtensions);

        VkResult CreateLogicalDevice(
            VkInstance instance,
            VkPhysicalDevice physicalDevice,
            VkDevice& outLogicalDevice);

        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessengerCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT severity,
            VkDebugUtilsMessageTypeFlagsEXT type,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData);

        static bool SupportsPresent(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VulkanQueue& queue);


        static std::vector<const char*> GetRequiredLayers();
        static std::vector<const char*> GetRequiredInstanceExtensions();
        static std::vector<const char*> GetRequiredDeviceExtensions(VkPhysicalDevice device);
    };

    RENDER_VULKAN_API Reference<RenderDevice> CreateRenderDevice();
}
