anemone_add_executable(AnemoneCrashReporter)
target_sources(AnemoneCrashReporter
    PRIVATE
        "Main.cxx"
)
target_link_libraries(AnemoneCrashReporter PUBLIC AnemoneRuntime)

add_subdirectory("Resources")
