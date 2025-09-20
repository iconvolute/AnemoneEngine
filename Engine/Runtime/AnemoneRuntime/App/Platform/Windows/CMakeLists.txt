if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "Application.cxx"
        "Dialogs.cxx"
        "SplashScreen.cxx"
        "Window.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "Application.hxx"
        "SplashScreen.hxx"
        "Window.hxx"
)

endif()
