target_sources(AnemoneRuntime
    PRIVATE
        "Debug.cxx"
        "Debug.hxx"
        "Performance.hxx"
        "StackTrace.hxx"
        "StandardOutputTraceListener.hxx"
        "Static.cxx"
        "Static.hxx"
        "Trace.cxx"
        "Trace.hxx"
        "TraceLevel.hxx"
        "TraceListener.hxx"
)

if(WIN32)
    add_subdirectory("Windows")
elseif(ANDROID OR LINUX)
    add_subdirectory("Posix")
endif()
