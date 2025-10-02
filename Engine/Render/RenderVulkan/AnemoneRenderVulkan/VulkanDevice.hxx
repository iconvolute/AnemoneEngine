#pragma once
#include "AnemoneRender/Device.hxx"
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#if ANEMONE_PLATFORM_WINDOWS
#include <vulkan/vulkan_win32.h>
#endif

#include <volk.h>

namespace Anemone
{
    class VulkanDevice : public RenderDevice
    {
    private:
        VkInstance _instance{};
        VkDevice _device{};
        VkDebugReportCallbackEXT _debugReportCallback{};

    public:
        VulkanDevice();
        VulkanDevice(VulkanDevice const&) = delete;
        VulkanDevice(VulkanDevice&&) = delete;
        ~VulkanDevice() override;
        VulkanDevice& operator=(VulkanDevice const&) = delete;
        VulkanDevice& operator=(VulkanDevice&&) = delete;
    };

    RENDER_VULKAN_API Reference<RenderDevice> CreateRenderDevice();
}
