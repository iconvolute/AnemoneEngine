add_executable(
    AnemoneCrashReporter
)

target_sources(AnemoneCrashReporter
    PRIVATE
        "Main.cxx"
)

target_link_libraries(AnemoneCrashReporter
    PRIVATE
        "Dbghelp.lib"
)

add_subdirectory("Resources")
