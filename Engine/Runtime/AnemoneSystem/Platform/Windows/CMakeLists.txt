if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "SystemAllocator.cxx"
        "WindowsClipboard.cxx"
        "WindowsEnvironment.cxx"
        "WindowsProcess.cxx"
        "WindowsProcessorProperties.cxx"
        "WindowsSharedLibrary.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "WindowsClipboard.hxx"
        "WindowsEnvironment.hxx"
        "WindowsProcess.hxx"
        "WindowsProcessorProperties.hxx"
        "WindowsSharedLibrary.hxx"
)

endif()
