target_sources(AnemoneRuntime
    PRIVATE
        "IApplication.cxx"
        "IApplicationEvents.cxx"
        "ISplashScreen.cxx"
        "IWindow.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "Dialogs.hxx"
        "IApplication.hxx"
        "IApplicationEvents.hxx"
        "ISplashScreen.hxx"
        "IWindow.hxx"
)

add_subdirectory("Platform")
