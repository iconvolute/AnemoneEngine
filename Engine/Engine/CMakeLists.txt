anemone_add_module(AnemoneEngine
    EXPORT
        "ENGINE_API"
    ALIAS
        Anemone::Engine
)

target_link_libraries(AnemoneEngine
    PUBLIC
        AnemoneRuntime
)

add_subdirectory("AnemoneEngine")
