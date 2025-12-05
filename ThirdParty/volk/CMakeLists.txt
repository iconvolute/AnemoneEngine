# C:\VulkanSDK\1.4.328.1\Include\Volk
anemone_add_target(
    STATIC SDK
    NAME
        SdkVolk
)

target_include_directories(SdkVolk
    PUBLIC
        ${Vulkan_INCLUDE_DIRS}
)

if (WIN32)
target_compile_definitions(SdkVolk
    PUBLIC
        VK_USE_PLATFORM_WIN32_KHR
)
endif()

if (LINUX)
target_compile_definitions(SdkVolk
    PUBLIC
        VK_USE_PLATFORM_XLIB_KHR
        VK_USE_PLATFORM_WAYLAND_KHR
)
endif()

if (ANDROID)
target_compile_definitions(SdkVolk
    PUBLIC
        VK_USE_PLATFORM_ANDROID_KHR
)
endif()

add_subdirectory("source")
add_subdirectory("include")
