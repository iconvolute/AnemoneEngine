if (ANDROID OR LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "DateTime.hxx"
        "FileSystem.hxx"
        "Headers.hxx"
        "Process.hxx"
        "SafeHandle.hxx"
        "Threading.hxx"
)

endif()
