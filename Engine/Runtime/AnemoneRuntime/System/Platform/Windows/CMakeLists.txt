if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "GameInputImpl.cxx"
        "GameInputImpl.hxx"
        "Input.cxx"
        "Input.hxx"
        "XInput.cxx"
        "XInput.hxx"
        "WindowsApplication.cxx"
        "WindowsApplication.hxx"
        "WindowsClipboard.cxx"
        "WindowsDialogs.cxx"
        "WindowsEnvironment.cxx"
        "WindowsFileHandle.cxx"
        "WindowsFileHandle.hxx"
        "WindowsProcess.cxx"
        "WindowsProcess.hxx"
        "WindowsProcessorProperties.cxx"
        "WindowsSharedLibrary.cxx"
        "WindowsSharedLibrary.hxx"
        "WindowsWindow.cxx"
        "WindowsWindow.hxx"
)

endif()
