if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "LinuxPlatform.cxx"
        "LinuxPlatform.hxx"

        "LinuxSplashScreen.cxx"
        "LinuxSplashScreen.hxx"

        "LinuxWindow.cxx"
        "LinuxWindow.hxx"
)

endif()
