#include "AnemoneRuntime/Numerics/Random.hxx"
#include "AnemoneRuntime/Bitwise.hxx"
#include "AnemoneRuntime/Float.hxx"
#include "AnemoneRuntime/Numerics/Numbers.hxx"
#include "AnemoneRuntime/Numerics/Functions.hxx"
#include "AnemoneRuntime/Diagnostic/Assert.hxx"

#include <limits>
#include <algorithm>
#include <string_view>

namespace Anemone::Numerics::Private
{
    constexpr uint64_t SplitMix64(uint64_t& seed)
    {
        // Based on: https://github.com/svaarala/duktape/blob/master/misc/splitmix64.c

        seed += 0x9E3779B97F4A7C15u;
        uint64_t const z0 = seed;
        uint64_t const z1 = (z0 ^ (z0 >> 30)) * 0xBF58476D1CE4E5B9u;
        uint64_t const z2 = (z1 ^ (z1 >> 27)) * 0x94D049BB133111EBu;
        uint64_t const z3 = (z2 ^ (z2 >> 31));
        return z3;
    }
}

namespace Anemone::Numerics
{
    uint64_t Random::GenerateBits()
    {
        uint64_t s0 = this->_state[0];
        uint64_t s1 = this->_state[1];
        uint64_t s2 = this->_state[2];
        uint64_t s3 = this->_state[3];

        uint64_t const result = Bitwise::BitRotateLeft(s1 * 5, 7) * 9;
        uint64_t t = s1 << 17;

        s2 ^= s0;
        s3 ^= s1;
        s1 ^= s2;
        s0 ^= s3;

        s2 ^= t;
        s3 = Bitwise::BitRotateLeft(s3, 45);

        this->_state[0] = s0;
        this->_state[1] = s1;
        this->_state[2] = s2;
        this->_state[3] = s3;

        return result;
    }


    Random::Random()
        : _state{1, 2, 3, 4}
    {
    }

    Random::Random(uint64_t seed)
    {
        this->_state[0] = Private::SplitMix64(seed);
        this->_state[1] = Private::SplitMix64(seed);
        this->_state[2] = Private::SplitMix64(seed);
        this->_state[3] = Private::SplitMix64(seed);
    }

    void Random::Forward()
    {
        constexpr uint64_t indices[] = {0x76e15d3efefdcbbf, 0xc5004e441c522fb3, 0x77710069854ee241, 0x39109bb02acbe635};

        uint64_t s0 = 0;
        uint64_t s1 = 0;
        uint64_t s2 = 0;
        uint64_t s3 = 0;

        for (uint64_t const index : indices)
        {
            for (size_t b = 0; b < 64; ++b)
            {
                if ((index & (uint64_t{1} << b)) != 0)
                {
                    s0 ^= this->_state[0];
                    s1 ^= this->_state[1];
                    s2 ^= this->_state[2];
                    s3 ^= this->_state[3];
                }

                (void)this->GenerateBits();
            }
        }

        this->_state[0] = s0;
        this->_state[1] = s1;
        this->_state[2] = s2;
        this->_state[3] = s3;
    }
}

namespace Anemone::Numerics
{
    void Random::Sample(std::span<std::byte> buffer)
    {
#if true
        while (not buffer.empty())
        {
            size_t const count = std::min(buffer.size(), sizeof(uint64_t));
            uint64_t const sample = this->GenerateBits();
            std::memcpy(buffer.data(), &sample, count);
            buffer = buffer.subspan(count);
        }
#else
        while (buffer.size() >= sizeof(uint64_t))
        {
            uint64_t const bits = this->GenerateBits();
            std::memcpy(buffer.data(), &bits, sizeof(uint64_t));
            buffer = buffer.subspan(sizeof(uint64_t));
        }

        if (not buffer.empty())
        {
            uint64_t const bits = this->GenerateBits();
            std::memcpy(buffer.data(), &bits, buffer.size());
        }
#endif
    }

    uint64_t Random::NextUInt64()
    {
        return this->GenerateBits();
    }

    uint64_t Random::NextUInt64(uint64_t max)
    {
        return this->NextUInt64(0, max);
    }

    uint64_t Random::NextUInt64(uint64_t min, uint64_t max)
    {
        AE_ASSERT(min <= max);

        uint64_t const range = max - min;

        constexpr uint64_t MaxValue = std::numeric_limits<uint64_t>::max();

        if (range == MaxValue)
        {
            return this->NextUInt64();
        }

        uint64_t const span = range + 1;
        uint64_t const cutoff = MaxValue - (MaxValue % span);

        uint64_t result;

        do
        {
            result = this->NextUInt64();
        } while (result >= cutoff);

        return min + (result % span);
    }

    int64_t Random::NextInt64()
    {
        while (true)
        {
            int64_t const sample = static_cast<int64_t>(this->GenerateBits() >> 1u);

            if (sample != std::numeric_limits<int64_t>::max())
            {
                return sample;
            }
        }
    }

    // Random value from range [0..max)
    int64_t Random::NextInt64(int64_t max)
    {
        AE_ASSERT(max >= 0);

        return this->NextInt64(0, max);
    }

    int64_t Random::NextInt64(int64_t min, int64_t max)
    {
        AE_ASSERT(min <= max);

        uint64_t const exclusive = static_cast<uint64_t>(max - min);

        if (exclusive == static_cast<uint64_t>(std::numeric_limits<int64_t>::max()))
        {
            return this->NextInt64() + min;
        }

        if (exclusive > 1)
        {
            return static_cast<int64_t>(this->NextUInt64(exclusive - 1)) + min;
        }

        AE_ASSERT((min == max) || ((min + 1) == max));
        return min;
    }

    uint32_t Random::NextUInt32()
    {
        uint64_t const sample = this->GenerateBits();

        return static_cast<uint32_t>(sample >> 32u);
    }

    uint32_t Random::NextUInt32(uint32_t max)
    {
        return this->NextUInt32(0, max);
    }

    uint32_t Random::NextUInt32(uint32_t min, uint32_t max)
    {
        AE_ASSERT(min <= max);

        uint32_t const range = max - min;

        constexpr uint32_t MaxValue = std::numeric_limits<uint32_t>::max();

        if (range == MaxValue)
        {
            return this->NextUInt32();
        }

        uint32_t const span = range + 1;
        uint32_t const cutoff = MaxValue - (MaxValue % span);

        uint32_t result;

        do
        {
            result = this->NextUInt32();
        } while (result >= cutoff);

        return min + (result % span);
    }

    int32_t Random::NextInt32()
    {
        while (true)
        {
            int32_t const sample = static_cast<int32_t>(this->GenerateBits() >> 33u);

            if (sample != std::numeric_limits<int32_t>::max())
            {
                return static_cast<int32_t>(sample);
            }
        }
    }

    int32_t Random::NextInt32(int32_t max)
    {
        return this->NextInt32(0, max);
    }

    int32_t Random::NextInt32(int32_t min, int32_t max)
    {
        AE_ASSERT(min <= max);

        uint64_t const exclusive = static_cast<uint64_t>(max - min);

        if (exclusive == static_cast<uint64_t>(std::numeric_limits<int32_t>::max()))
        {
            return this->NextInt32() + min;
        }

        if (exclusive > 1)
        {
            return static_cast<int32_t>(this->NextUInt64(exclusive - 1)) + min;
        }

        AE_ASSERT((min == max) || ((min + 1) == max));
        return min;
    }

    float Random::NextFloat()
    {
        uint64_t const sample = this->GenerateBits();
        return Float32::Compose(0, sample >> 32) - 1.0f;
    }

    float Random::NextFloat(float max)
    {
        float const sample = this->NextFloat();
        float const result = sample * max;
        return result;
    }

    float Random::NextFloat(float min, float max)
    {
        float const sample = this->NextFloat();
        float const scale = (max - min);
        float const result = (sample * scale) + min;
        return result;
    }

    double Random::NextDouble()
    {
        uint64_t const sample = this->GenerateBits();
        return Float64::Compose(0, sample) - 1.0;
    }

    double Random::NextDouble(double max)
    {
        double const sample = this->NextDouble();
        double const result = sample * max;
        return result;
    }

    double Random::NextDouble(double min, double max)
    {
        double const sample = this->NextDouble();
        double const scale = (max - min);
        double const result = (sample * scale) + min;
        return result;
    }

    float Random::NextLaplace(float mean, float scale)
    {
        float const variable = this->NextFloat();

        if (variable < 0.5f)
        {
            return mean + scale * log(2.0f * variable);
        }

        return mean - scale * log(2.0f * (1.0f - variable));
    }

    float Random::NextNormal()
    {
        float const u1 = this->NextFloat();
        float const u2 = this->NextFloat();

        float const r = sqrt(-2.0f * log(u1));
        float const theta = Pi2<float> * u2;
        return r * sin(theta);
    }

    float Random::NextNormal(float mean, float stddev)
    {
        return mean + this->NextNormal() * stddev;
    }
}

namespace Anemone::Numerics::Private
{
    inline constexpr std::string_view g_Base32Letters = "0123456789abcdefghjknpqrstuvwxyz";

    static_assert(g_Base32Letters.size() == 32);
}

namespace Anemone::Numerics
{
    void GenerateReadableString(Random& generator, std::span<char> buffer)
    {
        while (not buffer.empty())
        {
            size_t const count = buffer.size() % sizeof(uint64_t);

            std::array<uint8_t, 8> const sample = std::bit_cast<std::array<uint8_t, 8>>(generator.NextUInt64());

            for (size_t i = 0; i < count; ++i)
            {
                buffer[i] = Private::g_Base32Letters[sample[i] & 0x1F];
            }

            buffer = buffer.subspan(count);
        }
    }

    Float2 NextFloat2(Random& generator)
    {
        std::array<uint32_t, 2> const sample = std::bit_cast<std::array<uint32_t, 2>>(generator.NextUInt64());

        float const x = Float32::Compose(0, sample[0]) - 1.0f;
        float const y = Float32::Compose(0, sample[1]) - 1.0f;

        return {x, y};
    }

    Float3 InsideUnitSphere(Random& generator)
    {
        float const theta = generator.NextFloat(Pi2<float>);
        float const v = generator.NextFloat(-1.0f, 1.0f);
        float const phi = Acos(v);
        float const r = Power(generator.NextFloat(), 1.0f / 3.0f);

        auto [sin_phi, cos_phi] = SinCos(phi);
        auto [sin_theta, cos_theta] = SinCos(theta);

        return {
            r * sin_phi * cos_theta,
            r * sin_phi * sin_theta,
            r * cos_phi,
        };
    }

    Float3 OnUnitSphere(Random& generator)
    {
        float const x = generator.NextNormal();
        float const y = generator.NextNormal();
        float const z = generator.NextNormal();

        float const invLength = ReciprocalSqrt((x * x) + (y * y) + (z * z));

        return {
            x * invLength,
            y * invLength,
            z * invLength,
        };
    }

    Float3 InsideUnitCircle(Random& generator)
    {
        auto [r, t] = NextFloat2(generator);

        float const radius = Sqrt(r);
        float const theta = t * Pi2<float>;

        auto [sinTheta, cosTheta] = SinCos(theta);

        return {
            radius * sinTheta,
            radius * cosTheta,
        };
    }

    Float3 OnUnitCircle(Random& generator)
    {
        float const theta = generator.NextFloat(Pi2<float>);

        auto [sinTheta, cosTheta] = SinCos(theta);
        return {sinTheta, cosTheta};
    }
}

namespace Anemone::Numerics
{
    void ComputeHistogram(std::vector<uint64_t>& histogram, std::span<uint8_t const> data)
    {
        histogram.clear();
        histogram.resize(256, 0);

        for (uint8_t const value : data)
        {
            histogram[value] += 1;
        }
    }

    void ComputeHistogram(std::vector<uint64_t>& histogram, std::span<float const> data, size_t buckets, float min, float max)
    {
        if (buckets >= 2)
        {
            histogram.clear();
            histogram.resize(buckets, 0);

            float const range = max - min;
            float const buckets_range = static_cast<float>(buckets);

            for (float const value : data)
            {
                if ((min <= value) && (value <= max))
                {
                    size_t const index = static_cast<size_t>((value - min) / range * buckets_range);

                    histogram[index] += 1;
                }
            }
        }
    }

    double ComputeEntropy(std::span<uint64_t const> histogram, uint64_t total)
    {
        double result = 0.0;
        double length = static_cast<double>(total);

        for (uint64_t const value : histogram)
        {
            if (value != 0)
            {
                double const probability = static_cast<double>(value) / length;
                result -= probability * (std::log(probability) / std::log(2.0));
            }
        }

        return result;
    }

    double ComputeEntropy(std::span<uint64_t const> histogram)
    {
        uint64_t total = 0;

        for (uint64_t const value : histogram)
        {
            total += value;
        }

        return ComputeEntropy(histogram, total);
    }
}
