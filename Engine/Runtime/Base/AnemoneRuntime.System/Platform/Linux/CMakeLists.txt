if (LINUX)

target_sources(AnemoneRuntime.Base
    PRIVATE
        "LinuxClipboard.cxx"
        "LinuxEnvironment.cxx"
        "LinuxProcess.cxx"
        "LinuxProcessorProperties.cxx"
        "LinuxSharedLibrary.cxx"
        "SystemAllocator.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "LinuxClipboard.hxx"
        "LinuxEnvironment.hxx"
        "LinuxProcess.hxx"
        "LinuxProcessorProperties.hxx"
        "LinuxSharedLibrary.hxx"
)

endif()
