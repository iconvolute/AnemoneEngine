target_sources(AnemoneRuntime
    PRIVATE
        "IpAddress.cxx"
        "IpEndPoint.cxx"
        "Local.cxx"
        "Socket.cxx"
        "Tcp.cxx"
        "Udp.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "Detail.hxx"
        "IpAddress.hxx"
        "IpEndPoint.hxx"
        "Local.hxx"
        "Socket.hxx"
        "Tcp.hxx"
        "Udp.hxx"
)
