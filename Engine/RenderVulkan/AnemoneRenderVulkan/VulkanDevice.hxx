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
