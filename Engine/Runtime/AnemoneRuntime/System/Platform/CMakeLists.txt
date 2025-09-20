add_subdirectory("Linux")
add_subdirectory("Windows")

target_sources(AnemoneRuntime
    PUBLIC FILE_SET HEADERS FILES
        "Platform.hxx"
        "Windows.hxx"
        "Linux.hxx"
)
