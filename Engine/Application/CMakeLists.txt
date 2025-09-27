anemone_add_module(AnemoneApplication
    ALIAS   Anemone::Application
    EXPORT  "APPLICATION_API"
)

target_link_libraries(AnemoneApplication
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
