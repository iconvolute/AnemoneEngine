#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#include <cstdint>
#include <span>
#include <compare>
#include <string_view>
#include <bit>
#include <optional>
#include <expected>
#include <charconv>
#include <array>

#include <fmt/format.h>

namespace Anemone::Math
{
    class Random;
}

namespace Anemone
{
    struct DateTime;

    enum class UuidVariant
    {
        Future,
        NCS,
        RFC4112,
        Microsoft,
    };

    enum class UuidVersion
    {
        Empty = 0,
        Mac = 1,
        Dce = 2,
        Md5 = 3,
        Random = 4,
        Sha1 = 5,
        SortMac = 6,
        SortRand = 7,
        Custom = 8,
        Max = 15,
    };

    enum class UuidStringFormat
    {
        // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
        None,
        // XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
        Dashes,
        // {XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX}
        Braces,
        // {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}
        BracesDashes,
    };

    struct Uuid final
    {
    public:
        uint8_t Elements[16];

    public:
        [[nodiscard]] friend constexpr auto operator<=>(Uuid const& self, Uuid const& other) = default;

        static constexpr Uuid Create(uint64_t upper, uint64_t lower)
        {
            std::array values{lower, upper};
            return std::bit_cast<Uuid>(values);
        }

        RUNTIME_API static Uuid CreateRandom();

        RUNTIME_API static Uuid CreateRandom(Math::Random& generator);

        RUNTIME_API static Uuid CreateSortable(Math::Random& generator);

        RUNTIME_API static Uuid CreateSortable(DateTime dateTime, Math::Random& generator);

        RUNTIME_API static Uuid Create(std::string_view name, uint64_t seed = 0);

        RUNTIME_API static Uuid CreateDerived(Uuid const& base, std::string_view name, uint64_t seed = 0);

        static constexpr Uuid Empty()
        {
            return Uuid{};
        }

        static constexpr Uuid Max()
        {
            return Create(std::numeric_limits<uint64_t>::max(), std::numeric_limits<uint64_t>::max());
        }

        constexpr UuidVariant GetVariant() const
        {
            uint8_t const variant = this->Elements[8];

            if ((variant & 0x80) == 0x00)
            {
                return UuidVariant::NCS;
            }

            if ((variant & 0xc0) == 0x80)
            {
                return UuidVariant::RFC4112;
            }

            if ((variant & 0xe0) == 0xc0)
            {
                return UuidVariant::Microsoft;
            }

            return UuidVariant::Future;
        }

        constexpr UuidVersion GetVersion() const
        {
            return static_cast<UuidVersion>(this->Elements[6] >> 4);
        }

        constexpr void SetVersion(UuidVersion version)
        {
            this->Elements[6] &= 0x0f;
            this->Elements[6] |= static_cast<uint8_t>(version) << 4;
        }
    };

    static_assert(sizeof(Uuid) == 16);
    static_assert(std::is_trivially_constructible_v<Uuid>);
    static_assert(std::is_standard_layout_v<Uuid>);

    // TODO: Implement in terms of string_builder when available.
    [[nodiscard]] RUNTIME_API bool TryParse(Uuid& result, std::string_view value);

    [[nodiscard]] RUNTIME_API bool TryFormat(std::string& result, Uuid const& value, UuidStringFormat format);

    [[nodiscard]] RUNTIME_API size_t TryFormat(std::span<char> buffer, Uuid const& value, UuidStringFormat format);

    inline bool TryFormat(std::string& result, Uuid const& value)
    {
        return TryFormat(result, value, UuidStringFormat::BracesDashes);
    }

    inline constexpr Uuid NAMESPACE_DNS{{0x6b, 0xa7, 0xb8, 0x10, 0x9d, 0xad, 0x11, 0xd1, 0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8}};
    inline constexpr Uuid NAMESPACE_OID{{0x6b, 0xa7, 0xb8, 0x12, 0x9d, 0xad, 0x11, 0xd1, 0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8}};
    inline constexpr Uuid NAMESPACE_URL{{0x6b, 0xa7, 0xb8, 0x11, 0x9d, 0xad, 0x11, 0xd1, 0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8}};
    inline constexpr Uuid NAMESPACE_X500{{0x6b, 0xa7, 0xb8, 0x14, 0x9d, 0xad, 0x11, 0xd1, 0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8}};
}

template <>
struct fmt::formatter<Anemone::Uuid>
{
private:
    Anemone::UuidStringFormat m_Format{Anemone::UuidStringFormat::BracesDashes};

public:
    constexpr auto parse(auto& context) noexcept
    {
        auto it = context.begin();

        if (it != context.end())
        {
            switch (*it)
            {
            case 'n':
                {
                    ++it;
                    this->m_Format = Anemone::UuidStringFormat::None;
                    break;
                }
            case 'd':
                {
                    ++it;
                    this->m_Format = Anemone::UuidStringFormat::Dashes;
                    break;
                }
            case 'b':
                {
                    ++it;
                    this->m_Format = Anemone::UuidStringFormat::Braces;
                    break;
                }

            case 'f':
                {
                    ++it;
                    this->m_Format = Anemone::UuidStringFormat::BracesDashes;
                    break;
                }

            default:
                {
                    break;
                }
            }
        }

        return it;
    }

    auto format(Anemone::Uuid const& value, auto& context) noexcept
    {
        std::array<char, 48> buffer{};

        size_t const processed = Anemone::TryFormat(buffer, value, this->m_Format);
        if (processed != 0)
        {
            return std::copy(buffer.data(), buffer.data() + processed, context.out());
        }

        context.on_error("Could not format Uuid to string");
        return context.out();
    }
};
