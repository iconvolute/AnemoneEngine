add_subdirectory("Linux")
add_subdirectory("Windows")

target_sources(AnemoneRuntime
    PRIVATE
        "Platform.hxx"
        "Windows.hxx"
        "Linux.hxx"
)
