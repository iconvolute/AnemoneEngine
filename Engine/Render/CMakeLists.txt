anemone_add_target(
    MODULE
    NAME
        AnemoneRender
    ALIAS
        Anemone::Render
    EXPORT
        "RENDER_API"
    BUILD_DEPENDENCIES
        PUBLIC
            AnemoneRuntime
            AnemoneApplication
)

add_subdirectory("AnemoneRender")
