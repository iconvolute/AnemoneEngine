target_sources(AnemoneRuntime
    PRIVATE
        "Debugger.hxx"
        "Error.hxx"
)

add_subdirectory("Common")
add_subdirectory("Linux")
add_subdirectory("Windows")
