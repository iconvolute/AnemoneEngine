anemone_add_target(
    MODULE
    NAME
        AnemoneProfiler 
    ALIAS
        Anemone::Profiler
    EXPORT
        "PROFILER_API"
    BUILD_DEPENDENCIES
        PUBLIC
            AnemoneRuntime
)

add_subdirectory("AnemoneProfiler")

if(ANEMONE_BUILD_PROFILING)
target_link_libraries(AnemoneProfiler 
    PRIVATE
        nvtx3-cpp
)

if (ITT_FOUND)
target_link_libraries(AnemoneProfiler
    PUBLIC
        ITT::ITT
)
endif()

endif()
