if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "Clipboard.cxx"
        "Environment.cxx"
        "FileHandle.cxx"
        "Process.cxx"
        "SharedLibrary.cxx"
)

endif()
