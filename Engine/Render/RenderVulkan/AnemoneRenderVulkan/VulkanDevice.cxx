#include "AnemoneRenderVulkan/VulkanDevice.hxx"
#include "AnemoneRenderVulkan/VulkanError.hxx"
#include "AnemoneApplication/HostApplication.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

namespace Anemone
{
    namespace
    {
        void* VKAPI_PTR FnVkAllocationFunction(
            void* pUserData,
            size_t size,
            size_t alignment,
            VkSystemAllocationScope allocationScope)
        {
            (void)pUserData;
            (void)allocationScope;
            return _aligned_malloc(size, alignment);
        }

        void VKAPI_PTR FnVkFreeFunction(
            void* pUserData,
            void* pMemory)
        {
            (void)pUserData;
            _aligned_free(pMemory);
        }

        void VKAPI_PTR FnVkInternalAllocationNotification(
            void* pUserData,
            size_t size,
            VkInternalAllocationType allocationType,
            VkSystemAllocationScope allocationScope)
        {
            AE_TRACE(Error, "VK::InternalAlloc size={}, type={}, scope={}", size, std::to_underlying(allocationType), std::to_underlying(allocationScope));
            (void)pUserData;
            (void)size;
            (void)allocationType;
            (void)allocationScope;
        }

        void VKAPI_PTR FnVkInternalFreeNotification(
            void* pUserData,
            size_t size,
            VkInternalAllocationType allocationType,
            VkSystemAllocationScope allocationScope)
        {
            AE_TRACE(Error, "VK::InternalFree size={}, type={}, scope={}", size, std::to_underlying(allocationType), std::to_underlying(allocationScope));
            (void)pUserData;
            (void)size;
            (void)allocationType;
            (void)allocationScope;
        }

        void* VKAPI_PTR FnVkReallocationFunction(
            void* pUserData,
            void* pOriginal,
            size_t size,
            size_t alignment,
            VkSystemAllocationScope allocationScope)
        {
            (void)pUserData;
            (void)allocationScope;

            if (size == 0)
            {
                _aligned_free(pOriginal);
                return nullptr;
            }

            return _aligned_realloc(pOriginal, size, alignment);
        }

        constexpr VkAllocationCallbacks gVkAllocationCallbacks{
            .pUserData = nullptr,
            .pfnAllocation = &FnVkAllocationFunction,
            .pfnReallocation = &FnVkReallocationFunction,
            .pfnFree = &FnVkFreeFunction,
            .pfnInternalAllocation = &FnVkInternalAllocationNotification,
            .pfnInternalFree = &FnVkInternalFreeNotification,
        };

#if !ANEMONE_BUILD_SHIPPING
        VkBool32 VKAPI_PTR FnVkDebugReportCallbackEXT(
            VkDebugReportFlagsEXT flags,
            VkDebugReportObjectTypeEXT objectType,
            uint64_t object,
            size_t location,
            int32_t messageCode,
            const char* pLayerPrefix,
            const char* pMessage,
            void* pUserData)
        {
            AE_TRACE(Error, "VKDEBUG: flags={}, objectType={},object={},location={},messageCode={},layerPrefix={},message={},userData={}",
                flags,
                std::to_underlying(objectType),
                object,
                location,
                messageCode,
                pLayerPrefix,
                pMessage,
                fmt::ptr(pUserData));

            return VK_FALSE;
        }
#endif
    }
    VulkanDevice::VulkanDevice()
    {
        volkInitialize();

        VkApplicationInfo applicationInfo{
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = "Anemone Engine",
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "Anemone Engine",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_API_VERSION_1_4,
        };

#if !ANEMONE_BUILD_SHIPPING
        const char* enabledLayers[]{
            "VK_LAYER_KHRONOS_validation",
        };
#endif

        const char* enabledExtensions[]{
            "VK_KHR_surface",

#if !ANEMONE_BUILD_SHIPPING
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
            VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#endif

#if ANEMONE_PLATFORM_WINDOWS
            "VK_KHR_win32_surface",
#endif

#if ANEMONE_PLATFORM_LINUX
            "VK_KHR_xcb_surface",
            "VK_KHR_wayland_surface",
#endif
        };

        VkInstanceCreateInfo info{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pApplicationInfo = &applicationInfo,
#if !ANEMONE_BUILD_SHIPPING
            .enabledLayerCount = std::size(enabledLayers),
            .ppEnabledLayerNames = enabledLayers,
#else
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
#endif
            .enabledExtensionCount = std::size(enabledExtensions),
            .ppEnabledExtensionNames = enabledExtensions,
        };

        AE_VK_CALL(vkCreateInstance(&info, &gVkAllocationCallbacks, &this->_instance));

        volkLoadInstanceOnly(this->_instance);

#if !ANEMONE_BUILD_SHIPPING
        VkDebugReportCallbackCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        createInfo.flags = VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT;
        createInfo.pfnCallback = FnVkDebugReportCallbackEXT;

        AE_VK_CALL(vkCreateDebugReportCallbackEXT(this->_instance, &createInfo, &gVkAllocationCallbacks, &this->_debugReportCallback));
#endif

        uint32_t devCount{};
        std::vector<VkPhysicalDevice> devices{};

        AE_VK_CALL(vkEnumeratePhysicalDevices(this->_instance, &devCount, nullptr));

        devices.resize(devCount);
        AE_VK_CALL(vkEnumeratePhysicalDevices(this->_instance, &devCount, devices.data()));

        auto window = Anemone::HostApplication::Get().MakeWindow(WindowType::Game, WindowMode::Windowed);

        VkWin32SurfaceCreateInfoKHR win32ci{
            .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .hinstance = GetModuleHandleW(nullptr),
            .hwnd = static_cast<HWND>(window->GetNativeHandle()),
        };

        VkSurfaceKHR surface{};
        AE_VK_CALL(vkCreateWin32SurfaceKHR(this->_instance, &win32ci, &gVkAllocationCallbacks, &surface));

        for (VkPhysicalDevice device : devices)
        {
            VkPhysicalDeviceFeatures pdf{};
            VkDeviceCreateInfo dci{
                .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueCreateInfoCount = 0,
                .pQueueCreateInfos = nullptr,
                .enabledLayerCount = 0,
                .ppEnabledLayerNames = 0,
                .enabledExtensionCount = 0,
                .ppEnabledExtensionNames = nullptr,
                .pEnabledFeatures = &pdf,
            };

            AE_VK_CALL(vkCreateDevice(device, &dci, nullptr, &this->_device));
            volkLoadDevice(this->_device);

            if (vkGetPhysicalDeviceDisplayPropertiesKHR)
            {
                uint32_t propertyCount{};
                AE_VK_CALL(vkGetPhysicalDeviceDisplayPropertiesKHR(device, &propertyCount, nullptr));

                std::vector<VkDisplayPropertiesKHR> properties{};
                AE_VK_CALL(vkGetPhysicalDeviceDisplayPropertiesKHR(device, &propertyCount, properties.data()));
            }

            vkDestroyDevice(this->_device, &gVkAllocationCallbacks);
        }
    }

    VulkanDevice::~VulkanDevice()
    {
        vkDestroyInstance(this->_instance, &gVkAllocationCallbacks);
        volkFinalize();
    }


}

namespace Anemone
{
    RENDER_VULKAN_API Reference<RenderDevice> CreateRenderDevice()
    {
        return MakeReference<VulkanDevice>();
    }
}
