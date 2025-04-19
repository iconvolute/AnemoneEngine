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
        "LinuxFileHandle.cxx"
        "LinuxFileHandle.hxx"
        "LinuxProcess.cxx"
        "LinuxProcess.hxx"
        "LinuxProcessorProperties.cxx"
        "LinuxProcessorProperties.hxx"
        "LinuxSharedLibrary.cxx"
        "LinuxSharedLibrary.hxx"
        "LinuxWindow.cxx"
        "LinuxWindow.hxx"
)

endif()
