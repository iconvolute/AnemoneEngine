anemone_add_target(
    MODULE
    NAME
        AnemoneVM
    ALIAS
        Anemone::VM
    EXPORT
        "VM_API"
    BUILD_DEPENDENCIES
        PUBLIC
            AnemoneRuntime
)

add_subdirectory("AnemoneVM")
