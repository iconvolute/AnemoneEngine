#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#include <cstddef>
#include <cstdint>
#include <array>
#include <string_view>
#include <span>

namespace Anemone::Crypto
{
    struct Sha256Context
    {
        std::array<std::byte, 64> _buffer;
        std::array<uint32_t, 8> _state;
        std::array<uint32_t, 2> _total_length;
    };

    using Sha256Hash = std::array<uint8_t, 32>;

    RUNTIME_API void sha256_initialize(Sha256Context& context);

    RUNTIME_API void sha256_update(Sha256Context& context, std::span<std::byte const> buffer);

    inline void sha256_update(Sha256Context& context, std::string_view view)
    {
        sha256_update(context, std::as_bytes(std::span{view}));
    }

    [[nodiscard]] RUNTIME_API auto sha256_finalize(Sha256Context& context) -> Sha256Hash;

    [[nodiscard]] RUNTIME_API auto sha256_from_buffer(std::span<std::byte const> buffer) -> Sha256Hash;

    [[nodiscard]] RUNTIME_API auto sha256_from_string(std::string_view string) -> Sha256Hash;
}
