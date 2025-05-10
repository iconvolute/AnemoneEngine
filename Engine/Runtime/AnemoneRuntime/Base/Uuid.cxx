#include "AnemoneRuntime/Base/Uuid.hxx"
#include "AnemoneRuntime/Math/Random.hxx"
#include "AnemoneRuntime/Base/DateTime.hxx"
#include "AnemoneRuntime/Security/SHA256.hxx"
#include "AnemoneRuntime/Base/Bitwise.hxx"

namespace Anemone
{
    static_assert(sizeof(Uuid) == 16);
    static_assert(std::is_trivially_constructible_v<Uuid>);
    static_assert(std::is_standard_layout_v<Uuid>);

    Uuid UuidGenerator::CreateRandom(Math::Random& generator)
    {
        uint64_t const lo = generator.NextUInt64();
        uint64_t const hi = generator.NextUInt64();

        std::array elements{lo, hi};

        return std::bit_cast<Uuid>(elements);
    }

    Uuid UuidGenerator::CreateSortable(Math::Random& generator)
    {
        return CreateSortable(generator, DateTime::UtcNow());
    }

    Uuid UuidGenerator::CreateSortable(Math::Random& generator, DateTime dateTime)
    {
        uint64_t const milliseconds = static_cast<uint64_t>((dateTime - UnixEpoch).ToMilliseconds());

        // Big-endian representation of the milliseconds since Unix epoch
        uint64_t const bytes_bigendian = Bitwise::ToBigEndian(milliseconds << 16u);

        uint64_t const lo = generator.NextUInt64();
        uint64_t const hi = generator.NextUInt64();

        std::array elements{lo, hi};

        Uuid result = std::bit_cast<Uuid>(elements);

        // Copy 48 bits of the big-endian representation of the milliseconds since Unix epoch. It should work for year 10889 with ease
        std::memcpy(result.Elements, &bytes_bigendian, 6);

        return result;
    }

    Uuid UuidGenerator::CreateNamed(std::string_view name, uint64_t seed)
    {
        std::array<std::byte, 32> hash;

        SHA256 context{};
        (void)context.Initialize();
        (void)context.Update(std::as_bytes(std::span{name}));
        (void)context.Update(std::as_bytes(std::span{&seed, 1}));
        (void)context.Finalize(hash);

        Uuid result;
        std::memcpy(result.Elements, hash.data(), std::size(result.Elements));

        return result;
    }

    Uuid UuidGenerator::CreateNamed(Uuid const& base, std::string_view name, uint64_t seed)
    {
        std::array<std::byte, 32> hash;

        SHA256 context{};

        (void)context.Initialize();
        (void)context.Update(std::as_bytes(std::span{base.Elements}));
        (void)context.Update(std::as_bytes(std::span{name}));
        (void)context.Update(std::as_bytes(std::span{&seed, 1}));
        (void)context.Finalize(hash);

        Uuid result;
        std::memcpy(result.Elements, hash.data(), std::size(result.Elements));
        return result;
    }
}
