anemone_add_target(
    MODULE
    NAME
        AnemoneUI
    ALIAS
        Anemone::UI
    EXPORT
        "UI_API"
    BUILD_DEPENDENCIES
        PUBLIC
            AnemoneRuntime
)

add_subdirectory("AnemoneUI")
