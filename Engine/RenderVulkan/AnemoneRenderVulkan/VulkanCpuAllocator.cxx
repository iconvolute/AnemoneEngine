#include "AnemoneRenderVulkan/VulkanCpuAllocator.hxx"

#include <cstdlib>

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneApplication/Platform/Windows/WindowsHostWindow.hxx"
#endif

#if !ANEMONE_BUILD_SHIPPING

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

#if defined(_MSC_VER)
        return _aligned_malloc(size, alignment);
#else
        // aligned_alloc requires size % alignment == 0
        size_t alignedSize = (size + alignment - 1) & ~(alignment - 1);
        return aligned_alloc(alignment, alignedSize);
#endif
    }

    void VKAPI_PTR FnVkFreeFunction(
        void* pUserData,
        void* pMemory)
    {
        (void)pUserData;
#if defined(_MSC_VER)
        _aligned_free(pMemory);
#else
        free(pMemory);
#endif
    }

    void VKAPI_PTR FnVkInternalAllocationNotification(
        void* pUserData,
        size_t size,
        VkInternalAllocationType allocationType,
        VkSystemAllocationScope allocationScope)
    {
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
#if defined(_MSC_VER)
            _aligned_free(pOriginal);
#else
            free(pOriginal);
#endif
            return nullptr;
        }

#if defined(_MSC_VER)
        void* result = _aligned_realloc(pOriginal, size, alignment);
        if (!result && size)
        {
            // Fallback: manually allocate + copy on failure
            void* newMem = _aligned_malloc(size, alignment);
            if (newMem && pOriginal)
            {
                // Copy old content (unknown old size, so copy min)
                memcpy(newMem, pOriginal, size);
            }
            _aligned_free(pOriginal);
            result = newMem;
        }
#else
        // aligned_alloc requires size multiple of alignment
        size_t alignedSize = (size + alignment - 1) & ~(alignment - 1);
        void* newMem = aligned_alloc(alignment, alignedSize);
        if (newMem && pOriginal)
        {
            // No reliable old size, assume smaller of new/old
            memcpy(newMem, pOriginal, size);
            free(pOriginal);
        }
        void* result = newMem;
#endif
        return result;
    }

    VkAllocationCallbacks gVulkanCpuAllocator{
        .pUserData = nullptr,
        .pfnAllocation = &FnVkAllocationFunction,
        .pfnReallocation = &FnVkReallocationFunction,
        .pfnFree = &FnVkFreeFunction,
        .pfnInternalAllocation = &FnVkInternalAllocationNotification,
        .pfnInternalFree = &FnVkInternalFreeNotification,
    };

    VkAllocationCallbacks* VulkanCpuAllocator = &gVulkanCpuAllocator;
}
#endif
