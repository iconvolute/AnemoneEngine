if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "LinuxApplication.cxx"
        "LinuxApplication.hxx"

        "LinuxClipboard.cxx"

        "LinuxDebugger.cxx"

        "LinuxDialogs.cxx"

        "LinuxEnvironment.cxx"

        "LinuxFileHandle.cxx"
        "LinuxFileHandle.hxx"

        "LinuxFileSystem.cxx"

        #"LinuxInput.cxx"
        #"LinuxInput.hxx"

        "LinuxNamedMutex.cxx"
        "LinuxNamedMutex.hxx"

        "LinuxPlatform.cxx"
        "LinuxPlatform.hxx"

        "LinuxPlatformTraceListeners.cxx"

        "LinuxProcess.cxx"
        "LinuxProcess.hxx"

        "LinuxSharedLibrary.cxx"
        "LinuxSharedLibrary.hxx"

        #"LinuxSplashScreen.cxx"
        #"LinuxSplashScreen.hxx"

        "LinuxStackTrace.cxx"

        "LinuxSystemAllocator.cxx"

        "LinuxWindow.cxx"
        "LinuxWindow.hxx"
)

endif()
