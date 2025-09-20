if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "LinuxNamedMutex.cxx"
        "LinuxSystemAllocator.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "LinuxNamedMutex.hxx"
)

endif()
