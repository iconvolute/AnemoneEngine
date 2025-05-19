if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "Debugger.cxx"
        "Debugger.hxx"

        "StackTrace.cxx"
)

endif()
