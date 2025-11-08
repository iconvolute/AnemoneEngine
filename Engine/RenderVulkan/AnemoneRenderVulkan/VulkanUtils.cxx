#include "AnemoneRenderVulkan/VulkanUtils.hxx"
#include "AnemoneDiagnostics/Trace.hxx"

#include <algorithm>

#if ANEMONE_VULKAN_VALIDATION

namespace Anemone::VulkanGraphics
{
    void SetObjectNameImpl(
        VkDevice device,
        VkObjectType objectType,
        uint64_t objectHandle,
        const char* name)
    {
        VkDebugUtilsObjectNameInfoEXT debugUtilsObjectNameInfo{
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .pNext = nullptr,
            .objectType = objectType,
            .objectHandle = objectHandle,
            .pObjectName = name,
        };
        AE_VK_CALL(vkSetDebugUtilsObjectNameEXT(device, &debugUtilsObjectNameInfo));
    }
}

#endif

namespace Anemone::VulkanGraphics
{
    VkResult QueryInstanceLayersAndExtensions(
        InstanceLayers& outInstanceLayers,
        InstanceExtensions& outInstanceExtensions,
        std::vector<const char*>& outLayerNames,
        std::vector<const char*>& outExtensionNames,
        [[maybe_unused]] bool withDiagnostics)
    {
        outInstanceLayers = {};
        outInstanceExtensions = {};

        outLayerNames.clear();
        outExtensionNames.clear();

        std::vector<VkLayerProperties> layerProperties{};
        if (EnumerateInstanceLayerProperties(layerProperties) != VK_SUCCESS)
        {
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        std::vector<VkExtensionProperties> extensionProperties{};
        if (EnumerateInstanceExtensionProperties(extensionProperties, nullptr) != VK_SUCCESS)
        {
            return VK_ERROR_INITIALIZATION_FAILED;
        }

#if ANEMONE_VULKAN_VALIDATION
        //
        // Validate layers
        //

        auto validateLayer = [&](const char* layerName) -> bool
        {
            for (const VkLayerProperties& properties : layerProperties)
            {
                if (std::strcmp(layerName, properties.layerName) == 0)
                {
                    outLayerNames.emplace_back(layerName);

                    return true;
                }
            }

            return false;
        };

        if (withDiagnostics)
        {
            outInstanceLayers.KHRONOS_validation = validateLayer("VK_LAYER_KHRONOS_validation");
            outInstanceLayers.KHRONOS_profiles = validateLayer("VK_LAYER_KHRONOS_profiles");
            outInstanceLayers.GOOGLE_unique_objects = validateLayer("VK_LAYER_GOOGLE_unique_objects");
            outInstanceLayers.GOOGLE_threading = validateLayer("VK_LAYER_GOOGLE_threading");
            outInstanceLayers.LUNARG_parameter_validation = validateLayer("VK_LAYER_LUNARG_parameter_validation");
            outInstanceLayers.LUNARG_object_tracker = validateLayer("VK_LAYER_LUNARG_object_tracker");
            outInstanceLayers.LUNARG_core_validation = validateLayer("VK_LAYER_LUNARG_core_validation");

#if ANEMONE_VULKAN_FEATURE_API_DUMP
            outInstanceLayers.LUNARG_api_dump = validateLayer("VK_LAYER_LUNARG_api_dump");
#endif

#if ANEMONE_VULKAN_FEATURE_GFXRECONSTRUCT
            outInstanceLayers.LUNARG_gfxreconstruct = validateLayer("VK_LAYER_LUNARG_gfxreconstruct");
#endif

#if ANEMONE_VULKAN_FEATURE_VKTRACE
            outInstanceLayers.LUNARG_vktrace = validateLayer("VK_LAYER_LUNARG_vktrace");
#endif

#if ANEMONE_VULKAN_FEATURE_CRASH_DIAGNOSTIC
            outInstanceLayers.LUNARG_crash_diagnostic = validateLayer("VK_LAYER_LUNARG_crash_diagnostic");
#endif

            outInstanceLayers.RENDERDOC_capture = validateLayer("VK_LAYER_RENDERDOC_Capture");
            outInstanceLayers.NV_nsight = validateLayer("VK_LAYER_NV_nsight");
        }

#endif


        auto validateExtension = [&](const char* extensionName) -> bool
        {
            for (const VkExtensionProperties& properties : extensionProperties)
            {
                if (std::strcmp(extensionName, properties.extensionName) == 0)
                {
                    outExtensionNames.emplace_back(extensionName);
                    return true;
                }
            }

            return false;
        };

        auto requireExtension = [&](const char* extensionName) -> bool
        {
            if (!validateExtension(extensionName))
            {
                AE_TRACE(Error, "Required Vulkan instance extension '{}' not found", extensionName);
                return false;
            }

            return true;
        };

        //
        // Platform extensions
        //

#if ANEMONE_PLATFORM_WINDOWS
        if (!requireExtension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME))
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
#endif

#if ANEMONE_PLATFORM_LINUX
        if (!requireExtension(VK_KHR_XCB_SURFACE_EXTENSION_NAME))
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
        if (!requireExtension(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME))
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
#endif

        //
        // Diagnostic extensions
        //
#if ANEMONE_VULKAN_VALIDATION

        if (withDiagnostics)
        {
            // https://docs.vulkan.org/refpages/latest/refpages/source/VK_EXT_debug_utils.html
            outInstanceExtensions.DebugUtils = validateExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            // https://docs.vulkan.org/refpages/latest/refpages/source/VK_EXT_debug_report.html
            outInstanceExtensions.DebugReport = validateExtension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
            // https://docs.vulkan.org/refpages/latest/refpages/source/VK_EXT_device_address_binding_report.html
            outInstanceExtensions.DeviceAddressBindingReport = validateExtension(VK_EXT_DEVICE_ADDRESS_BINDING_REPORT_EXTENSION_NAME);
        }

#endif

        //
        // Required extensions
        //

        if (!requireExtension(VK_KHR_SURFACE_EXTENSION_NAME))
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }

        if (!requireExtension(VK_KHR_DISPLAY_EXTENSION_NAME))
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }

        //
        // Optional extensions
        //

        return VK_SUCCESS;
    }

    VkResult QueryPhysicalDeviceExtensions(
        VkPhysicalDevice device,
        PhysicalDeviceExtensions& outDeviceExtensions,
        std::vector<const char*>& outExtensionNames,
        std::vector<VkQueueFamilyProperties>& outQueueFamilyProperties,
        [[maybe_unused]] bool withDiagnostics)
    {
        outDeviceExtensions = {};
        outExtensionNames.clear();

        //
        // Check supported Vulkan version.
        //

        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(device, &physicalDeviceProperties);

        if (physicalDeviceProperties.apiVersion < VK_API_VERSION_1_4)
        {
            return VK_ERROR_FEATURE_NOT_PRESENT;
        }

        //
        // Check queue family properties.
        //

        if (EnumeratePhysicalDeviceQueueFamilyProperties(device, outQueueFamilyProperties) != VK_SUCCESS)
        {
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        bool const supportsGraphics = std::any_of(
            outQueueFamilyProperties.begin(),
            outQueueFamilyProperties.end(),
            [](VkQueueFamilyProperties const& qfp)
        {
            return (qfp.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0;
        });

        if (!supportsGraphics)
        {
            //
            // Device does not support graphics queue.
            //

            return VK_ERROR_FEATURE_NOT_PRESENT;
        }

        //
        // Check supported extensions.
        //

        std::vector<VkExtensionProperties> extensionProperties{};
        if (EnumeratePhysicalDeviceExtensionProperties(extensionProperties, device) != VK_SUCCESS)
        {
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        auto validateExtension = [&](const char* extensionName) -> bool
        {
            for (const VkExtensionProperties& properties : extensionProperties)
            {
                if (std::strcmp(extensionName, properties.extensionName) == 0)
                {
                    outExtensionNames.emplace_back(extensionName);
                    return true;
                }
            }

            return false;
        };

        auto requireExtension = [&](const char* extensionName) -> bool
        {
            if (!validateExtension(extensionName))
            {
                AE_TRACE(Error, "Required Vulkan device extension '{}' not found", extensionName);
                return false;
            }

            return true;
        };

        //
        // Diagnostic extensions
        //

        struct PhysicalExtensionInfo final
        {
            using FeaturePointer = bool(PhysicalDeviceExtensions::*);
            const char* name;
            FeaturePointer ptr;
        };

#if ANEMONE_VULKAN_VALIDATION

        if (withDiagnostics)
        {
            static constexpr PhysicalExtensionInfo DiagnosticExtensions[] =
                {
                    {VK_EXT_TOOLING_INFO_EXTENSION_NAME, &PhysicalDeviceExtensions::ToolingInfo},
                    {VK_EXT_DEBUG_MARKER_EXTENSION_NAME, &PhysicalDeviceExtensions::DebugMarker},
                };

            for (const auto& extension : DiagnosticExtensions)
            {
                outDeviceExtensions.*(extension.ptr) = validateExtension(extension.name);
            }
        }

#endif

        //
        // Required extensions
        //

        static constexpr PhysicalExtensionInfo RequiredExtensions[]{
            {VK_KHR_SWAPCHAIN_EXTENSION_NAME, nullptr},
            {VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME, nullptr},
            {VK_KHR_SPIRV_1_4_EXTENSION_NAME, nullptr},
            {VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME, nullptr},
            {VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME, nullptr},
            {VK_EXT_LOAD_STORE_OP_NONE_EXTENSION_NAME, nullptr},
            {VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME, nullptr},
            {VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME, nullptr},
            {VK_KHR_IMAGE_FORMAT_LIST_EXTENSION_NAME, nullptr},
            {VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME, nullptr},
            {VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME, nullptr},
            // {VK_KHR_SAMPLER_MIRROR_CLAMP_TO_EDGE_EXTENSION_NAME, nullptr}, // Vulkan 1.2
        };

        for (const auto& extension : RequiredExtensions)
        {
            if (!requireExtension(extension.name))
            {
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }

        //
        // Optional extensions
        //

        static constexpr PhysicalExtensionInfo OptionalExtensions[]{
            // No dependencies
            {VK_EXT_VALIDATION_CACHE_EXTENSION_NAME, &PhysicalDeviceExtensions::ValidationCache},
            // Vulkan 1.1
            {VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME, &PhysicalDeviceExtensions::CalibratedTimestamps},
            // Vulkan 1.1
            {VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME, &PhysicalDeviceExtensions::HostQueryReset},
            // Vulkan 1.1
            {VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME, &PhysicalDeviceExtensions::FragmentShadingRate},
            // VK_KHR_surface
            {VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME, &PhysicalDeviceExtensions::SurfaceCapabilities2},
        };

        for (const auto& extension : OptionalExtensions)
        {
            outDeviceExtensions.*(extension.ptr) = validateExtension(extension.name);
        }

        if (outDeviceExtensions.SurfaceCapabilities2)
        {
            outDeviceExtensions.FullscreenExclusive = validateExtension(VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME);
        }

        //
        // Query device features.
        //

        VkPhysicalDeviceVulkan11Features vulkan11Features{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
            .pNext = nullptr,
        };

        VkPhysicalDeviceVulkan12Features vulkan12Features{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
            .pNext = &vulkan11Features,
        };

        VkPhysicalDeviceVulkan13Features vulkan13Features{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
            .pNext = &vulkan12Features,
        };

        VkPhysicalDeviceVulkan14Features vulkan14Features{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
            .pNext = &vulkan13Features,
        };

        VkPhysicalDeviceExtendedDynamicStateFeaturesEXT extendedDynamicStateFeatures{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT,
            .pNext = &vulkan14Features,
        };
        VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR,
            .pNext = &extendedDynamicStateFeatures,
        };

        VkPhysicalDeviceRayQueryFeaturesKHR rayQueryFeatures{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR,
            .pNext = &accelerationStructureFeatures,
        };

        VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures separateDepthStencilLayoutsFeatures{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES,
            .pNext = &rayQueryFeatures,
        };

        VkPhysicalDeviceSynchronization2Features synchronization2Features{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
            .pNext = &separateDepthStencilLayoutsFeatures,
        };

        VkPhysicalDeviceFragmentDensityMapFeaturesEXT fragmentDensityMapFeatures{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_FEATURES_EXT,
            .pNext = &synchronization2Features,
        };

        VkPhysicalDeviceMeshShaderFeaturesEXT meshShaderFeatures{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT,
            .pNext = &fragmentDensityMapFeatures,
        };

        VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingPipelineFeatures{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR,
            .pNext = &meshShaderFeatures,
        };

        VkPhysicalDeviceDescriptorIndexingFeatures physicalDeviceDescriptorIndexingFeatures{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
            .pNext = &rayTracingPipelineFeatures,
        };

        VkPhysicalDeviceFeatures2 features2{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
            .pNext = &physicalDeviceDescriptorIndexingFeatures,
        };

        vkGetPhysicalDeviceFeatures2(device, &features2);

        if (!features2.features.samplerAnisotropy)
        {
            return VK_ERROR_FEATURE_NOT_PRESENT;
        }

        if (!vulkan13Features.dynamicRendering)
        {
            return VK_ERROR_FEATURE_NOT_PRESENT;
        }

        if (!extendedDynamicStateFeatures.extendedDynamicState)
        {
            return VK_ERROR_FEATURE_NOT_PRESENT;
        }

        if (!vulkan12Features.descriptorBindingSampledImageUpdateAfterBind)
        {
            return VK_ERROR_FEATURE_NOT_PRESENT;
        }

        if (!vulkan12Features.descriptorBindingSampledImageUpdateAfterBind)
        {
            return VK_ERROR_FEATURE_NOT_PRESENT;
        }

        if (!vulkan12Features.descriptorBindingPartiallyBound)
        {
            return VK_ERROR_FEATURE_NOT_PRESENT;
        }

        if (!vulkan12Features.descriptorBindingVariableDescriptorCount)
        {
            return VK_ERROR_FEATURE_NOT_PRESENT;
        }

        if (!vulkan12Features.runtimeDescriptorArray)
        {
            return VK_ERROR_FEATURE_NOT_PRESENT;
        }

        if (!vulkan12Features.shaderSampledImageArrayNonUniformIndexing)
        {
            return VK_ERROR_FEATURE_NOT_PRESENT;
        }

        if (!vulkan12Features.bufferDeviceAddress)
        {
            return VK_ERROR_FEATURE_NOT_PRESENT;
        }

        if (!accelerationStructureFeatures.accelerationStructure)
        {
            return VK_ERROR_FEATURE_NOT_PRESENT;
        }

        if (!rayQueryFeatures.rayQuery)
        {
            return VK_ERROR_FEATURE_NOT_PRESENT;
        }

        outDeviceExtensions.SeparateDepthStencilLayouts =
            separateDepthStencilLayoutsFeatures.separateDepthStencilLayouts != VK_FALSE;

        outDeviceExtensions.Synchronization2 =
            synchronization2Features.synchronization2 != VK_FALSE;

        outDeviceExtensions.RenderPass2 =
            true; // Always true for Vulkan 1.3 and above

        outDeviceExtensions.GeometryShader =
            features2.features.geometryShader != VK_FALSE;

        outDeviceExtensions.FragmentDensityMap =
            fragmentDensityMapFeatures.fragmentDensityMap != VK_FALSE;

        outDeviceExtensions.MeshShader =
            (meshShaderFeatures.meshShader != VK_FALSE) &&
            (meshShaderFeatures.multiviewMeshShader != VK_FALSE);

        outDeviceExtensions.AccelerationStructure =
            accelerationStructureFeatures.accelerationStructure != VK_FALSE;

        outDeviceExtensions.RayTracingPipeline =
            rayTracingPipelineFeatures.rayTracingPipeline != VK_FALSE;

        outDeviceExtensions.BindlessRendering =
            physicalDeviceDescriptorIndexingFeatures.descriptorBindingPartiallyBound &&
            physicalDeviceDescriptorIndexingFeatures.runtimeDescriptorArray;

        //
        // TODO: Validate extension dependencies
        //

        return VK_SUCCESS;
    }

    VkResult QueryPhysicalDevice(
        VkInstance instance,
        VkPhysicalDevice& outPhysicalDevice,
        PhysicalDeviceExtensions& outDeviceExtensions,
        std::vector<const char*>& outExtensionNames,
        std::vector<VkQueueFamilyProperties>& outQueueFamilyProperties,
        bool withDiagnostics)
    {
        std::vector<VkPhysicalDevice> devices{};

        if (EnumeratePhysicalDevices(instance, devices) != VK_SUCCESS)
        {
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        for (VkPhysicalDevice const device : devices)
        {
            if (QueryPhysicalDeviceExtensions(
                device,
                outDeviceExtensions,
                outExtensionNames,
                outQueueFamilyProperties,
                withDiagnostics) == VK_SUCCESS)
            {
                outPhysicalDevice = device;
                return VK_SUCCESS;
            }
        }

        outPhysicalDevice = VK_NULL_HANDLE;
        outDeviceExtensions = {};
        outExtensionNames.clear();

        return VK_ERROR_INITIALIZATION_FAILED;
    }
}

namespace Anemone::VulkanGraphics
{
    VkResult EnumerateInstanceExtensionProperties(
        std::vector<VkExtensionProperties>& outExtensionProperties,
        const char* layerName)
    {
        VkResult result;
        uint32_t count = 0;

        do
        {
            result = vkEnumerateInstanceExtensionProperties(layerName, &count, nullptr);

            if (result == VK_SUCCESS)
            {
                outExtensionProperties.resize(count);
                result = vkEnumerateInstanceExtensionProperties(layerName, &count, outExtensionProperties.data());
            }
        } while (result == VK_INCOMPLETE);

        AE_ASSERT(count <= outExtensionProperties.size());

        if (count < outExtensionProperties.size())
        {
            outExtensionProperties.resize(count);
        }

        return result;
    }

    VkResult EnumerateInstanceLayerProperties(std::vector<VkLayerProperties>& outLayerProperties)
    {
        VkResult result;
        uint32_t count = 0;

        do
        {
            result = vkEnumerateInstanceLayerProperties(&count, nullptr);

            if (result == VK_SUCCESS)
            {
                outLayerProperties.resize(count);
                result = vkEnumerateInstanceLayerProperties(&count, outLayerProperties.data());
            }
        } while (result == VK_INCOMPLETE);

        if (result < outLayerProperties.size())
        {
            outLayerProperties.resize(count);
        }

        return result;
    }

    VkResult EnumeratePhysicalDeviceExtensionProperties(
        std::vector<VkExtensionProperties>& outExtensionProperties,
        VkPhysicalDevice device,
        const char* layerName)
    {
        VkResult result;
        uint32_t count = 0;

        do
        {
            result = vkEnumerateDeviceExtensionProperties(device, layerName, &count, nullptr);

            if (result == VK_SUCCESS)
            {
                outExtensionProperties.resize(count);
                result = vkEnumerateDeviceExtensionProperties(device, layerName, &count, outExtensionProperties.data());
            }
        } while (result == VK_INCOMPLETE);

        if (count < outExtensionProperties.size())
        {
            outExtensionProperties.resize(count);
        }

        return result;
    }

    VkResult EnumerateDeviceLayerProperties(
        std::vector<VkLayerProperties>& outLayerProperties,
        VkPhysicalDevice device)
    {
        VkResult result;
        uint32_t count = 0;

        do
        {
            result = vkEnumerateDeviceLayerProperties(device, &count, nullptr);

            if (result == VK_SUCCESS)
            {
                outLayerProperties.resize(count);
                result = vkEnumerateDeviceLayerProperties(device, &count, outLayerProperties.data());
            }
        } while (result == VK_INCOMPLETE);

        if (count < outLayerProperties.size())
        {
            outLayerProperties.resize(count);
        }

        return result;
    }

    VkResult EnumeratePhysicalDevices(
        VkInstance instance,
        std::vector<VkPhysicalDevice>& outPhysicalDevices)
    {
        VkResult result;
        uint32_t count = 0;

        do
        {
            result = vkEnumeratePhysicalDevices(instance, &count, nullptr);

            if (result == VK_SUCCESS)
            {
                outPhysicalDevices.resize(count);
                result = vkEnumeratePhysicalDevices(instance, &count, outPhysicalDevices.data());
            }
        } while (result == VK_INCOMPLETE);

        AE_ASSERT(count <= outPhysicalDevices.size());

        if (count < outPhysicalDevices.size())
        {
            outPhysicalDevices.resize(count);
        }

        return result;
    }

    
    VkResult EnumeratePhysicalDeviceQueueFamilyProperties(
        VkPhysicalDevice device,
        std::vector<VkQueueFamilyProperties>& outProperties)
    {
        uint32_t count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);

        outProperties.resize(count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &count, outProperties.data());

        if (count < outProperties.size())
        {
            outProperties.resize(count);
        }

        return VK_SUCCESS;
    }

    VkResult EnumeratePhysicalDeviceSurfaceFormats(
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

    VkResult EnumerateSurfacePresentModes(
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
