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
}

namespace Anemone
{
    struct Uuid final
    {
        uint8_t Elements[16];

        constexpr auto operator<=>(Uuid const&) const = default;
    };

    struct UuidParser final
    {
        static constexpr uint8_t ParseDigit(char c) noexcept
        {
            if (c >= '0' && c <= '9')
            {
                return static_cast<uint8_t>(c - '0');
            }

            if (c >= 'A' && c <= 'F')
            {
                return static_cast<uint8_t>(10 + c - 'A');
            }

            if (c >= 'a' && c <= 'f')
            {
                return static_cast<uint8_t>(10 + c - 'a');
            }

            return 16;
        }

        static constexpr auto Parse(std::string_view value) -> std::optional<Uuid>
        {
            // Valid UUID string representation will have one of these lengths.
            size_t const size = value.size();

            if ((size == 34u) or (size == 38u))
            {
                // '{uuid}'
                value.remove_prefix(1);
                value.remove_suffix(1);
            }

            bool const dashes = (size == 36u) or (size == 38u);

            uint8_t maxDigit = 0;

            Uuid result;

            size_t current = 0;

            for (size_t i = 0; i < 16; ++i)
            {
                if (dashes)
                {
                    if ((i == 4) or (i == 6) or (i == 8) or (i == 10))
                    {
                        if (value[current] != '-')
                        {
                            return std::nullopt;
                        }

                        ++current;
                    }
                }

                uint8_t const hi = ParseDigit(value[current++]);
                uint8_t const lo = ParseDigit(value[current++]);

                maxDigit |= hi;
                maxDigit |= lo;

                result.Elements[i] = static_cast<uint8_t>(
                    (static_cast<uint8_t>(hi) << 4u) | static_cast<uint8_t>(lo));
            }

            if (maxDigit >= 16u)
            {
                return std::nullopt;
            }

            return result;
        }
    };

    struct UuidGenerator final
    {
        RUNTIME_API static Uuid CreateRandom(Math::Random& generator);

        RUNTIME_API static Uuid CreateSortable(Math::Random& generator);

        RUNTIME_API static Uuid CreateSortable(Math::Random& generator, DateTime dateTime);

        RUNTIME_API static Uuid CreateNamed(std::string_view name, uint64_t seed = 0);

        RUNTIME_API static Uuid CreateNamed(Uuid const& base, std::string_view name, uint64_t seed = 0);
    };

    inline constexpr Uuid NAMESPACE_DNS{
        {0x6b, 0xa7, 0xb8, 0x10, 0x9d, 0xad, 0x11, 0xd1, 0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8},
    };
    inline constexpr Uuid NAMESPACE_OID{
        {0x6b, 0xa7, 0xb8, 0x12, 0x9d, 0xad, 0x11, 0xd1, 0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8},
    };
    inline constexpr Uuid NAMESPACE_URL{
        {0x6b, 0xa7, 0xb8, 0x11, 0x9d, 0xad, 0x11, 0xd1, 0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8},
    };
    inline constexpr Uuid NAMESPACE_X500{
        {0x6b, 0xa7, 0xb8, 0x14, 0x9d, 0xad, 0x11, 0xd1, 0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8},
    };
}

template <>
struct fmt::formatter<Anemone::Uuid>
{
private:
    bool m_dashes = true;
    bool m_braces = true;

public:
    constexpr auto parse(auto& context) noexcept
    {
        auto it = context.begin();

        if (it != context.end())
        {
            switch (*it)
            {
            case 'd':
                {
                    ++it;
                    this->m_braces = false;
                    this->m_dashes = true;
                    break;
                }
            case 'b':
                {
                    ++it;
                    this->m_braces = true;
                    this->m_dashes = false;
                    break;
                }

            case 'f':
                {
                    ++it;
                    this->m_braces = true;
                    this->m_dashes = true;
                    break;
                }

            case 'r':
                {
                    ++it;
                    this->m_braces = false;
                    this->m_dashes = false;
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

    auto format(Anemone::Uuid const& value, auto& context) const noexcept
    {
        auto out = context.out();

        size_t i = 0u;

        if (this->m_braces)
        {
            (*out++) = '{';
        }

        constexpr const char* digits = "0123456789abcdef";

        if (this->m_dashes)
        {
            for (uint8_t const element : value.Elements)
            {

                if (i == 4 || i == 6 || i == 8 || i == 10)
                {
                    (*out++) = '-';
                }

                (*out++) = digits[static_cast<size_t>((element >> 4u) & 0x0Fu)];
                (*out++) = digits[static_cast<size_t>(element & 0x0Fu)];

                ++i;
            }
        }
        else
        {
            for (uint8_t const element : value.Elements)
            {
                (*out++) = digits[static_cast<size_t>((element >> 4u) & 0x0Fu)];
                (*out++) = digits[static_cast<size_t>(element & 0x0Fu)];

                ++i;
            }
        }

        if (this->m_braces)
        {
            (*out++) = '}';
        }

        return out;
    }
};
