if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "LinuxApplication.cxx"
        "LinuxApplication.hxx"

        "LinuxClipboard.cxx"
        "LinuxClipboard.hxx"

        "LinuxDebugger.cxx"
        "LinuxDebugger.hxx"

        "LinuxEnvironment.cxx"
        "LinuxEnvironment.hxx"

        "LinuxFileHandle.cxx"
        "LinuxFileHandle.hxx"

        #"LinuxInput.cxx"
        #"LinuxInput.hxx"

        "LinuxNamedMutex.cxx"
        "LinuxNamedMutex.hxx"

        "LinuxPlatform.cxx"
        "LinuxPlatform.hxx"

        "LinuxProcess.cxx"
        "LinuxProcess.hxx"

        "LinuxSharedLibrary.cxx"
        "LinuxSharedLibrary.hxx"

        #"LinuxSplashScreen.cxx"
        #"LinuxSplashScreen.hxx"

        "LinuxStackTrace.cxx"
        "LinuxStackTrace.hxx"

        "LinuxSystemAllocator.cxx"
        "LinuxSystemAllocator.hxx"

        "LinuxWindow.cxx"
        "LinuxWindow.hxx"
)

endif()
