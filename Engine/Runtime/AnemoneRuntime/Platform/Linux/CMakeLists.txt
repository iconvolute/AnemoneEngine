if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "LinuxNamedMutex.cxx"
        "LinuxNamedMutex.hxx"
)

endif()
