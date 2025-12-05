anemone_add_target(
    MODULE
    NAME
        AnemoneApplication
    ALIAS
        Anemone::Application
    EXPORT
        "APPLICATION_API"
    BUILD_DEPENDENCIES
        PUBLIC
            AnemoneRuntime
)

if (WIN32)

target_link_libraries(AnemoneApplication
    PRIVATE
        "gameinput.lib"
        "dwmapi.lib"
        "Xinput.lib"
)

endif()

add_subdirectory("AnemoneApplication")
