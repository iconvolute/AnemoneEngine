#pragma once
#include "AnemoneRuntime.RenderVulkan/VulkanHeaders.hxx"

namespace Anemone
{
#if ANEMONE_BUILD_SHIPPING
    inline constexpr VkAllocationCallbacks* VulkanCpuAllocator = nullptr;
#else
    extern VkAllocationCallbacks* VulkanCpuAllocator;
#endif
}
