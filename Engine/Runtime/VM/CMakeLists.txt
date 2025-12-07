anemone_add_target(
    MODULE
    NAME
        AnemoneRuntime.VM
    ALIAS
        Anemone::Runtime.VM
    EXPORT
        "ANEMONE_RUNTIME_VM_API"
    BUILD_DEPENDENCIES
        PUBLIC
            AnemoneRuntime.Base
)

add_subdirectory("AnemoneRuntime.VM")
