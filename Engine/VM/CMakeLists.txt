anemone_add_module(AnemoneVM
    EXPORT
        "VM_API"
    ALIAS
        Anemone::VM
)

target_link_libraries(AnemoneVM
    PUBLIC
        AnemoneRuntime
)

add_subdirectory("AnemoneVM")
