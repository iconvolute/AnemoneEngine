if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "FileHandle.cxx"
        "FileHandle.hxx"
        "FileSystem.cxx"
        "FileSystem.hxx"
)

endif()
