anemone_add_target(
    EXECUTABLE
    NAME
        Scratchpad
    BUILD_DEPENDENCIES
        PUBLIC
            AnemoneRuntime.EntryPoint
            AnemoneRuntime.Base
)

target_sources(Scratchpad
    PRIVATE
        "Main.cxx"
)

if (Vulkan_FOUND)

target_link_libraries(Scratchpad
    PUBLIC
        AnemoneRuntime.RenderVulkan
        Vulkan::shaderc_combined
        SdkSpirvReflect
)

endif()

add_subdirectory("Resources")
