anemone_add_module(AnemoneUI
    STATIC
    ALIAS
        Anemone::UI
)

target_link_libraries(AnemoneUI
    PUBLIC
        AnemoneRuntime
)

add_subdirectory("AnemoneUI")
