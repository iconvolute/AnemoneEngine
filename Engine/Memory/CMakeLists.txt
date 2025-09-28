anemone_add_module(AnemoneMemory
    ALIAS   Anemone::Memory
    EXPORT  "MEMORY_API"
)

target_link_libraries(AnemoneMemory
    PUBLIC
        AnemoneRuntime
)

add_subdirectory("AnemoneMemory")
