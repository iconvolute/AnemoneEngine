target_sources(AnemoneRuntime
    PRIVATE
        "Runtime.hxx"
)

if(WIN32)
    add_subdirectory("Windows")
elseif(ANDROID)
    add_subdirectory("Android")
elseif(LINUX)
    add_subdirectory("Linux")
endif()
