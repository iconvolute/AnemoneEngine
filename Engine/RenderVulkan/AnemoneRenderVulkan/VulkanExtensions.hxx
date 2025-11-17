#pragma once
#include "AnemoneRenderVulkan/VulkanHeaders.hxx"

namespace Anemone
{
    class VulkanDevice;
    struct VulkanDeviceFeatures;

    class VulkanDeviceExtension
    {
    private:
        const char* m_name{};
        uint32_t m_coreVersion{};
        bool m_available{};
        bool m_enabled{};

    protected:
        VulkanDeviceExtension(const char* name, uint32_t coreVersion)
            : m_name{name}
            , m_coreVersion{coreVersion}
        {
        }

        VulkanDeviceExtension(const char* name)
            : VulkanDeviceExtension(name, UINT32_MAX)
        {
        }

        virtual ~VulkanDeviceExtension() = default;

        virtual void BeforeQueryFeatures(VulkanDeviceFeatures& features, VkPhysicalDeviceFeatures2& query)
        {
            (void)features;
            (void)query;
        }

        virtual void AfterQueryFeatures(VulkanDeviceFeatures& features, VkPhysicalDeviceFeatures2& query)
        {
            (void)features;
            (void)query;
        }

        virtual void BeforeCreateDevice(VulkanDevice& device, VkDeviceCreateInfo& createInfo)
        {
            (void)device;
            (void)createInfo;
        }
    };

    struct RHIVkPhysicalDeviceVulkan14 : VulkanDeviceExtension
    {
        RHIVkPhysicalDeviceVulkan14()
            : VulkanDeviceExtension{nullptr, VK_API_VERSION_1_4}
        {
        }

        VkPhysicalDeviceVulkan14Features features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES};
        VkPhysicalDeviceVulkan14Properties properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_PROPERTIES};
    };

    struct RHIVkPhysicalDeviceVulkan13 : VulkanDeviceExtension
    {
        RHIVkPhysicalDeviceVulkan13()
            : VulkanDeviceExtension{nullptr, VK_API_VERSION_1_3}
        {
        }

        VkPhysicalDeviceVulkan13Features features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
        VkPhysicalDeviceVulkan13Properties properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES};
    };

    struct RHIVulkanDeviceExtensions final
    {
        RHIVkPhysicalDeviceVulkan14 vulkan13{};
        RHIVkPhysicalDeviceVulkan14 vulkan14{};
    };
}
