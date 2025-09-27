if (WIN32)

target_sources(AnemoneApplication
    PRIVATE
        "WindowsGameInput.cxx"
        "WindowsHostApplication.cxx"
        "WindowsHostDialogs.cxx"
        "WindowsHostInput.cxx"
        "WindowsHostSplashScreen.cxx"
        "WindowsHostWindow.cxx"
        "WindowsXInput.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "WindowsGameInput.hxx"
        "WindowsHostApplication.hxx"
        "WindowsHostDialogs.hxx"
        "WindowsHostInput.hxx"
        "WindowsHostSplashScreen.hxx"
        "WindowsHostWindow.hxx"
        "WindowsXInput.hxx"
)

endif()
