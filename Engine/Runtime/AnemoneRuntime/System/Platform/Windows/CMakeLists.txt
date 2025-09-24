if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "WindowsClipboard.cxx"
        "Environment.cxx"
        "WindowsProcess.cxx"
        "WindowsSharedLibrary.cxx"
        "SystemAllocator.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "WindowsClipboard.hxx"
        "WindowsProcess.hxx"
        "WindowsSharedLibrary.hxx"
)

endif()
