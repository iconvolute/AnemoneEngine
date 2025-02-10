#pragma once
#include "AnemoneRuntime/Security/HashAlgorithm.hxx"

#include <array>

namespace Anemone
{
    class RUNTIME_API SHA256 final : public HashAlgorithm
    {
    private:
        std::array<uint8_t, 64> _buffer;
        std::array<uint32_t, 64> _w;
        std::array<uint32_t, 8> _state;
        uint64_t _count;

    public:
        SHA256() = default;
        SHA256(SHA256 const&) = default;
        SHA256(SHA256&&) = default;
        SHA256& operator=(SHA256 const&) = default;
        SHA256& operator=(SHA256&&) = default;
        ~SHA256() override = default;

    public:
        std::expected<void, ErrorCode> Initialize() override;

        std::expected<void, ErrorCode> Update(std::span<std::byte const> buffer) override;

        std::expected<void, ErrorCode> Finalize(std::span<std::byte> hash) override;
    };
}

