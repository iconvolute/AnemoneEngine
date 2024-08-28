#include "AnemoneRuntime/Network/IpAddress.hxx"
#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"
#elif ANEMONE_PLATFORM_LINUX || ANEMONE_PLATFORM_ANDROID
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"
#endif

#include "Private.hxx"

#include <array>
#include <charconv>

namespace Anemone::Network
{
    IpAddress::IpAddress(Platform::NativeIpAddress const& native)
    {
        Platform::Create(this->Native, native);
    }

    IpAddress::IpAddress()
    {
        Platform::Create(this->Native);
    }

    IpAddress::IpAddress(IpAddress const& other)
    {
        Platform::Create(this->Native, Platform::Get(other.Native));
    }

    IpAddress::IpAddress(IpAddress&& other) noexcept
    {
        Platform::Create(this->Native, Platform::Get(other.Native));
    }

    IpAddress& IpAddress::operator=(IpAddress const& other)
    {
        Platform::NativeIpAddress& nativeThis = Platform::Get(this->Native);
        Platform::NativeIpAddress const& nativeOther = Platform::Get(other.Native);
        nativeThis = nativeOther;

        return *this;
    }

    IpAddress& IpAddress::operator=(IpAddress&& other) noexcept
    {
        Platform::NativeIpAddress& nativeThis = Platform::Get(this->Native);
        Platform::NativeIpAddress const& nativeOther = Platform::Get(other.Native);
        nativeThis = nativeOther;

        return *this;
    }

    IpAddress::~IpAddress()
    {
        Platform::Destroy(this->Native);
    }

    std::optional<IpAddress> IpAddress::FromBytes(std::span<std::byte const> bytes)
    {
        if (bytes.size() == sizeof(in_addr))
        {
            return IpAddress{
                Private::into_NativeIpAddress(
                    *reinterpret_cast<in_addr const*>(bytes.data())),
            };
        }

        if (bytes.size() == sizeof(in6_addr))
        {
            return IpAddress{
                Private::into_NativeIpAddress(
                    *reinterpret_cast<in6_addr const*>(bytes.data())),
            };
        }

        return std::nullopt;
    }

    std::optional<IpAddress> IpAddress::FromBytes(std::span<std::byte const> bytes, uint32_t scopeId)
    {
        if (bytes.size() == sizeof(in6_addr))
        {
            return IpAddress{
                Private::into_NativeIpAddress(
                    *reinterpret_cast<in6_addr const*>(bytes.data()), scopeId),
            };
        }

        return std::nullopt;
    }


    AddressFamily IpAddress::Family() const
    {
        Platform::NativeIpAddress const& nativeThis = Platform::Get(this->Native);

        return Private::into_AddressFamily(nativeThis.AddressFamily);
    }

    uint32_t IpAddress::ScopeId() const
    {
        Platform::NativeIpAddress const& nativeThis = Platform::Get(this->Native);

        if (nativeThis.AddressFamily == AF_INET6)
        {
            return nativeThis.ScopeId;
        }

        return 0;
    }

    bool IpAddress::IsV4() const
    {
        Platform::NativeIpAddress const& nativeThis = Platform::Get(this->Native);

        return nativeThis.AddressFamily == AF_INET;
    }

    bool IpAddress::IsV6() const
    {
        Platform::NativeIpAddress const& nativeThis = Platform::Get(this->Native);

        return nativeThis.AddressFamily == AF_INET6;
    }

    bool IpAddress::IsAny() const
    {
        Platform::NativeIpAddress const& nativeThis = Platform::Get(this->Native);

        if (nativeThis.AddressFamily == AF_INET)
        {
            return nativeThis.Address.IPv4.s_addr == INADDR_ANY;
        }

        if (nativeThis.AddressFamily == AF_INET6)
        {
            for (size_t i = 0; i < sizeof(in6_addr); ++i)
            {
                if (nativeThis.Address.IPv6.s6_addr[i] != 0)
                {
                    return false;
                }
            }
        }

        return false;
    }

    bool IpAddress::IsBroadcast() const
    {
        Platform::NativeIpAddress const& nativeThis = Platform::Get(this->Native);

        if (nativeThis.AddressFamily == AF_INET)
        {
            return nativeThis.Address.IPv4.s_addr == INADDR_BROADCAST;
        }

        return false;
    }

    bool IpAddress::IsLoopback() const
    {
        Platform::NativeIpAddress const& nativeThis = Platform::Get(this->Native);

        if (nativeThis.AddressFamily == AF_INET)
        {
            return nativeThis.Address.IPv4.s_addr == INADDR_LOOPBACK;
        }

        if (nativeThis.AddressFamily == AF_INET6)
        {
            for (size_t i = 0; i < sizeof(in6_addr) - 1; ++i)
            {
                if (nativeThis.Address.IPv6.s6_addr[i] != 0)
                {
                    return false;
                }
            }

            return nativeThis.Address.IPv6.s6_addr[15] == 1;
        }

        return false;
    }

    std::span<std::byte const> IpAddress::ToBytes() const
    {
        Platform::NativeIpAddress const& nativeThis = Platform::Get(this->Native);

        if (nativeThis.AddressFamily == AF_INET)
        {
            return std::span<std::byte const>{
                reinterpret_cast<std::byte const*>(&nativeThis.Address.IPv4.s_addr),
                sizeof(in_addr),
            };
        }

        if (nativeThis.AddressFamily == AF_INET6)
        {
            return std::span<std::byte const>{
                reinterpret_cast<std::byte const*>(&nativeThis.Address.IPv6.s6_addr),
                sizeof(in6_addr),
            };
        }

        return {};
    }

    std::span<std::byte> IpAddress::ToBytes()
    {
        Platform::NativeIpAddress& nativeThis = Platform::Get(this->Native);

        if (nativeThis.AddressFamily == AF_INET)
        {
            return std::span<std::byte>{
                reinterpret_cast<std::byte*>(&nativeThis.Address.IPv4.s_addr),
                sizeof(in_addr),
            };
        }

        if (nativeThis.AddressFamily == AF_INET6)
        {
            return std::span<std::byte>{
                reinterpret_cast<std::byte*>(&nativeThis.Address.IPv6.s6_addr),
                sizeof(in6_addr),
            };
        }

        return {};
    }

    IpAddress IpAddress::Mask(const IpAddress& address, const IpAddress& mask)
    {
        Platform::NativeIpAddress const& nativeAddress = Platform::Get(address.Native);
        Platform::NativeIpAddress const& nativeMask = Platform::Get(mask.Native);

        if (nativeAddress.AddressFamily == AF_INET)
        {
            in_addr result;
            result.s_addr = nativeAddress.Address.IPv4.s_addr & nativeMask.Address.IPv4.s_addr;

            return IpAddress{
                Private::into_NativeIpAddress(result),
            };
        }

        if (nativeAddress.AddressFamily == AF_INET6)
        {
            in6_addr result;
            for (size_t i = 0; i < sizeof(in6_addr); ++i)
            {
                result.s6_addr[i] = nativeAddress.Address.IPv6.s6_addr[i] & nativeMask.Address.IPv6.s6_addr[i];
            }

            return IpAddress{
                Private::into_NativeIpAddress(result),
            };
        }

        return IpAddress{};
    }

    IpAddress IpAddress::Mask(const IpAddress& address, const IpAddress& mask, const IpAddress& set)
    {
        Platform::NativeIpAddress const& nativeAddress = Platform::Get(address.Native);
        Platform::NativeIpAddress const& nativeMask = Platform::Get(mask.Native);
        Platform::NativeIpAddress const& nativeSet = Platform::Get(set.Native);

        if (nativeAddress.AddressFamily == AF_INET)
        {
            in_addr result;
            result.s_addr = (nativeAddress.Address.IPv4.s_addr & nativeMask.Address.IPv4.s_addr) |
                (~nativeMask.Address.IPv4.s_addr & nativeSet.Address.IPv4.s_addr);

            return IpAddress{
                Private::into_NativeIpAddress(result),
            };
        }

        if (nativeAddress.AddressFamily == AF_INET6)
        {
            in6_addr result;
            for (size_t i = 0; i < sizeof(in6_addr); ++i)
            {
                result.s6_addr[i] = (nativeAddress.Address.IPv6.s6_addr[i] & nativeMask.Address.IPv6.s6_addr[i]) |
                    (~nativeMask.Address.IPv6.s6_addr[i] & nativeSet.Address.IPv6.s6_addr[i]);
            }

            return IpAddress{
                Private::into_NativeIpAddress(result),
            };
        }

        return IpAddress{};
    }

    bool IpAddress::operator==(const IpAddress& other) const
    {
        Platform::NativeIpAddress const& nativeThis = Platform::Get(this->Native);
        Platform::NativeIpAddress const& nativeOther = Platform::Get(other.Native);

        if (nativeThis.AddressFamily != nativeOther.AddressFamily)
        {
            return false;
        }

        if (nativeThis.AddressFamily == AF_INET)
        {
            return nativeThis.Address.IPv4.s_addr == nativeOther.Address.IPv4.s_addr;
        }

        if (nativeThis.AddressFamily == AF_INET6)
        {
            for (size_t i = 0; i < sizeof(in6_addr); ++i)
            {
                if (nativeThis.Address.IPv6.s6_addr[i] != nativeOther.Address.IPv6.s6_addr[i])
                {
                    return false;
                }
            }

            return true;
        }

        return false;
    }

    IpAddress IpAddress::Any(AddressFamily family) 
    {
        if (family == AddressFamily::IPv4)
        {
            return IpAddress{Private::into_NativeIpAddress(INADDR_ANY)};
        }

        if (family == AddressFamily::IPv6)
        {
            return IpAddress{Private::into_NativeIpAddress(in6addr_any)};
        }

        return {};
    }

    IpAddress IpAddress::Broadcast(AddressFamily family) 
    {
        if (family == AddressFamily::IPv4)
        {
            return IpAddress{Private::into_NativeIpAddress(INADDR_BROADCAST)};
        }

        return {};
    }

    IpAddress IpAddress::Loopback(AddressFamily family) 
    {
        if (family == AddressFamily::IPv4)
        {
            return IpAddress{Private::into_NativeIpAddress(INADDR_LOOPBACK)};
        }

        if (family == AddressFamily::IPv6)
        {
            return IpAddress{Private::into_NativeIpAddress(in6addr_loopback)};
        }

        return {};
    }

    IpAddress IpAddress::None(AddressFamily family) 
    {
        if (family == AddressFamily::IPv4)
        {
            return IpAddress{Private::into_NativeIpAddress(INADDR_NONE)};
        }

        return {};
    }

    bool IpAddress::ToString(std::string& result) const
    {
        Platform::NativeIpAddress const& nativeThis = Platform::Get(this->Native);

        std::array<char, INET6_ADDRSTRLEN> buffer;

        if (nativeThis.AddressFamily == AF_INET)
        {
            if (inet_ntop(AF_INET, &nativeThis.Address.IPv4, buffer.data(), buffer.size()) == nullptr)
            {
                return false;
            }

            result = buffer.data();
            return true;
        }

        if (nativeThis.AddressFamily == AF_INET6)
        {
            if (inet_ntop(AF_INET6, &nativeThis.Address.IPv6, buffer.data(), buffer.size()) == nullptr)
            {
                return false;
            }

            result = buffer.data();

            if (nativeThis.ScopeId != 0)
            {
                auto[ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), nativeThis.ScopeId);

                if (ec != std::errc{})
                {
                    return false;
                }

                result.append(1, '%');
                result.append(buffer.data(), ptr);
            }
            return true;
        }

        return false;
    }

    bool IpAddress::TryParse(IpAddress& result, std::string_view address)
    {
        Platform::NativeIpAddress& nativeResult = Platform::Get(result.Native);

        bool ipv6 = false;

        if (address.starts_with('[') and address.ends_with(']'))
        {
            ipv6 = true;
            address = address.substr(1, address.size() - 2);
        }

        std::string s{address};

        if (not ipv6)
        {
            if (in_addr addr4; inet_pton(AF_INET, s.c_str(), &addr4) == 1)
            {
                nativeResult = Private::into_NativeIpAddress(addr4);
                return true;
            }
        }

        uint32_t scope_id = 0;

        if (auto n = s.find_last_of('%'); n != std::string::npos)
        {
            // Parse ScopeId
            if (auto[ptr, ec] = std::from_chars(s.data() + n + 1, s.data() + s.size(), scope_id); (ec != std::errc{}) or (*ptr != 0))
            {
                return false;
            }

            // Erase it from address part.
            s.erase(n);
        }

        if (in6_addr addr6; inet_pton(AF_INET6, s.c_str(), &addr6) == 1)
        {
            nativeResult = Private::into_NativeIpAddress(addr6, scope_id);
            return true;
        }

        return false;
    }

    bool IpAddress::TryParse(IpAddress& result, std::string_view address, AddressFamily family)
    {
        Platform::NativeIpAddress& nativeResult = Platform::Get(result.Native);

        if (family == AddressFamily::IPv6)
        {
            if (address.starts_with('[') and address.ends_with(']'))
            {
                address = address.substr(1, address.size() - 2);
            }
        }

        std::string s{address};

        if (family == AddressFamily::IPv4)
        {
            if (in_addr addr4; inet_pton(AF_INET, s.c_str(), &addr4) == 1)
            {
                nativeResult = Private::into_NativeIpAddress(addr4);
                return true;
            }
        }

        if (family == AddressFamily::IPv6)
        {
            if (auto n = s.find_last_of('%'); n != std::string::npos)
            {
                // Parse ScopeId
                if (auto[ptr, ec] = std::from_chars(s.data() + n + 1, s.data() + s.size(), nativeResult.ScopeId); (ec != std::errc{}) or (*ptr != 0))
                {
                    return false;
                }

                // Erase it from address part.
                s.erase(n);
            }

            if (in6_addr addr6; inet_pton(AF_INET6, s.c_str(), &addr6) == 1)
            {
                nativeResult = Private::into_NativeIpAddress(addr6);
                return true;
            }
        }

        return false;
    }
}
