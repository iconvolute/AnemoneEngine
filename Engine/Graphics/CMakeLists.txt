anemone_add_module(AnemoneGraphics
    EXPORT
        "GRAPHICS_API"
    ALIAS
        Anemone::Graphics
)

target_link_libraries(AnemoneGraphics
    PUBLIC
        AnemoneRuntime
)

add_subdirectory("AnemoneGraphics")
