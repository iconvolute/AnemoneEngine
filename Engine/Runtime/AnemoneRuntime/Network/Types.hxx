#pragma once
#include "AnemoneRuntime/Platform/UninitializedStorage.hxx"

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

namespace Anemone::Platform
{
    struct NativeIpAddress;
    struct NativeSocket;

#if ANEMONE_PLATFORM_WINDOWS
    using NativeIpAddressStorage = UninitializedStorage<NativeIpAddress, 32, 8>;
    using NativeSocketStorage = UninitializedStorage<NativeSocket, 8, 8>;
#elif ANEMONE_PLATFORM_LINUX
    using NativeIpAddressStorage = UninitializedStorage<NativeIpAddress, 32, 8>;
    using NativeSocketStorage = UninitializedStorage<NativeSocket, 8, 8>;
#elif ANEMONE_PLATFORM_ANDROID
    using NativeIpAddressStorage = UninitializedStorage<NativeIpAddress, 32, 8>;
    using NativeSocketStorage = UninitializedStorage<NativeSocket, 8, 8>;
#else
#error "Not implemented"
#endif
}
