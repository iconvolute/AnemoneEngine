#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Diagnostics/Status.hxx"

#include <span>
#include <array>

namespace Anemone
{
    class RUNTIME_API SymmetricAlgorithm
    {
    public:
        SymmetricAlgorithm() = default;

        SymmetricAlgorithm(SymmetricAlgorithm const&) = delete;

        SymmetricAlgorithm(SymmetricAlgorithm&&) = delete;

        SymmetricAlgorithm& operator=(SymmetricAlgorithm const&) = delete;

        SymmetricAlgorithm& operator=(SymmetricAlgorithm&&) = delete;

        virtual ~SymmetricAlgorithm() = default;

    public:
        virtual Status Initialize(std::span<std::byte const> key) = 0;

        virtual Status Encrypt(std::span<std::byte const> input, std::span<std::byte> output) = 0;

        virtual Status Decrypt(std::span<std::byte const> input, std::span<std::byte> output) = 0;
    };

    class RUNTIME_API AES final : public SymmetricAlgorithm
    {
    public:
        AES() = default;
        AES(AES const&) = delete;
        AES(AES&&) = delete;
        AES& operator=(AES const&) = delete;
        AES& operator=(AES&&) = delete;
        ~AES() override = default;

    public:
        Status Initialize(std::span<std::byte const> key) override;

        Status Encrypt(std::span<std::byte const> input, std::span<std::byte> output) override;

        Status Decrypt(std::span<std::byte const> input, std::span<std::byte> output) override;

    private:
        size_t _rounds{};
        std::array<uint32_t, 60> _encryptionKey{};
        std::array<uint32_t, 60> _decryptionKey{};
    };
}
