if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "WindowsNamedMutex.cxx"
        "WindowsNamedMutex.hxx"

        "WindowsStackTrace.cxx"
)

endif()
