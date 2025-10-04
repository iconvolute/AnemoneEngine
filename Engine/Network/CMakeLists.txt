anemone_add_module(AnemoneNetwork
    ALIAS   Anemone::Network
    EXPORT  "NETWORK_API"
)

target_link_libraries(AnemoneNetwork
    PUBLIC
        AnemoneRuntime
)

add_subdirectory("AnemoneNetwork")
