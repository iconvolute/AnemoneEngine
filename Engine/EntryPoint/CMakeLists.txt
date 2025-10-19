anemone_add_module(AnemoneEntryPoint
    ALIAS   Anemone::EntryPoint
    HEADER
)

target_link_libraries(AnemoneEntryPoint
    INTERFACE
        AnemoneRuntime
)

add_subdirectory("AnemoneEntryPoint")
