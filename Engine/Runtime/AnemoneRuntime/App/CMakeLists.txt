target_sources(AnemoneRuntime
    PRIVATE
        "IApplication.cxx"
        "IApplication.hxx"
        "IApplicationEvents.cxx"
        "IApplicationEvents.hxx"
        "IWindow.cxx"
        "IWindow.hxx"
)

add_subdirectory("Platform")
