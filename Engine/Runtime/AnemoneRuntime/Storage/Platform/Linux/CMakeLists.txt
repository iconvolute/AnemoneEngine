if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "LinuxFileHandle.cxx"
        "LinuxFileSystem.cxx"
        "LinuxMemoryMappedFile.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "LinuxFileHandle.hxx"
        "LinuxFileSystem.hxx"
        "LinuxMemoryMappedFile.hxx"
)

endif()
