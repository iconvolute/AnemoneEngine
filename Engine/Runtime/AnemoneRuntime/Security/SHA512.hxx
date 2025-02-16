#pragma once
#include "AnemoneRuntime/Security/HashAlgorithm.hxx"

#include <array>

namespace Anemone
{
    class RUNTIME_API SHA512 final : public HashAlgorithm
    {
    private:
        std::array<uint8_t, 128> _buffer{};
        std::array<uint64_t, 80> _w{};
        std::array<uint64_t, 8> _state{};
        uint64_t _count{};

    public:
        SHA512() = default;
        SHA512(SHA512 const&) = default;
        SHA512(SHA512&&) = default;
        SHA512& operator=(SHA512 const&) = default;
        SHA512& operator=(SHA512&&) = default;
        ~SHA512() override = default;

    public:
        static constexpr size_t HashSize = 64;
        static constexpr size_t BlockSize = 128;

    public:
        std::expected<void, ErrorCode> Initialize() override;

        std::expected<void, ErrorCode> Update(std::span<std::byte const> buffer) override;

        std::expected<void, ErrorCode> Finalize(std::span<std::byte> hash) override;

        size_t GetHashSize() const override;

        size_t GetBlockSize() const override;
    };
}
