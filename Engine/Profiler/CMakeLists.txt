anemone_add_module(AnemoneProfiler
    ALIAS   Anemone::Profiler
    EXPORT  "PROFILER_API"
)

target_link_libraries(AnemoneProfiler
    PUBLIC
        AnemoneRuntime
)

if(ANEMONE_BUILD_PROFILING)
target_link_libraries(AnemoneProfiler 
    PRIVATE
        nvtx3-cpp)

if (ITT_FOUND)
target_link_libraries(AnemoneProfiler
    PUBLIC
        ITT::ITT)
endif()

endif()

add_subdirectory("AnemoneProfiler")
