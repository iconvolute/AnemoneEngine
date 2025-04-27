if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "DateTime.hxx"
        "Environment.hxx"
        "FileSystem.hxx"
        "Headers.hxx"
        "MemoryMappedFile.hxx"
        "Process.hxx"
        "Registry.hxx"
        "SafeHandle.hxx"
        "System.hxx"
        "Text.hxx"
        "Threading.hxx"
        "UI.hxx"
)

endif()
