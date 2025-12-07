anemone_add_target(
    HEADER
    NAME
        AnemoneRuntime.EntryPoint
    ALIAS
        Anemone::Runtime.EntryPoint
    BUILD_DEPENDENCIES
        INTERFACE
            AnemoneRuntime.Base
)

add_subdirectory("AnemoneRuntime.EntryPoint")
