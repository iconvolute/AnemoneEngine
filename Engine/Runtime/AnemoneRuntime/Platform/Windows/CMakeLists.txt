if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "CrashHandler.hxx"
        "Functions.hxx"
        "Headers.hxx"
        "Types.hxx"

        "WindowsInput.cxx"
        "WindowsInput.hxx"

        "WindowsPlatform.cxx"
        "WindowsPlatform.hxx"

        "WindowsSplashScreen.cxx"
        "WindowsSplashScreen.hxx"

        "WindowsWindow.cxx"
        "WindowsWindow.hxx"
)

endif()
