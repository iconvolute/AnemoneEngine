anemone_add_target(
    HEADER
    NAME
        AnemoneEntryPoint
    ALIAS
        Anemone::EntryPoint
    BUILD_DEPENDENCIES
        INTERFACE
            AnemoneRuntime
)

add_subdirectory("AnemoneEntryPoint")
