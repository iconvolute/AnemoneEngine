#include "AnemoneRenderVulkan/VulkanDevice.hxx"
#include "AnemoneRenderVulkan/VulkanError.hxx"
#include "AnemoneApplication/HostApplication.hxx"
#include "AnemoneDiagnostics/Trace.hxx"
#include "AnemoneRenderVulkan/VulkanSwapChain.hxx"
#include "AnemoneRenderVulkan/VulkanQueue.hxx"
#include "AnemoneRenderVulkan/VulkanUtils.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneApplication/Platform/Windows/WindowsHostWindow.hxx"
#endif

namespace Anemone
{
    VulkanDevice::VulkanDevice()
    {
        volkInitialize();

        AE_VK_CALL(this->CreateInstance(this->_instance));

        volkLoadInstanceOnly(this->_instance);

        // Setup debug utils
        {
#if ANEMONE_VULKAN_VALIDATION
            VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo{
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                .pNext = nullptr,
                .flags = 0,
                .messageSeverity =
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                .messageType =
                    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                .pfnUserCallback = DebugUtilsMessengerCallback,
                .pUserData = this,
            };

            if constexpr (false)
            {
                debugUtilsMessengerCreateInfo.messageType |=
                    VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
            }

            AE_VK_CALL(vkCreateDebugUtilsMessengerEXT(
                this->_instance,
                &debugUtilsMessengerCreateInfo,
                &VulkanCpuAllocator,
                &this->_debugMessenger));
#endif
        }

#if ANEMONE_VULKAN_VALIDATION
        bool const withDiagnostics = true;
#else
        bool const withDiagnostics = false;
#endif

        std::vector<const char*> deviceExtensions{};
        AE_VK_CALL(VulkanGraphics::QueryPhysicalDevice(
            this->_instance,
            this->_physicalDevice,
            this->m_physicalDeviceExtensions,
            deviceExtensions,
            this->m_queueFamilyProperties,
            withDiagnostics));

        AE_VK_CALL(this->CreateLogicalDevice(
            this->_instance,
            this->_physicalDevice,
            this->_logicalDevice,
            deviceExtensions,
            this->m_queueFamilyProperties));

        VmaVulkanFunctions vmaVulkanFunctions{
            .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
            .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
        };

        VmaAllocatorCreateInfo vmaCreateInfo{
            .flags = 0,
            .physicalDevice = this->_physicalDevice,
            .device = this->_logicalDevice,
            .preferredLargeHeapBlockSize = 0,
            .pAllocationCallbacks = &VulkanCpuAllocator,
            .pDeviceMemoryCallbacks = nullptr,
            .pHeapSizeLimit = nullptr,
            .pVulkanFunctions = &vmaVulkanFunctions,
            .instance = this->_instance,
            .vulkanApiVersion = VK_API_VERSION_1_4,
            .pTypeExternalMemoryHandleTypes = nullptr,
        };

        AE_VK_CALL(vmaCreateAllocator(&vmaCreateInfo, &this->_allocator));
    }

    VulkanDevice::~VulkanDevice()
    {
        this->_transferQueue = {};
        this->_computeQueue = {};
        this->_graphicsQueue = {};

        vmaDestroyAllocator(this->_allocator);
        vkDestroyDevice(this->_logicalDevice, &VulkanCpuAllocator);

#if ANEMONE_VULKAN_VALIDATION
        AE_ASSERT(this->_debugMessenger != nullptr);
        vkDestroyDebugUtilsMessengerEXT(this->_instance, this->_debugMessenger, &VulkanCpuAllocator);
#endif

        vkDestroyInstance(this->_instance, &VulkanCpuAllocator);
        volkFinalize();
    }

    Reference<RenderSwapChain> VulkanDevice::CreateSwapChain(Reference<HostWindow> window)
    {
        return MakeReference<VulkanSwapChain>(this, std::move(window));
    }

    VkResult VulkanDevice::CreateInstance(VkInstance& outInstance)
    {
        std::vector<VkLayerProperties> layerPropertiesCollection{};

        if (VulkanGraphics::EnumerateInstanceLayerProperties(layerPropertiesCollection) != VK_SUCCESS)
        {
            return VK_ERROR_LAYER_NOT_PRESENT;
        }

        constexpr VkApplicationInfo applicationInfo{
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = "Anemone Engine",
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "Anemone Engine",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_API_VERSION_1_4,
        };

        std::vector<const char*> enabledLayers{};
        std::vector<const char*> enabledExtensions{};
        AE_VK_CALL(VulkanGraphics::QueryInstanceLayersAndExtensions(
            this->m_instanceLayers,
            this->m_instanceExtensions,
            enabledLayers,
            enabledExtensions,
            true));

        VkInstanceCreateInfo instanceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pApplicationInfo = &applicationInfo,
            .enabledLayerCount = static_cast<uint32_t>(enabledLayers.size()),
            .ppEnabledLayerNames = enabledLayers.data(),
            .enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size()),
            .ppEnabledExtensionNames = enabledExtensions.data(),
        };

        return vkCreateInstance(&instanceCreateInfo, &VulkanCpuAllocator, &outInstance);
    }

    VkResult VulkanDevice::CreateLogicalDevice(
        VkInstance instance,
        VkPhysicalDevice physicalDevice,
        VkDevice& outLogicalDevice,
        std::vector<const char*> const& extensionNames,
        std::vector<VkQueueFamilyProperties> const& queueFamilyProperties)
    {
        (void)instance;
        (void)outLogicalDevice;

        std::optional<uint32_t> graphicsQueueFamilyIndex{};
        std::optional<uint32_t> computeQueueFamilyIndex{};
        std::optional<uint32_t> transferQueueFamilyIndex{};

        bool const supportsParallelRendering =
            this->m_physicalDeviceExtensions.SeparateDepthStencilLayouts &&
            this->m_physicalDeviceExtensions.Synchronization2 &&
            this->m_physicalDeviceExtensions.RenderPass2;

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
                else
                {
                    // TODO: Support multiple graphic queues?
                }
            }

            if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
                if (not computeQueueFamilyIndex)
                {
                    if (graphicsQueueFamilyIndex != familyIndex)
                    {
                        if (supportsParallelRendering)
                        {
                            computeQueueFamilyIndex = familyIndex;
                            valid = true;
                        }
                    }
                }
            }

            if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
            {
                if (not transferQueueFamilyIndex)
                {
                    if (!(queueFamily.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)))
                    {
                        if (supportsParallelRendering)
                        {
                            transferQueueFamilyIndex = familyIndex;
                            valid = true;
                        }
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
                AE_TRACE(Error, "Vulkan queue family {} has unsupported flags: {}", familyIndex, queueFamily.queueFlags);
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

        //
        // Enable device features.
        //

        constexpr VkPhysicalDeviceFeatures physicalDeviceFeatures{};

        VkPhysicalDeviceDynamicRenderingFeatures featuresDynamicRendering{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
            .pNext = nullptr,
            .dynamicRendering = VK_TRUE,
        };

        VkPhysicalDeviceSynchronization2Features featuresSynchronization2{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
            .pNext = &featuresDynamicRendering,
            .synchronization2 = VK_TRUE,
        };

        VkPhysicalDeviceDescriptorIndexingFeatures physicalDeviceDescriptorIndexingFeatures{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
            .pNext = &featuresSynchronization2,
            .shaderSampledImageArrayNonUniformIndexing = VK_TRUE,
            .descriptorBindingPartiallyBound = VK_TRUE,
            .descriptorBindingVariableDescriptorCount = VK_TRUE,
            .runtimeDescriptorArray = VK_TRUE,
        };

        VkDeviceCreateInfo deviceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = &physicalDeviceDescriptorIndexingFeatures,
            .flags = 0,
            .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
            .pQueueCreateInfos = queueCreateInfos.data(),
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = static_cast<uint32_t>(extensionNames.size()),
            .ppEnabledExtensionNames = extensionNames.data(),
            .pEnabledFeatures = &physicalDeviceFeatures,
        };

        if (vkCreateDevice(physicalDevice, &deviceCreateInfo, &VulkanCpuAllocator, &outLogicalDevice) != VK_SUCCESS)
        {
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        // Load functions for the created device
        volkLoadDevice(this->_logicalDevice);

        this->_graphicsQueue = MakeReference<VulkanQueue>(
            this,
            *graphicsQueueFamilyIndex,
            VulkanQueueType::Graphics);
        this->_activeQueueFamilies.emplace_back(*graphicsQueueFamilyIndex);

        if (computeQueueFamilyIndex)
        {
            this->_computeQueue = MakeReference<VulkanQueue>(
                this,
                *computeQueueFamilyIndex,
                VulkanQueueType::Compute);
            this->_activeQueueFamilies.emplace_back(*computeQueueFamilyIndex);
        }

        if (transferQueueFamilyIndex)
        {
            this->_transferQueue = MakeReference<VulkanQueue>(
                this,
                *transferQueueFamilyIndex,
                VulkanQueueType::Transfer);
            this->_activeQueueFamilies.emplace_back(*transferQueueFamilyIndex);
        }

        return VK_SUCCESS;
    }


    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDevice::DebugUtilsMessengerCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {
        VulkanDevice* self = static_cast<VulkanDevice*>(pUserData);
        (void)self;

        if ((severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) ||
            (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT))
        {
            anemone_debugbreak();
        }

        AE_TRACE(Error, "Vulkan Validation Layer: severity {} type {} msg: {}",
            std::to_underlying(severity),
            type,
            pCallbackData->pMessage);

        return VK_FALSE;
    }
}

namespace Anemone
{
    RENDER_VULKAN_API Reference<RenderDevice> CreateRenderDevice()
    {
        return MakeReference<VulkanDevice>();
    }
}
