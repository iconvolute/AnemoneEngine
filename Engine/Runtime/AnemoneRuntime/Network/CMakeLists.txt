if (WIN32)
add_subdirectory("Windows")
elseif(ANDROID OR LINUX)
add_subdirectory("Posix")
endif()

target_sources(AnemoneRuntime
    PRIVATE
        "Dns.cxx"
        "Dns.hxx"
        "IpAddress.cxx"
        "IpAddress.hxx"
        "IpEndPoint.cxx"
        "IpEndPoint.hxx"
        "Private.hxx"
        "Socket.cxx"
        "Socket.hxx"
        "Static.hxx"
        "Types.hxx"
)
