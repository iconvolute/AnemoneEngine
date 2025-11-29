add_subdirectory("Catch2")
add_subdirectory("Fmt")
add_subdirectory("LZ4")
add_subdirectory("NVTX")


if (Vulkan_FOUND)

add_subdirectory("volk")
add_subdirectory("VulkanMemoryAllocator")
add_subdirectory("SPIRV-Reflect")

endif()
