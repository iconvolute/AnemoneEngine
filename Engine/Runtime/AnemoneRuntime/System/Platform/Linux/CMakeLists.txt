if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "LinuxClipboard.cxx"
        "LinuxClipboard.hxx"
        "LinuxDialogs.cxx"
        "LinuxDialogs.hxx"
        "LinuxEnvironment.cxx"
        "LinuxEnvironment.hxx"
        "LinuxProcessorProperties.cxx"
        "LinuxProcessorProperties.hxx"
)

endif()
