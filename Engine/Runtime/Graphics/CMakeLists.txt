anemone_add_target(
    MODULE
    NAME
        AnemoneRuntime.Graphics
    ALIAS
        Anemone::Runtime.Graphics
    EXPORT
        "ANEMONE_RUNTIME_GRAPHICS_API"
    BUILD_DEPENDENCIES
        PUBLIC
            AnemoneRuntime.Base
)

add_subdirectory("AnemoneRuntime.Graphics")
