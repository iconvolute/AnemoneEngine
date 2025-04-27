target_sources(AnemoneRuntime
    PRIVATE
        "Headers.hxx"
        "SafeHandle.hxx"
        "StringBuffer.hxx"
)

add_subdirectory("Linux")
add_subdirectory("Windows")
