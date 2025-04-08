if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "LinuxApplication.cxx"
        "LinuxApplication.hxx"
        "LinuxClipboard.cxx"
        "LinuxClipboard.hxx"
        "LinuxDialogs.cxx"
        "LinuxDialogs.hxx"
        "LinuxEnvironment.cxx"
        "LinuxEnvironment.hxx"
        "LinuxProcessorProperties.cxx"
        "LinuxProcessorProperties.hxx"
        "LinuxWindow.cxx"
        "LinuxWindow.hxx"
)

endif()
