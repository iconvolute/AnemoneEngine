if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "Clipboard.cxx"
        "Environment.cxx"
        "WindowsProcess.cxx"
        "SharedLibrary.cxx"
        "SystemAllocator.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "WindowsProcess.hxx"
)

endif()
