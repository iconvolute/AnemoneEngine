anemone_add_target(
    MODULE
    NAME
        AnemoneMemory
    ALIAS
        Anemone::Memory
    EXPORT
        "MEMORY_API"
    BUILD_DEPENDENCIES
        PUBLIC
            AnemoneRuntime
)

add_subdirectory("AnemoneMemory")
