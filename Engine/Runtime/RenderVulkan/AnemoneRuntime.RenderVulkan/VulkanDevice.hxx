#pragma once
#include "AnemoneRuntime.Render/Gpu.hxx"
#include "AnemoneRuntime.Render/Device.hxx"
#include "AnemoneRuntime.RenderVulkan/VulkanHeaders.hxx"
#include "AnemoneRuntime.RenderVulkan/VulkanUtils.hxx"
#include "AnemoneRuntime.Base/Version.hxx"
#include "AnemoneRuntime.Threading/CriticalSection.hxx"

namespace Anemone
{
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

    // Verify https://docs.vulkan.org/spec/latest/appendices/versions.html for extensions
    // promoted to given Vulkan version.
    //
    // This renderer requires Vulkan 1.3+ support.

    struct VulkanPhysicalDeviceExtensions
    {
        // Vulkan 1.4
        bool KHR_dynamic_rendering_local_read;
        bool KHR_global_priority;
        bool KHR_index_type_uint8;
        bool KHR_line_rasterization;
        bool KHR_load_store_op_none;
        bool KHR_maintenance5;
        bool KHR_maintenance6;
        bool KHR_map_memory2;
        bool KHR_push_descriptor;
        bool KHR_shader_expect_assume;
        bool KHR_shader_float_controls2;
        bool KHR_shader_subgroup_rotate;
        bool KHR_vertex_attribute_divisor;
        bool EXT_host_image_copy;
        bool EXT_pipeline_protected_access;
        bool EXT_pipeline_robustness;

        // Optional extensions.
        bool KHR_swapchain;
        bool EXT_load_store_op_none;
        bool KHR_pipeline_library;
        bool KHR_deferred_host_operations;
        bool EXT_validation_cache;
        bool KHR_calibrated_timestamps;
        bool KHR_fragment_shading_rate;
        bool KHR_get_surface_capabilities2;
        bool KHR_portability_subset;
        bool EXT_device_address_binding_report;
        bool KHR_external_memory_win32;
        bool EXT_full_screen_exclusive;
        bool EXT_shader_object;

        bool KHR_acceleration_structure;
        bool KHR_ray_query;
        bool EXT_fragment_density_map;
        bool EXT_mesh_shader;
        bool KHR_ray_tracing_pipeline;
    };

    // Features enabled on the device.
    struct VulkanDeviceFeatures
    {
        bool SeparateDepthStencilLayouts;
        bool Synchronization2;
        bool DynamicRendering;
        bool RenderPass2;
        bool GeometryShader;
        bool FragmentDensityMap;
        bool MeshShader;
        bool AccelerationStructure;
        bool RayTracingPipeline;
        bool BindlessRendering;
        bool FragmentShadingRate;
    };

    struct VulkanDeviceCapabilities
    {
        bool MeshShader;
        bool AccelerationStructures;
        bool RayTracingPipeline;
        bool BindlessRendering;
    };

    struct VulkanPhysicalDeviceFeatures
    {
        // Vulkan 1.1
        VkPhysicalDevice16BitStorageFeatures physicalDevice16BitStorageFeatures;
        VkPhysicalDeviceMultiviewFeatures physicalDeviceMultiviewFeatures;
        VkPhysicalDeviceSamplerYcbcrConversionFeatures physicalDeviceSamplerYcbcrConversionFeatures;
        VkPhysicalDeviceVariablePointersFeatures physicalDeviceVariablePointersFeatures;

        // Vulkan 1.2
        VkPhysicalDevice8BitStorageFeatures physicalDevice8BitStorageFeatures;
        VkPhysicalDeviceBufferDeviceAddressFeatures physicalDeviceBufferDeviceAddressFeatures;
        VkPhysicalDeviceImagelessFramebufferFeatures physicalDeviceImagelessFramebufferFeatures;
        VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures physicalDeviceSeparateDepthStencilLayoutsFeatures;
        VkPhysicalDeviceShaderAtomicInt64Features physicalDeviceShaderAtomicInt64Features;
        VkPhysicalDeviceShaderFloat16Int8Features physicalDeviceShaderFloat16Int8Features;
        VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures physicalDeviceShaderSubgroupExtendedTypesFeatures;
        VkPhysicalDeviceTimelineSemaphoreFeatures physicalDeviceTimelineSemaphoreFeatures;
        VkPhysicalDeviceUniformBufferStandardLayoutFeatures physicalDeviceUniformBufferStandardLayoutFeatures;
        VkPhysicalDeviceVulkanMemoryModelFeatures physicalDeviceVulkanMemoryModelFeatures;
        VkPhysicalDeviceDescriptorIndexingFeatures physicalDeviceDescriptorIndexingFeatures;
        VkPhysicalDeviceHostQueryResetFeatures physicalDeviceHostQueryResetFeatures;
        VkPhysicalDeviceScalarBlockLayoutFeatures physicalDeviceScalarBlockLayoutFeatures;

        // Vulkan 1.3
        VkPhysicalDeviceDynamicRenderingFeatures physicalDeviceDynamicRenderingFeatures;
        VkPhysicalDeviceMaintenance4Features physicalDeviceMaintenance4Features;
        VkPhysicalDeviceShaderIntegerDotProductFeatures physicalDeviceShaderIntegerDotProductFeatures;
        VkPhysicalDeviceShaderTerminateInvocationFeatures physicalDeviceShaderTerminateInvocationFeatures;
        VkPhysicalDeviceSynchronization2Features physicalDeviceSynchronization2Features;
        VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures physicalDeviceZeroInitializeWorkgroupMemoryFeatures;
        VkPhysicalDevice4444FormatsFeaturesEXT physicalDevice4444FormatsFeaturesEXT;
        VkPhysicalDeviceExtendedDynamicStateFeaturesEXT physicalDeviceExtendedDynamicStateFeaturesEXT;
        VkPhysicalDeviceExtendedDynamicState2FeaturesEXT physicalDeviceExtendedDynamicState2FeaturesEXT;
        VkPhysicalDeviceImageRobustnessFeatures physicalDeviceImageRobustnessFeatures;
        VkPhysicalDeviceInlineUniformBlockFeatures physicalDeviceInlineUniformBlockFeatures;
        VkPhysicalDevicePipelineCreationCacheControlFeatures physicalDevicePipelineCreationCacheControlFeatures;
        VkPhysicalDevicePrivateDataFeatures physicalDevicePrivateDataFeatures;
        VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures physicalDeviceShaderDemoteToHelperInvocationFeatures;
        VkPhysicalDeviceSubgroupSizeControlFeatures physicalDeviceSubgroupSizeControlFeatures;
        VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT physicalDeviceTexelBufferAlignmentFeaturesEXT;
        VkPhysicalDeviceTextureCompressionASTCHDRFeatures physicalDeviceTextureCompressionASTCHDRFeatures;
        VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT physicalDeviceYcbcr2Plane444FormatsFeaturesEXT;

        // Vulkan 1.4
        VkPhysicalDeviceDynamicRenderingLocalReadFeatures physicalDeviceDynamicRenderingLocalReadFeatures;
        VkPhysicalDeviceGlobalPriorityQueryFeatures physicalDeviceGlobalPriorityQueryFeatures;
        VkPhysicalDeviceIndexTypeUint8Features physicalDeviceIndexTypeUint8Features;
        VkPhysicalDeviceLineRasterizationFeatures physicalDeviceLineRasterizationFeatures;
        VkPhysicalDeviceMaintenance5Features physicalDeviceMaintenance5Features;
        VkPhysicalDeviceMaintenance6Features physicalDeviceMaintenance6Features;
        VkPhysicalDeviceShaderExpectAssumeFeatures physicalDeviceShaderExpectAssumeFeatures;
        VkPhysicalDeviceShaderFloatControls2Features physicalDeviceShaderFloatControls2Features;
        VkPhysicalDeviceShaderSubgroupRotateFeatures physicalDeviceShaderSubgroupRotateFeatures;
        VkPhysicalDeviceVertexAttributeDivisorFeatures physicalDeviceVertexAttributeDivisorFeatures;
        VkPhysicalDeviceHostImageCopyFeatures physicalDeviceHostImageCopyFeatures;
        VkPhysicalDevicePipelineProtectedAccessFeatures physicalDevicePipelineProtectedAccessFeatures;
        VkPhysicalDevicePipelineRobustnessFeatures physicalDevicePipelineRobustnessFeatures;
    };

    struct VulkanPhysicalDeviceProperties
    {
        // Vulkan 1.2
        VkPhysicalDeviceDepthStencilResolveProperties physicalDeviceDepthStencilResolveProperties;
        VkPhysicalDeviceDriverProperties physicalDeviceDriverProperties;
        VkPhysicalDeviceFloatControlsProperties physicalDeviceFloatControlsProperties;
        VkPhysicalDeviceTimelineSemaphoreProperties physicalDeviceTimelineSemaphoreProperties;
        VkPhysicalDeviceDescriptorIndexingProperties physicalDeviceDescriptorIndexingProperties;
        VkPhysicalDeviceSamplerFilterMinmaxProperties physicalDeviceSamplerFilterMinmaxProperties;
        VkPhysicalDeviceGroupProperties physicalDeviceGroupProperties;
        VkPhysicalDeviceMemoryProperties2 physicalDeviceMemoryProperties2;
        VkPhysicalDevicePointClippingProperties physicalDevicePointClippingProperties;
        VkPhysicalDeviceMaintenance3Properties physicalDeviceMaintenance3Properties;
        VkPhysicalDeviceMultiviewProperties physicalDeviceMultiviewProperties;

        // Vulkan 1.3
        VkPhysicalDeviceMaintenance4Properties pysicalDeviceMaintenance4Properties;
        VkPhysicalDeviceShaderIntegerDotProductProperties physicalDeviceShaderIntegerDotProductProperties;
        VkPhysicalDeviceInlineUniformBlockProperties physicalDeviceInlineUniformBlockProperties;
        VkPhysicalDeviceSubgroupSizeControlProperties physicalDeviceSubgroupSizeControlProperties;
        VkPhysicalDeviceTexelBufferAlignmentProperties physicalDeviceTexelBufferAlignmentProperties;
        VkPhysicalDeviceToolProperties physicalDeviceToolProperties;

        // Vulkan 1.4
        VkPhysicalDeviceLineRasterizationProperties physicalDeviceLineRasterizationProperties;
        VkPhysicalDeviceMaintenance5Properties physicalDeviceMaintenance5Properties;
        VkPhysicalDeviceMaintenance6Properties physicalDeviceMaintenance6Properties;
        VkPhysicalDevicePushDescriptorProperties physicalDevicePushDescriptorProperties;
        VkPhysicalDeviceVertexAttributeDivisorProperties physicalDeviceVertexAttributeDivisorProperties;
        VkPhysicalDeviceHostImageCopyProperties physicalDeviceHostImageCopyProperties;
        VkPhysicalDevicePipelineRobustnessProperties physicalDevicePipelineRobustnessProperties;
    };

    class VulkanQueue;
    class VulkanViewport;
    class VulkanSemaphore;
    class VulkanFence;
    class VulkanCommandBuffer;
    class VulkanCommandBufferPool;
    class VulkanCommandListContext;
    class VulkanDescriptorPool;
    class VulkanGraphicsPipeline;
    class VulkanComputePipeline;
    class VulkanPipelineCache;
    class VulkanVertexDeclaration;
    class VulkanVertexShader;
    class VulkanTessellationControlShader;
    class VulkanTessellationEvaluationShader;
    class VulkanGeometryShader;
    class VulkanFragmentShader;
    class VulkanMeshShader;
    class VulkanTaskShader;
    class VulkanComputeShader;

    class VulkanDevice : public GpuDevice
    {
        friend class VulkanSemaphore;
        friend class VulkanViewport;
        friend class VulkanFence;
        friend class VulkanQueue;
        friend class VulkanCommandBuffer;
        friend class VulkanCommandBufferPool;
        friend class VulkanDescriptorPool;
        friend class VulkanGraphicsPipeline;
        friend class VulkanComputePipeline;
        friend class VulkanPipelineCache;
        friend class VulkanVertexDeclaration;
        friend class VulkanVertexShader;
        friend class VulkanTessellationControlShader;
        friend class VulkanTessellationEvaluationShader;
        friend class VulkanGeometryShader;
        friend class VulkanFragmentShader;
        friend class VulkanMeshShader;
        friend class VulkanTaskShader;
        friend class VulkanComputeShader;

    public:
        VulkanDevice();
        ~VulkanDevice() override;

        GpuViewportRef CreateViewport(Reference<HostWindow> window) override;
        GpuVertexDeclarationRef CreateVertexDeclaration(GpuVertexDeclarationInitializer const& initializer) override;
        GpuGraphicsPipelineRef CreateGraphicsPipeline(GpuGraphicsPipelineInitializer const& initializer) override;
        GpuComputePipelineRef CreateComputePipeline(GpuComputePipelineInitializer const& initializer) override;
        GpuCommandListContextRef GetImmediateContext() override;

    private:
        VkInstance m_instance{};
        VulkanInstanceLayers m_instanceLayers{};
        VulkanInstanceExtensions m_instanceExtensions{};

        VkPhysicalDevice m_physicalDevice{};
        VkDevice m_device{};
        VmaAllocator m_allocator{};

        VulkanPhysicalDeviceExtensions m_physicalDeviceExtensions{};
        VulkanPhysicalDeviceFeatures m_physicalDeviceFeatures{};
        VulkanPhysicalDeviceProperties m_physicalDeviceProperties{};
        VulkanDeviceFeatures m_deviceFeatures{};
        VulkanDeviceCapabilities m_deviceCapabilities{};

        Version m_driverVersion{};
        uint32_t m_vulkanVersion{};

        Reference<VulkanQueue> m_graphicsQueue{};
        Reference<VulkanQueue> m_computeQueue{};
        Reference<VulkanQueue> m_transferQueue{};

        Reference<VulkanCommandListContext> m_immediateContext{};

        // Used when creating buffers/images with specific queue family ownership.
        std::vector<uint32_t> m_activeQueueFamilies{};

        CriticalSection m_barrierEventsLock{};
        std::vector<VkEvent> m_barrierEvents{};

        IntrusiveList<VulkanVertexDeclaration> m_vertexDeclarations{};

    public:
        VkEvent AcquireBarrierEvent();
        void ReleaseBarrierEvent(VkEvent event);
        void ReleaseBarrierEvents(std::span<VkEvent const> events);

    private:
        void CreateInstance();
        void DestroyInstance();

        void CreateVulkanDevice();
        void DestroyVulkanDevice();

        static VkResult QueryPhysicalDeviceExtensionProperties(
            VkPhysicalDevice device,
            std::vector<VkExtensionProperties>& outExtensionProperties,
            const char* layerName = nullptr);

        static VkResult QueryPhysicalDeviceLayerProperties(
            VkPhysicalDevice device,
            std::vector<VkLayerProperties>& outLayerProperties);

        static VkResult QueryPhysicalDeviceQueueFamilyProperties(
            VkPhysicalDevice physicalDevice,
            std::vector<VkQueueFamilyProperties>& outQueueFamilyProperties);

        static VkResult QueryInstanceLayerProperties(
            std::vector<VkLayerProperties>& outLayerProperties);

        static VkResult QueryInstanceExtensionProperties(
            std::vector<VkExtensionProperties>& outExtensionProperties,
            const char* layerName = nullptr);

        static VkResult QueryPhysicalDevices(
            VkInstance instance,
            std::vector<VkPhysicalDevice>& outPhysicalDevices);

        VkPhysicalDevice SelectPhysicalDevice(GpuVendor preferredVendor) const;

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

namespace Anemone
{
    ANEMONE_RUNTIME_RENDER_VULKAN_API Reference<GpuDevice> CreateGpuDevice();
}
