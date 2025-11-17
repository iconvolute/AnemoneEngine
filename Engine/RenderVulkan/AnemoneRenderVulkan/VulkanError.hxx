#pragma once
#include "AnemoneRenderVulkan/VulkanHeaders.hxx"
#include "AnemoneDiagnostics/Debug.hxx"

namespace Anemone
{
    struct VulkanError
    {
        static void ReportError(VkResult result);
    };
}

#if ANEMONE_BUILD_SHIPPING

#define AE_VULKAN_ENSURE(expression) (void)(expression)

#else

#define AE_VULKAN_ENSURE(expression) \
    do \
    { \
        VkResult const aeVulkanEnsureResult = (expression); \
        if (aeVulkanEnsureResult != VK_SUCCESS) \
        { \
            Anemone::VulkanError::ReportError(aeVulkanEnsureResult); \
        } \
    } while (false)

#endif
