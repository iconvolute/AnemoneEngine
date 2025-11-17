#pragma once
#include "AnemoneRenderVulkan/VulkanHeaders.hxx"

namespace Anemone
{
#if ANEMONE_BUILD_SHIPPING
    inline constexpr VkAllocationCallbacks* VulkanCpuAllocator = nullptr;
#else
    extern VkAllocationCallbacks* VulkanCpuAllocator;
#endif
}
