if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "Application.cxx"
        "Application.hxx"
        "Dialogs.cxx"
        "SplashScreen.cxx"
        "SplashScreen.hxx"
        "Window.cxx"
        "Window.hxx"
)

endif()
