#include "AnemoneRuntime.Network/IpEndPoint.hxx"
#include "AnemoneRuntime.Network/Detail.hxx"
#include "AnemoneRuntime.Diagnostics/Debug.hxx"

#if ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX
#include <arpa/inet.h>
#include <netinet/ip.h>
#endif

#include <utility>

namespace Anemone::Network
{
    constexpr IpEndPoint::Native CreateNativeIpEndPoint()
    {
        return IpEndPoint::Native{.header = {.sa_family = AF_UNSPEC}};
    }

    constexpr IpEndPoint::Native CreateNativeIpEndPoint(sockaddr_in const& address)
    {
        return IpEndPoint::Native{.v4 = address};
    }

    constexpr IpEndPoint::Native CreateNativeIpEndPoint(sockaddr_in6 const& address)
    {
        return IpEndPoint::Native{.v6 = address};
    }

    constexpr IpEndPoint::Native CreateNativeIpEndPoint(in_addr const& address, uint16_t port)
    {
        return IpEndPoint::Native{
            .v4 = sockaddr_in{
                .sin_family = AF_INET,
                .sin_port = HostToNetwork<uint16_t>(port),
                .sin_addr = address,
                .sin_zero = {},
            },
        };
    }

    constexpr IpEndPoint::Native CreateNativeIpEndPoint(in6_addr const& address, uint16_t port, uint32_t scopeId)
    {
        return IpEndPoint::Native{
            .v6 = sockaddr_in6{
                .sin6_family = AF_INET6,
                .sin6_port = HostToNetwork<uint16_t>(port),
                .sin6_flowinfo = 0,
                .sin6_addr = address,
                .sin6_scope_id = scopeId,
            },
        };
    }

    IpEndPoint::IpEndPoint(IpAddress const& address, uint16_t port)
    {
        if (auto innerAddressV4 = address.GetV4())
        {
            this->m_native.v4.sin_family = AF_INET;
            this->m_native.v4.sin_port = HostToNetwork<uint16_t>(port);
            this->m_native.v4.sin_addr = std::bit_cast<in_addr>(innerAddressV4->GetOctets());
            // nativeThis.Address.V4.sin_zero = {};
        }
        else if (auto innerAddressV6 = address.GetV6())
        {
            this->m_native.v6.sin6_family = AF_INET6;
            this->m_native.v6.sin6_port = HostToNetwork<uint16_t>(port);
            this->m_native.v6.sin6_flowinfo = 0;
            this->m_native.v6.sin6_addr = std::bit_cast<in6_addr>(innerAddressV6->GetOctets());
            this->m_native.v6.sin6_scope_id = innerAddressV6->GetScopeId();
        }
        else
        {
            std::unreachable();
        }
    }

    bool IpEndPoint::operator==(IpEndPoint const& other) const
    {
        if (this->m_native.header.sa_family != other.m_native.header.sa_family)
        {
            return false;
        }

        if (this->m_native.header.sa_family == AF_INET)
        {
            sockaddr_in const& thisAddress = this->m_native.v4;
            sockaddr_in const& otherAddress = other.m_native.v4;

            return (thisAddress.sin_port == otherAddress.sin_port) &&
                (thisAddress.sin_addr.s_addr == otherAddress.sin_addr.s_addr);
        }

        if (this->m_native.header.sa_family == AF_INET6)
        {
            sockaddr_in6 const& thisAddress = this->m_native.v6;
            sockaddr_in6 const& otherAddress = other.m_native.v6;

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
        if (this->m_native.header.sa_family == AF_INET)
        {
            return IpAddress{
                IpAddressV4{
                    std::bit_cast<std::array<uint8_t, 4>>(this->m_native.v4.sin_addr.s_addr),
                },
            };
        }

        if (this->m_native.header.sa_family == AF_INET6)
        {
            return IpAddress{
                IpAddressV6{
                    std::bit_cast<std::array<uint8_t, 16>>(this->m_native.v6.sin6_addr.s6_addr),
                    this->m_native.v6.sin6_scope_id,
                },
            };
        }

        return {};
    }

    std::optional<uint16_t> IpEndPoint::GetPort() const
    {
        if (this->m_native.header.sa_family == AF_INET)
        {
            return NetworkToHost<uint16_t>(this->m_native.v4.sin_port);
        }

        if (this->m_native.header.sa_family == AF_INET6)
        {
            return NetworkToHost<uint16_t>(this->m_native.v6.sin6_port);
        }

        return {};
    }
}
