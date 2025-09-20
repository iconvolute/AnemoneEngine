if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "WindowsNamedMutex.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "Types.hxx"
        "WindowsNamedMutex.hxx"
)

endif()
