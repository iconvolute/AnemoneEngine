anemone_add_console_executable(DevDebugger)

target_sources(DevDebugger
    PRIVATE
        "Main.cxx"
)

target_link_libraries(DevDebugger
    PRIVATE
        "dbgeng.lib"
)

add_subdirectory("Resources")
