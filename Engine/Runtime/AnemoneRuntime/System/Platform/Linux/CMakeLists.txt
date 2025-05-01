if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "LinuxApplication.cxx"
        "LinuxApplication.hxx"
        "LinuxClipboard.cxx"
        "LinuxDialogs.cxx"
        "LinuxEnvironment.cxx"
        "LinuxFileHandle.cxx"
        "LinuxFileHandle.hxx"
        "LinuxProcess.cxx"
        "LinuxProcess.hxx"
        "LinuxProcessorProperties.cxx"
        "LinuxSharedLibrary.cxx"
        "LinuxSharedLibrary.hxx"
        "LinuxWindow.cxx"
        "LinuxWindow.hxx"
)

endif()
