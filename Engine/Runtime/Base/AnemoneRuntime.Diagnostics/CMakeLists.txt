target_sources(AnemoneRuntime.Base
    PRIVATE
        "ConsoleTraceListener.cxx"
        "Debug.cxx"
        "Error.cxx"
        "FileTraceListener.cxx"
        "Trace.cxx"
        "TraceDispatcher.cxx"
        "TraceListener.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "ConsoleTraceListener.hxx"
        "Debug.hxx"
        "Error.hxx"
        "FileTraceListener.hxx"
        "Trace.hxx"
        "TraceDispatcher.hxx"
        "TraceListener.hxx"
)

add_subdirectory("Platform")
