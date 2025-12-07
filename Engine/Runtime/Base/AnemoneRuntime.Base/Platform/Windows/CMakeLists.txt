if (WIN32)

target_sources(AnemoneRuntime.Base
    PRIVATE
        "WindowsDateTime.cxx"
        "WindowsInstant.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "WindowsDateTime.hxx"
        "WindowsInstant.hxx"
)

endif()
