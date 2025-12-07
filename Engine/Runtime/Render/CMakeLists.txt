anemone_add_target(
    MODULE
    NAME
        AnemoneRuntime.Render
    ALIAS
        Anemone::Runtime.Render
    EXPORT
        "ANEMONE_RUNTIME_RENDER_API"
    BUILD_DEPENDENCIES
        PUBLIC
            AnemoneRuntime.Base
)

target_sources(
    AnemoneRuntime.Render
    PRIVATE
        "Module.cxx"
)

add_subdirectory("AnemoneRuntime.Render")
