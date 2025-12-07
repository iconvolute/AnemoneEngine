if (WIN32)
add_subdirectory("Windows")
elseif(ANDROID OR LINUX)
add_subdirectory("Posix")
endif()

target_sources(AnemoneRuntime.Base
    PUBLIC FILE_SET HEADERS FILES
        "Allocator.hxx"
        "SlabAllocator.hxx"
        "SystemAllocator.hxx"
)
