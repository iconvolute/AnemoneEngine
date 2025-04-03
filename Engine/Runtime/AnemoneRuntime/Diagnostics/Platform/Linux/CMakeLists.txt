if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "LinuxDebugger.cxx"
        "LinuxDebugger.hxx"
)

endif()
