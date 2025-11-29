#include "AnemoneRenderVulkan/VulkanInstance.hxx"
#include "AnemoneRenderVulkan/VulkanError.hxx"
#include "AnemoneRenderVulkan/VulkanCpuAllocator.hxx"
#include "AnemoneBase/UninitializedObject.hxx"
#include "AnemoneDiagnostics/Trace.hxx"

#include <algorithm>

namespace Anemone
{
    namespace
    {
        UninitializedObject<VulkanInstance> gVulkanInstance{};
    }

    void VulkanInstance::StaticInitialize()
    {
        gVulkanInstance.Create();
    }

    void VulkanInstance::StaticFinalize()
    {
        gVulkanInstance.Destroy();
    }

    VulkanInstance& VulkanInstance::Get()
    {
        return gVulkanInstance.Get();
    }

    VulkanInstance::VulkanInstance()
    {
        volkInitialize();

        constexpr VkApplicationInfo applicationInfo{
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = "Anemone Engine",
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "Anemone Engine",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_API_VERSION_1_3,
        };

        std::vector<const char*> enabledLayers{};
        std::vector<const char*> enabledExtensions{};

        std::vector<VkLayerProperties> layerProperties{};
        AE_VULKAN_ENSURE(QueryInstanceLayerProperties(layerProperties));

        std::vector<VkExtensionProperties> layerExtensions{};
        AE_VULKAN_ENSURE(QueryInstanceExtensionProperties(layerExtensions));

        //
        // Layers
        //

        // TODO: Make this enabled on-demand instead on every non-shipping build.
#if ANEMONE_VULKAN_VALIDATION
        auto validateLayer = [&](const char* layerName) -> bool
        {
            for (const VkLayerProperties& properties : layerProperties)
            {
                if (std::strcmp(layerName, properties.layerName) == 0)
                {
                    enabledLayers.emplace_back(layerName);

                    return true;
                }
            }

            return false;
        };

        this->m_instanceLayers = {};
        this->m_instanceLayers.KHRONOS_validation = validateLayer("VK_LAYER_KHRONOS_validation");
        // this->m_instanceLayers.KHRONOS_profiles = validateLayer("VK_LAYER_KHRONOS_profiles");
        this->m_instanceLayers.GOOGLE_unique_objects = validateLayer("VK_LAYER_GOOGLE_unique_objects");
        this->m_instanceLayers.GOOGLE_threading = validateLayer("VK_LAYER_GOOGLE_threading");
        this->m_instanceLayers.LUNARG_parameter_validation = validateLayer("VK_LAYER_LUNARG_parameter_validation");
        this->m_instanceLayers.LUNARG_object_tracker = validateLayer("VK_LAYER_LUNARG_object_tracker");
        this->m_instanceLayers.LUNARG_core_validation = validateLayer("VK_LAYER_LUNARG_core_validation");
        // this->m_instanceLayers.RENDERDOC_Capture = validateLayer("VK_LAYER_RENDERDOC_Capture");
        // this->m_instanceLayers.NV_nsight = validateLayer("VK_LAYER_NV_nsight");

#if ANEMONE_VULKAN_FEATURE_API_DUMP
        this->m_instanceLayers.LUNARG_api_dump = validateLayer("VK_LAYER_LUNARG_api_dump");
#endif

#if ANEMONE_VULKAN_FEATURE_GFXRECONSTRUCT
        this->m_instanceLayers.LUNARG_gfxreconstruct = validateLayer("VK_LAYER_LUNARG_gfxreconstruct");
#endif

#if ANEMONE_VULKAN_FEATURE_VKTRACE
        this->m_instanceLayers.LUNARG_vktrace = validateLayer("VK_LAYER_LUNARG_vktrace");
#endif

#if ANEMONE_VULKAN_FEATURE_CRASH_DIAGNOSTIC
        this->m_instanceLayers.LUNARG_crash_diagnostic = validateLayer("VK_LAYER_LUNARG_crash_diagnostic");
#endif

#endif


        //
        // Extensions
        //

        auto validateExtension = [&](const char* extensionName) -> bool
        {
            for (const VkExtensionProperties& properties : layerExtensions)
            {
                if (std::strcmp(extensionName, properties.extensionName) == 0)
                {
                    enabledExtensions.emplace_back(extensionName);
                    return true;
                }
            }

            return false;
        };

        //
        // Required extensions
        //

        this->m_instanceExtensions = {};
        this->m_instanceExtensions.KHR_surface = validateExtension(VK_KHR_SURFACE_EXTENSION_NAME);
        this->m_instanceExtensions.KHR_display = validateExtension(VK_KHR_DISPLAY_EXTENSION_NAME);

        if (!this->m_instanceExtensions.KHR_surface)
        {
            AE_PANIC("Vulkan Instance is not capable of rendering to a surface");
        }


        //
        // Optional extensions.
        //

        this->m_instanceExtensions.KHR_get_surface_capabilities2 = validateExtension(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME);
        this->m_instanceExtensions.KHR_surface_maintenance1 = validateExtension(VK_KHR_SURFACE_MAINTENANCE_1_EXTENSION_NAME);

#if ANEMONE_PLATFORM_WINDOWS
        this->m_instanceExtensions.KHR_win32_surface = validateExtension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

#endif

#if ANEMONE_PLATFORM_LINUX
        this->m_instanceExtensions.KHR_xcb_surface = validateExtension(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
        this->m_instanceExtensions.KHR_wayland_surface = validateExtension(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
        this->m_instanceExtensions.EXT_acquire_xlib_display = validateExtension(VK_EXT_ACQUIRE_XLIB_DISPLAY_EXTENSION_NAME);
        this->m_instanceExtensions.KHR_xlib_surface = validateExtension(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#endif

#if ANEMONE_PLATFORM_ANDROID
        this->m_instanceExtensions.KHR_android_surface = validateExtension(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif

#if ANEMONE_PLATFORM_NN
        this->m_instanceExtensions.NN_vi_surface = validateExtension(VK_NN_VI_SURFACE_EXTENSION_NAME);
#endif

#if ANEMONE_VULKAN_VALIDATION
        this->m_instanceExtensions.EXT_debug_utils = validateExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        this->m_instanceExtensions.EXT_debug_report = validateExtension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif


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

        AE_VULKAN_ENSURE(vkCreateInstance(&instanceCreateInfo, VulkanCpuAllocator, &this->m_instance));

        volkLoadInstanceOnly(this->m_instance);

#if ANEMONE_VULKAN_VALIDATION
        // Setup debug utils
        if (this->m_instanceExtensions.EXT_debug_utils)
        {
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

            AE_VULKAN_ENSURE(vkCreateDebugUtilsMessengerEXT(
                this->m_instance,
                &debugUtilsMessengerCreateInfo,
                VulkanCpuAllocator,
                &this->m_debugUtilsMessenger));
        }
        else if (this->m_instanceExtensions.EXT_debug_report)
        {
            VkDebugReportCallbackCreateInfoEXT debugReportCallbackCreateInfo{
                .sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
                .pNext = nullptr,
                .flags =
                    VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
                    VK_DEBUG_REPORT_WARNING_BIT_EXT |
                    VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
                    VK_DEBUG_REPORT_ERROR_BIT_EXT |
                    VK_DEBUG_REPORT_DEBUG_BIT_EXT,
                .pfnCallback = DebugReportCallback,
                .pUserData = this,
            };
            AE_VULKAN_ENSURE(vkCreateDebugReportCallbackEXT(
                this->m_instance,
                &debugReportCallbackCreateInfo,
                VulkanCpuAllocator,
                &this->m_debugReportCallback));
        }
#endif
    }

    VulkanInstance::~VulkanInstance()
    {
#if ANEMONE_VULKAN_VALIDATION
        if (this->m_instanceExtensions.EXT_debug_utils)
        {
            if (this->m_debugUtilsMessenger != nullptr)
            {
                vkDestroyDebugUtilsMessengerEXT(this->m_instance, this->m_debugUtilsMessenger, VulkanCpuAllocator);
            }
        }

        if (this->m_instanceExtensions.EXT_debug_report)
        {
            if (this->m_debugReportCallback != nullptr)
            {
                vkDestroyDebugReportCallbackEXT(this->m_instance, this->m_debugReportCallback, VulkanCpuAllocator);
            }
        }
#endif

        // FIXME(windows): AppVerifier reports leaked function addresses from driver.
        vkDestroyInstance(this->m_instance, VulkanCpuAllocator);

        this->m_instance = VK_NULL_HANDLE;

        volkFinalize();
    }

    VkPhysicalDevice VulkanInstance::SelectPhysicalDevice(GpuVendor preferredVendor) const
    {
        std::vector<VkPhysicalDevice> physicalDevices{};
        AE_VULKAN_ENSURE(QueryPhysicalDevices(this->m_instance, physicalDevices));

        struct PhysicalDeviceInfo final
        {
            size_t Index{};
            VkPhysicalDevice PhysicalDevice{};
            VkPhysicalDeviceProperties2 PhysicalDeviceProperties2{};

            PhysicalDeviceInfo(size_t index, VkPhysicalDevice handle)
                : Index{index}
                , PhysicalDevice{handle}
                , PhysicalDeviceProperties2{
                      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
                  }
            {
                vkGetPhysicalDeviceProperties2(this->PhysicalDevice, &this->PhysicalDeviceProperties2);
            }
        };

        std::vector<PhysicalDeviceInfo> devices{};

        for (size_t i = 0; i < physicalDevices.size(); ++i)
        {
            devices.emplace_back(i, physicalDevices[i]);
        }

        // Sort devices by device type.
        std::sort(devices.begin(), devices.end(),
            [](PhysicalDeviceInfo const& a, PhysicalDeviceInfo const& b) -> bool
        {
            // Keep relative order for same device types.
            if (a.PhysicalDeviceProperties2.properties.deviceType == b.PhysicalDeviceProperties2.properties.deviceType)
            {
                return a.Index < b.Index;
            }

            // Prefer discrete GPUs over integrated GPUs.
            return (a.PhysicalDeviceProperties2.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) ||
                (b.PhysicalDeviceProperties2.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
        });

        // If a preferred vendor is specified, try to find a device from that vendor.
        if (preferredVendor != GpuVendor::Unknown)
        {
            uint32_t const preferredVendorId = std::to_underlying(preferredVendor);

            for (PhysicalDeviceInfo const& deviceInfo : devices)
            {
                if (deviceInfo.PhysicalDeviceProperties2.properties.vendorID == preferredVendorId)
                {
                    return deviceInfo.PhysicalDevice;
                }
            }
        }

        // Select first matching device
        for (PhysicalDeviceInfo const& deviceInfo : devices)
        {
            // Filter out non-GPU devices.
            if ((deviceInfo.PhysicalDeviceProperties2.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) ||
                (deviceInfo.PhysicalDeviceProperties2.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU))
            {
                return deviceInfo.PhysicalDevice;
            }
        }

        return VK_NULL_HANDLE;
    }

    VkResult VulkanInstance::QueryInstanceLayerProperties(
        std::vector<VkLayerProperties>& outLayerProperties)
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

        if (count < outLayerProperties.size())
        {
            outLayerProperties.resize(count);
        }

        return result;
    }

    VkResult VulkanInstance::QueryInstanceExtensionProperties(
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

    VkResult VulkanInstance::QueryPhysicalDevices(
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

#if ANEMONE_VULKAN_VALIDATION
    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanInstance::DebugUtilsMessengerCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {
        VulkanDevice* self = static_cast<VulkanDevice*>(pUserData);
        (void)self;

        AE_TRACE(Error, "VulkanDebug: severity {} type {} msg: {}",
            string_VkDebugUtilsMessageSeverityFlagsEXT(severity),
            string_VkDebugUtilsMessageTypeFlagsEXT(type),
            pCallbackData->pMessage);

        if ((severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) ||
            (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT))
        {
            anemone_debugbreak();
        }

        return VK_FALSE;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanInstance::DebugReportCallback(
        VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objectType,
        uint64_t object,
        size_t location,
        int32_t messageCode,
        const char* pLayerPrefix,
        const char* pMessage,
        void* pUserData)
    {
        VulkanDevice* self = static_cast<VulkanDevice*>(pUserData);
        (void)self;

        AE_TRACE(Error, "Vulkan Debug Report: flags {}, type {}, object {}, location {}, code {}, prefix {}, message: {}",
            string_VkDebugReportFlagsEXT(flags),
            string_VkDebugReportObjectTypeEXT(objectType),
            object,
            location,
            messageCode,
            pLayerPrefix,
            pMessage);

        if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
        {
            anemone_debugbreak();
        }
        return VK_FALSE;
    }
#endif
}
