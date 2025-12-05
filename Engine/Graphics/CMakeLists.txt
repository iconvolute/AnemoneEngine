anemone_add_target(
    MODULE
    NAME
        AnemoneGraphics
    ALIAS
        Anemone::Graphics
    EXPORT
        "GRAPHICS_API"
    BUILD_DEPENDENCIES
        PUBLIC
            AnemoneRuntime
)

add_subdirectory("AnemoneGraphics")
