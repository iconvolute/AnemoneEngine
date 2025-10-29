if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "LinuxDebug.cxx"
        "LinuxTrace.cxx"
        "StackTrace.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "LinuxDebug.hxx"
        "LinuxTrace.hxx"
)

endif()
