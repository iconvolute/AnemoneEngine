#include "AnemoneRenderVulkan/VulkanDevice.hxx"
#include "AnemoneRenderVulkan/VulkanError.hxx"
#include "AnemoneApplication/HostApplication.hxx"
#include "AnemoneDiagnostics/Trace.hxx"
#include "AnemoneRenderVulkan/VulkanSwapChain.hxx"
#include "AnemoneRenderVulkan/VulkanQueue.hxx"
#include "AnemoneRenderVulkan/VulkanUtils.hxx"
#include "AnemoneRenderVulkan/VulkanBuffer.hxx"
#include "AnemoneRenderVulkan/VulkanCommandList.hxx"
#include "AnemoneRenderVulkan/VulkanInstance.hxx"
#include "AnemoneRenderVulkan/VulkanVertexDeclaration.hxx"

#include "AnemoneBase/Uuid.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneApplication/Platform/Windows/WindowsHostWindow.hxx"
#endif

#include <memory>

namespace Anemone
{
    VulkanDevice::VulkanDevice()
    {
        this->CreateVulkanDevice();
    }

    VulkanDevice::~VulkanDevice()
    {
        this->DestroyVulkanDevice();
    }

    VkEvent VulkanDevice::AcquireBarrierEvent()
    {
        VkEvent handle = this->m_barrierEventsLock.With([this] -> VkEvent
        {
            if (!this->m_barrierEvents.empty())
            {
                VkEvent result = this->m_barrierEvents.back();
                this->m_barrierEvents.pop_back();
                return result;
            }

            return VK_NULL_HANDLE;
        });

        if (handle == VK_NULL_HANDLE)
        {
            constexpr VkEventCreateInfo eventCreateInfo{
                .sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
                .pNext = nullptr,
                .flags = VK_EVENT_CREATE_DEVICE_ONLY_BIT,
            };
            AE_VULKAN_ENSURE(vkCreateEvent(
                this->m_device,
                &eventCreateInfo,
                VulkanCpuAllocator,
                &handle));
        }

        return handle;
    }

    void VulkanDevice::ReleaseBarrierEvent(VkEvent event)
    {
        UniqueLock scope{this->m_barrierEventsLock};
        this->m_barrierEvents.emplace_back(event);
    }

    void VulkanDevice::ReleaseBarrierEvents(std::span<VkEvent const> events)
    {
        UniqueLock scope{this->m_barrierEventsLock};
        this->m_barrierEvents.insert(this->m_barrierEvents.end(), events.begin(), events.end());
    }

    void VulkanDevice::CreateVulkanDevice()
    {
        this->m_physicalDevice = VulkanInstance::Get().SelectPhysicalDevice(GpuVendor::Unknown);

        AE_ASSERT(this->m_physicalDevice != VK_NULL_HANDLE);

        //
        // Do basic validation of the physical device.
        //

        VkPhysicalDeviceProperties2 physicalDeviceProperties2{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
        vkGetPhysicalDeviceProperties2(this->m_physicalDevice, &physicalDeviceProperties2);

        this->m_vulkanVersion = physicalDeviceProperties2.properties.apiVersion;

        if (physicalDeviceProperties2.properties.apiVersion < VK_API_VERSION_1_3)
        {
            AE_PANIC("Device do not support required Vulkan 1.3");
        }


        if (physicalDeviceProperties2.properties.vendorID == 0x1002)
        {
            // AMD
            // TODO: Check minimum driver version.
        }

        if (physicalDeviceProperties2.properties.vendorID == 0x10DE)
        {
            this->m_driverVersion = Version{
                (physicalDeviceProperties2.properties.driverVersion >> 22u) & 0x3FFu,
                (physicalDeviceProperties2.properties.driverVersion >> 14u) & 0xFFu,
                (physicalDeviceProperties2.properties.driverVersion >> 6u) & 0xFFu,
                physicalDeviceProperties2.properties.driverVersion & 0x3Fu};
        }
        else
        {
            this->m_driverVersion = Version{
                VK_VERSION_MAJOR(physicalDeviceProperties2.properties.driverVersion),
                VK_VERSION_MINOR(physicalDeviceProperties2.properties.driverVersion),
                VK_VERSION_PATCH(physicalDeviceProperties2.properties.driverVersion),
            };
        }

        AE_TRACE(Information, "Device: {}",
            physicalDeviceProperties2.properties.deviceName);
        AE_TRACE(Information, "Device ID: {:08x}:{:08x}",
            physicalDeviceProperties2.properties.vendorID,
            physicalDeviceProperties2.properties.deviceID);
        AE_TRACE(Information, "Driver version {}",
            this->m_driverVersion);
        AE_TRACE(Information, "Vulkan API version {}.{}.{}",
            VK_API_VERSION_MAJOR(physicalDeviceProperties2.properties.apiVersion),
            VK_API_VERSION_MINOR(physicalDeviceProperties2.properties.apiVersion),
            VK_API_VERSION_PATCH(physicalDeviceProperties2.properties.apiVersion));

        std::vector<VkExtensionProperties> extensionProperties{};
        AE_VULKAN_ENSURE(QueryPhysicalDeviceExtensionProperties(this->m_physicalDevice, extensionProperties));

        //
        // First, check for supported API version and extensions.
        //


        std::vector<const char*> enabledExtensions{};

        auto hasExtension = [&](const char* extensionName, uint32_t promotedVersion) -> bool
        {
            if (this->m_vulkanVersion >= promotedVersion)
            {
                return true;
            }

            for (const VkExtensionProperties& properties : extensionProperties)
            {
                if (std::strcmp(extensionName, properties.extensionName) == 0)
                {
                    enabledExtensions.emplace_back(extensionName);
                    return true;
                }
            }

            return false;
        };

        this->m_physicalDeviceExtensions = {};

        // Vulkan 1.4
        this->m_physicalDeviceExtensions.KHR_dynamic_rendering_local_read = hasExtension(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME, VK_API_VERSION_1_4);
        this->m_physicalDeviceExtensions.KHR_global_priority = hasExtension(VK_KHR_GLOBAL_PRIORITY_EXTENSION_NAME, VK_API_VERSION_1_4);
        this->m_physicalDeviceExtensions.KHR_index_type_uint8 = hasExtension(VK_KHR_INDEX_TYPE_UINT8_EXTENSION_NAME, VK_API_VERSION_1_4);
        this->m_physicalDeviceExtensions.KHR_line_rasterization = hasExtension(VK_KHR_LINE_RASTERIZATION_EXTENSION_NAME, VK_API_VERSION_1_4);
        this->m_physicalDeviceExtensions.KHR_load_store_op_none = hasExtension(VK_KHR_LOAD_STORE_OP_NONE_EXTENSION_NAME, VK_API_VERSION_1_4);
        this->m_physicalDeviceExtensions.KHR_maintenance5 = hasExtension(VK_KHR_MAINTENANCE_5_EXTENSION_NAME, VK_API_VERSION_1_4);
        this->m_physicalDeviceExtensions.KHR_maintenance6 = hasExtension(VK_KHR_MAINTENANCE_6_EXTENSION_NAME, VK_API_VERSION_1_4);
        this->m_physicalDeviceExtensions.KHR_map_memory2 = hasExtension(VK_KHR_MAP_MEMORY_2_EXTENSION_NAME, VK_API_VERSION_1_4);
        this->m_physicalDeviceExtensions.KHR_push_descriptor = hasExtension(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME, VK_API_VERSION_1_4);
        this->m_physicalDeviceExtensions.KHR_shader_expect_assume = hasExtension(VK_KHR_SHADER_EXPECT_ASSUME_EXTENSION_NAME, VK_API_VERSION_1_4);
        this->m_physicalDeviceExtensions.KHR_shader_float_controls2 = hasExtension(VK_KHR_SHADER_FLOAT_CONTROLS_2_EXTENSION_NAME, VK_API_VERSION_1_4);
        this->m_physicalDeviceExtensions.KHR_shader_subgroup_rotate = hasExtension(VK_KHR_SHADER_SUBGROUP_ROTATE_EXTENSION_NAME, VK_API_VERSION_1_4);
        this->m_physicalDeviceExtensions.KHR_vertex_attribute_divisor = hasExtension(VK_KHR_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME, VK_API_VERSION_1_4);
        this->m_physicalDeviceExtensions.EXT_host_image_copy = hasExtension(VK_EXT_HOST_IMAGE_COPY_EXTENSION_NAME, VK_API_VERSION_1_4);
        this->m_physicalDeviceExtensions.EXT_pipeline_protected_access = hasExtension(VK_EXT_PIPELINE_PROTECTED_ACCESS_EXTENSION_NAME, VK_API_VERSION_1_4);
        this->m_physicalDeviceExtensions.EXT_pipeline_robustness = hasExtension(VK_EXT_PIPELINE_ROBUSTNESS_EXTENSION_NAME, VK_API_VERSION_1_4);

        // Optional extensions.
        this->m_physicalDeviceExtensions.KHR_swapchain = hasExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME, UINT32_MAX);
        this->m_physicalDeviceExtensions.EXT_load_store_op_none = hasExtension(VK_EXT_LOAD_STORE_OP_NONE_EXTENSION_NAME, UINT32_MAX);
        this->m_physicalDeviceExtensions.KHR_pipeline_library = hasExtension(VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME, UINT32_MAX);
        this->m_physicalDeviceExtensions.KHR_deferred_host_operations = hasExtension(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME, UINT32_MAX);
        this->m_physicalDeviceExtensions.EXT_validation_cache = hasExtension(VK_EXT_VALIDATION_CACHE_EXTENSION_NAME, UINT32_MAX);
        this->m_physicalDeviceExtensions.KHR_calibrated_timestamps = hasExtension(VK_KHR_CALIBRATED_TIMESTAMPS_EXTENSION_NAME, UINT32_MAX);
        this->m_physicalDeviceExtensions.KHR_fragment_shading_rate = hasExtension(VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME, UINT32_MAX);
        //this->m_physicalDeviceExtensions.KHR_portability_subset = hasExtension(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME, UINT32_MAX);
        this->m_physicalDeviceExtensions.KHR_external_memory_win32 = hasExtension(VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME, UINT32_MAX);
        this->m_physicalDeviceExtensions.EXT_shader_object = hasExtension(VK_EXT_SHADER_OBJECT_EXTENSION_NAME, UINT32_MAX);

        if (VulkanInstance::Get().m_instanceExtensions.KHR_get_surface_capabilities2)
        {
            this->m_physicalDeviceExtensions.EXT_full_screen_exclusive = hasExtension(VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME, UINT32_MAX);
        }

        this->m_physicalDeviceExtensions.KHR_acceleration_structure = hasExtension(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, UINT32_MAX);
        this->m_physicalDeviceExtensions.KHR_ray_query = hasExtension(VK_KHR_RAY_QUERY_EXTENSION_NAME, UINT32_MAX);
        this->m_physicalDeviceExtensions.EXT_fragment_density_map = hasExtension(VK_EXT_FRAGMENT_DENSITY_MAP_2_EXTENSION_NAME, UINT32_MAX);
        this->m_physicalDeviceExtensions.EXT_mesh_shader = hasExtension(VK_EXT_MESH_SHADER_EXTENSION_NAME, UINT32_MAX);
        this->m_physicalDeviceExtensions.KHR_ray_tracing_pipeline = hasExtension(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME, UINT32_MAX);

        this->m_physicalDeviceExtensions.EXT_device_address_binding_report = VulkanInstance::Get().m_instanceExtensions.EXT_debug_utils && hasExtension(VK_EXT_DEVICE_ADDRESS_BINDING_REPORT_EXTENSION_NAME, UINT32_MAX);


        //
        // Query physical device features.
        //
        // We enable all features that are supported by the device.
        //

        VkPhysicalDeviceFeatures2 physicalDeviceFeatures2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};

        // Vulkan 1.1
        VkPhysicalDeviceVulkan11Features physicalDeviceVulkan11Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES};
        RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDeviceVulkan11Features);

        // Vulkan 1.2
        VkPhysicalDeviceVulkan12Features physicalDeviceVulkan12Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
        RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDeviceVulkan12Features);

        // Vulkan 1.3
        VkPhysicalDeviceVulkan13Features physicalDeviceVulkan13Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
        RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDeviceVulkan13Features);

        VkPhysicalDeviceTextureCompressionASTCHDRFeatures physicalDeviceTextureCompressionASTCHDRFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_HDR_FEATURES};
        RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDeviceTextureCompressionASTCHDRFeatures);

        VkPhysicalDeviceExtendedDynamicState2FeaturesEXT physicalDeviceExtendedDynamicState2FeaturesEXT{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT};
        RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDeviceExtendedDynamicState2FeaturesEXT);

        VkPhysicalDeviceExtendedDynamicStateFeaturesEXT physicalDeviceExtendedDynamicStateFeaturesEXT{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT};
        RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDeviceExtendedDynamicStateFeaturesEXT);

        VkPhysicalDevice4444FormatsFeaturesEXT physicalDevice4444FormatsFeaturesEXT{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_4444_FORMATS_FEATURES_EXT};
        RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDevice4444FormatsFeaturesEXT);

        // Vulkan 1.4
        VkPhysicalDeviceVulkan14Features physicalDeviceVulkan14Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES};
        if (this->m_vulkanVersion >= VK_API_VERSION_1_4)
        {
            RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDeviceVulkan14Features);
        }

        VkPhysicalDeviceDynamicRenderingLocalReadFeatures physicalDeviceDynamicRenderingLocalReadFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_LOCAL_READ_FEATURES};
        if (this->m_physicalDeviceExtensions.EXT_load_store_op_none)
        {
            RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDeviceDynamicRenderingLocalReadFeatures);
        }

        VkPhysicalDeviceGlobalPriorityQueryFeatures physicalDeviceGlobalPriorityQueryFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GLOBAL_PRIORITY_QUERY_FEATURES};
        if (this->m_physicalDeviceExtensions.KHR_global_priority)
        {
            RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDeviceGlobalPriorityQueryFeatures);
        }

        VkPhysicalDeviceIndexTypeUint8Features physicalDeviceIndexTypeUint8Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES};
        if (this->m_physicalDeviceExtensions.KHR_index_type_uint8)
        {
            RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDeviceIndexTypeUint8Features);
        }

        VkPhysicalDeviceLineRasterizationFeatures physicalDeviceLineRasterizationFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES};
        if (this->m_physicalDeviceExtensions.KHR_line_rasterization)
        {
            RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDeviceLineRasterizationFeatures);
        }

        VkPhysicalDeviceMaintenance5Features physicalDeviceMaintenance5Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_5_FEATURES};
        if (this->m_physicalDeviceExtensions.KHR_maintenance5)
        {
            RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDeviceMaintenance5Features);
        }

        VkPhysicalDeviceMaintenance6Features physicalDeviceMaintenance6Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_6_FEATURES};
        if (this->m_physicalDeviceExtensions.KHR_maintenance6)
        {
            RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDeviceMaintenance6Features);
        }

        VkPhysicalDeviceShaderExpectAssumeFeatures physicalDeviceShaderExpectAssumeFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_EXPECT_ASSUME_FEATURES};
        if (this->m_physicalDeviceExtensions.KHR_shader_expect_assume)
        {
            RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDeviceShaderExpectAssumeFeatures);
        }

        VkPhysicalDeviceShaderFloatControls2Features physicalDeviceShaderFloatControls2Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT_CONTROLS_2_FEATURES};
        if (this->m_physicalDeviceExtensions.KHR_shader_float_controls2)
        {
            RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDeviceShaderFloatControls2Features);
        }

        VkPhysicalDeviceShaderSubgroupRotateFeatures physicalDeviceShaderSubgroupRotateFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_ROTATE_FEATURES};
        if (this->m_physicalDeviceExtensions.KHR_shader_subgroup_rotate)
        {
            RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDeviceShaderSubgroupRotateFeatures);
        }

        VkPhysicalDeviceVertexAttributeDivisorFeatures physicalDeviceVertexAttributeDivisorFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES};
        if (this->m_physicalDeviceExtensions.KHR_vertex_attribute_divisor)
        {
            RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDeviceVertexAttributeDivisorFeatures);
        }

        VkPhysicalDeviceHostImageCopyFeatures physicalDeviceHostImageCopyFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_IMAGE_COPY_FEATURES};
        if (this->m_physicalDeviceExtensions.EXT_host_image_copy)
        {
            RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDeviceHostImageCopyFeatures);
        }

        VkPhysicalDevicePipelineProtectedAccessFeatures physicalDevicePipelineProtectedAccessFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_PROTECTED_ACCESS_FEATURES};
        if (this->m_physicalDeviceExtensions.EXT_pipeline_protected_access)
        {
            RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDevicePipelineProtectedAccessFeatures);
        }

        VkPhysicalDevicePipelineRobustnessFeatures physicalDevicePipelineRobustnessFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_ROBUSTNESS_FEATURES};
        if (this->m_physicalDeviceExtensions.EXT_pipeline_robustness)
        {
            RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDevicePipelineRobustnessFeatures);
        }

        // Optional extensions
        VkPhysicalDeviceRayTracingPipelineFeaturesKHR physicalDeviceRayTracingPipelineFeaturesKHR{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR};
        if (this->m_physicalDeviceExtensions.KHR_ray_tracing_pipeline)
        {
            RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDeviceRayTracingPipelineFeaturesKHR);
        }

        VkPhysicalDeviceMeshShaderFeaturesEXT physicalDeviceMeshShaderFeaturesEXT{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT};
        if (this->m_physicalDeviceExtensions.EXT_mesh_shader)
        {
            RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDeviceMeshShaderFeaturesEXT);
        }

        VkPhysicalDeviceFragmentDensityMapFeaturesEXT physicalDeviceFragmentDensityMapFeaturesEXT{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_FEATURES_EXT};
        if (this->m_physicalDeviceExtensions.EXT_fragment_density_map)
        {
            RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDeviceFragmentDensityMapFeaturesEXT);
        }

        VkPhysicalDeviceRayQueryFeaturesKHR physicalDeviceRayQueryFeaturesKHR{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR};
        if (this->m_physicalDeviceExtensions.KHR_ray_query)
        {
            RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDeviceRayQueryFeaturesKHR);
        }

        VkPhysicalDeviceAccelerationStructureFeaturesKHR physicalDeviceAccelerationStructureFeaturesKHR{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR};
        if (this->m_physicalDeviceExtensions.KHR_acceleration_structure)
        {
            RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDeviceAccelerationStructureFeaturesKHR);
        }

        VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR physicalDeviceSwapchainMaintenance1FeaturesKHR{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SWAPCHAIN_MAINTENANCE_1_FEATURES_KHR};
        if (VulkanInstance::Get().m_instanceExtensions.KHR_surface_maintenance1)
        {
            RHIVulkanPushStruct(physicalDeviceFeatures2, physicalDeviceSwapchainMaintenance1FeaturesKHR);
        }

        vkGetPhysicalDeviceFeatures2(this->m_physicalDevice, &physicalDeviceFeatures2);

        // Validate physical device features.

        if (!physicalDeviceFeatures2.features.samplerAnisotropy)
        {
            AE_PANIC("Device does not support required feature: samplerAnisotropy");
        }

        if (!physicalDeviceVulkan12Features.descriptorBindingSampledImageUpdateAfterBind)
        {
            AE_PANIC("Device does not support required feature: descriptorBindingSampledImageUpdateAfterBind");
        }

        if (!physicalDeviceVulkan13Features.dynamicRendering)
        {
            AE_PANIC("Device does not support required feature: dynamicRendering");
        }

        if (!physicalDeviceExtendedDynamicStateFeaturesEXT.extendedDynamicState)
        {
            AE_PANIC("Device does not support required feature: extendedDynamicState");
        }

        if (!physicalDeviceVulkan12Features.descriptorBindingPartiallyBound)
        {
            AE_PANIC("Device does not support required feature: descriptorBindingPartiallyBound");
        }

        if (!physicalDeviceVulkan12Features.descriptorBindingVariableDescriptorCount)
        {
            AE_PANIC("Device does not support required feature: descriptorBindingVariableDescriptorCount");
        }

        if (!physicalDeviceVulkan12Features.runtimeDescriptorArray)
        {
            AE_PANIC("Device does not support required feature: runtimeDescriptorArray");
        }

        if (!physicalDeviceVulkan12Features.shaderSampledImageArrayNonUniformIndexing)
        {
            AE_PANIC("Device does not support required feature: shaderSampledImageArrayNonUniformIndexing");
        }

        if (!physicalDeviceVulkan12Features.bufferDeviceAddress)
        {
            AE_PANIC("Device does not support required feature: bufferDeviceAddress");
        }

        if (!physicalDeviceAccelerationStructureFeaturesKHR.accelerationStructure)
        {
            AE_PANIC("Device does not support required feature: accelerationStructure");
        }

        if (!physicalDeviceRayQueryFeaturesKHR.rayQuery)
        {
            AE_PANIC("Device does not support required feature: rayQuery");
        }


        // Query device properties
        {
            physicalDeviceProperties2 = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
            // Vulkan 1.4
            VkPhysicalDeviceHostImageCopyProperties physicalDeviceHostImageCopyProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_IMAGE_COPY_PROPERTIES};
            if (this->m_physicalDeviceExtensions.EXT_host_image_copy)
            {
                RHIVulkanPushStruct(physicalDeviceProperties2, physicalDeviceHostImageCopyProperties);
            }

            VkPhysicalDeviceLineRasterizationProperties physicalDeviceLineRasterizationProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_PROPERTIES};
            if (this->m_physicalDeviceExtensions.KHR_line_rasterization)
            {
                RHIVulkanPushStruct(physicalDeviceProperties2, physicalDeviceLineRasterizationProperties);
            }

            VkPhysicalDeviceMaintenance5Properties physicalDeviceMaintenance5Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_5_PROPERTIES};
            if (this->m_physicalDeviceExtensions.KHR_maintenance5)
            {
                RHIVulkanPushStruct(physicalDeviceProperties2, physicalDeviceMaintenance5Properties);
            }

            VkPhysicalDeviceMaintenance6Properties physicalDeviceMaintenance6Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_6_PROPERTIES};
            if (this->m_physicalDeviceExtensions.KHR_maintenance6)
            {
                RHIVulkanPushStruct(physicalDeviceProperties2, physicalDeviceMaintenance6Properties);
            }

            VkPhysicalDevicePipelineRobustnessProperties physicalDevicePipelineRobustnessProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_ROBUSTNESS_PROPERTIES};
            if (this->m_physicalDeviceExtensions.EXT_pipeline_robustness)
            {
                RHIVulkanPushStruct(physicalDeviceProperties2, physicalDevicePipelineRobustnessProperties);
            }

            VkPhysicalDevicePushDescriptorProperties physicalDevicePushDescriptorProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES};
            if (this->m_physicalDeviceExtensions.KHR_push_descriptor)
            {
                RHIVulkanPushStruct(physicalDeviceProperties2, physicalDevicePushDescriptorProperties);
            }

            VkPhysicalDeviceVertexAttributeDivisorProperties physicalDeviceVertexAttributeDivisorProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_PROPERTIES};
            if (this->m_physicalDeviceExtensions.KHR_vertex_attribute_divisor)
            {
                RHIVulkanPushStruct(physicalDeviceProperties2, physicalDeviceVertexAttributeDivisorProperties);
            }

            VkPhysicalDeviceVulkan14Properties physicalDeviceVulkan14Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_PROPERTIES};
            if (this->m_vulkanVersion >= VK_API_VERSION_1_4)
            {
                RHIVulkanPushStruct(physicalDeviceProperties2, physicalDeviceVulkan14Properties);
            }

            // Vulkan 1.3
            VkPhysicalDeviceVulkan13Properties physicalDeviceVulkan13Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES};
            RHIVulkanPushStruct(physicalDeviceProperties2, physicalDeviceVulkan13Properties);

            // Vulkan 1.2
            VkPhysicalDeviceVulkan12Properties physicalDeviceVulkan12Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES};
            RHIVulkanPushStruct(physicalDeviceProperties2, physicalDeviceVulkan12Properties);

            // Vulkan 1.1
            VkPhysicalDeviceVulkan11Properties physicalDeviceVulkan11Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES};
            RHIVulkanPushStruct(physicalDeviceProperties2, physicalDeviceVulkan11Properties);

            vkGetPhysicalDeviceProperties2(this->m_physicalDevice, &physicalDeviceProperties2);

            AE_TRACE(Information, "Device UUID: {}", std::bit_cast<Uuid>(physicalDeviceVulkan11Properties.deviceUUID));
            AE_TRACE(Information, "Driver UUID: {}", std::bit_cast<Uuid>(physicalDeviceVulkan11Properties.driverUUID));
        }

        std::vector<VkQueueFamilyProperties> queueFamilyProperties{};

        AE_VULKAN_ENSURE(QueryPhysicalDeviceQueueFamilyProperties(this->m_physicalDevice, queueFamilyProperties));

        std::optional<uint32_t> graphicsQueueFamilyIndex{};
        std::optional<uint32_t> computeQueueFamilyIndex{};
        std::optional<uint32_t> transferQueueFamilyIndex{};

        // Pick up queues to create
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};

        uint32_t prioritiesCount = 0;
        uint32_t familyIndex = 0;

        for (const VkQueueFamilyProperties& queueFamily : queueFamilyProperties)
        {
            bool valid = false;

            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                if (not graphicsQueueFamilyIndex)
                {
                    graphicsQueueFamilyIndex = familyIndex;
                    valid = true;
                }
            }

            if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
                if (not computeQueueFamilyIndex)
                {
                    if (graphicsQueueFamilyIndex != familyIndex)
                    {
                        computeQueueFamilyIndex = familyIndex;
                        valid = true;
                    }
                }
            }

            if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
            {
                if (not transferQueueFamilyIndex)
                {
                    if (!(queueFamily.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)))
                    {
                        transferQueueFamilyIndex = familyIndex;
                        valid = true;
                    }
                }
            }

            if (valid)
            {
                VkDeviceQueueCreateInfo& createInfo = queueCreateInfos.emplace_back();
                createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                createInfo.pNext = nullptr;
                createInfo.flags = 0;
                createInfo.queueFamilyIndex = familyIndex;
                createInfo.queueCount = queueFamily.queueCount;

                prioritiesCount += createInfo.queueCount;
            }
            else
            {
#if ANEMONE_VULKAN_VALIDATION
                AE_TRACE(Error, "Vulkan queue family {} has unsupported flags: {}",
                    familyIndex,
                    string_VkQueueFlags(queueFamily.queueFlags));
#endif
            }

            ++familyIndex;
        }

        std::vector<float> queuePriorities{};
        queuePriorities.resize(prioritiesCount);
        float* currentPriority = queuePriorities.data();

        for (auto& currentQueue : queueCreateInfos)
        {
            currentQueue.pQueuePriorities = currentPriority;

            const VkQueueFamilyProperties& currentProperties = queueFamilyProperties[currentQueue.queueFamilyIndex];

            for (uint32_t index = 0; index < currentProperties.queueCount; ++index)
            {
                *currentPriority++ = 1.0f;
            }
        }

        // Prepare list of features to enable
        VkDeviceCreateInfo deviceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
            .pQueueCreateInfos = queueCreateInfos.data(),
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size()),
            .ppEnabledExtensionNames = enabledExtensions.data(),
            .pEnabledFeatures = nullptr,
        };

        VkPhysicalDeviceFeatures2 enabledPhysicalDeviceFeatures2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
        enabledPhysicalDeviceFeatures2.features.samplerAnisotropy = VK_TRUE;
        RHIVulkanPushStruct(deviceCreateInfo, enabledPhysicalDeviceFeatures2);

        VkPhysicalDeviceVulkan11Features enabledPhysicalDeviceVulkan11Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES};
        RHIVulkanPushStruct(deviceCreateInfo, enabledPhysicalDeviceVulkan11Features);

        VkPhysicalDeviceVulkan12Features enabledPhysicalDeviceVulkan12Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
        enabledPhysicalDeviceVulkan12Features.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
        enabledPhysicalDeviceVulkan12Features.descriptorBindingPartiallyBound = VK_TRUE;
        enabledPhysicalDeviceVulkan12Features.descriptorBindingVariableDescriptorCount = VK_TRUE;
        enabledPhysicalDeviceVulkan12Features.runtimeDescriptorArray = VK_TRUE;
        enabledPhysicalDeviceVulkan12Features.timelineSemaphore = VK_TRUE;
        RHIVulkanPushStruct(deviceCreateInfo, enabledPhysicalDeviceVulkan12Features);

        VkPhysicalDeviceVulkan13Features enabledPhysicalDeviceVulkan13Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
        enabledPhysicalDeviceVulkan13Features.dynamicRendering = VK_TRUE;
        enabledPhysicalDeviceVulkan13Features.synchronization2 = VK_TRUE;
        RHIVulkanPushStruct(deviceCreateInfo, enabledPhysicalDeviceVulkan13Features);

        AE_VULKAN_ENSURE(vkCreateDevice(this->m_physicalDevice, &deviceCreateInfo, VulkanCpuAllocator, &this->m_device));

        volkLoadDevice(this->m_device);

        this->m_graphicsQueue = MakeReference<VulkanQueue>(
            *this,
            VulkanQueueType::Graphics,
            *graphicsQueueFamilyIndex,
            queueFamilyProperties[*graphicsQueueFamilyIndex].queueFlags);

        this->m_activeQueueFamilies.emplace_back(*graphicsQueueFamilyIndex);

        if (computeQueueFamilyIndex)
        {
            this->m_computeQueue = MakeReference<VulkanQueue>(
                *this,
                VulkanQueueType::Compute,
                *computeQueueFamilyIndex,

                queueFamilyProperties[*computeQueueFamilyIndex].queueFlags);
            this->m_activeQueueFamilies.emplace_back(*computeQueueFamilyIndex);
        }

        if (transferQueueFamilyIndex)
        {
            this->m_transferQueue = MakeReference<VulkanQueue>(
                *this,
                VulkanQueueType::Transfer,
                *transferQueueFamilyIndex,
                queueFamilyProperties[*transferQueueFamilyIndex].queueFlags);
            this->m_activeQueueFamilies.emplace_back(*transferQueueFamilyIndex);
        }

        //
        // Create immediate context.
        //
        this->m_immediateContext = MakeReference<VulkanCommandListContext>(
            *this,
            *this->m_graphicsQueue,
            GpuPipeline::Graphics,
            nullptr);


        //
        // Initialize VulkanMemoryAllocator.
        //

        VmaVulkanFunctions vmaVulkanFunctions{
            .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
            .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
        };

        VmaAllocatorCreateInfo vmaCreateInfo{
            .flags = 0,
            .physicalDevice = this->m_physicalDevice,
            .device = this->m_device,
            .preferredLargeHeapBlockSize = 0,
            .pAllocationCallbacks = VulkanCpuAllocator,
            .pDeviceMemoryCallbacks = nullptr,
            .pHeapSizeLimit = nullptr,
            .pVulkanFunctions = &vmaVulkanFunctions,
            .instance = VulkanInstance::Get().m_instance,
            .vulkanApiVersion = VK_API_VERSION_1_3,
            .pTypeExternalMemoryHandleTypes = nullptr,
        };

        AE_VULKAN_ENSURE(vmaCreateAllocator(&vmaCreateInfo, &this->m_allocator));
    }

    void VulkanDevice::DestroyVulkanDevice()
    {
        // Release cached objects
        this->m_vertexDeclarations.ForEach([](VulkanVertexDeclaration const& declaration)
        {
            declaration.ReleaseReference();
        });

        this->m_vertexDeclarations.Clear();

        for (VkEvent event : this->m_barrierEvents)
        {
            vkDestroyEvent(
                this->m_device,
                event,
                VulkanCpuAllocator);
        }

        this->m_immediateContext = {};

        this->m_transferQueue = {};
        this->m_computeQueue = {};
        this->m_graphicsQueue = {};

        vmaDestroyAllocator(this->m_allocator);
        this->m_allocator = VK_NULL_HANDLE;

        vkDestroyDevice(this->m_device, VulkanCpuAllocator);
        this->m_device = VK_NULL_HANDLE;

        this->m_physicalDevice = VK_NULL_HANDLE;
    }

    VkResult VulkanDevice::QueryPhysicalDeviceExtensionProperties(
        VkPhysicalDevice device,
        std::vector<VkExtensionProperties>& outExtensionProperties,
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

    VkResult VulkanDevice::QueryPhysicalDeviceLayerProperties(
        VkPhysicalDevice device,
        std::vector<VkLayerProperties>& outLayerProperties)
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

    VkResult VulkanDevice::QueryPhysicalDeviceQueueFamilyProperties(
        VkPhysicalDevice physicalDevice,
        std::vector<VkQueueFamilyProperties>& outQueueFamilyProperties)
    {
        uint32_t count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);

        outQueueFamilyProperties.resize(count);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, outQueueFamilyProperties.data());

        if (count < outQueueFamilyProperties.size())
        {
            outQueueFamilyProperties.resize(count);
        }

        return VK_SUCCESS;
    }
}

namespace Anemone
{
    RENDER_VULKAN_API Reference<GpuDevice> CreateGpuDevice()
    {
        return MakeReference<VulkanDevice>();
    }
}
