target_sources(AnemoneRuntime.Base
    PUBLIC FILE_SET HEADERS FILES
        "DynamicArray.hxx"
        "Headers.hxx"
        "MemoryBuffer.hxx"
        "RingBuffer.hxx"
        "SafeHandle.hxx"
        "StringBuffer.hxx"
)

add_subdirectory("Linux")
add_subdirectory("Windows")
