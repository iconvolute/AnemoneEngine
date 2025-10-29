add_subdirectory("Platform")

target_sources(AnemoneRuntime
    PRIVATE
        "CommandLine.cxx"
        "Process.cxx"
        "ProcessorProperties.cxx"
        "SharedLibrary.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "Clipboard.hxx"
        "CommandLine.hxx"
        "Environment.hxx"
        "Process.hxx"
        "ProcessorProperties.hxx"
        "SharedLibrary.hxx"
        "SystemAllocator.hxx"
)
