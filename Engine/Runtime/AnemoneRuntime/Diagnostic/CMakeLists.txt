target_sources(AnemoneRuntime
    PRIVATE
        "Performance.hxx"
        "Runtime.hxx"
        "StackTrace.hxx"
        "StandardOutputTraceListener.hxx"
)

if(WIN32)
    add_subdirectory("Windows")
elseif(ANDROID OR LINUX)
    add_subdirectory("Posix")
endif()
