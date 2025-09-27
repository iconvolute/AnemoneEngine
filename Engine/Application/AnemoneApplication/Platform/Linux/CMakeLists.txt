if (LINUX)

target_sources(AnemoneApplication
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
