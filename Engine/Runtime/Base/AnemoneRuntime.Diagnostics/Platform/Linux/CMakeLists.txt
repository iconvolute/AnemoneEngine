if (LINUX)

target_sources(AnemoneRuntime.Base
    PRIVATE
        "LinuxDebug.cxx"
        "LinuxTrace.cxx"
        "StackTrace.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "LinuxDebug.hxx"
        "LinuxTrace.hxx"
)

endif()
