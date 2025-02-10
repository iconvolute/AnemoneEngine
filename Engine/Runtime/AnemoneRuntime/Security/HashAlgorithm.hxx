#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"

#include <span>
#include <expected>

namespace Anemone
{
    class RUNTIME_API HashAlgorithm
    {
    protected:
        HashAlgorithm() = default;

    public:
        HashAlgorithm(HashAlgorithm const&) = default;
        HashAlgorithm(HashAlgorithm&&) = default;
        HashAlgorithm& operator=(HashAlgorithm const&) = default;
        HashAlgorithm& operator=(HashAlgorithm&&) = default;
        virtual ~HashAlgorithm() = default;

    public:
        virtual std::expected<void, ErrorCode> Initialize() = 0;

        virtual std::expected<void, ErrorCode> Update(std::span<std::byte const> buffer) = 0;

        virtual std::expected<void, ErrorCode> Finalize(std::span<std::byte> hash) = 0;

    private:
        anemone_noinline static std::expected<void, ErrorCode> ComputeCore(
            HashAlgorithm& algorithm,
            std::span<std::byte const> buffer,
            std::span<std::byte> hash);

    public:
        template <typename HashAlgorithmT>
            requires(std::is_base_of_v<HashAlgorithm, HashAlgorithmT>)
        static std::expected<void, ErrorCode> Compute(
            std::span<std::byte const> buffer,
            std::span<std::byte> hash)
        {
            HashAlgorithmT algorithm{};
            return ComputeCore(algorithm, buffer, hash);
        }
    };
}
