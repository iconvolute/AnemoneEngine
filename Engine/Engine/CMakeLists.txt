anemone_add_target(
    MODULE
    NAME
        AnemoneEngine
    ALIAS
        Anemone::Engine
    EXPORT
        "ENGINE_API"
    BUILD_DEPENDENCIES
        PUBLIC
            AnemoneRuntime
)

add_subdirectory("AnemoneEngine")
