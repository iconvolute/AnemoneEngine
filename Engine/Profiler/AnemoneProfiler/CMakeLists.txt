target_sources(AnemoneProfiler
    PRIVATE
        "Module.cxx"
        "NvidiaProfilerBackend.cxx"
        "Profiler.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "Module.hxx"
        "NvidiaProfilerBackend.hxx"
        "Profiler.hxx"
)
