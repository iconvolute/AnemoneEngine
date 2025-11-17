#pragma once
#include "AnemoneRenderVulkan/VulkanHeaders.hxx"
#include "AnemoneRenderVulkan/VulkanError.hxx"

#include <bit>

#if ANEMONE_VULKAN_VALIDATION

namespace Anemone
{
    inline void RHISetObjectNameImpl(
        VkDevice device,
        VkObjectType objectType,
        uint64_t objectHandle,
        const char* name)
    {
        VkDebugUtilsObjectNameInfoEXT debugUtilsObjectNameInfo{
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .pNext = nullptr,
            .objectType = objectType,
            .objectHandle = objectHandle,
            .pObjectName = name,
        };
        AE_VULKAN_ENSURE(vkSetDebugUtilsObjectNameEXT(device, &debugUtilsObjectNameInfo));
    }

    inline void RHISetObjectName(VkDevice device, VkFence handle, const char* name)
    {
        RHISetObjectNameImpl(device, VK_OBJECT_TYPE_FENCE, std::bit_cast<uint64_t>(handle), name);
    }

    inline void RHISetObjectName(VkDevice device, VkSemaphore handle, const char* name)
    {
        RHISetObjectNameImpl(device, VK_OBJECT_TYPE_SEMAPHORE, std::bit_cast<uint64_t>(handle), name);
    }

    inline void RHISetObjectName(VkDevice device, VkImage handle, const char* name)
    {
        RHISetObjectNameImpl(device, VK_OBJECT_TYPE_IMAGE, std::bit_cast<uint64_t>(handle), name);
    }

    inline void RHISetObjectName(VkDevice device, VkImageView handle, const char* name)
    {
        RHISetObjectNameImpl(device, VK_OBJECT_TYPE_IMAGE_VIEW, std::bit_cast<uint64_t>(handle), name);
    }

    inline void RHISetObjectName(VkDevice device, VkBuffer handle, const char* name)
    {
        RHISetObjectNameImpl(device, VK_OBJECT_TYPE_BUFFER, std::bit_cast<uint64_t>(handle), name);
    }
}

#define AE_VULKAN_SET_OBJECT_NAME(device, handle, name) Anemone::RHISetObjectName(device, handle, name)

#else

#define AE_VULKAN_SET_OBJECT_NAME(device, handle, name) \
    do \
    { \
    } while (false)

#endif

namespace Anemone
{
    template <typename Head, typename Tail>
    anemone_forceinline void RHIVulkanPushStruct(Head& head, Tail& tail)
    {
        tail.pNext = const_cast<void*>(head.pNext);
        head.pNext = &tail;
    }

    template <typename T>
    anemone_forceinline void RHIVulkanInitialize(T& object, VkStructureType type)
    {
        static_assert(!std::is_pointer_v<T>);
        static_assert(sizeof(T::sType) == sizeof(uint32_t));
        object = {.sType = type};
    }

    template <typename T, typename U>
    anemone_forceinline void RHIVulkanInitialize(T& object, VkStructureType type, U& next)
    {
        static_assert(!std::is_pointer_v<T>);
        static_assert(!std::is_pointer_v<U>);
        static_assert(sizeof(T::sType) == sizeof(uint32_t));
        object = {.sType = type, .pNext = &next};
    }
}
