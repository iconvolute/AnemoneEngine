#include "AnemoneRenderVulkan/VulkanDevice.hxx"
#include "AnemoneRenderVulkan/VulkanError.hxx"
#include "AnemoneApplication/HostApplication.hxx"
#include "AnemoneDiagnostics/Trace.hxx"
#include "AnemoneRenderVulkan/VulkanSwapChain.hxx"
#include "AnemoneRenderVulkan/VulkanQueue.hxx"

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

        AE_VK_CALL(this->CreateDebugUtilsMessenger(this->_instance, this->_debugMessenger));

        AE_VK_CALL(this->ChoosePhysicalDevice(this->_instance, this->_physicalDevice));

        AE_VK_CALL(EnumeratePhysicalDeviceQueueFamilyProperties(this->_physicalDevice, this->_queueFamilyProperties));

        AE_VK_CALL(this->CreateLogicalDevice(
            this->_instance,
            this->_physicalDevice,
            this->_logicalDevice));

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

        if (this->_debugMessenger)
        {
            vkDestroyDebugUtilsMessengerEXT(this->_instance, this->_debugMessenger, &VulkanCpuAllocator);
        }

        vkDestroyInstance(this->_instance, &VulkanCpuAllocator);
        volkFinalize();
    }

    Reference<RenderSwapChain> VulkanDevice::CreateSwapChain(Reference<HostWindow> window)
    {
        return MakeReference<VulkanSwapChain>(this, std::move(window));
    }

    VkResult VulkanDevice::EnumerateLayerProperties(std::vector<VkLayerProperties>& outProperties)
    {
        VkResult result;
        uint32_t count = 0;

        do
        {
            result = vkEnumerateInstanceLayerProperties(&count, nullptr);

            if (result == VK_SUCCESS)
            {
                outProperties.resize(count);
                result = vkEnumerateInstanceLayerProperties(&count, outProperties.data());
            }
        } while (result == VK_INCOMPLETE);

        AE_ASSERT(count <= outProperties.size());

        if (count < outProperties.size())
        {
            outProperties.resize(count);
        }

        return result;
    }

    VkResult VulkanDevice::EnumerateInstanceExtensionProperties(std::vector<VkExtensionProperties>& outProperties)
    {
        VkResult result;
        uint32_t count = 0;

        do
        {
            result = vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

            if (result == VK_SUCCESS)
            {
                outProperties.resize(count);
                result = vkEnumerateInstanceExtensionProperties(nullptr, &count, outProperties.data());
            }
        } while (result == VK_INCOMPLETE);

        AE_ASSERT(count <= outProperties.size());

        if (count < outProperties.size())
        {
            outProperties.resize(count);
        }

        return result;
    }

    VkResult VulkanDevice::EnumeratePhysicalDevices(VkInstance instance, std::vector<VkPhysicalDevice>& outPhysicalDevices)
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

    VkResult VulkanDevice::EnumeratePhysicalDeviceQueueFamilyProperties(VkPhysicalDevice device, std::vector<VkQueueFamilyProperties>& outProperties)
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

    VkResult VulkanDevice::EnumeratePhysicalDeviceExtensionProperties(
        std::vector<VkExtensionProperties>& outProperties,
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
                outProperties.resize(count);
                result = vkEnumerateDeviceExtensionProperties(device, layerName, &count, outProperties.data());
            }
        } while (result == VK_INCOMPLETE);

        if (count < outProperties.size())
        {
            outProperties.resize(count);
        }

        return result;
    }

    VkResult VulkanDevice::EnumeratePhysicalDeviceSurfaceFormats(
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

    VkResult VulkanDevice::EnumerateSurfacePresentModes(
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

    VkResult VulkanDevice::CreateInstance(VkInstance& outInstance)
    {
        constexpr VkApplicationInfo applicationInfo{
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = "Anemone Engine",
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "Anemone Engine",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_API_VERSION_1_4,
        };

        std::vector<const char*> requiredLayers = GetRequiredLayers();

        std::vector<VkLayerProperties> layerPropertiesCollection{};

        if (EnumerateLayerProperties(layerPropertiesCollection) != VK_SUCCESS)
        {
            return VK_ERROR_LAYER_NOT_PRESENT;
        }

        for (const auto requiredLayer : requiredLayers)
        {
            bool found = false;
            for (const VkLayerProperties& properties : layerPropertiesCollection)
            {
                if (std::strcmp(requiredLayer, properties.layerName) == 0)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                return VK_ERROR_LAYER_NOT_PRESENT;
            }
        }


        std::vector<VkExtensionProperties> extensionProperties{};
        if (EnumerateInstanceExtensionProperties(extensionProperties) != VK_SUCCESS)
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }

        std::vector<const char*> requiredInstanceExtensions = GetRequiredInstanceExtensions();

        for (const auto* requiredExtension : requiredInstanceExtensions)
        {
            bool found = false;

            for (const VkExtensionProperties& current : extensionProperties)
            {
                if (std::strcmp(requiredExtension, current.extensionName) == 0)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                AE_TRACE(Error, "Required Vulkan extensino '{}' not found", requiredExtension);
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }

        VkInstanceCreateInfo instanceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pApplicationInfo = &applicationInfo,
            .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
            .ppEnabledLayerNames = requiredLayers.data(),
            .enabledExtensionCount = static_cast<uint32_t>(requiredInstanceExtensions.size()),
            .ppEnabledExtensionNames = requiredInstanceExtensions.data(),
        };

        return vkCreateInstance(&instanceCreateInfo, &VulkanCpuAllocator, &outInstance);
    }

    VkResult VulkanDevice::CreateDebugUtilsMessenger(VkInstance instance, VkDebugUtilsMessengerEXT& messenger)
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo{
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = 0,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = DebugUtilsMessengerCallback,
            .pUserData = nullptr,
        };

        return vkCreateDebugUtilsMessengerEXT(instance, &createInfo, &VulkanCpuAllocator, &messenger);
    }

    VkResult VulkanDevice::ChoosePhysicalDevice(VkInstance instance, VkPhysicalDevice& outPhysicalDevice)
    {
        std::vector<VkPhysicalDevice> devices{};
        if (EnumeratePhysicalDevices(instance, devices) != VK_SUCCESS)
        {
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        for (const VkPhysicalDevice& device : devices)
        {
            VulkanDeviceExtensions deviceExtensions{};
            if (IsPhysicalDeviceSuitable(device, deviceExtensions))
            {
                this->_deviceExtensions = deviceExtensions;
                outPhysicalDevice = device;
                return VK_SUCCESS;
            }
        }

        outPhysicalDevice = VK_NULL_HANDLE;
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    bool VulkanDevice::IsPhysicalDeviceSuitable(VkPhysicalDevice device, VulkanDeviceExtensions& outDeviceExtensions)
    {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(device, &properties);

        if (properties.apiVersion < VK_API_VERSION_1_3)
        {
            return false;
        }

        std::vector<VkQueueFamilyProperties> queueFamilyProperties{};
        EnumeratePhysicalDeviceQueueFamilyProperties(device, queueFamilyProperties);

        bool const supportsGraphics = std::any_of(
            queueFamilyProperties.begin(),
            queueFamilyProperties.end(),
            [](VkQueueFamilyProperties const& qfp)
        {
            return (qfp.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0;
        });

        if (!supportsGraphics)
        {
            return false;
        }

        std::vector<VkExtensionProperties> deviceExtensions{};

        if (EnumeratePhysicalDeviceExtensionProperties(deviceExtensions, device, nullptr) != VK_SUCCESS)
        {
            return false;
        }

        std::vector const requiredExtensions = GetRequiredDeviceExtensions(device);

        for (const char* requiredExtension : requiredExtensions)
        {
            bool found = std::any_of(
                deviceExtensions.begin(),
                deviceExtensions.end(),
                [requiredExtension](VkExtensionProperties const& ext)
            {
                return std::strcmp(requiredExtension, ext.extensionName) == 0;
            });

            if (!found)
            {
                AE_TRACE(Error, "Required device extension '{}' not found", requiredExtension);
                return false;
            }
        }

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

        VkPhysicalDeviceFeatures2 features2{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
            .pNext = &rayTracingPipelineFeatures,
        };

        vkGetPhysicalDeviceFeatures2(device, &features2);

        if (!features2.features.samplerAnisotropy)
        {
            return false;
        }

        if (!vulkan13Features.dynamicRendering)
        {
            return false;
        }

        if (!extendedDynamicStateFeatures.extendedDynamicState)
        {
            return false;
        }

        if (!vulkan12Features.descriptorBindingSampledImageUpdateAfterBind)
        {
            return false;
        }

        if (!vulkan12Features.descriptorBindingSampledImageUpdateAfterBind)
        {
            return false;
        }

        if (!vulkan12Features.descriptorBindingPartiallyBound)
        {
            return false;
        }

        if (!vulkan12Features.descriptorBindingVariableDescriptorCount)
        {
            return false;
        }

        if (!vulkan12Features.runtimeDescriptorArray)
        {
            return false;
        }

        if (!vulkan12Features.shaderSampledImageArrayNonUniformIndexing)
        {
            return false;
        }

        if (!vulkan12Features.bufferDeviceAddress)
        {
            return false;
        }

        if (!accelerationStructureFeatures.accelerationStructure)
        {
            return false;
        }

        if (!rayQueryFeatures.rayQuery)
        {
            return false;
        }

        outDeviceExtensions.HasSeparateDepthStencilLayouts = separateDepthStencilLayoutsFeatures.separateDepthStencilLayouts != VK_FALSE;
        outDeviceExtensions.HasKHRSynchronization2 = synchronization2Features.synchronization2 != VK_FALSE;
        outDeviceExtensions.HasKHRRenderPass2 = true; // Always true for Vulkan 1.3 and above

        outDeviceExtensions.HasGeometryShader = features2.features.geometryShader != VK_FALSE;

        outDeviceExtensions.HasKHRFragmentShadingRate = std::any_of(deviceExtensions.begin(), deviceExtensions.end(),
            [](VkExtensionProperties const& ext)
        {
            return std::strcmp(ext.extensionName, VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME) == 0;
        });

        outDeviceExtensions.HasEXTFragmentDensityMap = fragmentDensityMapFeatures.fragmentDensityMap != VK_FALSE;

        outDeviceExtensions.HasEXTMeshShader = (meshShaderFeatures.meshShader != VK_FALSE) && (meshShaderFeatures.multiviewMeshShader != VK_FALSE);
        outDeviceExtensions.HasAccelerationStructure = accelerationStructureFeatures.accelerationStructure != VK_FALSE;
        outDeviceExtensions.HasRayTracingPipeline = rayTracingPipelineFeatures.rayTracingPipeline != VK_FALSE;
        outDeviceExtensions.HasEXTFullscreenExclusive = std::any_of(deviceExtensions.begin(), deviceExtensions.end(),
            [](VkExtensionProperties const& ext)
        {
            return std::strcmp(ext.extensionName, VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME) == 0;
        });

        return true;
    }

    VkResult VulkanDevice::CreateLogicalDevice(
        VkInstance instance,
        VkPhysicalDevice physicalDevice,
        VkDevice& outLogicalDevice)
    {
        (void)instance;
        (void)outLogicalDevice;

        std::optional<uint32_t> graphicsQueueFamilyIndex{};
        std::optional<uint32_t> computeQueueFamilyIndex{};
        std::optional<uint32_t> transferQueueFamilyIndex{};

        bool const supportsParallelRendering =
            this->_deviceExtensions.HasSeparateDepthStencilLayouts &&
            this->_deviceExtensions.HasKHRSynchronization2 &&
            this->_deviceExtensions.HasKHRRenderPass2;

        // Pick up queues to create
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};

        uint32_t prioritiesCount = 0;
        uint32_t familyIndex = 0;

        for (const VkQueueFamilyProperties& queueFamily : this->_queueFamilyProperties)
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

            const VkQueueFamilyProperties& currentProperties = this->_queueFamilyProperties[currentQueue.queueFamilyIndex];

            for (uint32_t index = 0; index < currentProperties.queueCount; ++index)
            {
                *currentPriority++ = 1.0f;
            }
        }

        constexpr VkPhysicalDeviceFeatures physicalDeviceFeatures{};

        std::vector const requiredDeviceExtensions = GetRequiredDeviceExtensions(physicalDevice);
        // std::vector const requiredLayers = GetRequiredLayers();

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

        VkDeviceCreateInfo deviceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = &featuresSynchronization2,
            .flags = 0,
            .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
            .pQueueCreateInfos = queueCreateInfos.data(),
            .enabledLayerCount = 0, // static_cast<uint32_t>(requiredLayers.size()),
            .ppEnabledLayerNames = nullptr, // requiredLayers.data(),
            .enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size()),
            .ppEnabledExtensionNames = requiredDeviceExtensions.data(),
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
        (void)pUserData;
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

    bool VulkanDevice::SupportsPresent(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VulkanQueue& queue)
    {
        VkBool32 presentSupport = false;
        AE_VK_CALL(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queue._familyIndex, surface, &presentSupport));
        return presentSupport == VK_TRUE;
    }

    std::vector<const char*> VulkanDevice::GetRequiredLayers()
    {
        std::vector<const char*> result{};

#if ANEMONE_VULKAN_VALIDATION
        result.emplace_back("VK_LAYER_KHRONOS_validation");
        result.emplace_back("VK_LAYER_KHRONOS_profiles");
        //result.emplace_back("VK_LAYER_GOOGLE_threading");
        //result.emplace_back("VK_LAYER_LUNARG_parameter_validation");
        //result.emplace_back("VK_LAYER_LUNARG_object_tracker");
        //result.emplace_back("VK_LAYER_LUNARG_core_validation");
#endif

        return result;
    }

    std::vector<const char*> VulkanDevice::GetRequiredInstanceExtensions()
    {
        std::vector<const char*> result{};

        result.emplace_back("VK_KHR_surface");

#if !ANEMONE_BUILD_SHIPPING
        result.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        result.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

#if ANEMONE_PLATFORM_WINDOWS
        result.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

#if ANEMONE_PLATFORM_LINUX
        result.emplace_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
        result.emplace_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#endif

        return result;
    }

    std::vector<const char*> VulkanDevice::GetRequiredDeviceExtensions(VkPhysicalDevice device)
    {
        (void)device;
        std::vector<const char*> result{};

        result.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        result.emplace_back(VK_KHR_SPIRV_1_4_EXTENSION_NAME);
        result.emplace_back(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);
        result.emplace_back(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME);
        result.emplace_back(VK_EXT_LOAD_STORE_OP_NONE_EXTENSION_NAME);
        result.emplace_back(VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME);
        result.emplace_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
        result.emplace_back(VK_KHR_IMAGE_FORMAT_LIST_EXTENSION_NAME);
        result.emplace_back(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME);
        result.emplace_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
        result.emplace_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);

#if ANEMONE_VULKAN_VALIDATION
        result.emplace_back(VK_EXT_TOOLING_INFO_EXTENSION_NAME);
        // result.emplace_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME); // verify if device supports that extension
#endif

        return result;
    }
}

namespace Anemone
{
    RENDER_VULKAN_API Reference<RenderDevice> CreateRenderDevice()
    {
        return MakeReference<VulkanDevice>();
    }
}
