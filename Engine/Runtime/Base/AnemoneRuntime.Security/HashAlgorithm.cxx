#include "AnemoneRuntime.Security/HashAlgorithm.hxx"

namespace Anemone
{
    std::expected<void, Error> HashAlgorithm::ComputeCore(
        HashAlgorithm& algorithm,
        std::span<std::byte const> buffer,
        std::span<std::byte> hash)
    {
        if (auto result = algorithm.Initialize(); not result)
        {
            return std::unexpected(result.error());
        }

        if (auto result = algorithm.Update(buffer); not result)
        {
            return std::unexpected(result.error());
        }

        if (auto result = algorithm.Finalize(hash); not result)
        {
            return std::unexpected(result.error());
        }

        return {};
    }
}
