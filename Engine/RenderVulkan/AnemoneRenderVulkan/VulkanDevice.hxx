#pragma once
#include "AnemoneRender/Device.hxx"
#include "AnemoneRenderVulkan/VulkanHeaders.hxx"

namespace Anemone
{
    class VulkanDevice : public RenderDevice
    {
    private:
        VkInstance _instance{};
        VkDevice _device{};
#if !ANEMONE_BUILD_SHIPPING
        VkDebugReportCallbackEXT _debugReportCallback{};
#endif

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
