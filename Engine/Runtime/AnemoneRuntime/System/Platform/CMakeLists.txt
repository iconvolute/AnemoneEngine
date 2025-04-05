add_subdirectory("Linux")
add_subdirectory("Windows")

target_sources(AnemoneRuntime
    PRIVATE
        "Clipboard.hxx"
        "Dialogs.hxx"
        "ProcessorProperties.hxx"
)
