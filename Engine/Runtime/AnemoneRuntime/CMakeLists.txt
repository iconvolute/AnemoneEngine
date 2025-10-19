target_sources(AnemoneRuntime
    PRIVATE
        "MemoryBuffer.cxx"
        "Module.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "MemoryBuffer.hxx"
        "Module.hxx"
)

add_subdirectory("Base")
add_subdirectory("Diagnostics")
add_subdirectory("Hash")
add_subdirectory("Interop")
add_subdirectory("Math")
add_subdirectory("Memory")
add_subdirectory("Platform")
add_subdirectory("Storage")
add_subdirectory("System")
add_subdirectory("Threading")
add_subdirectory("Security")
