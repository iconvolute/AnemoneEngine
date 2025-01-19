#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Types.hxx"

#include "AnemoneRuntime/Math/Types.hxx"

#include "AnemoneRuntime/Math/Numbers.hxx"
#include "AnemoneRuntime/Math/Functions.hxx"

#include <array>
#include <span>
#include <vector>

namespace Anemone::Math
{
    class [[nodiscard]] RUNTIME_API Random
    {
    private:
        std::array<uint64_t, 4> _state;

    public:
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

    template <typename T>
    struct UniformDistribution
    {
        T Min;
        T Max;

        [[nodiscard]] T operator()(Random& generator) const;
    };

    template <>
    anemone_noinline inline float UniformDistribution<float>::operator()(Random& generator) const
    {
        uint64_t const bits = generator.GenerateBits();
        float const sample = Float32::Compose(0, bits >> 32) - 1.0f;
        float const scale = (this->Max - this->Min);
        float const result = (sample * scale) + this->Min;
        return result;
    }

    template <>
    anemone_noinline inline double UniformDistribution<double>::operator()(Random& generator) const
    {
        uint64_t const bits = generator.GenerateBits();
        double const sample = Float64::Compose(0, bits) - 1.0;
        double const scale = (this->Max - this->Min);
        double const result = (sample * scale) + this->Min;
        return result;
    }

    [[nodiscard]] RUNTIME_API Float2 NextFloat2(Random& generator);

    [[nodiscard]] RUNTIME_API Float3 InsideUnitSphere(Random& generator);
    [[nodiscard]] RUNTIME_API Float3 OnUnitSphere(Random& generator);

    [[nodiscard]] RUNTIME_API Float3 InsideUnitCircle(Random& generator);
    [[nodiscard]] RUNTIME_API Float3 OnUnitCircle(Random& generator);

    [[nodiscard]] inline Math::Vector3F NextVector3F(Random& generator)
    {
        float const x = generator.NextFloat();
        float const y = generator.NextFloat();
        float const z = generator.NextFloat();
        return Math::Vector3F::Create(x, y, z);
    }

    [[nodiscard]] inline Math::Vector3F NextInsideUnitSphere(Random& generator)
    {
        float const theta = generator.NextFloat(Pi2<float>);
        float const v = generator.NextFloat(-1.0f, 1.0f);
        float const phi = Acos(v);
        float const r = Power(generator.NextFloat(), 1.0f / 3.0f);

        auto [sin_phi, cos_phi] = SinCos(phi);
        auto [sin_theta, cos_theta] = SinCos(theta);

        return Math::Vector3F::Create(
            r * sin_phi * cos_theta,
            r * sin_phi * sin_theta,
            r * cos_phi);
    }

    template <typename T>
    [[nodiscard]] T Next(Random& generator) = delete;

    template <>
    [[nodiscard]] inline Math::Packed::Vector2F Next<Math::Packed::Vector2F>(Random& generator)
    {
        float const x = generator.NextFloat();
        float const y = generator.NextFloat();

        return Math::Packed::Vector2F{x, y};
    }
}

namespace Anemone::Math
{
    RUNTIME_API void ComputeHistogram(std::vector<uint64_t>& histogram, std::span<uint8_t const> data);

    RUNTIME_API void ComputeHistogram(std::vector<uint64_t>& histogram, std::span<float const> data, size_t buckets, float min, float max);

    RUNTIME_API double ComputeEntropy(std::span<uint64_t const> histogram, uint64_t total);

    RUNTIME_API double ComputeEntropy(std::span<uint64_t const> histogram);
}
