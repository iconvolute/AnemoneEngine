if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "WindowsClipboard.cxx"
        "WindowsEnvironment.cxx"
        "WindowsProcess.cxx"
        "WindowsSharedLibrary.cxx"
        "SystemAllocator.cxx"
        "WindowsProcessorProperties.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "WindowsClipboard.hxx"
        "WindowsEnvironment.hxx"
        "WindowsProcess.hxx"
        "WindowsSharedLibrary.hxx"
        "WindowsProcessorProperties.hxx"
)

endif()
