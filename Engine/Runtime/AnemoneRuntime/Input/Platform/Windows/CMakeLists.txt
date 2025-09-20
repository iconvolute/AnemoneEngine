if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "GameInput.cxx"
        "Runtime.cxx"
        "WindowsInput.cxx"
        "XInput.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "GameInput.hxx"
        "WindowsInput.hxx"
        "XInput.hxx"
)

endif()
