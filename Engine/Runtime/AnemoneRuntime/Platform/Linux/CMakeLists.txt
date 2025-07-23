if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "LinuxNamedMutex.cxx"
        "LinuxNamedMutex.hxx"

        "LinuxStackTrace.cxx"
)

endif()
