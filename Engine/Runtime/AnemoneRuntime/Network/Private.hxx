#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#if ANEMONE_PLATFORM_WINDOWS

#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

#elif ANEMONE_PLATFORM_LINUX || ANEMONE_PLATFORM_ANDROID
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"
#else
#error "Not implemented"
#endif

#include <utility>

namespace Anemone::Network::Private
{
    constexpr int from_AddressFamily(AddressFamily value)
    {
        switch (value)
        {
        case AddressFamily::IPv4:
            return AF_INET;

        case AddressFamily::IPv6:
            return AF_INET6;

        case AddressFamily::Unix:
            return AF_UNIX;

        default:
        case AddressFamily::Unspecified:
            return AF_UNSPEC;
        }
    }

    constexpr AddressFamily into_AddressFamily(int value)
    {
        switch (value)
        {
        case AF_INET:
            return AddressFamily::IPv4;

        case AF_INET6:
            return AddressFamily::IPv6;

        case AF_UNIX:
            return AddressFamily::Unix;

        default:
        case AF_UNSPEC:
            return AddressFamily::Unspecified;
        }
    }

    constexpr int from_SocketType(SocketType value)
    {
        switch (value)
        {
        case SocketType::Stream:
            return SOCK_STREAM;

        case SocketType::Datagram:
            return SOCK_DGRAM;

        case SocketType::Raw:
            return SOCK_RAW;

        case SocketType::Rdm:
            return SOCK_RDM;

        case SocketType::SequentialPacket:
            return SOCK_SEQPACKET;

        default:
            break;
        }

        std::unreachable();
    }

    constexpr SocketType into_SocketType(int value)
    {
        switch (value)
        {
        case SOCK_STREAM:
            return SocketType::Stream;

        case SOCK_DGRAM:
            return SocketType::Datagram;

        case SOCK_RAW:
            return SocketType::Raw;

        case SOCK_RDM:
            return SocketType::Rdm;

        case SOCK_SEQPACKET:
            return SocketType::SequentialPacket;

        default:
            break;
        }

        std::unreachable();
    }

    constexpr int from_ProtocolType(ProtocolType value)
    {
        switch (value)
        {
        case ProtocolType::IPv4:
            return IPPROTO_IP;

        case ProtocolType::IPv6:
            return IPPROTO_IPV6;

        case ProtocolType::TCP:
            return IPPROTO_TCP;

        case ProtocolType::UDP:
            return IPPROTO_UDP;

        default:
            break;
        }

        std::unreachable();
    }

    constexpr ProtocolType into_ProtocolType(int value)
    {
        switch (value)
        {
        case IPPROTO_IP:
            return ProtocolType::IPv4;

        case IPPROTO_IPV6:
            return ProtocolType::IPv6;

        case IPPROTO_TCP:
            return ProtocolType::TCP;

        case IPPROTO_UDP:
            return ProtocolType::UDP;

        default:
            break;
        }

        return ProtocolType::Unknown;
    }

    constexpr Platform::NativeIpAddress into_NativeIpAddress(in_addr const& value)
    {
        return Platform::NativeIpAddress{
            .Address = {.IPv4 = value},
            .ScopeId = 0,
            .AddressFamily = AF_INET,
        };
    }

    constexpr Platform::NativeIpAddress into_NativeIpAddress(uint32_t value)
    {
        in_addr addr;
        addr.s_addr = value;

        return into_NativeIpAddress(addr);
    }

    constexpr Platform::NativeIpAddress into_NativeIpAddress(in6_addr const& value, uint32_t scope = 0)
    {
        return Platform::NativeIpAddress{
            .Address = {.IPv6 = value},
            .ScopeId = scope,
            .AddressFamily = AF_INET6,
        };
    }

    constexpr Platform::NativeIpAddress into_NativeIpAddress(sockaddr_in const& value)
    {
        return Platform::NativeIpAddress{
            .Address = {.IPv4 = value.sin_addr},
            .ScopeId = 0,
            .AddressFamily = AF_INET,
        };
    }

    constexpr Platform::NativeIpAddress into_NativeIpAddress(sockaddr_in6 const& value)
    {
        return Platform::NativeIpAddress{
            .Address = {.IPv6 = value.sin6_addr},
            .ScopeId = value.sin6_scope_id,
            .AddressFamily = AF_INET6,
        };
    }
}
