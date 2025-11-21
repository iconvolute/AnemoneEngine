#pragma once
#include "AnemoneRenderVulkan/VulkanHeaders.hxx"
#include "AnemoneDiagnostics/Debug.hxx"

namespace Anemone
{
    struct VulkanError
    {
        static std::string_view VkResultToString(VkResult result);

        static void ReportError(VkResult result);
        static void ReportError(VkResult result, std::source_location const& location);
    };
}

// Include source location only in validation builds to reduce overhead in release builds

#if ANEMONE_VULKAN_VALIDATION

#define AE_VULKAN_ENSURE(expression) \
    do \
    { \
        VkResult const aeVulkanEnsureResult = (expression); \
        if (aeVulkanEnsureResult != VK_SUCCESS) \
        { \
            Anemone::VulkanError::ReportError(aeVulkanEnsureResult, std::source_location::current()); \
            anemone_debugbreak(); \
        } \
    } while (false)

#else

#define AE_VULKAN_ENSURE(expression) \
    do \
    { \
        VkResult const aeVulkanEnsureResult = (expression); \
        if (aeVulkanEnsureResult != VK_SUCCESS) \
        { \
            Anemone::VulkanError::ReportError(aeVulkanEnsureResult); \
            anemone_debugbreak(); \
        } \
    } while (false)

#endif
