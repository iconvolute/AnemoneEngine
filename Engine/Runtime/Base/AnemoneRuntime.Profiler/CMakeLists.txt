target_sources(AnemoneRuntime.Base
    PRIVATE
        "NvidiaProfilerBackend.cxx"
        "Profiler.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "NvidiaProfilerBackend.hxx"
        "Profiler.hxx"
)
