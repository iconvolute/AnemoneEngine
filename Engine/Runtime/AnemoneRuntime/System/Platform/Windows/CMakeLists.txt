if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "Clipboard.cxx"
        "Environment.cxx"
        "FileHandle.cxx"
        "Process.cxx"
        "SharedLibrary.cxx"
        "SystemAllocator.cxx"
)

endif()
