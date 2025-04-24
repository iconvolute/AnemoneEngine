add_subdirectory("Platform")
add_subdirectory("Internal")

target_sources(AnemoneRuntime
    PRIVATE
        "Application.cxx"
        "Application.hxx"
        "Clipboard.hxx"
        "Dialogs.hxx"
        "Environment.hxx"
        "Process.hxx"
        "ProcessorProperties.hxx"
        "Window.hxx"
        "SharedLibrary.hxx"
)
