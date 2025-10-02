anemone_add_ui_executable(Scratchpad)

target_sources(Scratchpad
    PRIVATE
        "Main.cxx"
)
target_link_libraries(Scratchpad
    PUBLIC
        AnemoneRuntime
        AnemoneApplication
        AnemoneMemory
        AnemoneRenderVulkan
)

add_subdirectory("Resources")
