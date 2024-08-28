anemone_add_module(AnemoneRuntime
    ALIAS   Anemone::Runtime
    EXPORT  "RUNTIME_API"
)

target_link_libraries(AnemoneRuntime
    PUBLIC
        SdkFmt
        SdkLZ4
)

if(WIN32)
target_link_libraries(AnemoneRuntime
    PRIVATE
        "Synchronization.lib"
        "Winmm.lib"
        "Comctl32.lib"
        "Ws2_32.lib"
        "Version.lib"
        "Bcrypt.lib"
        "Iphlpapi.lib"
        "onecore.lib"
)
elseif(LINUX)
target_link_libraries(AnemoneRuntime
    PRIVATE
        "unwind"
)
elseif(ANDROID)
target_link_libraries(AnemoneRuntime
    PUBLIC
        "android"
        "log"
)
endif()

if(ANEMONE_BUILD_PROFILING)
target_link_libraries(AnemoneRuntime
    PRIVATE
        nvtx3-cpp)
endif()

add_subdirectory(AnemoneRuntime)
