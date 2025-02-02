if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "WindowsApplication.cxx"
        "WindowsApplication.hxx"

        "WindowsClipboard.cxx"

        "WindowsCrashHandler.hxx"

        "WindowsDebugger.cxx"
        "WindowsDebugger.hxx"
        
        "WindowsDialogs.cxx"

        "WindowsEnvironment.cxx"
        "WindowsEnvironment.hxx"

        "WindowsFileHandle.cxx"
        "WindowsFileHandle.hxx"

        "WindowsHeaders.hxx"

        "WindowsInput.cxx"
        "WindowsInput.hxx"

        "WindowsInterop.hxx"

        "WindowsNamedMutex.cxx"
        "WindowsNamedMutex.hxx"

        "WindowsPlatform.cxx"
        "WindowsPlatform.hxx"

        "WindowsPlatformTraceListeners.cxx"

        "WindowsProcess.cxx"
        "WindowsProcess.hxx"

        "WindowsSharedLibrary.cxx"
        "WindowsSharedLibrary.hxx"

        "WindowsSplashScreen.cxx"
        "WindowsSplashScreen.hxx"

        "WindowsStackTrace.cxx"

        "WindowsSystemAllocator.cxx"

        "WindowsWindow.cxx"
        "WindowsWindow.hxx"
)

endif()
