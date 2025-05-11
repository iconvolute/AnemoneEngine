target_sources(AnemoneRuntime
    PRIVATE
        "Assert.cxx"
        "Assert.hxx"

        "Debugger.cxx"
        "Debugger.hxx"

        "Status.hxx"

        "Trace.cxx"
        "Trace.hxx"

        "TraceDispatcher.cxx"
        "TraceDispatcher.hxx"

        "TraceListener.cxx"
        "TraceListener.hxx"
)

add_subdirectory("Platform")
add_subdirectory("Internal")
