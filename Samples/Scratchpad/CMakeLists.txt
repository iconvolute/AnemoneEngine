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
)

if (Vulkan_FOUND AND WIN32)

target_link_libraries(Scratchpad
    PUBLIC
        AnemoneRenderVulkan
)

endif()

add_subdirectory("Resources")
