if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "FileHandle.cxx"
        "FileHandle.hxx"
        "FileSystem.cxx"
        "FileSystem.hxx"
)

endif()
