anemone_add_module(AnemoneUI
    ALIAS Anemone::UI
    EXPORT "UI_API"
)

target_link_libraries(AnemoneUI
    PUBLIC
        AnemoneRuntime
)

add_subdirectory("AnemoneUI")
