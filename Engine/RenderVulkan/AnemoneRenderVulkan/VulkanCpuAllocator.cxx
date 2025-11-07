#include "AnemoneRenderVulkan/VulkanCpuAllocator.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneApplication/Platform/Windows/WindowsHostWindow.hxx"
#endif

namespace Anemone
{
    void* VKAPI_PTR FnVkAllocationFunction(
        void* pUserData,
        size_t size,
        size_t alignment,
        VkSystemAllocationScope allocationScope)
    {
        (void)pUserData;
        (void)allocationScope;
        void* result = _aligned_malloc(size, alignment);
        // AE_TRACE(Error, "VK::Alloc size={}, alignment={}, scope={} => {}", size, alignment, std::to_underlying(allocationScope), fmt::ptr(result));
        return result;
    }

    void VKAPI_PTR FnVkFreeFunction(
        void* pUserData,
        void* pMemory)
    {
        // AE_TRACE(Error, "VK::Free ptr={}", fmt::ptr(pMemory));

        (void)pUserData;
        _aligned_free(pMemory);
    }

    void VKAPI_PTR FnVkInternalAllocationNotification(
        void* pUserData,
        size_t size,
        VkInternalAllocationType allocationType,
        VkSystemAllocationScope allocationScope)
    {
        // AE_TRACE(Error, "VK::InternalAlloc size={}, type={}, scope={}", size, std::to_underlying(allocationType), std::to_underlying(allocationScope));
        (void)pUserData;
        (void)size;
        (void)allocationType;
        (void)allocationScope;
    }

    void VKAPI_PTR FnVkInternalFreeNotification(
        void* pUserData,
        size_t size,
        VkInternalAllocationType allocationType,
        VkSystemAllocationScope allocationScope)
    {
        // AE_TRACE(Error, "VK::InternalFree size={}, type={}, scope={}", size, std::to_underlying(allocationType), std::to_underlying(allocationScope));
        (void)pUserData;
        (void)size;
        (void)allocationType;
        (void)allocationScope;
    }

    void* VKAPI_PTR FnVkReallocationFunction(
        void* pUserData,
        void* pOriginal,
        size_t size,
        size_t alignment,
        VkSystemAllocationScope allocationScope)
    {
        (void)pUserData;
        (void)allocationScope;

        if (size == 0)
        {
            // AE_TRACE(Error, "VM::Realloc ptr={}, size=0 => free", fmt::ptr(pOriginal));
            _aligned_free(pOriginal);
            return nullptr;
        }

        void* result = _aligned_realloc(pOriginal, size, alignment);
        // AE_TRACE(Error, "VK::Realloc ptr={}, size={}, alignment={}, scope={} => {}", fmt::ptr(pOriginal), size, alignment, std::to_underlying(allocationScope), fmt::ptr(result));
        return result;
    }

    VkAllocationCallbacks VulkanCpuAllocator{
        .pUserData = nullptr,
        .pfnAllocation = &FnVkAllocationFunction,
        .pfnReallocation = &FnVkReallocationFunction,
        .pfnFree = &FnVkFreeFunction,
        .pfnInternalAllocation = &FnVkInternalAllocationNotification,
        .pfnInternalFree = &FnVkInternalFreeNotification,
    };
}
