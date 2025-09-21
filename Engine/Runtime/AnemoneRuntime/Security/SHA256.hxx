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
        static constexpr size_t HashSize = 32;
        static constexpr size_t BlockSize = 64;

    public:
        std::expected<void, Error> Initialize() override;

        std::expected<void, Error> Update(std::span<std::byte const> buffer) override;

        std::expected<void, Error> Finalize(std::span<std::byte> hash) override;

        size_t GetHashSize() const override;

        size_t GetBlockSize() const override;
    };
}
