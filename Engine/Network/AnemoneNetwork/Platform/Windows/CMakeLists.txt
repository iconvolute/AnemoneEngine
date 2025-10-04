if (WIN32)

target_sources(AnemoneNetwork
    PRIVATE
        "WindowsSocket.cxx"

    PUBLIC FILE_SET HEADERS FILES
        "WindowsSocket.hxx"
)

endif()
