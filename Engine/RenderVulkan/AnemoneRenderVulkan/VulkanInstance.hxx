#pragma once
#include "AnemoneRenderVulkan/VulkanHeaders.hxx"
#include "AnemoneRender/Gpu.hxx"

#include <vector>

namespace Anemone
{
    class VulkanDevice;
    class VulkanViewport;

    struct VulkanInstanceLayers final
    {
        bool KHRONOS_validation;
        bool KHRONOS_profiles;
        bool GOOGLE_unique_objects;
        bool GOOGLE_threading;
        bool LUNARG_parameter_validation;
        bool LUNARG_object_tracker;
        bool LUNARG_core_validation;
        bool LUNARG_api_dump;
        bool LUNARG_gfxreconstruct;
        bool LUNARG_vktrace;
        bool LUNARG_crash_diagnostic;
        bool RENDERDOC_Capture;
        bool NV_nsight;
    };

    struct VulkanInstanceExtensions final
    {
        bool KHR_surface;
        bool KHR_display;
        bool EXT_debug_utils;
        bool EXT_debug_report;
        bool KHR_win32_surface;
        bool KHR_get_surface_capabilities2;
        bool KHR_surface_maintenance1;
    };

    class VulkanInstance final
    {
        friend class VulkanDevice;
        friend class VulkanViewport;

    public:
        static void StaticInitialize();
        static void StaticFinalize();
        static VulkanInstance& Get();

    public:
        VulkanInstance();
        ~VulkanInstance();

        VkPhysicalDevice SelectPhysicalDevice(GpuVendor preferredVendor) const;

    private:
        VkInstance m_instance{};
        VulkanInstanceLayers m_instanceLayers{};
        VulkanInstanceExtensions m_instanceExtensions{};

    private:
        static VkResult QueryInstanceLayerProperties(
            std::vector<VkLayerProperties>& outLayerProperties);

        static VkResult QueryInstanceExtensionProperties(
            std::vector<VkExtensionProperties>& outExtensionProperties,
            const char* layerName = nullptr);

        static VkResult QueryPhysicalDevices(
            VkInstance instance,
            std::vector<VkPhysicalDevice>& outPhysicalDevices);


#if ANEMONE_VULKAN_VALIDATION
    private:
        VkDebugUtilsMessengerEXT m_debugUtilsMessenger{};
        VkDebugReportCallbackEXT m_debugReportCallback{};

    private:
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessengerCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT severity,
            VkDebugUtilsMessageTypeFlagsEXT type,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData);

        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
            VkDebugReportFlagsEXT flags,
            VkDebugReportObjectTypeEXT objectType,
            uint64_t object,
            size_t location,
            int32_t messageCode,
            const char* pLayerPrefix,
            const char* pMessage,
            void* pUserData);
#endif
    };
}
