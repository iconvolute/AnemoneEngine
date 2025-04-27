if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "WindowsFileSystem.cxx"

        "WindowsNamedMutex.cxx"
        "WindowsNamedMutex.hxx"

        "WindowsSplashScreen.cxx"
        "WindowsSplashScreen.hxx"

        "WindowsStackTrace.cxx"
)

endif()
