if (UNIX)

target_sources(AnemoneRuntime
    PRIVATE
        "UnixHeaders.hxx"

        "UnixInterop.hxx"
)

endif()
