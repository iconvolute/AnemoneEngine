anemone_add_executable(Scratchpad)
target_sources(Scratchpad
    PRIVATE
        "Main.cxx"
)
target_link_libraries(Scratchpad PUBLIC AnemoneRuntime)

if (WIN32)
set_target_properties(Scratchpad
    PROPERTIES
        WIN32_EXECUTABLE TRUE
)
endif()

add_subdirectory("Resources")
