if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "Application.cxx"
        "Dialogs.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "Application.hxx"
)

endif()
