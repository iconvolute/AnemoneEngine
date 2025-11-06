# https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/releases/tag/v3.3.0
anemone_add_sdk(SdkVulkanMemoryAllocator STATIC)

target_link_libraries(SdkVulkanMemoryAllocator PUBLIC SdkVolk)

add_subdirectory("include")
add_subdirectory("source")
