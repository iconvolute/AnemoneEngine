if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "LinuxApplication.cxx"
        "LinuxApplication.hxx"

        "LinuxEnvironment.cxx"

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

        "LinuxWindow.cxx"
        "LinuxWindow.hxx"
)

endif()
