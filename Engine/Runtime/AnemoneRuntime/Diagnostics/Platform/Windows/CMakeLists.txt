if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "DebugEngine.cxx"
        "DebugEngine.hxx"

        "Debugger.cxx"
        "Debugger.hxx"

        "Error.cxx"
        "Error.hxx"

        "StackTrace.cxx"
)

endif()
