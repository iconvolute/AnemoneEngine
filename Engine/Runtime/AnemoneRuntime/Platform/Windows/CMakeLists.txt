if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "WindowsFileSystem.cxx"

        "WindowsNamedMutex.cxx"
        "WindowsNamedMutex.hxx"

        "WindowsStackTrace.cxx"
)

endif()
