#pragma once
#include "AnemoneRuntime/Hash/FNV.hxx"

#include <compare>
#include <cstdint>

namespace Anemone::Storage
{
    struct BinarySignature final
    {
        uint64_t Value;

        [[nodiscard]] constexpr auto operator<=>(BinarySignature const& other) const = default;

        [[nodiscard]] static constexpr BinarySignature Create(std::string_view name)
        {
            return {FNV1A64::FromString(name)};
        }
    };

    struct BinaryFormatVersion final
    {
        uint32_t Major;
        uint32_t Minor;

        [[nodiscard]] constexpr auto operator<=>(const BinaryFormatVersion&) const = default;
    };

    struct BinaryBlockHeader final
    {
        BinarySignature Signature;
        uint64_t Size;
        uint64_t Offset;
    };

    struct BinaryFormatHeader final
    {
        BinarySignature Signature;
        BinaryFormatVersion Version;
        uint32_t HeadersCount;
    };
}
