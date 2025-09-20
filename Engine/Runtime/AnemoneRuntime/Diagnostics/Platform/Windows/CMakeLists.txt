if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "Debug.cxx"
        "Error.cxx"
        "StackTrace.cxx"
        "Trace.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "Debug.hxx"
)

endif()
