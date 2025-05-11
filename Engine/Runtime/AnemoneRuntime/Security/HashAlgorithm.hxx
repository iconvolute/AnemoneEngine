#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Diagnostics/Status.hxx"

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
        virtual std::expected<void, Status> Initialize() = 0;

        virtual std::expected<void, Status> Update(std::span<std::byte const> buffer) = 0;

        virtual std::expected<void, Status> Finalize(std::span<std::byte> hash) = 0;

        virtual size_t GetHashSize() const = 0;

        virtual size_t GetBlockSize() const = 0;

    private:
        static std::expected<void, Status> ComputeCore(
            HashAlgorithm& algorithm,
            std::span<std::byte const> buffer,
            std::span<std::byte> hash);

    public:
        template <typename HashAlgorithmT>
            requires(std::is_base_of_v<HashAlgorithm, HashAlgorithmT>)
        static std::expected<void, Status> Compute(
            std::span<std::byte const> buffer,
            std::span<std::byte> hash)
        {
            HashAlgorithmT algorithm{};
            return ComputeCore(algorithm, buffer, hash);
        }
    };
}
