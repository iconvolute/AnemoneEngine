#include "AnemoneRenderVulkan/VulkanError.hxx"
#include "AnemoneDiagnostics/Trace.hxx"

namespace Anemone
{
    void VulkanError::ReportError([[maybe_unused]] VkResult result)
    {
#if ANEMONE_VULKAN_VALIDATION
        AE_PANIC("Vulkan error: {} ({})", string_VkResult(result), std::to_underlying(result));
#else
        AE_PANIC("Vulkan error: {}", std::to_underlying(result));
#endif
    }
}
