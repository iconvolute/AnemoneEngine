add_subdirectory("Android")
add_subdirectory("Linux")
add_subdirectory("Posix")
add_subdirectory("Windows")

target_sources(AnemoneRuntime
    PRIVATE
        "Detect.hxx"
        "Main.hxx"

        "Platform.hxx"

        "PlatformEvents.hxx"

        "SplashScreen.hxx"

        "Types.hxx"

        "Window.cxx"
        "Window.hxx"
)
