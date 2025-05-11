add_subdirectory("Linux")
add_subdirectory("Windows")

target_sources(AnemoneRuntime
    PRIVATE
        "FilePath.cxx"
        "FilePath.hxx"
        "FileSystem.hxx"
        "NamedMutex.hxx"
        "StackTrace.hxx"
        "Types.hxx"
)
