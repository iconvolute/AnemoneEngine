#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#include <span>
#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace Anemone::Crypto
{
    struct Sha512Context
    {
        std::array<std::byte, 128> _buffer;
        std::array<uint64_t, 8> _state;
        std::array<uint64_t, 2> _total_length;
    };

    using Sha512Hash = std::array<uint8_t, 64>;

    RUNTIME_API void sha512_initialize(Sha512Context& context);

    RUNTIME_API void sha512_update(Sha512Context& context, std::span<std::byte const> buffer);

    inline void sha512_update(Sha512Context& context, std::string_view view)
    {
        sha512_update(context, std::as_bytes(std::span{view}));
    }

    [[nodiscard]] RUNTIME_API auto sha512_finalize(Sha512Context& context) -> Sha512Hash;

    [[nodiscard]] RUNTIME_API auto sha512_from_buffer(std::span<std::byte const> buffer) -> Sha512Hash;

    [[nodiscard]] RUNTIME_API auto sha512_from_string(std::string_view string) -> Sha512Hash;
}
