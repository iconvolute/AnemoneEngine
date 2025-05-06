target_sources(AnemoneRuntime
    PRIVATE
        "Assert.cxx"
        "Assert.hxx"

        "Debugger.hxx"

        "Trace.cxx"
        "Trace.hxx"

        "TraceLevel.hxx"

        "TraceListener.cxx"
        "TraceListener.hxx"
)

add_subdirectory("Platform")
add_subdirectory("Internal")
