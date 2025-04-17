if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE

        "WindowsCrashHandler.hxx"

        "WindowsFileSystem.cxx"

        "WindowsHeaders.hxx"

        "WindowsInput.cxx"
        "WindowsInput.hxx"

        "WindowsInterop.hxx"

        "WindowsMemoryMappedFile.cxx"
        "WindowsMemoryMappedFile.hxx"

        "WindowsNamedMutex.cxx"
        "WindowsNamedMutex.hxx"

        "WindowsSharedLibrary.cxx"
        "WindowsSharedLibrary.hxx"

        "WindowsSplashScreen.cxx"
        "WindowsSplashScreen.hxx"

        "WindowsStackTrace.cxx"

        "WindowsSystemAllocator.cxx"
)

endif()
