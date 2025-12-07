#include "AnemoneRenderVulkan/VulkanPhysicalDevice.hxx"
#include "AnemoneRenderVulkan/VulkanUtils.hxx"

namespace Anemone::VulkanGraphics
{
    template <typename Head, typename Tail>
    constexpr void ChainStructs(Head& head, Tail& tail)
    {
        tail.pNext = head.pNext;
        head.pNext = &tail;
    }

    PhysicalDevice::~PhysicalDevice() = default;

    VkResult PhysicalDevice::Initialize()
    {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(this->m_device, &physicalDeviceProperties);

        if (physicalDeviceProperties.apiVersion < VK_API_VERSION_1_4)
        {
            return VK_ERROR_FEATURE_NOT_PRESENT;
        }
        if (physicalDeviceProperties.apiVersion < VK_API_VERSION_1_1)
        {
            return VK_ERROR_FEATURE_NOT_PRESENT;
        }

        std::vector<VkExtensionProperties> extensionProperties{};

        auto supportExtension = [&](const char* name) -> bool
        {
            for (VkExtensionProperties const& properties : extensionProperties)
            {
                if (std::strcmp(name, properties.extensionName) == 0)
                {
                    return true;
                }
            }

            return false;
        };

        // Extensions
        {
            EnumeratePhysicalDeviceExtensionProperties(extensionProperties, this->m_device);

            // Vulkan 1.4
            this->m_extensions.KHR_DYNAMIC_RENDERING_LOCAL_READ = supportExtension(VK_KHR_DYNAMIC_RENDERING_LOCAL_READ_EXTENSION_NAME);
            this->m_extensions.KHR_GLOBAL_PRIORITY = supportExtension(VK_KHR_GLOBAL_PRIORITY_EXTENSION_NAME);
            this->m_extensions.KHR_INDEX_TYPE_UINT8 = supportExtension(VK_KHR_INDEX_TYPE_UINT8_EXTENSION_NAME);
            this->m_extensions.KHR_LINE_RASTERIZATION = supportExtension(VK_KHR_LINE_RASTERIZATION_EXTENSION_NAME);
            this->m_extensions.KHR_LOAD_STORE_OP_NONE = supportExtension(VK_KHR_LOAD_STORE_OP_NONE_EXTENSION_NAME);
            this->m_extensions.KHR_MAINTENANCE_5 = supportExtension(VK_KHR_MAINTENANCE_5_EXTENSION_NAME);
            this->m_extensions.KHR_MAINTENANCE_6 = supportExtension(VK_KHR_MAINTENANCE_6_EXTENSION_NAME);
            this->m_extensions.KHR_MAP_MEMORY_2 = supportExtension(VK_KHR_MAP_MEMORY_2_EXTENSION_NAME);
            this->m_extensions.KHR_PUSH_DESCRIPTOR = supportExtension(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME);
            this->m_extensions.KHR_SHADER_EXPECT_ASSUME = supportExtension(VK_KHR_SHADER_EXPECT_ASSUME_EXTENSION_NAME);
            this->m_extensions.KHR_SHADER_FLOAT_CONTROLS_2 = supportExtension(VK_KHR_SHADER_FLOAT_CONTROLS_2_EXTENSION_NAME);
            this->m_extensions.KHR_SHADER_SUBGROUP_ROTATE = supportExtension(VK_KHR_SHADER_SUBGROUP_ROTATE_EXTENSION_NAME);
            this->m_extensions.KHR_VERTEX_ATTRIBUTE_DIVISOR = supportExtension(VK_KHR_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME);
            this->m_extensions.EXT_HOST_IMAGE_COPY = supportExtension(VK_EXT_HOST_IMAGE_COPY_EXTENSION_NAME);
            this->m_extensions.EXT_PIPELINE_PROTECTED_ACCESS = supportExtension(VK_EXT_PIPELINE_PROTECTED_ACCESS_EXTENSION_NAME);
            this->m_extensions.EXT_PIPELINE_ROBUSTNESS = supportExtension(VK_EXT_PIPELINE_ROBUSTNESS_EXTENSION_NAME);


            // Vulkan 1.3
            this->m_extensions.KHR_COPY_COMMANDS_2 = supportExtension(VK_KHR_COPY_COMMANDS_2_EXTENSION_NAME);
            this->m_extensions.KHR_DYNAMIC_RENDERING = supportExtension(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
            this->m_extensions.KHR_FORMAT_FEATURE_FLAGS_2 = supportExtension(VK_KHR_FORMAT_FEATURE_FLAGS_2_EXTENSION_NAME);
            this->m_extensions.KHR_MAINTENANCE_4 = supportExtension(VK_KHR_MAINTENANCE_4_EXTENSION_NAME);
            this->m_extensions.KHR_SHADER_INTEGER_DOT_PRODUCT = supportExtension(VK_KHR_SHADER_INTEGER_DOT_PRODUCT_EXTENSION_NAME);
            this->m_extensions.KHR_SHADER_NON_SEMANTIC_INFO = supportExtension(VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME);
            this->m_extensions.KHR_SHADER_TERMINATE_INVOCATION = supportExtension(VK_KHR_SHADER_TERMINATE_INVOCATION_EXTENSION_NAME);
            this->m_extensions.KHR_SYNCHRONIZATION_2 = supportExtension(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);
            this->m_extensions.KHR_ZERO_INITIALIZE_WORKGROUP_MEMORY = supportExtension(VK_KHR_ZERO_INITIALIZE_WORKGROUP_MEMORY_EXTENSION_NAME);
            this->m_extensions.EXT_4444_FORMATS = supportExtension(VK_EXT_4444_FORMATS_EXTENSION_NAME);
            this->m_extensions.EXT_EXTENDED_DYNAMIC_STATE = supportExtension(VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME);
            this->m_extensions.EXT_EXTENDED_DYNAMIC_STATE_2 = supportExtension(VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME);
            this->m_extensions.EXT_IMAGE_ROBUSTNESS = supportExtension(VK_EXT_IMAGE_ROBUSTNESS_EXTENSION_NAME);
            this->m_extensions.EXT_INLINE_UNIFORM_BLOCK = supportExtension(VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME);
            this->m_extensions.EXT_PIPELINE_CREATION_CACHE_CONTROL = supportExtension(VK_EXT_PIPELINE_CREATION_CACHE_CONTROL_EXTENSION_NAME);
            this->m_extensions.EXT_PIPELINE_CREATION_FEEDBACK = supportExtension(VK_EXT_PIPELINE_CREATION_FEEDBACK_EXTENSION_NAME);
            this->m_extensions.EXT_PRIVATE_DATA = supportExtension(VK_EXT_PRIVATE_DATA_EXTENSION_NAME);
            this->m_extensions.EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION = supportExtension(VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME);
            this->m_extensions.EXT_SUBGROUP_SIZE_CONTROL = supportExtension(VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME);
            this->m_extensions.EXT_TEXEL_BUFFER_ALIGNMENT = supportExtension(VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME);
            this->m_extensions.EXT_TEXTURE_COMPRESSION_ASTC_HDR = supportExtension(VK_EXT_TEXTURE_COMPRESSION_ASTC_HDR_EXTENSION_NAME);
            this->m_extensions.EXT_TOOLING_INFO = supportExtension(VK_EXT_TOOLING_INFO_EXTENSION_NAME);
            this->m_extensions.EXT_YCBCR_2PLANE_444_FORMATS = supportExtension(VK_EXT_YCBCR_2PLANE_444_FORMATS_EXTENSION_NAME);

            // Vulkan 1.2
            this->m_extensions.KHR_8BIT_STORAGE = supportExtension(VK_KHR_8BIT_STORAGE_EXTENSION_NAME);
            this->m_extensions.KHR_BUFFER_DEVICE_ADDRESS = supportExtension(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
            this->m_extensions.KHR_CREATE_RENDERPASS_2 = supportExtension(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME);
            this->m_extensions.KHR_DEPTH_STENCIL_RESOLVE = supportExtension(VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME);
            this->m_extensions.KHR_DRAW_INDIRECT_COUNT = supportExtension(VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME);
            this->m_extensions.KHR_DRIVER_PROPERTIES = supportExtension(VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME);
            this->m_extensions.KHR_IMAGE_FORMAT_LIST = supportExtension(VK_KHR_IMAGE_FORMAT_LIST_EXTENSION_NAME);
            this->m_extensions.KHR_IMAGELESS_FRAMEBUFFER = supportExtension(VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME);
            this->m_extensions.KHR_SAMPLER_MIRROR_CLAMP_TO_EDGE = supportExtension(VK_KHR_SAMPLER_MIRROR_CLAMP_TO_EDGE_EXTENSION_NAME);
            this->m_extensions.KHR_SEPARATE_DEPTH_STENCIL_LAYOUTS = supportExtension(VK_KHR_SEPARATE_DEPTH_STENCIL_LAYOUTS_EXTENSION_NAME);
            this->m_extensions.KHR_SHADER_ATOMIC_INT64 = supportExtension(VK_KHR_SHADER_ATOMIC_INT64_EXTENSION_NAME);
            this->m_extensions.KHR_SHADER_FLOAT16_INT8 = supportExtension(VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME);
            this->m_extensions.KHR_SHADER_FLOAT_CONTROLS = supportExtension(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME);
            this->m_extensions.KHR_SHADER_SUBGROUP_EXTENDED_TYPES = supportExtension(VK_KHR_SHADER_SUBGROUP_EXTENDED_TYPES_EXTENSION_NAME);
            this->m_extensions.KHR_SPIRV_1_4 = supportExtension(VK_KHR_SPIRV_1_4_EXTENSION_NAME);
            this->m_extensions.KHR_TIMELINE_SEMAPHORE = supportExtension(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME);
            this->m_extensions.KHR_UNIFORM_BUFFER_STANDARD_LAYOUT = supportExtension(VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT_EXTENSION_NAME);
            this->m_extensions.KHR_VULKAN_MEMORY_MODEL = supportExtension(VK_KHR_VULKAN_MEMORY_MODEL_EXTENSION_NAME);
            this->m_extensions.EXT_DESCRIPTOR_INDEXING = supportExtension(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
            this->m_extensions.EXT_HOST_QUERY_RESET = supportExtension(VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME);
            this->m_extensions.EXT_SAMPLER_FILTER_MINMAX = supportExtension(VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME);
            this->m_extensions.EXT_SCALAR_BLOCK_LAYOUT = supportExtension(VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME);
            this->m_extensions.EXT_SEPARATE_STENCIL_USAGE = supportExtension(VK_EXT_SEPARATE_STENCIL_USAGE_EXTENSION_NAME);
            this->m_extensions.EXT_SHADER_VIEWPORT_INDEX_LAYER = supportExtension(VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME);

            // Vulkan 1.1
            this->m_extensions.KHR_16BIT_STORAGE = supportExtension(VK_KHR_16BIT_STORAGE_EXTENSION_NAME);
            this->m_extensions.KHR_BIND_MEMORY_2 = supportExtension(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME);
            this->m_extensions.KHR_DEDICATED_ALLOCATION = supportExtension(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
            this->m_extensions.KHR_DESCRIPTOR_UPDATE_TEMPLATE = supportExtension(VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_EXTENSION_NAME);
            this->m_extensions.KHR_DEVICE_GROUP = supportExtension(VK_KHR_DEVICE_GROUP_EXTENSION_NAME);
            this->m_extensions.KHR_DEVICE_GROUP_CREATION = supportExtension(VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME);
            this->m_extensions.KHR_EXTERNAL_FENCE = supportExtension(VK_KHR_EXTERNAL_FENCE_EXTENSION_NAME);
            this->m_extensions.KHR_EXTERNAL_FENCE_CAPABILITIES = supportExtension(VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME);
            this->m_extensions.KHR_EXTERNAL_MEMORY = supportExtension(VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME);
            this->m_extensions.KHR_EXTERNAL_MEMORY_CAPABILITIES = supportExtension(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME);
            this->m_extensions.KHR_EXTERNAL_SEMAPHORE = supportExtension(VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME);
            this->m_extensions.KHR_EXTERNAL_SEMAPHORE_CAPABILITIES = supportExtension(VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME);
            this->m_extensions.KHR_GET_MEMORY_REQUIREMENTS_2 = supportExtension(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
            this->m_extensions.KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2 = supportExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
            this->m_extensions.KHR_MAINTENANCE_1 = supportExtension(VK_KHR_MAINTENANCE_1_EXTENSION_NAME);
            this->m_extensions.KHR_MAINTENANCE_2 = supportExtension(VK_KHR_MAINTENANCE_2_EXTENSION_NAME);
            this->m_extensions.KHR_MAINTENANCE_3 = supportExtension(VK_KHR_MAINTENANCE_3_EXTENSION_NAME);
            this->m_extensions.KHR_MULTIVIEW = supportExtension(VK_KHR_MULTIVIEW_EXTENSION_NAME);
            this->m_extensions.KHR_RELAXED_BLOCK_LAYOUT = supportExtension(VK_KHR_RELAXED_BLOCK_LAYOUT_EXTENSION_NAME);
            this->m_extensions.KHR_SAMPLER_YCBCR_CONVERSION = supportExtension(VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME);
            this->m_extensions.KHR_SHADER_DRAW_PARAMETERS = supportExtension(VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME);
            this->m_extensions.KHR_STORAGE_BUFFER_STORAGE_CLASS = supportExtension(VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME);
            this->m_extensions.KHR_VARIABLE_POINTERS = supportExtension(VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME);
        }

        // Device properties
        {
            VkPhysicalDeviceProperties2 physicalDeviceProperties2{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
                .pNext = nullptr,
            };

            // VK_KHR_line_rasterization
            VkPhysicalDeviceLineRasterizationProperties physicalDeviceLineRasterizationProperties{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_PROPERTIES,
            };

            if (this->m_extensions.KHR_LINE_RASTERIZATION)
            {
                ChainStructs(physicalDeviceProperties2, physicalDeviceLineRasterizationProperties);
            }

            // VK_KHR_maintenance5
            VkPhysicalDeviceMaintenance5Properties physicalDeviceMaintenance5Properties{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_5_PROPERTIES,
            };

            if (this->m_extensions.KHR_MAINTENANCE_5)
            {
                ChainStructs(physicalDeviceProperties2, physicalDeviceMaintenance5Properties);
            }

            // VK_KHR_maintenance6
            VkPhysicalDeviceMaintenance6Properties physicalDeviceMaintenance6Properties{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_6_PROPERTIES,
            };

            if (this->m_extensions.KHR_MAINTENANCE_6)
            {
                ChainStructs(physicalDeviceProperties2, physicalDeviceMaintenance6Properties);
            }

            // VK_KHR_push_descriptor
            VkPhysicalDevicePushDescriptorProperties physicalDevicePushDescriptorProperties{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES,
            };

            if (this->m_extensions.KHR_PUSH_DESCRIPTOR)
            {
                ChainStructs(physicalDeviceProperties2, physicalDevicePushDescriptorProperties);
            }


            // VK_KHR_vertex_attribute_divisor
            VkPhysicalDeviceVertexAttributeDivisorProperties physicalDeviceVertexAttributeDivisorProperties{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_PROPERTIES,
            };

            if (this->m_extensions.KHR_VERTEX_ATTRIBUTE_DIVISOR)
            {
                ChainStructs(physicalDeviceProperties2, physicalDeviceVertexAttributeDivisorProperties);
            }

            // VK_EXT_host_image_copy
            VkPhysicalDeviceHostImageCopyProperties physicalDeviceHostImageCopyProperties{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_IMAGE_COPY_PROPERTIES,
            };

            if (this->m_extensions.EXT_HOST_IMAGE_COPY)
            {
                ChainStructs(physicalDeviceProperties2, physicalDeviceHostImageCopyProperties);
            }

            // VK_EXT_pipeline_robustness
            VkPhysicalDevicePipelineRobustnessProperties physicalDevicePipelineRobustnessProperties{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_ROBUSTNESS_PROPERTIES,
            };

            if (this->m_extensions.EXT_PIPELINE_ROBUSTNESS)
            {
                ChainStructs(physicalDeviceProperties2, physicalDevicePipelineRobustnessProperties);
            }


            // Vulkan 1.3
            // VK_KHR_maintenance4
            VkPhysicalDeviceMaintenance4Properties physicalDeviceMaintenance4Properties{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_PROPERTIES,
            };

            if (this->m_extensions.KHR_MAINTENANCE_4)
            {
                ChainStructs(physicalDeviceProperties2, physicalDeviceMaintenance4Properties);
            }

            // VK_KHR_shader_integer_dot_product
            VkPhysicalDeviceShaderIntegerDotProductProperties physicalDeviceShaderIntegerDotProductProperties{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_PROPERTIES,
            };
            if (this->m_extensions.KHR_SHADER_INTEGER_DOT_PRODUCT)
            {
                ChainStructs(physicalDeviceProperties2, physicalDeviceShaderIntegerDotProductProperties);
            }

            // VK_EXT_inline_uniform_block
            VkPhysicalDeviceInlineUniformBlockProperties physicalDeviceInlineUniformBlockProperties{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_PROPERTIES,
            };
            if (this->m_extensions.EXT_INLINE_UNIFORM_BLOCK)
            {
                ChainStructs(physicalDeviceProperties2, physicalDeviceInlineUniformBlockProperties);
            }

            // VK_EXT_subgroup_size_control
            VkPhysicalDeviceSubgroupSizeControlProperties physicalDeviceSubgroupSizeControlProperties{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES,
            };
            if (this->m_extensions.EXT_SUBGROUP_SIZE_CONTROL)
            {
                ChainStructs(physicalDeviceProperties2, physicalDeviceSubgroupSizeControlProperties);
            }

            // VK_EXT_texel_buffer_alignment
            VkPhysicalDeviceTexelBufferAlignmentProperties physicalDeviceTexelBufferAlignmentProperties{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_PROPERTIES,
            };
            if (this->m_extensions.EXT_TEXEL_BUFFER_ALIGNMENT)
            {
                ChainStructs(physicalDeviceProperties2, physicalDeviceTexelBufferAlignmentProperties);
            }

            // Vulkan 1.2
            // VK_KHR_depth_stencil_resolve
            VkPhysicalDeviceDepthStencilResolveProperties physicalDeviceDepthStencilResolveProperties{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES,
            };
            if (this->m_extensions.KHR_DEPTH_STENCIL_RESOLVE)
            {
                ChainStructs(physicalDeviceProperties2, physicalDeviceDepthStencilResolveProperties);
            }

            // VK_KHR_driver_properties
            VkPhysicalDeviceDriverProperties physicalDeviceDriverProperties{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES,
            };
            if (this->m_extensions.KHR_DRIVER_PROPERTIES)
            {
                ChainStructs(physicalDeviceProperties2, physicalDeviceDriverProperties);
            }

            // VK_KHR_shader_float_controls
            VkPhysicalDeviceFloatControlsProperties physicalDeviceFloatControlsProperties{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT_CONTROLS_PROPERTIES,
            };
            if (this->m_extensions.KHR_SHADER_FLOAT_CONTROLS)
            {
                ChainStructs(physicalDeviceProperties2, physicalDeviceFloatControlsProperties);
            }

            // VK_KHR_timeline_semaphore
            VkPhysicalDeviceTimelineSemaphoreProperties physicalDeviceTimelineSemaphoreProperties{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_PROPERTIES,
            };
            if (this->m_extensions.KHR_TIMELINE_SEMAPHORE)
            {
                ChainStructs(physicalDeviceProperties2, physicalDeviceTimelineSemaphoreProperties);
            }

            // VK_EXT_descriptor_indexing
            VkPhysicalDeviceDescriptorIndexingProperties physicalDeviceDescriptorIndexingProperties{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES,
            };
            if (this->m_extensions.EXT_DESCRIPTOR_INDEXING)
            {
                ChainStructs(physicalDeviceProperties2, physicalDeviceDescriptorIndexingProperties);
            }

            // VK_EXT_sampler_filter_minmax
            VkPhysicalDeviceSamplerFilterMinmaxProperties physicalDeviceSamplerFilterMinmaxProperties{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_FILTER_MINMAX_PROPERTIES,
            };
            if (this->m_extensions.EXT_SAMPLER_FILTER_MINMAX)
            {
                ChainStructs(physicalDeviceProperties2, physicalDeviceSamplerFilterMinmaxProperties);
            }

            // Vulkan 1.1
            // VK_KHR_device_group_creation
            VkPhysicalDeviceGroupProperties physicalDeviceGroupProperties{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES,
            };
            if (this->m_extensions.KHR_DEVICE_GROUP_CREATION)
            {
                ChainStructs(physicalDeviceProperties2, physicalDeviceGroupProperties);
            }

            // VK_KHR_external_memory_capabilities
            VkPhysicalDeviceIDProperties physicalDeviceIDProperties{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES,
            };
            if (this->m_extensions.KHR_EXTERNAL_MEMORY_CAPABILITIES)
            {
                ChainStructs(physicalDeviceProperties2, physicalDeviceIDProperties);
            }

            // VK_KHR_maintenance2
            VkPhysicalDevicePointClippingProperties physicalDevicePointClippingProperties{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_POINT_CLIPPING_PROPERTIES,
            };
            if (this->m_extensions.KHR_MAINTENANCE_2)
            {
                ChainStructs(physicalDeviceProperties2, physicalDevicePointClippingProperties);
            }

            // VK_KHR_maintenance3
            VkPhysicalDeviceMaintenance3Properties physicalDeviceMaintenance3Properties{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES,
            };
            if (this->m_extensions.KHR_MAINTENANCE_3)
            {
                ChainStructs(physicalDeviceProperties2, physicalDeviceMaintenance3Properties);
            }

            // VK_KHR_multiview
            VkPhysicalDeviceMultiviewProperties physicalDeviceMultiviewProperties{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES,
            };
            if (this->m_extensions.KHR_MULTIVIEW)
            {
                ChainStructs(physicalDeviceProperties2, physicalDeviceMultiviewProperties);
            }

            vkGetPhysicalDeviceProperties2(this->m_device, &physicalDeviceProperties2);


            // VK_EXT_tooling_info
            uint32_t num_tools;
            vkGetPhysicalDeviceToolProperties(this->m_device, &num_tools, nullptr);
#if false
            std::vector<VkPhysicalDeviceToolProperties> toolProperties{};
            toolProperties.resize(num_tools, VkPhysicalDeviceToolProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TOOL_PROPERTIES});

            vkGetPhysicalDeviceToolPropertiesEXT(this->m_device, &num_tools, toolProperties.data());

            for (uint32_t i = 0; i < num_tools; ++i)
            {
                printf("%s:\n", toolProperties[i].name);
                printf("Version:\n");
                printf("%s:\n", toolProperties[i].version);
                printf("Description:\n");
                printf("\t%s\n", toolProperties[i].description);
                printf("Purposes:\n");
                printf("\t%s\n", string_VkToolPurposeFlags(toolProperties[i].purposes).c_str());
                if (strnlen_s(toolProperties[i].layer, VK_MAX_EXTENSION_NAME_SIZE) > 0)
                {
                    printf("Corresponding Layer:\n");
                    printf("\t%s\n", toolProperties[i].layer);
                }
            }
#endif
        }

        // Device features
        {
        }

        return VK_SUCCESS;
    }

    VkResult PhysicalDevice::Create(VkInstance instance, VkPhysicalDevice device, Reference<PhysicalDevice>& outPhysicalDevice)
    {
        (void)instance;
        AE_ASSERT(instance != VK_NULL_HANDLE);
        AE_ASSERT(device != VK_NULL_HANDLE);

        auto physicalDevice = MakeReference<PhysicalDevice>(device);

        VkResult const result = physicalDevice->Initialize();

        if (result != VK_SUCCESS)
        {
            physicalDevice = {};
        }

        outPhysicalDevice = physicalDevice;

        return result;
    }
}
