add_subdirectory("Linux")
add_subdirectory("Windows")

target_sources(AnemoneRuntime
    PRIVATE
        "FilePath.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "FilePath.hxx"
        "NamedMutex.hxx"
        "Types.hxx"
)
