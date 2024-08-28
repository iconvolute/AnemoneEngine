#include "AnemoneRuntime/Uuid.hxx"
#include "AnemoneRuntime/Numerics/Random.hxx"
#include "AnemoneRuntime/DateTime.hxx"
#include "AnemoneRuntime/Crypto/Sha256.hxx"
#include "AnemoneRuntime/Bitwise.hxx"

namespace Anemone
{
    Uuid Uuid::CreateRandom()
    {
        // TODO: This should use shared random number generator just for this purpose?
        Numerics::Random generator{static_cast<uint64_t>(DateTime::Now().Inner.Seconds)};

        return CreateRandom(generator);
    }

    Uuid Uuid::CreateRandom(Numerics::Random& generator)
    {
        std::array<uint64_t, 2> bits;
        bits[0] = generator.NextUInt64();
        bits[1] = generator.NextUInt64();

        Uuid result = std::bit_cast<Uuid>(bits);
        result.SetVersion(UuidVersion::Random);
        return result;
    }

    Uuid Uuid::CreateSortable(Numerics::Random& generator)
    {
        return CreateSortable(DateTime::UtcNow(), generator);
    }

    Uuid Uuid::CreateSortable(DateTime dateTime, Numerics::Random& generator)
    {
        uint64_t const milliseconds = static_cast<uint64_t>((dateTime - UnixEpoch).ToMilliseconds());

        // Big-endian representation of the milliseconds since Unix epoch
        uint64_t const bytes_bigendian = Bitwise::ToBigEndian(milliseconds << 16u);

        std::array<uint64_t, 2> bits;
        bits[0] = generator.NextUInt64();
        bits[1] = generator.NextUInt64();

        Uuid result = std::bit_cast<Uuid>(bits);

        // Copy 48 bits of the big-endian representation of the milliseconds since Unix epoch. It should work for year 10889 with ease
        std::memcpy(result.Elements, &bytes_bigendian, 6);
        result.SetVersion(UuidVersion::SortRand);
        return result;
    }

    Uuid Uuid::Create(std::string_view name, uint64_t seed)
    {
        Crypto::Sha256Context context;
        Crypto::sha256_initialize(context);
        Crypto::sha256_update(context, std::as_bytes(std::span{name}));
        Crypto::sha256_update(context, std::as_bytes(std::span{&seed, sizeof(seed)}));
        auto hash = Crypto::sha256_finalize(context);

        Uuid result;
        std::memcpy(result.Elements, hash.data(), std::size(result.Elements));
        result.SetVersion(UuidVersion::Custom);
        return result;
    }

    Uuid Uuid::CreateDerived(Uuid const& base, std::string_view name, uint64_t seed)
    {
        Crypto::Sha256Context context;
        Crypto::sha256_initialize(context);
        Crypto::sha256_update(context, std::as_bytes(std::span{base.Elements}));
        Crypto::sha256_update(context, std::as_bytes(std::span{name}));
        Crypto::sha256_update(context, std::as_bytes(std::span{&seed, 1}));
        auto hash = Crypto::sha256_finalize(context);

        Uuid result;
        std::memcpy(result.Elements, hash.data(), std::size(result.Elements));
        result.SetVersion(UuidVersion::Custom);
        return result;
    }

    bool TryParse(Uuid& result, std::string_view value)
    {
        constexpr auto parse_digit = [](char ch) -> uint32_t
        {
            if (('0' <= ch) and (ch <= '9'))
            {
                return static_cast<uint32_t>(ch - '0');
            }

            ch &= ~char{0x20};

            if (('A' <= ch) and (ch <= 'F'))
            {
                return static_cast<uint32_t>(ch - 'A' + 10);
            }

            return 16;
        };

        if (value.size() < 32)
        {
            // Not enough characters to represent a raw UUID in UuidStringFormat::None
            return false;
        }

        bool has_dashes = false;
        bool has_braces = false;

        if (value[0] == '{')
        {
            has_braces = true;
            value.remove_prefix(1);
        }

        for (size_t i = 0; i < 16; ++i)
        {
            if (i == 4)
            {
                if (!value.empty())
                {
                    if (value[0] == '-')
                    {
                        has_dashes = true;
                        value.remove_prefix(1);
                    }
                }
                else
                {
                    return false;
                }
            }
            else if (has_dashes and (i == 6 or i == 8 or i == 10))
            {
                if (not value.empty() and (value[0] == '-'))
                {
                    value.remove_prefix(1);
                }
                else
                {
                    return false;
                }
            }

            if (value.size() < 2)
            {
                return false;
            }

            uint32_t const hi = parse_digit(value[0]);
            uint32_t const lo = parse_digit(value[1]);

            value.remove_prefix(2);

            if ((hi >= 16) or (lo >= 16))
            {
                return false;
            }

            uint32_t const element = (hi << 4) | lo;

            result.Elements[i] = static_cast<uint8_t>(element);
        }

        return (not has_braces) or ((value.size() == 1) and (value[0] == '}'));
    }

    bool TryFormat(std::string& result, Uuid const& value, UuidStringFormat format)
    {
        std::array<char, 48> buffer;

        size_t const written = TryFormat(buffer, value, format);
        result.assign(buffer.data(), written);

        return true;
    }

    size_t TryFormat(std::span<char> buffer, Uuid const& value, UuidStringFormat format)
    {
        bool const braces = (format == UuidStringFormat::Braces || format == UuidStringFormat::BracesDashes);
        bool const dashes = (format == UuidStringFormat::Dashes || format == UuidStringFormat::BracesDashes);

        size_t required = 32;

        if (dashes)
        {
            required += 4;
        }

        if (braces)
        {
            required += 2;
        }

        if (buffer.size() < required)
        {
            return 0;
        }

        size_t current = 0;

        if (braces)
        {
            buffer[current++] = '{';
        }

        for (size_t i = 0; i < 16; ++i)
        {
            constexpr const char* digits = "0123456789abcdef";

            if (dashes)
            {
                if (i == 4 || i == 6 || i == 8 || i == 10)
                {
                    buffer[current++] = '-';
                }
            }

            uint8_t const element{value.Elements[i]};
            buffer[current++] = digits[(element >> 4) & 0x0F];
            buffer[current++] = digits[element & 0x0F];
        }

        if (braces)
        {
            buffer[current++] = '}';
        }

        return current;
    }
}
