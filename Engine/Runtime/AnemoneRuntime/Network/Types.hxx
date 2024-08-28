#pragma once
#include <cstddef>

namespace Anemone::Network
{
    enum class AddressFamily
    {
        Unknown,
        Unspecified,
        IPv4,
        IPv6,
        Unix,
    };

    enum class SocketType
    {
        Stream,
        Datagram,
        Raw,
        Rdm,
        SequentialPacket,
    };

    enum class ProtocolType
    {
        Unknown,
        Unspecified,
        IPv4,
        IPv6,
        TCP,
        UDP,
    };
}
