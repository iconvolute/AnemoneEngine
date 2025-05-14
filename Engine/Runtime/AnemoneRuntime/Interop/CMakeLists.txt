target_sources(AnemoneRuntime
    PRIVATE
        "Headers.hxx"
        "MemoryBuffer.hxx"
        "RingBuffer.hxx"
        "SafeHandle.hxx"
        "StringBuffer.hxx"
)

add_subdirectory("Linux")
add_subdirectory("Windows")
