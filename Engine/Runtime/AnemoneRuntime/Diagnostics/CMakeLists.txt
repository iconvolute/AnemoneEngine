target_sources(AnemoneRuntime
    PRIVATE
        "Debug.cxx"
        "Error.cxx"
        "Runtime.cxx"
        "Trace.cxx"
        "TraceDispatcher.cxx"
        "TraceListener.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "Debug.hxx"
        "Error.hxx"
        "Trace.hxx"
        "TraceDispatcher.hxx"
        "TraceListener.hxx"
)

add_subdirectory("Platform")
add_subdirectory("Internal")
