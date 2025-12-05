anemone_add_target(
    MODULE
    NAME
        AnemoneNetwork
    ALIAS
        Anemone::Network
    EXPORT
        "NETWORK_API"
    BUILD_DEPENDENCIES
        PUBLIC
            AnemoneRuntime
)

if(WIN32)
target_link_libraries(AnemoneNetwork
    PRIVATE
        "Ws2_32.lib"
)
endif()

add_subdirectory("AnemoneNetwork")
