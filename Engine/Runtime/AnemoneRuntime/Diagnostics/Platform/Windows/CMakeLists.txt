if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "WindowsDebugger.cxx"
        "WindowsDebugger.hxx"
)

endif()
