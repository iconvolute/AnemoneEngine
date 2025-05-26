if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "Debug.cxx"
        "Debug.hxx"

        "Error.cxx"

        "StackTrace.cxx"

        "Trace.cxx"
)

endif()
