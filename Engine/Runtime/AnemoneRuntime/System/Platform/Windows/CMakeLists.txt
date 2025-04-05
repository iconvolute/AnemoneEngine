if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "WindowsClipboard.cxx"
        "WindowsClipboard.hxx"
        "WindowsDialogs.cxx"
        "WindowsDialogs.hxx"
)

endif()
