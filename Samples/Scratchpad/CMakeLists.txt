anemone_add_target(
    EXECUTABLE
    NAME
        Scratchpad
    BUILD_DEPENDENCIES
        PUBLIC
            AnemoneApplication
            AnemoneMemory
            AnemoneTasks
            AnemoneNetwork
            AnemoneEntryPoint
            AnemoneRuntime
)

target_sources(Scratchpad
    PRIVATE
        "Main.cxx"
)

if (Vulkan_FOUND)

target_link_libraries(Scratchpad
    PUBLIC
        AnemoneRenderVulkan
        Vulkan::shaderc_combined
        SdkSpirvReflect
)

endif()

add_subdirectory("Resources")
