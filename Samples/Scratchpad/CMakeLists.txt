anemone_add_console_executable(Scratchpad)

target_sources(Scratchpad
    PRIVATE
        "Main.cxx"
)
target_link_libraries(Scratchpad
    PUBLIC
        AnemoneRuntime
        AnemoneApplication
        AnemoneMemory
        AnemoneTasks
        AnemoneNetwork
        AnemoneEntryPoint
)

if (Vulkan_FOUND)

target_link_libraries(Scratchpad
    PUBLIC
        AnemoneRenderVulkan
)

endif()

add_subdirectory("Resources")
