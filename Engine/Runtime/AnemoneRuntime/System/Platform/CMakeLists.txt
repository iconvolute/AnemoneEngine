add_subdirectory("Linux")
add_subdirectory("Windows")

target_sources(AnemoneRuntime
    PRIVATE
        "Dialogs.hxx"
)
