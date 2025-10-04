add_subdirectory("Catch2")
add_subdirectory("Fmt")
add_subdirectory("LZ4")
add_subdirectory("NVTX")


if (Vulkan_FOUND AND WIN32)

add_subdirectory("volk")

endif()
