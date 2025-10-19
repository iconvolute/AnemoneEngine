anemone_add_module(AnemoneNetwork
    ALIAS   Anemone::Network
    EXPORT  "NETWORK_API"
)

target_link_libraries(AnemoneNetwork
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
