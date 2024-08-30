target_sources(AnemoneRuntime
    PRIVATE
        "Assert.cxx"
        "Assert.hxx"
        "Debugger.hxx"
        "Log.cxx"
        "Log.hxx"
        "LogLevel.hxx"
        "Performance.hxx"
        "StackTrace.hxx"
)

if(WIN32)
    add_subdirectory("Windows")
elseif(ANDROID OR LINUX)
    add_subdirectory("Posix")
endif()
