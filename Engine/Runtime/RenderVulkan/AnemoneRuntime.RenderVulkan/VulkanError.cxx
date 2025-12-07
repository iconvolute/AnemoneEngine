#include "AnemoneRuntime.RenderVulkan/VulkanError.hxx"
#include "AnemoneRuntime.Diagnostics/Trace.hxx"

#include <iterator>

namespace Anemone
{
    std::string_view VulkanError::VkResultToString(VkResult result)
    {
        using namespace std::string_view_literals;

        // Last synced Vulkan 1.4
#define CASE_RETURN_STRING(x) case x: return #x ""sv

        switch (result)  // NOLINT(clang-diagnostic-switch-enum)
        {
            CASE_RETURN_STRING(VK_SUCCESS);
            CASE_RETURN_STRING(VK_NOT_READY);
            CASE_RETURN_STRING(VK_TIMEOUT);
            CASE_RETURN_STRING(VK_EVENT_SET);
            CASE_RETURN_STRING(VK_EVENT_RESET);
            CASE_RETURN_STRING(VK_INCOMPLETE);
            CASE_RETURN_STRING(VK_ERROR_OUT_OF_HOST_MEMORY);
            CASE_RETURN_STRING(VK_ERROR_OUT_OF_DEVICE_MEMORY);
            CASE_RETURN_STRING(VK_ERROR_INITIALIZATION_FAILED);
            CASE_RETURN_STRING(VK_ERROR_DEVICE_LOST);
            CASE_RETURN_STRING(VK_ERROR_MEMORY_MAP_FAILED);
            CASE_RETURN_STRING(VK_ERROR_LAYER_NOT_PRESENT);
            CASE_RETURN_STRING(VK_ERROR_EXTENSION_NOT_PRESENT);
            CASE_RETURN_STRING(VK_ERROR_FEATURE_NOT_PRESENT);
            CASE_RETURN_STRING(VK_ERROR_INCOMPATIBLE_DRIVER);
            CASE_RETURN_STRING(VK_ERROR_TOO_MANY_OBJECTS);
            CASE_RETURN_STRING(VK_ERROR_FORMAT_NOT_SUPPORTED);
            CASE_RETURN_STRING(VK_ERROR_FRAGMENTED_POOL);
            CASE_RETURN_STRING(VK_ERROR_UNKNOWN);
            CASE_RETURN_STRING(VK_ERROR_OUT_OF_POOL_MEMORY);
            CASE_RETURN_STRING(VK_ERROR_INVALID_EXTERNAL_HANDLE);
            CASE_RETURN_STRING(VK_ERROR_FRAGMENTATION);
            CASE_RETURN_STRING(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
            CASE_RETURN_STRING(VK_PIPELINE_COMPILE_REQUIRED);
            CASE_RETURN_STRING(VK_ERROR_NOT_PERMITTED);
            CASE_RETURN_STRING(VK_ERROR_SURFACE_LOST_KHR);
            CASE_RETURN_STRING(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
            CASE_RETURN_STRING(VK_SUBOPTIMAL_KHR);
            CASE_RETURN_STRING(VK_ERROR_OUT_OF_DATE_KHR);
            CASE_RETURN_STRING(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
            CASE_RETURN_STRING(VK_ERROR_VALIDATION_FAILED_EXT);
            CASE_RETURN_STRING(VK_ERROR_INVALID_SHADER_NV);
            CASE_RETURN_STRING(VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR);
            CASE_RETURN_STRING(VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR);
            CASE_RETURN_STRING(VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR);
            CASE_RETURN_STRING(VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR);
            CASE_RETURN_STRING(VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR);
            CASE_RETURN_STRING(VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR);
            CASE_RETURN_STRING(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
            CASE_RETURN_STRING(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
            CASE_RETURN_STRING(VK_THREAD_IDLE_KHR);
            CASE_RETURN_STRING(VK_THREAD_DONE_KHR);
            CASE_RETURN_STRING(VK_OPERATION_DEFERRED_KHR);
            CASE_RETURN_STRING(VK_OPERATION_NOT_DEFERRED_KHR);
            CASE_RETURN_STRING(VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR);
            CASE_RETURN_STRING(VK_ERROR_COMPRESSION_EXHAUSTED_EXT);
            CASE_RETURN_STRING(VK_INCOMPATIBLE_SHADER_BINARY_EXT);
            CASE_RETURN_STRING(VK_PIPELINE_BINARY_MISSING_KHR);
            CASE_RETURN_STRING(VK_ERROR_NOT_ENOUGH_SPACE_KHR);
        
        default:
            break;
        }

#undef CASE_RETURN_STRING

        return "Unknown VkResult";
    }

    void VulkanError::ReportError(VkResult result)
    {
        fmt::memory_buffer buffer{};
        fmt::format_to(std::back_inserter(buffer), "Vulkan error: {} ({})", VkResultToString(result), std::to_underlying(result));
        std::string_view message{buffer.data(), buffer.size()};

        AE_TRACE(Fatal, "{}", message);
        Debug::ReportApplicationStop(message);
    }

    void VulkanError::ReportError(VkResult result, std::source_location const& location)
    {
        fmt::memory_buffer buffer{};
        fmt::format_to(std::back_inserter(buffer), "Vulkan error: {} ({}) at {}:{}", VkResultToString(result), std::to_underlying(result), location.file_name(), location.line());
        std::string_view message{buffer.data(), buffer.size()};

        AE_TRACE(Fatal, "{}", message);
        Debug::ReportApplicationStop(message);
    }
}
