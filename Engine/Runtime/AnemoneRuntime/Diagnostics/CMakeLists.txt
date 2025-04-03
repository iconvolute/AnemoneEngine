target_sources(AnemoneRuntime
    PRIVATE
        "Assert.cxx"
        "Assert.hxx"

        "Debugger.hxx"

        "Trace.cxx"
        "Trace.hxx"
)

add_subdirectory("Platform")
add_subdirectory("Private")
