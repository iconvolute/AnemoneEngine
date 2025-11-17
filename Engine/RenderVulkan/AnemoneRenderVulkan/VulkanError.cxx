#include "AnemoneRenderVulkan/VulkanError.hxx"
#include "AnemoneDiagnostics/Trace.hxx"

namespace Anemone
{
    void VulkanError::ReportError([[maybe_unused]] VkResult result)
    {
        AE_PANIC("Vulkan error: {} ({})", string_VkResult(result), static_cast<int>(result));
    }
}
