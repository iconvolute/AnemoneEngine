# https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/releases/tag/v3.3.0
anemone_add_target(
    STATIC SDK
    NAME
        SdkVulkanMemoryAllocator
)

target_link_libraries(SdkVulkanMemoryAllocator PUBLIC SdkVolk)

add_subdirectory("include")
add_subdirectory("source")
