anemone_add_module(AnemoneRender
    ALIAS Anemone::Render
    EXPORT "RENDER_API"
)

target_link_libraries(AnemoneRender
    PUBLIC
        AnemoneRuntime
        AnemoneApplication
)

add_subdirectory("AnemoneRender")
