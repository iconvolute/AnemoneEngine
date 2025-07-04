if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "Debug.cxx"
        "StackTrace.cxx"
        "Trace.cxx"
)

endif()
