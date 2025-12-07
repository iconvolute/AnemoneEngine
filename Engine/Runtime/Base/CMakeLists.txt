anemone_add_target(
    MODULE
    NAME
        AnemoneRuntime.Base
    ALIAS
        Anemone::Runtime.Base
    EXPORT
        "ANEMONE_RUNTIME_BASE_API"
)

target_link_libraries(AnemoneRuntime.Base
    PUBLIC
        SdkFmt
        SdkLZ4
)

if(WIN32)
target_link_libraries(AnemoneRuntime.Base
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
        "gameinput.lib"
        "dwmapi.lib"
        "Xinput.lib"
    PUBLIC
        "Synchronization.lib"
        "Winmm.lib"
)

elseif(LINUX)
target_link_libraries(AnemoneRuntime.Base
    PRIVATE
        "unwind"
        #"uuid"
        "dl"
    PUBLIC
        "atomic"
)
elseif(ANDROID)
target_link_libraries(AnemoneRuntime.Base
    PUBLIC
        "android"
        "log"
)
endif()

if(ANEMONE_BUILD_PROFILING)
target_link_libraries(AnemoneRuntime.Base
    PRIVATE
        nvtx3-cpp
)

if (ITT_FOUND)
target_link_libraries(AnemoneRuntime.Base
    PUBLIC
        ITT::ITT
)
endif()

endif()

add_subdirectory("AnemoneRuntime.Base")
add_subdirectory("AnemoneRuntime.Diagnostics")
add_subdirectory("AnemoneRuntime.HostApplication")
add_subdirectory("AnemoneRuntime.Interop")
add_subdirectory("AnemoneRuntime.Math")
add_subdirectory("AnemoneRuntime.Memory")
add_subdirectory("AnemoneRuntime.Network")
add_subdirectory("AnemoneRuntime.Profiler")
add_subdirectory("AnemoneRuntime.Random")
add_subdirectory("AnemoneRuntime.Reflection")
add_subdirectory("AnemoneRuntime.Security")
add_subdirectory("AnemoneRuntime.Storage")
add_subdirectory("AnemoneRuntime.System")
add_subdirectory("AnemoneRuntime.Tasks")
add_subdirectory("AnemoneRuntime.Threading")

target_sources(AnemoneRuntime.Base
    PRIVATE
        "Module.cxx"
)
