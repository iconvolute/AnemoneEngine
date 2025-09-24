if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "Environment.cxx"
        "LinuxClipboard.cxx"
        "LinuxProcess.cxx"
        "LinuxSharedLibrary.cxx"
        "SystemAllocator.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "LinuxClipboard.hxx"
        "LinuxProcess.hxx"
        "LinuxSharedLibrary.hxx"
)

endif()
