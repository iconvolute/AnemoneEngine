#pragma once
#include "AnemoneRandom/Generator.hxx"
#include "AnemoneDiagnostics/Debug.hxx"
#include "AnemoneMath/Functions.hxx"
#include "AnemoneMath/Packed.hxx"
#include "AnemoneBase/Float.hxx"

namespace Anemone
{
    template <typename T>
    struct UniformDistribution;

    template <>
    struct UniformDistribution<uint64_t> final
    {
        template <typename G>
        [[nodiscard]] static uint64_t operator()(G& generator)
        {
            return generator.Next();
        }

        template <typename G>
        [[nodiscard]] static uint64_t operator()(G& generator, uint64_t max)
        {
            return operator()(generator, 0, max);
        }

        template <typename G>
        [[nodiscard]] static uint64_t operator()(G& generator, uint64_t min, uint64_t max)
        {
            AE_ASSERT(min <= max);

            uint64_t const range = max - min;

            constexpr uint64_t MaxValue = std::numeric_limits<uint64_t>::max();

            if (range == MaxValue)
            {
                return operator()(generator);
            }

            uint64_t const span = range + 1u;
            uint64_t cutoff = MaxValue - (MaxValue % span);

            uint64_t result;

            do
            {
                result = operator()(generator);
            } while (result >= cutoff);

            return min + (result % span);
        }
    };

    template <>
    struct UniformDistribution<uint32_t> final
    {
        template <typename G>
        [[nodiscard]] static uint32_t operator()(G& generator)
        {
            uint64_t const sample = generator.Next();
            return static_cast<uint32_t>(sample >> 32u);
        }

        template <typename G>
        [[nodiscard]] static uint32_t operator()(G& generator, uint32_t max)
        {
            return operator()(generator, 0, max);
        }

        template <typename G>
        [[nodiscard]] static uint32_t operator()(G& generator, uint32_t min, uint32_t max)
        {
            AE_ASSERT(min <= max);

            uint32_t const range = max - min;

            constexpr uint32_t MaxValue = std::numeric_limits<uint32_t>::max();

            if (range == MaxValue)
            {
                return operator()(generator);
            }

            uint32_t const span = range + 1u;
            uint32_t const cutoff = MaxValue - (MaxValue % span);

            uint32_t result;

            do
            {
                result = operator()(generator);
            } while (result >= cutoff);

            return min + (result % span);
        }
    };

    template <>
    struct UniformDistribution<int64_t> final
    {
        template <typename G>
        [[nodiscard]] static int64_t operator()(G& generator)
        {
            while (true)
            {
                int64_t const sample = static_cast<int64_t>(generator.Next() >> 1u);

                if (sample != std::numeric_limits<int64_t>::max())
                {
                    return sample;
                }
            }
        }

        template <typename G>
        [[nodiscard]] static int64_t operator()(G& generator, int64_t max)
        {
            AE_ASSERT(max >= 0);

            return operator()(generator, 0, max);
        }

        template <typename G>
        [[nodiscard]] static int64_t operator()(G& generator, int64_t min, int64_t max)
        {
            AE_ASSERT(min <= max);

            uint64_t const exclusive = static_cast<uint64_t>(max - min);

            if (exclusive == static_cast<uint64_t>(std::numeric_limits<int64_t>::max()))
            {
                return operator()(generator) + min;
            }

            if (exclusive > 1)
            {
                return static_cast<int64_t>(UniformDistribution<uint64_t>{}(generator, exclusive - 1)) + min;
            }

            AE_ASSERT((min == max) or ((min + 1) == max));
            return min;
        }
    };

    template <>
    struct UniformDistribution<int32_t> final
    {
        template <typename G>
        [[nodiscard]] static int32_t operator()(G& generator)
        {
            while (true)
            {
                int32_t const sample = static_cast<int32_t>(generator.Next() >> 33u);

                if (sample != std::numeric_limits<int32_t>::max())
                {
                    return sample;
                }
            }
        }

        template <typename G>
        [[nodiscard]] static int32_t operator()(G& generator, int32_t max)
        {
            AE_ASSERT(max >= 0);

            return operator()(generator, 0, max);
        }

        template <typename G>
        [[nodiscard]] static int32_t operator()(G& generator, int32_t min, int32_t max)
        {
            AE_ASSERT(min <= max);

            uint32_t const exclusive = static_cast<uint32_t>(max - min);

            if (exclusive == static_cast<uint32_t>(std::numeric_limits<int32_t>::max()))
            {
                return operator()(generator) + min;
            }

            if (exclusive > 1)
            {
                return static_cast<int32_t>(UniformDistribution<uint32_t>{}(generator, exclusive - 1)) + min;
            }

            AE_ASSERT((min == max) or ((min + 1) == max));
            return min;
        }
    };

    template <>
    struct UniformDistribution<double> final
    {
        template <typename G>
        [[nodiscard]] static double operator()(G& generator)
        {
            uint64_t const sample = generator.Next();
            return Float64::Compose(0, sample) - 1.0;
        }

        template <typename G>
        [[nodiscard]] static double operator()(G& generator, double max)
        {
            double const sample = operator()(generator);
            double const result = sample * max;
            return result;
        }

        template <typename G>
        [[nodiscard]] static double operator()(G& generator, double min, double max)
        {
            double const sample = operator()(generator);
            double const scale = (max - min);
            double const result = (sample * scale) + min;
            return result;
        }
    };

    template <>
    struct UniformDistribution<float> final
    {
        template <typename G>
        [[nodiscard]] static float operator()(G& generator)
        {
            uint64_t const sample = generator.Next();
            return Float32::Compose(0, sample >> 32u) - 1.0f;
        }

        template <typename G>
        [[nodiscard]] static float operator()(G& generator, float max)
        {
            float const sample = operator()(generator);
            float const result = sample * max;
            return result;
        }

        template <typename G>
        [[nodiscard]] static float operator()(G& generator, float min, float max)
        {
            float const sample = operator()(generator);
            float const scale = (max - min);
            float const result = (sample * scale) + min;
            return result;
        }
    };

    template <>
    struct UniformDistribution<Packed::Vector4D> final
    {
        template <typename G>
        [[nodiscard]] static Packed::Vector4D operator()(G& generator)
        {
            UniformDistribution<double> uniform{};

            double const x = uniform(generator);
            double const y = uniform(generator);
            double const z = uniform(generator);
            double const w = uniform(generator);

            return {x, y, z, w};
        }
    };

    template <>
    struct UniformDistribution<Packed::Vector3D> final
    {
        template <typename G>
        [[nodiscard]] static Packed::Vector3D operator()(G& generator)
        {
            UniformDistribution<double> uniform{};

            double const x = uniform(generator);
            double const y = uniform(generator);
            double const z = uniform(generator);

            return {x, y, z};
        }
    };

    template <>
    struct UniformDistribution<Packed::Vector2D> final
    {
        template <typename G>
        [[nodiscard]] static Packed::Vector2D operator()(G& generator)
        {
            UniformDistribution<double> uniform{};

            double const x = uniform(generator);
            double const y = uniform(generator);

            return {x, y};
        }
    };

    template <>
    struct UniformDistribution<Packed::Vector4F> final
    {
        template <typename G>
        [[nodiscard]] static Packed::Vector4F operator()(G& generator)
        {
            UniformDistribution<float> uniform{};

            float const x = uniform(generator);
            float const y = uniform(generator);
            float const z = uniform(generator);
            float const w = uniform(generator);

            return {x, y, z, w};
        }
    };

    template <>
    struct UniformDistribution<Packed::Vector3F> final
    {
        template <typename G>
        [[nodiscard]] static Packed::Vector3F operator()(G& generator)
        {
            UniformDistribution<float> uniform{};

            float const x = uniform(generator);
            float const y = uniform(generator);
            float const z = uniform(generator);

            return {x, y, z};
        }
    };

    template <>
    struct UniformDistribution<Packed::Vector2F> final
    {
        template <typename G>
        [[nodiscard]] static Packed::Vector2F operator()(G& generator)
        {
            UniformDistribution<float> uniform{};

            float const x = uniform(generator);
            float const y = uniform(generator);

            return {x, y};
        }
    };
}

namespace Anemone
{
    template <typename T>
    struct NormalDistribution;

    template <>
    struct NormalDistribution<double> final
    {
        template <typename G>
        [[nodiscard]] static double operator()(G& generator)
        {
            UniformDistribution<double> uniform{};

            double const u1 = uniform(generator);
            double const u2 = uniform(generator);

            double const r = Sqrt(-2.0 * Log(u1));
            double const theta = Pi2<double> * u2;

            return r * Sin(theta);
        }

        template <typename G>
        [[nodiscard]] static double operator()(G& generator, double mean, double stddev)
        {
            return mean + operator()(generator) * stddev;
        }
    };

    template <>
    struct NormalDistribution<float> final
    {
        template <typename G>
        [[nodiscard]] static float operator()(G& generator)
        {
            UniformDistribution<float> uniform{};

            float const u1 = uniform(generator);
            float const u2 = uniform(generator);

            float const r = Sqrt(-2.0f * Log(u1));
            float const theta = Pi2<float> * u2;

            return r * Sin(theta);
        }

        template <typename G>
        [[nodiscard]] static float operator()(G& generator, float mean, float stddev)
        {
            return mean + operator()(generator) * stddev;
        }
    };
}

namespace Anemone
{
    template <typename T>
    struct ExponentialDistribution;

    template <>
    struct ExponentialDistribution<double> final
    {
        template <typename G>
        [[nodiscard]] static double operator()(G& generator)
        {
            UniformDistribution<double> uniform{};
            return -Log(uniform(generator));
        }

        template <typename G>
        [[nodiscard]] static double operator()(G& generator, double mean)
        {
            return mean * operator()(generator);
        }
    };

    template <>
    struct ExponentialDistribution<float> final
    {
        template <typename G>
        [[nodiscard]] static float operator()(G& generator)
        {
            UniformDistribution<float> uniform{};
            return -Log(uniform(generator));
        }

        template <typename G>
        [[nodiscard]] static float operator()(G& generator, float mean)
        {
            return mean * operator()(generator);
        }
    };
}

namespace Anemone
{
    template <typename T>
    struct LaplaceDistribution;

    template <>
    struct LaplaceDistribution<double> final
    {
        template <typename G>
        [[nodiscard]] static double operator()(G& generator, double mean, double scale)
        {
            double const variable = UniformDistribution<double>{}(generator);

            if (variable < 0.5)
            {
                return mean + scale * Log(2.0 * variable);
            }

            return mean - scale * Log(2.0 * (1.0 - variable));
        }
    };

    template <>
    struct LaplaceDistribution<float> final
    {
        template <typename G>
        [[nodiscard]] static float operator()(G& generator, float mean, float scale)
        {
            float const variable = UniformDistribution<float>{}(generator);

            if (variable < 0.5f)
            {
                return mean + scale * Log(2.0f * variable);
            }

            return mean - scale * Log(2.0f * (1.0f - variable));
        }
    };
}

namespace Anemone
{
    template <typename T>
    struct UniformInsideUnitSphereDistribution;

    template <>
    struct UniformInsideUnitSphereDistribution<double> final
    {
        template <typename G>
        [[nodiscard]] static Packed::Vector3D operator()(G& generator)
        {
            UniformDistribution<double> uniform{};

            double const theta = uniform(generator, Pi2<double>);
            double const v = uniform(generator, -1.0, 1.0);
            double const phi = Acos(v);
            double const r = Power(uniform(generator), 1.0 / 3.0);

            auto [sinPhi, cosPhi] = SinCos(phi);
            auto [sinTheta, cosTheta] = SinCos(theta);

            return {
                r * sinPhi * cosTheta,
                r * sinPhi * sinTheta,
                r * cosPhi,
            };
        }
    };

    template <>
    struct UniformInsideUnitSphereDistribution<float> final
    {
        template <typename G>
        [[nodiscard]] static Packed::Vector3F operator()(G& generator)
        {
            UniformDistribution<float> uniform{};

            float const theta = uniform(generator, Pi2<float>);
            float const v = uniform(generator, -1.0f, 1.0f);
            float const phi = Acos(v);
            float const r = Power(uniform(generator), 1.0f / 3.0f);

            auto [sinPhi, cosPhi] = SinCos(phi);
            auto [sinTheta, cosTheta] = SinCos(theta);

            return {
                r * sinPhi * cosTheta,
                r * sinPhi * sinTheta,
                r * cosPhi,
            };
        }
    };
}

namespace Anemone
{
    template <typename T>
    struct UniformInsideUnitCircleDistribution;

    template <>
    struct UniformInsideUnitCircleDistribution<double> final
    {
        template <typename G>
        [[nodiscard]] static Packed::Vector2D operator()(G& generator)
        {
            UniformDistribution<double> uniform{};

            double const r = uniform(generator);
            double const t = uniform(generator);

            double const radius = Sqrt(r);
            double const theta = t * Pi2<double>;

            auto [sinTheta, cosTheta] = SinCos(theta);

            return {
                radius * sinTheta,
                radius * cosTheta,
            };
        }
    };

    template <>
    struct UniformInsideUnitCircleDistribution<float> final
    {
        template <typename G>
        [[nodiscard]] static Packed::Vector2F operator()(G& generator)
        {
            UniformDistribution<float> uniform{};

            float const r = uniform(generator);
            float const t = uniform(generator);

            float const radius = Sqrt(r);
            float const theta = t * Pi2<float>;

            auto [sinTheta, cosTheta] = SinCos(theta);

            return {
                radius * sinTheta,
                radius * cosTheta,
            };
        }
    };
}

namespace Anemone
{
    template <typename T>
    struct UniformOnUnitSphereDistribution;

    template <>
    struct UniformOnUnitSphereDistribution<double> final
    {
        template <typename G>
        [[nodiscard]] static Packed::Vector3D operator()(G& generator)
        {
            NormalDistribution<double> normal{};

            double const x = normal(generator);
            double const y = normal(generator);
            double const z = normal(generator);

            double const invLength = ReciprocalSqrt((x * x) + (y * y) + (z * z));

            return {
                x * invLength,
                y * invLength,
                z * invLength,
            };
        }
    };

    template <>
    struct UniformOnUnitSphereDistribution<float> final
    {
        template <typename G>
        [[nodiscard]] static Packed::Vector3F operator()(G& generator)
        {
            NormalDistribution<float> normal{};

            float const x = normal(generator);
            float const y = normal(generator);
            float const z = normal(generator);

            float const invLength = ReciprocalSqrt((x * x) + (y * y) + (z * z));

            return {
                x * invLength,
                y * invLength,
                z * invLength,
            };
        }
    };
}

namespace Anemone
{
    template <typename T>
    struct UniformOnUnitCircleDistribution;

    template <>
    struct UniformOnUnitCircleDistribution<double> final
    {
        template <typename G>
        [[nodiscard]] static Packed::Vector2D operator()(G& generator)
        {
            UniformDistribution<double> uniform{};

            double const theta = uniform(generator, Pi2<double>);

            auto [sinTheta, cosTheta] = SinCos(theta);
            return {sinTheta, cosTheta};
        }
    };

    template <>
    struct UniformOnUnitCircleDistribution<float> final
    {
        template <typename G>
        [[nodiscard]] static Packed::Vector2F operator()(G& generator)
        {
            UniformDistribution<float> uniform{};

            float const theta = uniform(generator, Pi2<float>);

            auto [sinTheta, cosTheta] = SinCos(theta);
            return {sinTheta, cosTheta};
        }
    };
}

namespace Anemone
{
    template <typename T>
    struct UniformBarycentricDistribution;

    template <>
    struct UniformBarycentricDistribution<double> final
    {
        template <typename G>
        [[nodiscard]] static Packed::Vector3D operator()(G& generator)
        {
            UniformDistribution<double> uniform{};

            double const r1 = Sqrt(uniform(generator));
            double const r2 = uniform(generator);

            return {
                1.0 - r1,
                r1 * (1.0 - r2),
                r1 * r2,
            };
        }
    };

    template <>
    struct UniformBarycentricDistribution<float> final
    {
        template <typename G>
        [[nodiscard]] static Packed::Vector3F operator()(G& generator)
        {
            UniformDistribution<float> uniform{};

            float const r1 = Sqrt(uniform(generator));
            float const r2 = uniform(generator);

            return {
                1.0f - r1,
                r1 * (1.0f - r2),
                r1 * r2,
            };
        }
    };
}

namespace Anemone
{
    template <>
    struct UniformDistribution<Packed::ColorD> final
    {
        template <typename G>
        [[nodiscard]] static Packed::ColorD operator()(G& generator)
        {
            UniformDistribution<double> uniform{};

            double const r = uniform(generator);
            double const g = uniform(generator);
            double const b = uniform(generator);

            return {r, g, b, 1.0};
        }

        template <typename G>
        [[nodiscard]] static Packed::ColorD operator()(G& generator, double minBrightness, double maxBrightness)
        {
            UniformDistribution<double> uniform{};

            double const r = uniform(generator, minBrightness, maxBrightness);
            double const g = uniform(generator, minBrightness, maxBrightness);
            double const b = uniform(generator, minBrightness, maxBrightness);

            return {r, g, b, 1.0};
        }

        template <typename G>
        [[nodiscard]] static Packed::ColorD operator()(G& generator, double minBrightness, double maxBrightness, double alpha)
        {
            UniformDistribution<double> uniform{};

            double const r = uniform(generator, minBrightness, maxBrightness);
            double const g = uniform(generator, minBrightness, maxBrightness);
            double const b = uniform(generator, minBrightness, maxBrightness);

            return {r, g, b, alpha};
        }

        template <typename G>
        [[nodiscard]] static Packed::ColorD operator()(G& generator, double minBrightness, double maxBrightness, double minAlpha, double maxAlpha)
        {
            UniformDistribution<double> uniform{};

            double const r = uniform(generator, minBrightness, maxBrightness);
            double const g = uniform(generator, minBrightness, maxBrightness);
            double const b = uniform(generator, minBrightness, maxBrightness);
            double const a = uniform(generator, minAlpha, maxAlpha);

            return {r, g, b, a};
        }
    };

    template <>
    struct UniformDistribution<Packed::ColorF> final
    {
        template <typename G>
        [[nodiscard]] static Packed::ColorF operator()(G& generator)
        {
            UniformDistribution<float> uniform{};

            float const r = uniform(generator);
            float const g = uniform(generator);
            float const b = uniform(generator);

            return {r, g, b, 1.0f};
        }

        template <typename G>
        [[nodiscard]] static Packed::ColorF operator()(G& generator, float minBrightness, float maxBrightness)
        {
            UniformDistribution<float> uniform{};

            float const r = uniform(generator, minBrightness, maxBrightness);
            float const g = uniform(generator, minBrightness, maxBrightness);
            float const b = uniform(generator, minBrightness, maxBrightness);

            return {r, g, b, 1.0f};
        }

        template <typename G>
        [[nodiscard]] static Packed::ColorF operator()(G& generator, float minBrightness, float maxBrightness, float alpha)
        {
            UniformDistribution<float> uniform{};

            float const r = uniform(generator, minBrightness, maxBrightness);
            float const g = uniform(generator, minBrightness, maxBrightness);
            float const b = uniform(generator, minBrightness, maxBrightness);

            return {r, g, b, alpha};
        }

        template <typename G>
        [[nodiscard]] static Packed::ColorF operator()(G& generator, float minBrightness, float maxBrightness, float minAlpha, float maxAlpha)
        {
            UniformDistribution<float> uniform{};

            float const r = uniform(generator, minBrightness, maxBrightness);
            float const g = uniform(generator, minBrightness, maxBrightness);
            float const b = uniform(generator, minBrightness, maxBrightness);
            float const a = uniform(generator, minAlpha, maxAlpha);

            return {r, g, b, a};
        }
    };
}
