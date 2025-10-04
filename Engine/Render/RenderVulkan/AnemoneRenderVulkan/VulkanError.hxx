#pragma once
#include "AnemoneRenderVulkan/VulkanHeaders.hxx"

namespace Anemone
{
    struct VulkanError
    {
        static void ReportError(VkResult result);
    };
}

#if ANEMONE_BUILD_SHIPPING

#define AE_VK_CALL(expression) (void)(expression)

#else

#define AE_VK_CALL(expression) \
    do { \
        VkResult const _ae_vk_result = (expression); \
        if (_ae_vk_result != VK_SUCCESS) { \
            Anemone::VulkanError::ReportError(_ae_vk_result); \
            AE_PANIC("Vulkan call failed: {} at {}:{}", #expression, __FILE__, __LINE__); \
        } \
    } while (false)

#endif
