if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "LinuxClipboard.cxx"
        "LinuxClipboard.hxx"
        "LinuxDialogs.cxx"
        "LinuxDialogs.hxx"
        "LinuxProcessorProperties.cxx"
        "LinuxProcessorProperties.hxx"
)

endif()
