#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include "AnemoneRuntime/Bitwise.hxx"

#include <compare>
#include <array>

namespace Anemone::Network::Detail
{
    [[nodiscard]] constexpr std::array<uint8_t, 16> ConvertSegmentsToOctets(std::array<uint16_t, 8> value)
    {
        value[0] = Bitwise::ToBigEndian(value[0]);
        value[1] = Bitwise::ToBigEndian(value[1]);
        value[2] = Bitwise::ToBigEndian(value[2]);
        value[3] = Bitwise::ToBigEndian(value[3]);
        value[4] = Bitwise::ToBigEndian(value[4]);
        value[5] = Bitwise::ToBigEndian(value[5]);
        value[6] = Bitwise::ToBigEndian(value[6]);
        value[7] = Bitwise::ToBigEndian(value[7]);
        return std::bit_cast<std::array<uint8_t, 16>>(value);
    }

    [[nodiscard]] constexpr std::array<uint16_t, 8> ConvertOctetsToSegments(std::array<uint8_t, 16> value)
    {
        std::array result = std::bit_cast<std::array<uint16_t, 8>>(value);
        result[0] = Bitwise::ToLittleEndian(result[0]);
        result[1] = Bitwise::ToLittleEndian(result[1]);
        result[2] = Bitwise::ToLittleEndian(result[2]);
        result[3] = Bitwise::ToLittleEndian(result[3]);
        result[4] = Bitwise::ToLittleEndian(result[4]);
        result[5] = Bitwise::ToLittleEndian(result[5]);
        result[6] = Bitwise::ToLittleEndian(result[6]);
        result[7] = Bitwise::ToLittleEndian(result[7]);
        return result;
    }
}

namespace Anemone::Network
{
    class IpAddressV4 final
    {
    private:
        std::array<uint8_t, 4> m_octets{};

    public:
        constexpr IpAddressV4() = default;
        constexpr IpAddressV4(IpAddressV4 const&) = default;
        constexpr IpAddressV4(IpAddressV4&&) noexcept = default;
        constexpr IpAddressV4& operator=(IpAddressV4 const&) = default;
        constexpr IpAddressV4& operator=(IpAddressV4&&) noexcept = default;
        constexpr ~IpAddressV4() = default;

        constexpr IpAddressV4(std::array<uint8_t, 4> const& octets)
            : m_octets{octets}
        {
        }

    public:
        static constexpr IpAddressV4 Localhost()
        {
            return IpAddressV4{{127, 0, 0, 1}};
        }

        static constexpr IpAddressV4 Any()
        {
            return IpAddressV4{{0, 0, 0, 0}};
        }

        static constexpr IpAddressV4 Broadcast()
        {
            return IpAddressV4{{255, 255, 255, 255}};
        }

    public:
        [[nodiscard]] constexpr auto operator<=>(IpAddressV4 const&) const = default;

    public:
        [[nodiscard]] constexpr std::array<uint8_t, 4> const& GetOctets() const
        {
            return this->m_octets;
        }

    public:
        [[nodiscard]] constexpr bool IsLocalhost() const
        {
            return this->m_octets == Localhost().GetOctets();
        }

        [[nodiscard]] constexpr bool IsAny() const
        {
            return this->m_octets == Any().GetOctets();
        }

        [[nodiscard]] constexpr bool IsBroadcast() const
        {
            return this->m_octets == Broadcast().GetOctets();
        }
    };
}

namespace Anemone::Network
{
    class IpAddressV6 final
    {
    private:
        std::array<uint8_t, 16> m_octets{};
        uint32_t m_scopeId{};

    public:
        constexpr IpAddressV6() = default;
        constexpr IpAddressV6(IpAddressV6 const&) = default;
        constexpr IpAddressV6(IpAddressV6&&) noexcept = default;
        constexpr IpAddressV6& operator=(IpAddressV6 const&) = default;
        constexpr IpAddressV6& operator=(IpAddressV6&&) noexcept = default;
        constexpr ~IpAddressV6() = default;

        explicit constexpr IpAddressV6(std::array<uint8_t, 16> const& octets)
            : m_octets{octets}
        {
        }

        explicit constexpr IpAddressV6(std::array<uint8_t, 16> const& octets, uint32_t scopeId)
            : m_octets{octets}
            , m_scopeId{scopeId}
        {
        }

        explicit constexpr IpAddressV6(std::array<uint16_t, 8> const& segments)
            : m_octets{Detail::ConvertSegmentsToOctets(segments)}
        {
        }

        explicit constexpr IpAddressV6(std::array<uint16_t, 8> const& segments, uint32_t scopeId)
            : m_octets{Detail::ConvertSegmentsToOctets(segments)}
            , m_scopeId{scopeId}
        {
        }

    public:
        static constexpr IpAddressV6 Localhost()
        {
            return IpAddressV6{std::array<uint8_t, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}};
        }

        static constexpr IpAddressV6 Any()
        {
            return IpAddressV6{std::array<uint8_t, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
        }

        static constexpr IpAddressV6 Mapped(IpAddressV4 const& address)
        {
            return IpAddressV6{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF, address.GetOctets()[0], address.GetOctets()[1], address.GetOctets()[2], address.GetOctets()[3]}};
        }

    public:
        [[nodiscard]] constexpr auto operator<=>(IpAddressV6 const&) const = default;

    public:
        [[nodiscard]] constexpr std::array<uint8_t, 16> const& GetOctets() const
        {
            return this->m_octets;
        }

        [[nodiscard]] constexpr std::array<uint16_t, 8> GetSegments() const
        {
            return Detail::ConvertOctetsToSegments(this->m_octets);
        }

        [[nodiscard]] constexpr uint32_t GetScopeId() const
        {
            return this->m_scopeId;
        }

    public:
        [[nodiscard]] constexpr bool IsLocalhost() const
        {
            return this->m_octets == Localhost().GetOctets();
        }

        [[nodiscard]] constexpr bool IsAny() const
        {
            return this->m_octets == Any().GetOctets();
        }
    };
}

namespace Anemone::Network
{
    class IpAddress final
    {
    private:
        bool m_isV6{false};
        IpAddressV4 m_v4{};
        IpAddressV6 m_v6{};

    public:
        constexpr IpAddress() = default;
        constexpr IpAddress(IpAddress const&) = default;
        constexpr IpAddress(IpAddress&&) noexcept = default;
        constexpr IpAddress& operator=(IpAddress const&) = default;
        constexpr IpAddress& operator=(IpAddress&&) noexcept = default;
        constexpr ~IpAddress() = default;

        constexpr IpAddress(IpAddressV4 const& v4)
            : m_v4{v4}
        {
        }

        constexpr IpAddress(IpAddressV6 const& v6)
            : m_isV6{true}
            , m_v6{v6}
        {
        }

    public:
        [[nodiscard]] constexpr auto operator<=>(IpAddress const& other) const
        {
            std::strong_ordering const sameType = (this->m_isV6 <=> other.m_isV6);

            if (sameType == std::strong_ordering::equal)
            {
                if (this->m_isV6)
                {
                    return this->m_v6 <=> other.m_v6;
                }

                return this->m_v4 <=> other.m_v4;
            }

            return sameType;
        }

    public:
        [[nodiscard]] constexpr std::optional<IpAddressV4> GetV4() const
        {
            if (not this->m_isV6)
            {
                return this->m_v4;
            }

            return std::nullopt;
        }

        [[nodiscard]] constexpr std::optional<IpAddressV6> GetV6() const
        {
            if (this->m_isV6)
            {
                return this->m_v6;
            }

            return std::nullopt;
        }

        [[nodiscard]] constexpr bool IsV4() const
        {
            return not this->m_isV6;
        }

        [[nodiscard]] constexpr bool IsV6() const
        {
            return this->m_isV6;
        }

    public:
        [[nodiscard]] constexpr bool IsLocalhost() const
        {
            return this->m_isV6 ? this->m_v6.IsLocalhost() : this->m_v4.IsLocalhost();
        }

        [[nodiscard]] constexpr bool IsAny() const
        {
            return this->m_isV6 ? this->m_v6.IsAny() : this->m_v4.IsAny();
        }
    };
}
