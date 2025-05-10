if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "GameInput.cxx"
        "GameInput.hxx"
        "Initialize.cxx"
        "WindowsInput.cxx"
        "WindowsInput.hxx"
        "XInput.cxx"
        "XInput.hxx"
)

endif()
