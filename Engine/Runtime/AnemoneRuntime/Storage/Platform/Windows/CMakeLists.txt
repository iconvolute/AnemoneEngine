if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "FileHandle.cxx"
        "FileSystem.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "FileHandle.hxx"
        "FileSystem.hxx"
)

endif()
