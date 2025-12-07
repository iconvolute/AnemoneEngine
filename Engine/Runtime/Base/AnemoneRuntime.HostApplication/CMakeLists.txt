target_sources(AnemoneRuntime.Base
    PRIVATE
        "HostApplication.cxx"
        "HostApplicationEvents.cxx"
        "HostDialogs.cxx"
        "HostSplashScreen.cxx"
        "HostWindow.cxx"

    PUBLIC FILE_SET HEADERS FILES
        "HostApplication.hxx"
        "HostApplicationEvents.hxx"
        "HostDialogs.hxx"
        "HostSplashScreen.hxx"
        "HostWindow.hxx"
)

add_subdirectory("Platform")
