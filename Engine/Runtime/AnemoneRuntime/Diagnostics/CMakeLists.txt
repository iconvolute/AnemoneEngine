target_sources(AnemoneRuntime
    PRIVATE
        "Assert.cxx"
        "Assert.hxx"
        "Debug.cxx"
        "Debug.hxx"
        "Trace.cxx"
        "Trace.hxx"
)

if(WIN32)
    add_subdirectory("Windows")
elseif(ANDROID OR LINUX)
    add_subdirectory("Posix")
endif()
