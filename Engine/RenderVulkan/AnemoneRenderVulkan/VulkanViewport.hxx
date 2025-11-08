#pragma once
#include "AnemoneRenderVulkan/VulkanHeaders.hxx"
#include "AnemoneBase/Intrusive.hxx"
#include "AnemoneBase/Reference.hxx"
#include "AnemoneApplication/HostWindow.hxx"

namespace Anemone
{
    class VulkanDevice;

    struct VulkanViewportSurface
    {
        VkSemaphore presentSemaphore{};
        VkSemaphore renderSemaphore{};

    };

    class VulkanViewport
        : public ReferenceCounted<VulkanViewport>
        , private IntrusiveListNode<VulkanViewport>
    {
    private:
        VulkanDevice* m_device{};
        Reference<HostWindow> m_window{};

    public:
        VulkanViewport(VulkanDevice* device, Reference<HostWindow> window)
            : m_device{device}
            , m_window(std::move(window))
        {
        }

        ~VulkanViewport()
        {
        }


    };
}
