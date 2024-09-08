if (WIN32)
add_subdirectory("Windows")
elseif(ANDROID OR LINUX)
add_subdirectory("Posix")
endif()

target_sources(AnemoneRuntime
    PRIVATE
        "Detect.hxx"
        "Main.hxx"
        "Runtime.hxx"
        "UninitializedStorage.hxx"
)
