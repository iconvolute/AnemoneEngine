target_sources(AnemoneRuntime
    PRIVATE
        "Application.cxx"
        "Application.hxx"
        "Runtime.hxx"
        "Window.cxx"
        "Window.hxx"
)

if(WIN32)
    add_subdirectory("Windows")
elseif(ANDROID)
    add_subdirectory("Android")
elseif(LINUX)
    add_subdirectory("Linux")
endif()
