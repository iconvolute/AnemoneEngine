add_subdirectory("Base")
add_subdirectory("Linux")
add_subdirectory("Unix")
add_subdirectory("Windows")

target_sources(AnemoneRuntime
    PRIVATE
        "EntryPoint.hxx"
        "FilePath.cxx"
        "FilePath.hxx"
        "FileSystem.hxx"
        "NamedMutex.hxx"
        "StackTrace.hxx"
        "Types.hxx"
)
