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
        "Comctl32.lib"
        "Ws2_32.lib"
        "Version.lib"
        "Bcrypt.lib"
        "Iphlpapi.lib"
        "onecore.lib"
        "Faultrep.lib"
        "advapi32.lib"
        "Appnotify.lib"
        #"DbgHelp.lib"
        "xaudio2.lib"
    PUBLIC
        "Synchronization.lib"
        "Winmm.lib"
)

elseif(LINUX)
target_link_libraries(AnemoneRuntime
    PRIVATE
        "unwind"
        #"uuid"
        "dl"
)
elseif(ANDROID)
target_link_libraries(AnemoneRuntime
    PUBLIC
        "android"
        "log"
)
endif()

add_subdirectory("AnemoneBase")
add_subdirectory("AnemoneDiagnostics")
add_subdirectory("AnemoneInterop")
add_subdirectory("AnemoneMath")
add_subdirectory("AnemoneRandom")
add_subdirectory("AnemoneSecurity")
add_subdirectory("AnemoneStorage")
add_subdirectory("AnemoneSystem")
add_subdirectory("AnemoneThreading")
