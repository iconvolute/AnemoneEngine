if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "LinuxFileSystem.cxx"

        "LinuxMemoryMappedFile.cxx"
        "LinuxMemoryMappedFile.hxx"

        "LinuxNamedMutex.cxx"
        "LinuxNamedMutex.hxx"

        "LinuxSharedLibrary.cxx"
        "LinuxSharedLibrary.hxx"

        "LinuxStackTrace.cxx"

        "LinuxSystemAllocator.cxx"
)

endif()
