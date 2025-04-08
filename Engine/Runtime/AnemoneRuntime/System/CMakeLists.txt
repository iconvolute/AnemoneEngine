add_subdirectory("Platform")

target_sources(AnemoneRuntime
    PRIVATE
        "Application.cxx"
        "Application.hxx"
        "Clipboard.hxx"
        "Dialogs.hxx"
        "Environment.hxx"
        "ProcessorProperties.hxx"
        "Window.hxx"
)
