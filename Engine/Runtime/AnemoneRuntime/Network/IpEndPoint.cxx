#include "AnemoneRuntime/Network/IpEndPoint.hxx"
#include "AnemoneRuntime/Network/Detail.hxx"

#if ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX
#include <arpa/inet.h>
#include <netinet/ip.h>
#endif

namespace Anemone::Network
{
    constexpr Interop::NativeIpEndPoint CreateNativeIpEndPoint()
    {
        return Interop::NativeIpEndPoint{{.Header = {.sa_family = AF_UNSPEC}}};
    }

    constexpr Interop::NativeIpEndPoint CreateNativeIpEndPoint(sockaddr_in const& address)
    {
        return Interop::NativeIpEndPoint{{.V4 = address}};
    }

    constexpr Interop::NativeIpEndPoint CreateNativeIpEndPoint(sockaddr_in6 const& address)
    {
        return Interop::NativeIpEndPoint{{.V6 = address}};
    }

    constexpr Interop::NativeIpEndPoint CreateNativeIpEndPoint(in_addr const& address, uint16_t port)
    {
        return Interop::NativeIpEndPoint{
            {
                .V4 = sockaddr_in{
                    .sin_family = AF_INET,
                    .sin_port = Bitwise::HostToNetwork<uint16_t>(port),
                    .sin_addr = address,
                    .sin_zero = {},
                },
            },
        };
    }

    constexpr Interop::NativeIpEndPoint CreateNativeIpEndPoint(in6_addr const& address, uint16_t port, uint32_t scopeId)
    {
        return Interop::NativeIpEndPoint{
            {
                .V6 = sockaddr_in6{
                    .sin6_family = AF_INET6,
                    .sin6_port = Bitwise::HostToNetwork<uint16_t>(port),
                    .sin6_flowinfo = 0,
                    .sin6_addr = address,
                    .sin6_scope_id = scopeId,
                },
            },
        };
    }

    IpEndPoint::IpEndPoint(IpAddress const& address, uint16_t port)
    {
        if (auto innerAddressV4 = address.GetV4())
        {
            this->m_native.Address.V4.sin_family = AF_INET;
            this->m_native.Address.V4.sin_port = Bitwise::HostToNetwork<uint16_t>(port);
            this->m_native.Address.V4.sin_addr = std::bit_cast<in_addr>(innerAddressV4->GetOctets());
            // nativeThis.Address.V4.sin_zero = {};
        }
        else if (auto innerAddressV6 = address.GetV6())
        {
            this->m_native.Address.V6.sin6_family = AF_INET6;
            this->m_native.Address.V6.sin6_port = Bitwise::HostToNetwork<uint16_t>(port);
            this->m_native.Address.V6.sin6_flowinfo = 0;
            this->m_native.Address.V6.sin6_addr = std::bit_cast<in6_addr>(innerAddressV6->GetOctets());
            this->m_native.Address.V6.sin6_scope_id = innerAddressV6->GetScopeId();
        }
        else
        {
            std::unreachable();
        }
    }

    bool IpEndPoint::operator==(IpEndPoint const& other) const
    {
        if (this->m_native.Address.Header.sa_family != other.m_native.Address.Header.sa_family)
        {
            return false;
        }

        if (this->m_native.Address.Header.sa_family == AF_INET)
        {
            sockaddr_in const& thisAddress = this->m_native.Address.V4;
            sockaddr_in const& otherAddress = other.m_native.Address.V4;

            return (thisAddress.sin_port == otherAddress.sin_port) &&
                (thisAddress.sin_addr.s_addr == otherAddress.sin_addr.s_addr);
        }

        if (this->m_native.Address.Header.sa_family == AF_INET6)
        {
            sockaddr_in6 const& thisAddress = this->m_native.Address.V6;
            sockaddr_in6 const& otherAddress = other.m_native.Address.V6;

            return (thisAddress.sin6_port == otherAddress.sin6_port) &&
                (memcmp(thisAddress.sin6_addr.s6_addr, otherAddress.sin6_addr.s6_addr, sizeof(thisAddress.sin6_addr)) == 0) &&
                (thisAddress.sin6_flowinfo == otherAddress.sin6_flowinfo) &&
                (thisAddress.sin6_scope_id == otherAddress.sin6_scope_id);
        }

        AE_PANIC("Invalid address family.");
        return false;
    }

    std::optional<IpAddress> IpEndPoint::GetAddress() const
    {
        if (this->m_native.Address.Header.sa_family == AF_INET)
        {
            return IpAddress{
                IpAddressV4{
                    std::bit_cast<std::array<uint8_t, 4>>(this->m_native.Address.V4.sin_addr.s_addr),
                },
            };
        }

        if (this->m_native.Address.Header.sa_family == AF_INET6)
        {
            return IpAddress{
                IpAddressV6{
                    std::bit_cast<std::array<uint8_t, 16>>(this->m_native.Address.V6.sin6_addr.s6_addr),
                    this->m_native.Address.V6.sin6_scope_id,
                },
            };
        }

        return {};
    }

    std::optional<uint16_t> IpEndPoint::GetPort() const
    {
        if (this->m_native.Address.Header.sa_family == AF_INET)
        {
            return Bitwise::NetworkToHost<uint16_t>(this->m_native.Address.V4.sin_port);
        }

        if (this->m_native.Address.Header.sa_family == AF_INET6)
        {
            return Bitwise::NetworkToHost<uint16_t>(this->m_native.Address.V6.sin6_port);
        }

        return {};
    }
}
