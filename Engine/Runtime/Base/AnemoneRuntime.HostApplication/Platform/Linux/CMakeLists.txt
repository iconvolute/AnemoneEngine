if (LINUX)

target_sources(AnemoneRuntime.Base
    PRIVATE
        "LinuxHostApplication.cxx"
        "LinuxHostDialogs.cxx"
        "LinuxHostSplashScreen.cxx"
        "LinuxHostWindow.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "LinuxHostApplication.hxx"
        "LinuxHostDialogs.hxx"
        "LinuxHostSplashScreen.hxx"
        "LinuxHostWindow.hxx"
)

endif()
