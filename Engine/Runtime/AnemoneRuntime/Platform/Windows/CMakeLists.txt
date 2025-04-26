if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "WindowsFileSystem.cxx"

        "WindowsHeaders.hxx"

        "WindowsInterop.hxx"

        "WindowsMemoryMappedFile.hxx"

        "WindowsNamedMutex.cxx"
        "WindowsNamedMutex.hxx"

        "WindowsSplashScreen.cxx"
        "WindowsSplashScreen.hxx"

        "WindowsStackTrace.cxx"
)

endif()
