if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "LinuxFileSystem.cxx"

        "LinuxNamedMutex.cxx"
        "LinuxNamedMutex.hxx"

        "LinuxStackTrace.cxx"

        "LinuxSystemAllocator.cxx"
)

endif()
