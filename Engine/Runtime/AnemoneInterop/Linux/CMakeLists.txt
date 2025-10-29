if (ANDROID OR LINUX)

target_sources(AnemoneRuntime
    PUBLIC FILE_SET HEADERS FILES
        "DateTime.hxx"
        "FileSystem.hxx"
        "Headers.hxx"
        "Process.hxx"
        "SafeHandle.hxx"
        "Threading.hxx"
)

endif()
