target_sources(AnemoneRuntime
    PRIVATE
        "Debug.cxx"
        "Debug.hxx"

        "Error.cxx"

        "Runtime.cxx"

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
