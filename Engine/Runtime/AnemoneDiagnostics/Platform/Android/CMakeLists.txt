if (ANDROID)

target_sources(AnemoneRuntime
    PRIVATE
        "AndroidDebugTraceListener.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "AndroidDebugTraceListener.hxx"
)

endif()
