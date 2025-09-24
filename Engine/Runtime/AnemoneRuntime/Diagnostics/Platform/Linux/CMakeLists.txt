if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "LinuxDebug.cxx"
        "StackTrace.cxx"
        "LinuxTrace.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "LinuxDebug.hxx"
        "LinuxTrace.hxx"
)

endif()
