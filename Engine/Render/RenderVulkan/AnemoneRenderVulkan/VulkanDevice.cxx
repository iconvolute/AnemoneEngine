#include "AnemoneRenderVulkan/VulkanDevice.hxx"

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

        const char* enabledLayers[]{
            "VK_LAYER_KHRONOS_validation",
        };

        const char* enabledExtensions[]{
            "VK_KHR_surface",

            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
            VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
            //VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
            //VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,

#if ANEMONE_PLATFORM_WINDOWS
            "VK_KHR_win32_surface",
#endif

#if ANEMONE_PLATFORM_LINUX
            "VK_KHR_xcb_surface",
            "VK_KHR_wayland_surface",
#endif
            //"VK_EXT_full_screen_exclusive"
            //"VK_KHR_win32_keyed_mutex",
            //"VK_KHR_external_memory_win32",
        };

        VkInstanceCreateInfo info{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pApplicationInfo = &applicationInfo,
            .enabledLayerCount = std::size(enabledLayers),
            .ppEnabledLayerNames = enabledLayers,
            .enabledExtensionCount = std::size(enabledExtensions),
            .ppEnabledExtensionNames = enabledExtensions,
        };

        VkResult vkr = vkCreateInstance(&info, &gVkAllocationCallbacks, &this->_instance);
        AE_ENSURE(vkr == VK_SUCCESS);

        volkLoadInstanceOnly(this->_instance);

        VkDebugReportCallbackCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        createInfo.flags = VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT;
        createInfo.pfnCallback = FnVkDebugReportCallbackEXT;

        vkr = vkCreateDebugReportCallbackEXT(this->_instance, &createInfo, &gVkAllocationCallbacks, &this->_debugReportCallback);
        AE_ENSURE(vkr == VK_SUCCESS);

        uint32_t devCount{};
        std::vector<VkPhysicalDevice> devices{};

        vkr = vkEnumeratePhysicalDevices(this->_instance, &devCount, nullptr);
        AE_ENSURE(vkr == VK_SUCCESS);

        devices.resize(devCount);
        vkr = vkEnumeratePhysicalDevices(this->_instance, &devCount, devices.data());
        AE_ENSURE(vkr == VK_SUCCESS);

        auto window = Anemone::HostApplication::Get().MakeWindow(WindowType::Game, WindowMode::Windowed);
        
        VkWin32SurfaceCreateInfoKHR win32ci{
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .hinstance = GetModuleHandleW(nullptr),
        .hwnd = static_cast<HWND>(window->GetNativeHandle()),
        };
        VkSurfaceKHR surface{};
        vkr = vkCreateWin32SurfaceKHR(this->_instance, &win32ci, &gVkAllocationCallbacks, &surface);
        AE_ENSURE(vkr == VK_SUCCESS);

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
            vkr = vkCreateDevice(device, &dci, nullptr, &this->_device);
            AE_ENSURE(vkr == VK_SUCCESS);
            volkLoadDevice(this->_device);

            uint32_t propertyCount{};
            vkr = vkGetPhysicalDeviceDisplayPropertiesKHR(device, &propertyCount, nullptr);
            AE_ENSURE(vkr == VK_SUCCESS);

            std::vector<VkDisplayPropertiesKHR> properties{};
            vkr = vkGetPhysicalDeviceDisplayPropertiesKHR(device, &propertyCount, properties.data());
            AE_ENSURE(vkr == VK_SUCCESS);
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
