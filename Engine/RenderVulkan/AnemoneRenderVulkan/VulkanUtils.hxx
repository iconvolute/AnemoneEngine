#pragma once
#include "AnemoneRenderVulkan/VulkanHeaders.hxx"
#include "AnemoneRenderVulkan/VulkanError.hxx"

#include <bit>

#if ANEMONE_VULKAN_VALIDATION

namespace Anemone::VulkanGraphics
{
    void SetObjectNameImpl(
        VkDevice device,
        VkObjectType objectType,
        uint64_t objectHandle,
        const char* name);

    inline void SetObjectName(VkDevice device, VkFence handle, const char* name)
    {
        SetObjectNameImpl(device, VK_OBJECT_TYPE_FENCE, std::bit_cast<uint64_t>(handle), name);
    }

    inline void SetObjectName(VkDevice device, VkSemaphore handle, const char* name)
    {
        SetObjectNameImpl(device, VK_OBJECT_TYPE_SEMAPHORE, std::bit_cast<uint64_t>(handle), name);
    }

    inline void SetObjectName(VkDevice device, VkImage handle, const char* name)
    {
        SetObjectNameImpl(device, VK_OBJECT_TYPE_IMAGE, std::bit_cast<uint64_t>(handle), name);
    }

    inline void SetObjectName(VkDevice device, VkImageView handle, const char* name)
    {
        SetObjectNameImpl(device, VK_OBJECT_TYPE_IMAGE_VIEW, std::bit_cast<uint64_t>(handle), name);
    }

    inline void SetObjectName(VkDevice device, VkBuffer handle, const char* name)
    {
        SetObjectNameImpl(device, VK_OBJECT_TYPE_BUFFER, std::bit_cast<uint64_t>(handle), name);
    }

}

#endif

namespace Anemone::VulkanGraphics
{
    struct InstanceLayers final
    {
        bool KHRONOS_validation;
        bool KHRONOS_profiles;
        bool GOOGLE_threading;
        bool GOOGLE_unique_objects;
        bool LUNARG_parameter_validation;
        bool LUNARG_object_tracker;
        bool LUNARG_core_validation;
        bool LUNARG_api_dump; 
        bool LUNARG_gfxreconstruct;
        bool LUNARG_vktrace;
        bool LUNARG_crash_diagnostic;
        bool RENDERDOC_capture;
        bool NV_nsight;
    };

    struct InstanceExtensions final
    {
        bool DebugUtils;
        bool DebugReport;
        bool DeviceAddressBindingReport;
        bool ValidationFeatures;
    };

    VkResult QueryInstanceLayersAndExtensions(
        InstanceLayers& outInstanceLayers,
        InstanceExtensions& outInstanceExtensions,
        std::vector<const char*>& outLayerNames,
        std::vector<const char*>& outExtensionNames,
        bool withDiagnostics);

    struct PhysicalDeviceExtensions final
    {
        bool ToolingInfo;
        bool DebugMarker;
        bool ValidationCache;
        bool CalibratedTimestamps;
        bool HostQueryReset;

        
        bool SeparateDepthStencilLayouts;
        bool Synchronization2;
        bool RenderPass2;
        bool GeometryShader;
        bool FragmentShadingRate;
        bool FragmentDensityMap;
        bool MeshShader;
        bool AccelerationStructure;
        bool RayTracingPipeline;
        bool FullscreenExclusive;
        bool BindlessRendering;
        bool SurfaceCapabilities2;

    };

    struct VulkanQueueFamilyProperties final
    {
        VkQueueFamilyProperties2 queueFamilyProperties2;
        VkQueueFamilyCheckpointProperties2NV queueFamilyCheckpointProperties2NV;
        VkQueueFamilyCheckpointPropertiesNV queueFamilyCheckpointPropertiesNV;
        VkQueueFamilyDataGraphProcessingEnginePropertiesARM queueFamilyDataGraphProcessingEnginePropertiesARM;
        VkQueueFamilyDataGraphPropertiesARM queueFamilyDataGraphPropertiesARM;
        VkQueueFamilyGlobalPriorityProperties queueFamilyGlobalPriorityProperties;
        VkQueueFamilyQueryResultStatusPropertiesKHR queueFamilyQueryResultStatusPropertiesKHR;
        VkQueueFamilyVideoPropertiesKHR queueFamilyVideoPropertiesKHR;
    };

    VkResult QueryPhysicalDeviceExtensions(
        VkPhysicalDevice device,
        PhysicalDeviceExtensions& outDeviceExtensions,
        std::vector<const char*>& outExtensionNames,
        std::vector<VkQueueFamilyProperties>& outQueueFamilyProperties,
        bool withDiagnostics);

    VkResult QueryPhysicalDevice(
        VkInstance instance,
        VkPhysicalDevice& outPhysicalDevice,
        PhysicalDeviceExtensions& outDeviceExtensions,
        std::vector<const char*>& outExtensionNames,
        std::vector<VkQueueFamilyProperties>& outQueueFamilyProperties,
        bool withDiagnostics);
}

namespace Anemone::VulkanGraphics
{
    VkResult EnumerateInstanceExtensionProperties(
        std::vector<VkExtensionProperties>& outExtensionProperties,
        const char* layerName = nullptr);

    VkResult EnumerateInstanceLayerProperties(
        std::vector<VkLayerProperties>& outLayerProperties);

    VkResult EnumeratePhysicalDeviceExtensionProperties(
        std::vector<VkExtensionProperties>& outExtensionProperties,
        VkPhysicalDevice device,
        const char* layerName = nullptr);

    VkResult EnumerateDeviceLayerProperties(
        std::vector<VkLayerProperties>& outLayerProperties,
        VkPhysicalDevice device);

    VkResult EnumeratePhysicalDevices(
        VkInstance instance,
        std::vector<VkPhysicalDevice>& outPhysicalDevices);

    VkResult EnumeratePhysicalDeviceQueueFamilyProperties(
        VkPhysicalDevice device,
        std::vector<VkQueueFamilyProperties>& outProperties);

    VkResult EnumeratePhysicalDeviceSurfaceFormats(
        VkPhysicalDevice physicalDevice,
        VkSurfaceKHR surface,
        std::vector<VkSurfaceFormatKHR>& outFormats);

    VkResult EnumerateSurfacePresentModes(
        VkPhysicalDevice physicalDevice,
        VkSurfaceKHR surface,
        std::vector<VkPresentModeKHR>& outPresentModes);
}
