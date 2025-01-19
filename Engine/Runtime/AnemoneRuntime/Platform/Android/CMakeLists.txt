if (ANDROID)

target_sources(AnemoneRuntime
    PRIVATE
        "AndroidPlatform.cxx"
        "AndroidPlatform.hxx"

        "AndroidSplashScreen.cxx"
        "AndroidSplashScreen.hxx"

        "AndroidWindow.cxx"
        "AndroidWindow.hxx"
)

endif()
