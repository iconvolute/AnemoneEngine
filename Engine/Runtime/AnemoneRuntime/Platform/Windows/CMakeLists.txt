if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "WindowsApplication.cxx"
        "WindowsApplication.hxx"

        "WindowsClipboard.cxx"
        "WindowsClipboard.hxx"

        "WindowsCrashHandler.hxx"

        "WindowsDebugger.cxx"
        "WindowsDebugger.hxx"

        "WindowsEnvironment.cxx"
        "WindowsEnvironment.hxx"

        "WindowsHeaders.hxx"

        "WindowsInput.cxx"
        "WindowsInput.hxx"

        "WindowsInterop.hxx"

        "WindowsNamedMutex.cxx"
        "WindowsNamedMutex.hxx"

        "WindowsPlatform.cxx"
        "WindowsPlatform.hxx"

        "WindowsProcess.cxx"
        "WindowsProcess.hxx"

        "WindowsSharedLibrary.cxx"
        "WindowsSharedLibrary.hxx"

        "WindowsSplashScreen.cxx"
        "WindowsSplashScreen.hxx"

        "WindowsStackTrace.cxx"
        "WindowsStackTrace.hxx"

        "WindowsSystemAllocator.cxx"
        "WindowsSystemAllocator.hxx"

        "WindowsWindow.cxx"
        "WindowsWindow.hxx"
)

endif()
