if (WIN32)
add_subdirectory("Windows")
elseif(ANDROID OR LINUX)
add_subdirectory("Posix")
endif()

target_sources(AnemoneRuntime
    PRIVATE
        "Detail.hxx"
        "IpAddress.cxx"
        "IpAddress.hxx"
        "IpEndPoint.cxx"
        "IpEndPoint.hxx"
        "Local.cxx"
        "Local.hxx"
        "Runtime.hxx"
        "Tcp.cxx"
        "Tcp.hxx"
        "Udp.cxx"
        "Udp.hxx"
)
