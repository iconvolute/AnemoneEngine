anemone_add_target(
    MODULE
    NAME
        AnemoneRuntime.UI
    ALIAS
        Anemone::Runtime.UI
    EXPORT
        "ANEMONE_RUNTIME_UI_API"
    BUILD_DEPENDENCIES
        PUBLIC
            AnemoneRuntime.Base
)

add_subdirectory("AnemoneRuntime.UI")
