target_sources(AnemoneRuntime
    PRIVATE
        "Debugger.hxx"
)

add_subdirectory("Linux")
add_subdirectory("Windows")
