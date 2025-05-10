add_subdirectory("Platform")

target_sources(AnemoneRuntime
    PRIVATE
        "Clipboard.hxx"
        "CommandLine.cxx"
        "CommandLine.hxx"
        "Environment.hxx"
        "FileHandle.hxx"
        "Process.hxx"
        "SharedLibrary.hxx"
        "SystemAllocator.hxx"
)
