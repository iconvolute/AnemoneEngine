add_subdirectory("Platform")

target_sources(AnemoneRuntime
    PRIVATE
        "CommandLine.cxx"
        "Process.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "Clipboard.hxx"
        "CommandLine.hxx"
        "Environment.hxx"
        "Process.hxx"
        "SharedLibrary.hxx"
        "SystemAllocator.hxx"
)
