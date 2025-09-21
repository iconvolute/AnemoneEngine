if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "Clipboard.cxx"
        "Environment.cxx"
        "LinuxProcess.cxx"
        "SharedLibrary.cxx"
        "SystemAllocator.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "LinuxProcess.hxx"
)

endif()
