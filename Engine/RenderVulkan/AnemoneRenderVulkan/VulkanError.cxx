#include "AnemoneRenderVulkan/VulkanError.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

namespace Anemone
{
    namespace
    {
        constexpr const char* VkResultToString(VkResult result)
        {
            switch (result)
            {
#define AE_CASE(x) \
    case x: \
        return #x
                AE_CASE(VK_SUCCESS);
                AE_CASE(VK_NOT_READY);
                AE_CASE(VK_TIMEOUT);
                AE_CASE(VK_EVENT_SET);
                AE_CASE(VK_EVENT_RESET);
                AE_CASE(VK_INCOMPLETE);
                AE_CASE(VK_ERROR_OUT_OF_HOST_MEMORY);
                AE_CASE(VK_ERROR_OUT_OF_DEVICE_MEMORY);
                AE_CASE(VK_ERROR_INITIALIZATION_FAILED);
                AE_CASE(VK_ERROR_DEVICE_LOST);
                AE_CASE(VK_ERROR_MEMORY_MAP_FAILED);
                AE_CASE(VK_ERROR_LAYER_NOT_PRESENT);
                AE_CASE(VK_ERROR_EXTENSION_NOT_PRESENT);
                AE_CASE(VK_ERROR_FEATURE_NOT_PRESENT);
                AE_CASE(VK_ERROR_INCOMPATIBLE_DRIVER);
                AE_CASE(VK_ERROR_TOO_MANY_OBJECTS);
                AE_CASE(VK_ERROR_FORMAT_NOT_SUPPORTED);
                AE_CASE(VK_ERROR_FRAGMENTED_POOL);
                AE_CASE(VK_ERROR_UNKNOWN);
                AE_CASE(VK_ERROR_OUT_OF_POOL_MEMORY);
                AE_CASE(VK_ERROR_INVALID_EXTERNAL_HANDLE);
                AE_CASE(VK_ERROR_FRAGMENTATION);
                AE_CASE(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
                AE_CASE(VK_PIPELINE_COMPILE_REQUIRED);
                AE_CASE(VK_ERROR_NOT_PERMITTED);
                AE_CASE(VK_ERROR_SURFACE_LOST_KHR);
                AE_CASE(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
                AE_CASE(VK_SUBOPTIMAL_KHR);
                AE_CASE(VK_ERROR_OUT_OF_DATE_KHR);
                AE_CASE(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
                AE_CASE(VK_ERROR_VALIDATION_FAILED_EXT);
                AE_CASE(VK_ERROR_INVALID_SHADER_NV);
                AE_CASE(VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR);
                AE_CASE(VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR);
                AE_CASE(VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR);
                AE_CASE(VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR);
                AE_CASE(VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR);
                AE_CASE(VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR);
                AE_CASE(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
                AE_CASE(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
                AE_CASE(VK_THREAD_IDLE_KHR);
                AE_CASE(VK_THREAD_DONE_KHR);
                AE_CASE(VK_OPERATION_DEFERRED_KHR);
                AE_CASE(VK_OPERATION_NOT_DEFERRED_KHR);
                AE_CASE(VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR);
                AE_CASE(VK_ERROR_COMPRESSION_EXHAUSTED_EXT);
                AE_CASE(VK_INCOMPATIBLE_SHADER_BINARY_EXT);
                AE_CASE(VK_PIPELINE_BINARY_MISSING_KHR);
                AE_CASE(VK_ERROR_NOT_ENOUGH_SPACE_KHR);
#undef AE_CASE

            case VK_RESULT_MAX_ENUM: // silly, but at least compiler will tell us about new enum values
                break;
            }

            return "Unknown";
        }
    }

    void VulkanError::ReportError([[maybe_unused]] VkResult result)
    {
        AE_PANIC("Vulkan error: {} ({})", VkResultToString(result), static_cast<int>(result));
    }
}
