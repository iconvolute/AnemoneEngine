#include "AnemoneRuntime/Network/IpEndPoint.hxx"
#include "AnemoneRuntime/Network/Detail.hxx"

#if ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX
#include <arpa/inet.h>
#include <netinet/ip.h>
#endif

namespace Anemone::Network
{
    IpEndPoint::IpEndPoint()
    {
        Platform::NativeIpEndPoint& nativeThis = Platform::Create(this->m_native);
        nativeThis.Inner.AddressV4 = sockaddr_in{
            .sin_family = AF_INET,
            .sin_port = 0,
            .sin_addr = std::bit_cast<in_addr>(IpAddressV4::Any().GetOctets()),
            .sin_zero = {},
        };
    }

    IpEndPoint::IpEndPoint(IpEndPoint const& other)
    {
        Platform::Create(this->m_native, Platform::Get(other.m_native));
    }

    IpEndPoint::IpEndPoint(IpEndPoint&& other) noexcept
    {
        Platform::Create(this->m_native, Platform::Get(other.m_native));
    }

    IpEndPoint& IpEndPoint::operator=(IpEndPoint const& other)
    {
        if (this != std::addressof(other))
        {
            Platform::Get(this->m_native) = Platform::Get(other.m_native);
        }

        return *this;
    }

    IpEndPoint& IpEndPoint::operator=(IpEndPoint&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            Platform::Get(this->m_native) = Platform::Get(other.m_native);
        }

        return *this;
    }

    IpEndPoint::~IpEndPoint()
    {
        Platform::Destroy(this->m_native);
    }

    IpEndPoint::IpEndPoint(IpAddress const& address, uint16_t port)
    {
        Platform::NativeIpEndPoint& nativeThis = Platform::Create(this->m_native);

        if (auto innerAddressV4 = address.GetV4())
        {
            nativeThis.Inner.AddressV4 = sockaddr_in{
                .sin_family = AF_INET,
                .sin_port = Bitwise::HostToNetwork<uint16_t>(port),
                .sin_addr = std::bit_cast<in_addr>(innerAddressV4->GetOctets()),
                .sin_zero = {},
            };
        }
        else if (auto innerAddressV6 = address.GetV6())
        {
            nativeThis.Inner.AddressV6 = sockaddr_in6{
                .sin6_family = AF_INET6,
                .sin6_port = Bitwise::HostToNetwork<uint16_t>(port),
                .sin6_flowinfo = 0,
                .sin6_addr = std::bit_cast<in6_addr>(innerAddressV6->GetOctets()),
                .sin6_scope_id = 0,
            };
        }
        else
        {
            std::unreachable();
        }
    }

    bool IpEndPoint::operator==(IpEndPoint const& other) const
    {
        Platform::NativeIpEndPoint const& nativeThis = Platform::Get(this->m_native);
        Platform::NativeIpEndPoint const& nativeOther = Platform::Get(other.m_native);

        if (nativeThis.Inner.Address.sa_family != nativeOther.Inner.Address.sa_family)
        {
            return false;
        }

        if (nativeThis.Inner.Address.sa_family == AF_INET)
        {
            sockaddr_in const& thisAddress = nativeThis.Inner.AddressV4;
            sockaddr_in const& otherAddress = nativeOther.Inner.AddressV4;

            return (thisAddress.sin_port == otherAddress.sin_port) &&
                (thisAddress.sin_addr.s_addr == otherAddress.sin_addr.s_addr);
        }

        if (nativeThis.Inner.Address.sa_family == AF_INET6)
        {
            sockaddr_in6 const& thisAddress = nativeThis.Inner.AddressV6;
            sockaddr_in6 const& otherAddress = nativeOther.Inner.AddressV6;

            return (thisAddress.sin6_port == otherAddress.sin6_port) &&
                (memcmp(thisAddress.sin6_addr.s6_addr, otherAddress.sin6_addr.s6_addr, sizeof(thisAddress.sin6_addr)) == 0) &&
                (thisAddress.sin6_flowinfo == otherAddress.sin6_flowinfo) &&
                (thisAddress.sin6_scope_id == otherAddress.sin6_scope_id);
        }

        AE_PANIC("Invalid address family.");
    }

    IpAddress IpEndPoint::GetAddress() const
    {
        Platform::NativeIpEndPoint const& nativeThis = Platform::Get(this->m_native);

        if (nativeThis.Inner.Address.sa_family == AF_INET)
        {
            return IpAddress{
                IpAddressV4{
                    std::bit_cast<std::array<uint8_t, 4>>(nativeThis.Inner.AddressV4.sin_addr.s_addr),
                },
            };
        }

        if (nativeThis.Inner.Address.sa_family == AF_INET6)
        {
            return IpAddress{
                IpAddressV6{
                    std::bit_cast<std::array<uint8_t, 16>>(nativeThis.Inner.AddressV6.sin6_addr.s6_addr),
                    nativeThis.Inner.AddressV6.sin6_scope_id,
                },
            };
        }

        AE_PANIC("Invalid address family.");
    }

    uint16_t IpEndPoint::GetPort() const
    {
        Platform::NativeIpEndPoint const& nativeThis = Platform::Get(this->m_native);

        if (nativeThis.Inner.Address.sa_family == AF_INET)
        {
            return Bitwise::NetworkToHost<uint16_t>(nativeThis.Inner.AddressV4.sin_port);
        }

        if (nativeThis.Inner.Address.sa_family == AF_INET6)
        {
            return Bitwise::NetworkToHost<uint16_t>(nativeThis.Inner.AddressV6.sin6_port);
        }

        AE_PANIC("Invalid address family.");
    }
}
