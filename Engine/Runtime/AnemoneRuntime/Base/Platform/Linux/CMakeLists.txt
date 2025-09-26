if (LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "LinuxDateTime.cxx"
        "LinuxInstant.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "LinuxDateTime.hxx"
        "LinuxInstant.hxx"
)

endif()
