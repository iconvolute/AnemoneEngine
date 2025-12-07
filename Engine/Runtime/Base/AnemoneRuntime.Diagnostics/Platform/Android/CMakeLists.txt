if (ANDROID)

target_sources(AnemoneRuntime.Base
    PRIVATE
        "AndroidDebugTraceListener.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "AndroidDebugTraceListener.hxx"
)

endif()
