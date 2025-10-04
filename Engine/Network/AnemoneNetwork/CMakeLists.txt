target_sources(AnemoneNetwork
    PRIVATE
        "Socket.cxx"
        

    PUBLIC FILE_SET HEADERS FILES
        "Socket.hxx"
)

add_subdirectory("Platform")
