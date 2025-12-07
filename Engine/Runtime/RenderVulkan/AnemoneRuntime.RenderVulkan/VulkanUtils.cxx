#include "AnemoneRuntime.RenderVulkan/VulkanUtils.hxx"
#include "AnemoneRuntime.Diagnostics/Trace.hxx"
#include "AnemoneRuntime.RenderVulkan/VulkanPhysicalDevice.hxx"

#include <algorithm>

#if ANEMONE_VULKAN_VALIDATION

namespace Anemone::VulkanGraphics
{
    void SetObjectNameImpl(
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
}

#endif

//    VkResult EnumeratePhysicalDeviceSurfaceFormats(
//        VkPhysicalDevice physicalDevice,
//        VkSurfaceKHR surface,
//        std::vector<VkSurfaceFormatKHR>& outFormats)
//    {
//        VkResult result;
//        uint32_t count = 0;
//
//        do
//        {
//            result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, nullptr);
//
//            if (result == VK_SUCCESS)
//            {
//                outFormats.resize(count);
//                result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, outFormats.data());
//            }
//        } while (result == VK_INCOMPLETE);
//
//        if (count < outFormats.size())
//        {
//            outFormats.resize(count);
//        }
//
//        return result;
//    }
//
//    VkResult EnumerateSurfacePresentModes(
//        VkPhysicalDevice physicalDevice,
//        VkSurfaceKHR surface,
//        std::vector<VkPresentModeKHR>& outPresentModes)
//    {
//        VkResult result;
//        uint32_t count = 0;
//
//        do
//        {
//            result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, nullptr);
//
//            if (result == VK_SUCCESS)
//            {
//                outPresentModes.resize(count);
//                result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, outPresentModes.data());
//            }
//        } while (result == VK_INCOMPLETE);
//
//        if (count < outPresentModes.size())
//        {
//            outPresentModes.resize(count);
//        }
//
//        return result;
//    }
//}
