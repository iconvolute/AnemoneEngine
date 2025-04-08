if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "LinuxFileHandle.cxx"
        "LinuxFileHandle.hxx"

        "LinuxFileSystem.cxx"

        "LinuxMemoryMappedFile.cxx"
        "LinuxMemoryMappedFile.hxx"

        "LinuxNamedMutex.cxx"
        "LinuxNamedMutex.hxx"

        "LinuxProcess.cxx"
        "LinuxProcess.hxx"

        "LinuxSharedLibrary.cxx"
        "LinuxSharedLibrary.hxx"

        "LinuxStackTrace.cxx"

        "LinuxSystemAllocator.cxx"
)

endif()
