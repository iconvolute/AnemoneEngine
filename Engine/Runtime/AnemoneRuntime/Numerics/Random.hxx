#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Types.hxx"

#include <array>
#include <span>
#include <vector>

namespace Anemone::Numerics
{
    class [[nodiscard]] RUNTIME_API Random
    {
    private:
        std::array<uint64_t, 4> _state;

    private:
        [[nodiscard]] uint64_t GenerateBits();

    public:
        Random();
        explicit Random(uint64_t seed);

        void Forward();

    public:
        void Sample(std::span<std::byte> buffer);

        [[nodiscard]] uint64_t NextUInt64();
        [[nodiscard]] uint64_t NextUInt64(uint64_t max);
        [[nodiscard]] uint64_t NextUInt64(uint64_t min, uint64_t max);

        [[nodiscard]] int64_t NextInt64();
        [[nodiscard]] int64_t NextInt64(int64_t max);
        [[nodiscard]] int64_t NextInt64(int64_t min, int64_t max);

        [[nodiscard]] uint32_t NextUInt32();
        [[nodiscard]] uint32_t NextUInt32(uint32_t max);
        [[nodiscard]] uint32_t NextUInt32(uint32_t min, uint32_t max);

        [[nodiscard]] int32_t NextInt32(int32_t max);
        [[nodiscard]] int32_t NextInt32();
        [[nodiscard]] int32_t NextInt32(int32_t min, int32_t max);

        [[nodiscard]] float NextFloat();
        [[nodiscard]] float NextFloat(float max);
        [[nodiscard]] float NextFloat(float min, float max);

        [[nodiscard]] double NextDouble();
        [[nodiscard]] double NextDouble(double max);
        [[nodiscard]] double NextDouble(double min, double max);

        [[nodiscard]] float NextLaplace(float mean, float scale);
        [[nodiscard]] float NextNormal();
        [[nodiscard]] float NextNormal(float mean, float stddev);
    };

    RUNTIME_API void GenerateReadableString(Random& generator, std::span<char> buffer);

    [[nodiscard]] RUNTIME_API Float2 NextFloat2(Random& generator);

    [[nodiscard]] RUNTIME_API Float3 InsideUnitSphere(Random& generator);
    [[nodiscard]] RUNTIME_API Float3 OnUnitSphere(Random& generator);

    [[nodiscard]] RUNTIME_API Float3 InsideUnitCircle(Random& generator);
    [[nodiscard]] RUNTIME_API Float3 OnUnitCircle(Random& generator);
}

namespace Anemone::Numerics
{
    RUNTIME_API void ComputeHistogram(std::vector<uint64_t>& histogram, std::span<uint8_t const> data);

    RUNTIME_API void ComputeHistogram(std::vector<uint64_t>& histogram, std::span<float const> data, size_t buckets, float min, float max);

    RUNTIME_API double ComputeEntropy(std::span<uint64_t const> histogram, uint64_t total);

    RUNTIME_API double ComputeEntropy(std::span<uint64_t const> histogram);
}
