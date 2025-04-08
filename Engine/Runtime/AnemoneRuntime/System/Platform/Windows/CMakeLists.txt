if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "WindowsApplication.cxx"
        "WindowsApplication.hxx"
        "WindowsClipboard.cxx"
        "WindowsClipboard.hxx"
        "WindowsDialogs.cxx"
        "WindowsDialogs.hxx"
        "WindowsEnvironment.cxx"
        "WindowsEnvironment.hxx"
        "WindowsProcessorProperties.cxx"
        "WindowsProcessorProperties.hxx"
        "WindowsWindow.cxx"
        "WindowsWindow.hxx"
)

endif()
