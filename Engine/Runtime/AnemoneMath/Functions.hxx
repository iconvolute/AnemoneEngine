#pragma once
#include "AnemoneInterop/Headers.hxx"
#include "AnemoneMath/Numbers.hxx"

#include <cfloat>
#include <type_traits>
#include <limits>
#include <cmath>
#include <concepts>

#include "Packed.hxx"

#if ANEMONE_FEATURE_AVX
#include <immintrin.h>
#endif

#if ANEMONE_ARCHITECTURE_ARM64

namespace Anemone::Math::Detail
{
    //    return vrecpes_f32(value);
    //    return vrecped_f64(value);
    //    return vrsqrtes_f32(value);
    //    return vrsqrted_f64(value);
}

#endif

#if ANEMONE_ARCHITECTURE_X64

namespace Anemone::Math::Detail
{
    // = sqrt(x)
    inline float AvxSquareRoot(float x)
    {
        float result;
        _mm_store_ss(&result, _mm_sqrt_ss(_mm_set_ss(x)));
        return result;
    }

    // = sqrt(x)
    inline double AvxSquareRoot(double x)
    {
        __m128d const vx = _mm_set_sd(x);
        double result;
        _mm_store_sd(&result, _mm_sqrt_sd(vx, vx));
        return result;
    }

    // = rcp(rsqrt(x)) ~= sqrt(x)
    inline float AvxSquareRootFast(float x)
    {
        float result;
        _mm_store_ss(&result, _mm_rcp_ss(_mm_rsqrt_ss(_mm_set_ss(x))));
        return result;
    }

    // = rcp(x)
    inline float AvxReciprocal(float x)
    {
        float result;
        _mm_store_ss(&result, _mm_rcp_ss(_mm_set_ss(x)));
        return result;
    }

    // = 1 / sqrt(x)
    inline float AvxReciprocalSquareRoot(float x)
    {
        float result;
        _mm_store_ss(&result, _mm_div_ps(_mm_set_ss(1.0), _mm_sqrt_ss(_mm_set_ss(x))));
        return result;
    }

    // = 1 / sqrt(x)
    inline double AvxReciprocalSquareRoot(double x)
    {
        __m128d const vx = _mm_set_sd(x);
        double result;
        _mm_store_sd(&result, _mm_div_pd(_mm_set_sd(1.0), _mm_sqrt_sd(vx, vx)));
        return result;
    }

    // = rcp(sqrt(x))
    inline float AvxReciprocalSquareRoot2(float x)
    {
        float result;
        _mm_store_ss(&result, _mm_rcp_ss(_mm_sqrt_ss(_mm_set_ss(x))));
        return result;
    }

    // = rsqrt(x)
    inline float AvxReciprocalSquareRootFast(float x)
    {
        float result;
        _mm_store_ss(&result, _mm_rsqrt_ss(_mm_set_ss(x)));
        return result;
    }
}

#endif

namespace Anemone::Math::Detail
{
    inline constexpr int32_t UlpToleranceFloat = 4;
    inline constexpr int64_t UlpToleranceDouble = 16;
}

namespace Anemone::Math
{
    template <typename T>
    constexpr T Min(T left, T right)
        requires(std::is_arithmetic_v<T>)
    {
        return (left < right) ? left : right;
    }

    template <typename T>
    constexpr T Min(T v1, T v2, T v3)
        requires(std::is_arithmetic_v<T>)
    {
        return Min(Min(v1, v2), v3);
    }

    template <typename T>
    constexpr T Min(T v1, T v2, T v3, T v4)
        requires(std::is_arithmetic_v<T>)
    {
        return Min(Min(v1, v2), Min(v3, v4));
    }

    template <typename T>
    constexpr T Max(T left, T right)
        requires(std::is_arithmetic_v<T>)
    {
        return (left < right) ? right : left;
    }

    template <typename T>
    constexpr T Max(T v1, T v2, T v3)
        requires(std::is_arithmetic_v<T>)
    {
        return Max(Max(v1, v2), v3);
    }

    template <typename T>
    constexpr T Max(T v1, T v2, T v3, T v4)
        requires(std::is_arithmetic_v<T>)
    {
        return Max(Max(v1, v2), Max(v3, v4));
    }

    template <typename T>
    constexpr size_t MinIndex(T x, T y)
        requires(std::is_arithmetic_v<T>)
    {
        return (x < y) ? 0 : 1;
    }

    template <typename T>
    constexpr size_t MaxIndex(T x, T y)
        requires(std::is_arithmetic_v<T>)
    {
        return (x < y) ? 1 : 0;
    }

    template <typename T>
    constexpr T Abs(T x)
        requires(std::is_arithmetic_v<T>)
    {
        return std::abs(x);
    }

    template <typename T>
    constexpr T Square(T x)
        requires(std::is_arithmetic_v<T>)
    {
        return x * x;
    }

    template <typename T>
    constexpr T SignedSquare(T x)
        requires(std::is_arithmetic_v<T>)
    {
        return x * Abs(x);
    }

    template <typename T>
    constexpr T Cube(T x)
        requires(std::is_arithmetic_v<T>)
    {
        return x * x * x;
    }

    template <typename T>
    constexpr T Sign(T x)
        requires(std::is_arithmetic_v<T>)
    {
        if (x > T(0))
        {
            return T(1);
        }

        if (x < T(0))
        {
            return T(-1);
        }

        return T(0);
    }

    template <typename T>
    constexpr T Clamp(T x, T lower, T upper)
        requires(std::is_arithmetic_v<T>)
    {
        if (x > upper)
        {
            return upper;
        }

        if (x < lower)
        {
            return lower;
        }

        return x;
    }

    template <typename T>
    constexpr T ClampRoll(T x, T min, T max)
        requires(std::is_arithmetic_v<T>)
    {
        if (x > max)
        {
            return min;
        }

        if (x < min)
        {
            return max;
        }

        return x;
    }

    template <typename T>
    constexpr T Saturate(T x)
        requires(std::is_arithmetic_v<T>)
    {
        if (x > T(1))
        {
            return T(1);
        }

        if (x < T(0))
        {
            return T(0);
        }

        return x;
    }

    template <typename T>
    T Reciprocal(T x)
        requires(std::is_floating_point_v<T>)
    {
        return T(1) / x;
    }

    template <typename T>
    constexpr T MultiplyAdd(T a, T b, T c)
        requires(std::is_floating_point_v<T>)
    {
        return (a * b) + c;
    }

    template <typename T>
    constexpr T MultiplySubtract(T a, T b, T c)
        requires(std::is_floating_point_v<T>)
    {
        return (a * b) - c;
    }

    template <typename T>
    constexpr T NegateMultiplyAdd(T a, T b, T c)
        requires(std::is_floating_point_v<T>)
    {
        return -(a * b) + c;
    }

    template <typename T>
    constexpr T NegateMultiplySubtract(T a, T b, T c)
        requires(std::is_floating_point_v<T>)
    {
        return -(a * b) - c;
    }

    template <typename T>
    constexpr T Sqrt(T x)
        requires(std::is_floating_point_v<T>)
    {
        return std::sqrt(x);
    }

    template <typename T>
    T SqrtEst(T x)
        requires(std::is_floating_point_v<T>)
    {
        return std::sqrt(x);
    }

    template <typename T>
    T ReciprocalSqrt(T x)
        requires(std::is_floating_point_v<T>)
    {
        return T(1) / std::sqrt(x);
    }

    template <typename T>
    T ReciprocalSqrtEst(T x)
        requires(std::is_floating_point_v<T>)
    {
        return T(1) / std::sqrt(x);
    }

    template <typename T>
    T Ceil(T x)
        requires(std::is_floating_point_v<T>)
    {
        return std::ceil(x);
    }

    template <typename T>
    T Floor(T x)
        requires(std::is_floating_point_v<T>)
    {
        return std::floor(x);
    }

    template <typename T>
    T Truncate(T x)
        requires(std::is_floating_point_v<T>)
    {
        return std::trunc(x);
    }

    template <typename T>
    T Round(T x)
        requires(std::is_floating_point_v<T>)
    {
        return std::round(x);
    }

    template <typename T>
    T Fraction(T x)
        requires(std::is_floating_point_v<T>)
    {
        return x - std::floor(x);
    }

    template <typename T>
    T Remainder(T x, T y)
        requires(std::is_floating_point_v<T>)
    {
        return std::fmod(x, y);
    }

    template <typename T>
    T Power(T x, T y)
        requires(std::is_floating_point_v<T>)
    {
        return std::pow(x, y);
    }

    template <typename T>
    T Exp(T x)
        requires(std::is_floating_point_v<T>)
    {
        return std::exp(x);
    }

    template <typename T>
    T Exp2(T x)
        requires(std::is_floating_point_v<T>)
    {
        return std::exp2(x);
    }

    template <typename T>
    T Exp10(T x)
        requires(std::is_floating_point_v<T>)
    {
        return std::pow(T(10), x);
    }

    template <typename T>
    T Log(T x)
        requires(std::is_floating_point_v<T>)
    {
        return std::log(x);
    }

    template <typename T>
    T Log(T x, T base)
        requires(std::is_floating_point_v<T>)
    {
        return std::log(x) / std::log(base);
    }

    template <typename T>
    T Log2(T x)
        requires(std::is_floating_point_v<T>)
    {
        return std::log2(x);
    }

    template <typename T>
    T Log10(T x)
        requires(std::is_floating_point_v<T>)
    {
        return std::log10(x);
    }

    template <typename T>
    struct SinCosResult final
    {
        T Sin;
        T Cos;
    };

    template <typename T>
    T Sin(T x)
        requires(std::is_floating_point_v<T>)
    {
#if ANEMONE_FEATURE_FAST_TRIGONOMETRIC_FUNCTIONS

        if constexpr (std::is_same_v<T, float>)
        {
            float quotient = x * InvPi2<float>;

            if (x >= 0.0f)
            {
                quotient = static_cast<float>(static_cast<int32_t>(quotient + 0.5f));
            }
            else
            {
                quotient = static_cast<float>(static_cast<int32_t>(quotient - 0.5f));
            }

            float y = x - (quotient * Pi2<float>);

            if (y > PiOver2<float>)
            {
                y = Pi<float> - y;
            }
            else if (y < -PiOver2<float>)
            {
                y = -Pi<float> - y;
            }

            float const y2 = y * y;

            float resultSin = (Detail::F32_SinC5 * y2) + Detail::F32_SinC4;
            resultSin = (resultSin * y2) + Detail::F32_SinC3;
            resultSin = (resultSin * y2) + Detail::F32_SinC2;
            resultSin = (resultSin * y2) + Detail::F32_SinC1;
            resultSin = (resultSin * y2) + Detail::F32_SinC0;
            return resultSin * y;
        }
        else
        {
            return std::sin(x);
        }

#else

        return std::sin(x);

#endif
    }

    template <typename T>
    T Cos(T x)
        requires(std::is_floating_point_v<T>)
    {
#if ANEMONE_FEATURE_FAST_TRIGONOMETRIC_FUNCTIONS

        if constexpr (std::is_same_v<T, float>)
        {
            float quotient = x * InvPi2<float>;

            if (x >= 0.0f)
            {
                quotient = static_cast<float>(static_cast<int32_t>(quotient + 0.5f));
            }
            else
            {
                quotient = static_cast<float>(static_cast<int32_t>(quotient - 0.5f));
            }

            float y = x - (quotient * Pi2<float>);

            float sign;

            if (y > PiOver2<float>)
            {
                y = Pi<float> - y;
                sign = -1.0f;
            }
            else if (y < -PiOver2<float>)
            {
                y = -Pi<float> - y;
                sign = -1.0f;
            }
            else
            {
                sign = +1.0f;
            }

            float const y2 = y * y;

            float resultCos = (Detail::F32_CosC5 * y2) + Detail::F32_CosC4;
            resultCos = (resultCos * y2) + Detail::F32_CosC3;
            resultCos = (resultCos * y2) + Detail::F32_CosC2;
            resultCos = (resultCos * y2) + Detail::F32_CosC1;
            resultCos = (resultCos * y2) + Detail::F32_CosC0;
            return resultCos * sign;
        }
        else
        {
            return std::cos(x);
        }
#else

        return std::cos(x);

#endif
    }

    template <typename T>
    SinCosResult<T> SinCos(T x)
        requires(std::is_floating_point_v<T>)
    {
#if ANEMONE_FEATURE_FAST_TRIGONOMETRIC_FUNCTIONS

        if constexpr (std::is_same_v<T, float>)
        {
            float quotient = x * InvPi2<float>;

            if (x >= 0.0f)
            {
                quotient = static_cast<float>(static_cast<int32_t>(quotient + 0.5f));
            }
            else
            {
                quotient = static_cast<float>(static_cast<int32_t>(quotient - 0.5f));
            }

            float y = x - (quotient * Pi2<float>);

            float sign;

            if (y > PiOver2<float>)
            {
                y = Pi<float> - y;
                sign = -1.0f;
            }
            else if (y < -PiOver2<float>)
            {
                y = -Pi<float> - y;
                sign = -1.0f;
            }
            else
            {
                sign = +1.0f;
            }

            float const y2 = y * y;

            // 11-deg polynomial
            float resultSin = (Detail::F32_SinC5 * y2) + Detail::F32_SinC4;
            resultSin = (resultSin * y2) + Detail::F32_SinC3;
            resultSin = (resultSin * y2) + Detail::F32_SinC2;
            resultSin = (resultSin * y2) + Detail::F32_SinC1;
            resultSin = (resultSin * y2) + Detail::F32_SinC0;

            // 10-deg polynomial
            float resultCos = (Detail::F32_CosC5 * y2) + Detail::F32_CosC4;
            resultCos = (resultCos * y2) + Detail::F32_CosC3;
            resultCos = (resultCos * y2) + Detail::F32_CosC2;
            resultCos = (resultCos * y2) + Detail::F32_CosC1;
            resultCos = (resultCos * y2) + Detail::F32_CosC0;

            return {resultSin * y, resultCos * sign};
        }
        else
        {
            return {std::sin(x), std::cos(x)};
        }

#else

        return {std::sin(x), std::cos(x)};

#endif
    }

    template <typename T>
    T Tan(T x)
        requires(std::is_floating_point_v<T>)
    {
        return std::tan(x);
    }

    template <typename T>
    T Asin(T x)
        requires(std::is_floating_point_v<T>)
    {
        return std::asin(x);
    }

    template <typename T>
    T Acos(T x)
        requires(std::is_floating_point_v<T>)
    {
        return std::acos(x);
    }

    template <typename T>
    T Atan(T x)
        requires(std::is_floating_point_v<T>)
    {
        return std::atan(x);
    }

    template <typename T>
    T Atan2(T y, T x)
        requires(std::is_floating_point_v<T>)
    {
        return std::atan2(y, x);
    }

    template <typename T>
    T Repeat(T t, T length)
        requires(std::is_floating_point_v<T>)
    {
        return Clamp(t - Floor(t / length) * length, T(0), length);
    }

    template <typename T>
    T WrapOriginal(T value, T min, T max)
        requires(std::is_floating_point_v<T>)
    {
        static_assert(std::is_floating_point_v<T>);
        const auto range = max - min;
        const auto progress = value - min;
        return min + progress - (range * Floor<T>(progress / range));
    }

    template <typename T>
    T Wrap(T value, T min, T max)
        requires(std::is_floating_point_v<T>)
    {
        return min + Repeat(value - min, max - min);
    }
}

namespace Anemone::Math
{
    inline Packed::Vector3F CartesianToSpherical(Packed::Vector3F const& value)
    {
        auto const radius = Sqrt(
            value.X * value.X +
            value.Y * value.Y +
            value.Z * value.Z);

        auto const theta = Acos(
            value.Z / radius);

        auto const phi = Atan(
            value.Y / value.X);

        return {radius, theta, phi};
    }

    inline Packed::Vector3D CartesianToSpherical(Packed::Vector3D const& value)
    {
        auto const radius = Sqrt(
            value.X * value.X +
            value.Y * value.Y +
            value.Z * value.Z);

        auto const theta = Acos(
            value.Z / radius);

        auto const phi = Atan(
            value.Y / value.X);

        return {radius, theta, phi};
    }

    inline Packed::Vector3F SphericalToCartesian(Packed::Vector3F const& value)
    {
        auto const [sinTheta, cosTheta] = SinCos(value.Y);

        auto const [sinPhi, cosPhi] = SinCos(value.Z);

        auto const radius = value.X;

        auto const partial = radius * sinTheta;

        return {partial * cosPhi, partial * sinPhi, radius * cosTheta};
    }

    inline Packed::Vector3D SphericalToCartesian(Packed::Vector3D const& value)
    {
        auto const [sinTheta, cosTheta] = SinCos(value.Y);

        auto const [sinPhi, cosPhi] = SinCos(value.Z);

        auto const radius = value.X;

        auto const partial = radius * sinTheta;

        return {partial * cosPhi, partial * sinPhi, radius * cosTheta};
    }

    inline Packed::Vector3F CartesianToCylindrical(Packed::Vector3F const& value)
    {
        auto const radius = Sqrt(
            value.X * value.X +
            value.Y * value.Y);

        auto const angle = Atan(
            value.Y / value.X);

        auto const elevation = value.Z;

        return {radius, angle, elevation};
    }

    inline Packed::Vector3D CartesianToCylindrical(Packed::Vector3D const& value)
    {
        auto const radius = Sqrt(
            value.X * value.X +
            value.Y * value.Y);

        auto const angle = Atan(
            value.Y / value.X);

        auto const elevation = value.Z;

        return {radius, angle, elevation};
    }

    inline Packed::Vector3F CylindricalToCartesian(Packed::Vector3F const& value)
    {
        auto const [radius, angle, elevation] = value;

        auto const [sinAngle, cosAngle] = SinCos(angle);

        return {radius * cosAngle, radius * sinAngle, elevation};
    }

    inline Packed::Vector3D CylindricalToCartesian(Packed::Vector3D const& value)
    {
        auto const [radius, angle, elevation] = value;

        auto const [sinAngle, cosAngle] = SinCos(angle);

        return {radius * cosAngle, radius * sinAngle, elevation};
    }

    inline Packed::Vector2F PolarToCartesian(Packed::Vector2F const& value)
    {
        auto const [radius, angle] = value;

        auto const [sinAngle, cosAngle] = SinCos(angle);

        auto const x = radius * cosAngle;
        auto const y = radius * sinAngle;

        return {x, y};
    }

    inline Packed::Vector2D PolarToCartesian(Packed::Vector2D const& value)
    {
        auto const [radius, angle] = value;

        auto const [sinAngle, cosAngle] = SinCos(angle);

        auto const x = radius * cosAngle;
        auto const y = radius * sinAngle;

        return {x, y};
    }

    inline Packed::Vector2F CartesianToPolar(Packed::Vector2F const& value)
    {
        auto const radius = Sqrt(
            value.X * value.X +
            value.Y * value.Y);

        auto const angle = Atan2(value.Y, value.X);

        return {radius, angle};
    }

    inline Packed::Vector2D CartesianToPolar(Packed::Vector2D const& value)
    {
        auto const radius = Sqrt(
            value.X * value.X +
            value.Y * value.Y);

        auto const angle = Atan2(value.Y, value.X);

        return {radius, angle};
    }

}

namespace Anemone::Math
{
    template <typename T>
    constexpr T PreciseLerp(T a, T b, T t)
        requires(std::is_floating_point_v<T>)
    {
        return (a * (T(1) - t)) + (b * t);
    }

    template <typename T>
    constexpr T Lerp(T a, T b, T t)
        requires(std::is_floating_point_v<T>)
    {
        return a + ((b - a) * t);
    }

    template <typename T>
    constexpr T PreciseBilinearLerp(T a, T b, T c, T d, T u, T v)
        requires(std::is_floating_point_v<T>)
    {
        T const ab = PreciseLerp(a, b, u);
        T const cd = PreciseLerp(c, d, u);

        return PreciseLerp(ab, cd, v);
    }

    template <typename T>
    constexpr T BilinearLerp(T a, T b, T c, T d, T u, T v)
        requires(std::is_floating_point_v<T>)
    {
        T const ab = Lerp(a, b, u);
        T const cd = Lerp(c, d, u);

        return Lerp(ab, cd, v);
    }

    template <typename T>
    constexpr T Barycentric(T a, T b, T c, T u, T v)
        requires(std::is_floating_point_v<T>)
    {
        T const ba = b - a;
        T const ca = c - a;
        return a + (u * ba) + (v * ca);
    }

    template <typename T>
    constexpr T Unlerp(T value, T min, T max)
        requires(std::is_floating_point_v<T>)
    {
        return (value - min) / (max - min);
    }

    template <typename T>
    constexpr T Midpoint(T a, T b)
        requires(std::is_floating_point_v<T>)
    {
        return (a + b) * T(0.5);
    }

    template <typename T>
    constexpr T Map(T value, T sourceLower, T sourceUpper, T targetLower, T targetUpper)
        requires(std::is_floating_point_v<T>)
    {
        T const t = Unlerp(value, sourceLower, sourceUpper);
        return Lerp(targetLower, targetUpper, t);
    }

    template <typename T>
    constexpr T PreciseMap(T value, T sourceLower, T sourceUpper, T targetLower, T targetUpper)
        requires(std::is_floating_point_v<T>)
    {
        T const t = Unlerp(value, sourceLower, sourceUpper);
        return PreciseLerp(targetLower, targetUpper, t);
    }

    template <typename T>
    constexpr T BezierPosition(T p0, T p1, T p2, T t)
        requires(std::is_floating_point_v<T>)
    {
        // https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Quadratic_B%C3%A9zier_curves
        // Note: This uses the explicit form of the quadratic Bezier curve.

        // B(t) = (1-t)^2 * p0
        //      + 2 * (1-t) * t * p1
        //      + t^2 * p2

        float const nt = T(1) - t;

        float r = nt * nt * p0;
        r += T(2) * nt * t * p1;
        r += t * t * p2;
        return r;
    }

    template <typename T>
    constexpr T BezierTangent(T p0, T p1, T p2, T t)
        requires(std::is_floating_point_v<T>)
    {
        // https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Quadratic_B%C3%A9zier_curves
        // Note: This uses the explicit form of the quadratic Bezier curve.

        // B'(t) = 2 * (1-t) * (p1 - p0)
        //       + 2 * t * (p2 - p1)

        float r = T(2) * (T(1) - t) * (p1 - p0);
        r += T(2) * t * (p2 - p1);
        return r;
    }

    template <typename T>
    constexpr T BezierAcceleration(T p0, T p1, T p2, T t)
        requires(std::is_floating_point_v<T>)
    {
        // https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Quadratic_B%C3%A9zier_curves
        // Note: This uses the explicit form of the quadratic Bezier curve.

        // B''(t) = 2 * (p2 - 2 * p1 + p0)

        (void)t;

        return T(2) * (p2 - T(2) * p1 + p0);
    }

    template <typename T>
    constexpr T BezierPosition(T p0, T p1, T p2, T p3, T t)
        requires(std::is_floating_point_v<T>)
    {
        // https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Cubic_B%C3%A9zier_curves
        // Note: This uses the explicit form of the quadratic Bezier curve.

        // B(t) = (1-t)^3 * p0
        //      + 3 * (1-t)^2 * t * p1
        //      + 3 * (1-t) * t^2 * p2
        //      + t^3 * p3

        float const nt = T(1) - t;
        float const nt2 = nt * nt;
        float const nt3 = nt2 * nt;
        float const t2 = t * t;
        float const t3 = t2 * t;

        float r = nt3 * p0;
        r += T(3) * nt2 * t * p1;
        r += T(3) * nt * t2 * p2;
        r += t3 * p3;
        return r;
    }

    template <typename T>
    constexpr T BezierTangent(T p0, T p1, T p2, T p3, T t)
        requires(std::is_floating_point_v<T>)
    {
        // https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Cubic_B%C3%A9zier_curves
        // Note: This uses the explicit form of the quadratic Bezier curve.

        // B'(t) = 3 * (1-t)^2 * (p1 - p0)
        //       + 6 * (1-t) * t * (p2 - p1)
        //       + 3 * t^2 * (p3 - p2)

        float const nt = T(1) - t;

        float r = T(3) * nt * nt * (p1 - p0);
        r += T(6) * nt * t * (p2 - p1);
        r += T(3) * t * t * (p3 - p2);
        return r;
    }

    template <typename T>
    constexpr T BezierAcceleration(T p0, T p1, T p2, T p3, T t)
        requires(std::is_floating_point_v<T>)
    {
        // https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Cubic_B%C3%A9zier_curves
        // Note: This uses the explicit form of the quadratic Bezier curve.

        // B''(t) = 6 * (1-t) * (p2 - 2 * p1 + p0)
        //        + 6 * t * (p3 - 2 * p2 + p1)
        float const nt = T(1) - t;

        float r = T(6) * nt * (p2 - T(2) * p1 + p0);
        r += T(6) * t * (p3 - T(2) * p2 + p1);
        return r;
    }

    template <typename T>
    constexpr T Hermite(T p0, T m0, T p1, T m1, T t)
        requires(std::is_floating_point_v<T>)
    {
        // https://en.wikipedia.org/wiki/Cubic_Hermite_spline

        // H(t) = (2t^3 - 3t^2 + 1) * p0
        //      + (t^3 - 2t^2 + t) * m0
        //      + (-2t^3 + 3t^2) * p1
        //      + (t^3 - t^2) * m1

        T const t2 = t * t;
        T const t3 = t2 * t;

        T const cp0 = T(2) * t3 - T(3) * t2 + T(1);
        T const cm0 = t3 - T(2) * t2 + t;
        T const cp1 = -T(2) * t3 + T(3) * t2;
        T const cm1 = t3 - t2;

        T r = p0 * cp0;
        r += m0 * cm0;
        r += p1 * cp1;
        r += m1 * cm1;

        return r;
    }

    template <typename T>
    constexpr T CatmullRom(T p0, T p1, T p2, T p3, T t)
        requires(std::is_floating_point_v<T>)
    {
        // https://en.wikipedia.org/wiki/Cubic_Hermite_spline#Catmull%E2%80%93Rom_spline

        // C(t) = 0.5 * (
        //         ((-t^3 + 2t^2 - t) * p0) +
        //         ((3t^3 - 5t^2 + 2) * p1) +
        //         ((-3t^3 + 4t^2 + t) * p2) +
        //         ((t^3 - t^2) * p3)
        //      )

        T const t2 = t * t;
        T const t3 = t2 * t;

        T const c0 = -t3 + (T(2) * t2) - t;
        T const c1 = (T(3) * t3) - (T(5) * t2) + T(2);
        T const c2 = (T(-3) * t3) + (T(4) * t2) + t;
        T const c3 = t3 - t2;

        T r = c0 * p0;
        r += c1 * p1;
        r += c2 * p2;
        r += c3 * p3;

        return T(0.5) * r;
    }

    template <typename T>
    constexpr T Step(T v1, T v2)
        requires(std::is_arithmetic_v<T>)
    {
        return (v1 <= v2) ? T(0) : T(1);
    }

    template <typename T>
    constexpr T LinearStep(T min, T max, T t)
        requires(std::is_floating_point_v<T>)
    {
        if (t <= min)
        {
            return T(0);
        }

        if (t >= max)
        {
            return T(1);
        }

        return (t - min) / (max - min);
    }

    template <typename T>
    T Trapezoid(T a, T b, T c, T d, T t)
        requires(std::is_floating_point_v<T>)
    {
        if (t < a)
        {
            return T(0);
        }

        if (t < b)
        {
            return (t - a) / (b - a);
        }

        if (t < c)
        {
            return T(1);
        }

        if (t < d)
        {
            return T(1) - (t - c) / (d - c);
        }

        return T(0);
    }

    template <typename T>
    T Trapezoid(T a, T b, T c, T d, T t, T min, T max)
        requires(std::is_floating_point_v<T>)
    {
        return Trapezoid<T>(a, b, c, d, t) * (max - min) + min;
    }

    template <typename T>
    T LowPassFilter(T input, T last, T rc, T dt)
        requires(std::is_floating_point_v<T>)
    {
        T alpha = dt / (rc + dt);
        return ((T(1) - alpha) * last) + (alpha * input);
    }
}

namespace Anemone::Math
{
    template <typename T>
    float NearbyInt(float value)
        requires(std::is_floating_point_v<T>)
    {
        return std::nearbyint(value);
    }

    template <typename T>
    float NextToward(float value, float to)
        requires(std::is_floating_point_v<T>)
    {
        return std::nexttoward(value, static_cast<long double>(to));
    }

    template <typename T>
    float NextAfter(float value, float to)
        requires(std::is_floating_point_v<T>)
    {
        return std::nextafter(value, to);
    }

    template <typename T>
    bool IsNaN(T value)
        requires(std::is_floating_point_v<T>)
    {
        return std::isnan(value);
    }

    template <typename T>
    bool IsInfinite(T value)
        requires(std::is_floating_point_v<T>)
    {
        return std::isinf(value);
    }

    template <typename T>
    bool IsFinite(T value)
        requires(std::is_floating_point_v<T>)
    {
        return std::isfinite(value);
    }

    template <typename T>
    bool IsNormal(T value)
        requires(std::is_floating_point_v<T>)
    {
        return std::isnormal(value);
    }

    template <typename T>
    bool IsUnordered(T value1, T value2)
    {
        return std::isunordered(value1, value2);
    }

    template <typename T>
    bool IsLess(T value1, T value2)
        requires(std::is_floating_point_v<T>)
    {
        return std::isless(value1, value2);
    }

    template <typename T>
    bool IsLessEqual(T value1, T value2)
        requires(std::is_floating_point_v<T>)
    {
        return std::islessequal(value1, value2);
    }

    template <typename T>
    bool IsGreater(T value1, T value2)
        requires(std::is_floating_point_v<T>)
    {
        return std::isgreater(value1, value2);
    }

    template <typename T>
    bool IsGreaterEqual(T value1, T value2)
        requires(std::is_floating_point_v<T>)
    {
        return std::isgreaterequal(value1, value2);
    }

    template <typename T>
    bool IsLessGreater(T value1, T value2)
        requires(std::is_floating_point_v<T>)
    {
        return std::islessgreater(value1, value2);
    }

    template <typename T>
    bool IsNearZero(T value, T tolerance)
        requires(std::is_floating_point_v<T>)
    {
        return (-tolerance <= value) and (value <= tolerance);
    }

    template <typename T>
    constexpr bool IsNearZero(T x)
        requires(std::is_floating_point_v<T>)
    {
        T const epsilon = std::numeric_limits<T>::epsilon();
        return (-epsilon <= x) and (x <= epsilon);
    }

    template <typename T>
    bool IsZero(T x)
        requires(std::is_floating_point_v<T>)
    {
        return x == T(0);
    }

    inline bool IsNearZeroPrecise(float x)
    {
        int32_t const bits = std::bit_cast<int32_t>(x);
        return Abs(bits) <= Detail::UlpToleranceFloat;
    }

    inline bool IsNearZeroPrecise(double x)
    {
        int64_t const bits = std::bit_cast<int64_t>(x);
        return Abs(bits) <= Detail::UlpToleranceDouble;
    }

    template <typename T>
    bool InRange(T value, T lower, T upper)
        requires(std::is_floating_point_v<T>)
    {
        return (lower <= value) and (value <= upper);
    }

    template <typename T>
    bool IsEqual(T value1, T value2)
        requires(std::is_floating_point_v<T>)
    {
        return value1 == value2;
    }

    template <typename T>
    bool IsNotEqual(T value1, T value2)
        requires(std::is_floating_point_v<T>)
    {
        return value1 != value2;
    }

    inline bool IsNearEqualPrecise(float a, float b)
    {
        // https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/

        if (float const diff = Abs(a - b); diff <= std::numeric_limits<float>::epsilon())
        {
            // a and b are close enough to be considered equal using floating point epsilon
            return true;
        }

        int32_t const bitsA = std::bit_cast<int32_t>(a);
        int32_t const bitsB = std::bit_cast<int32_t>(b);

        if ((bitsA < 0) != (bitsB < 0))
        {
            // Different signs means they are not close enough to be considered equal
            return false;
        }

        // FLT_EPSILON does not scale with the magnitude of the numbers being compared.
        // Use ULP (Units in the Last Place) to compare floating point numbers.
        return Abs(bitsA - bitsB) <= Detail::UlpToleranceFloat;
    }

    inline bool IsNearEqualPrecise(double a, double b)
    {
        // https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/

        if (double const diff = Abs(a - b); diff <= std::numeric_limits<double>::epsilon())
        {
            // a and b are close enough to be considered equal using floating point epsilon
            return true;
        }

        int64_t const bitsA = std::bit_cast<int64_t>(a);
        int64_t const bitsB = std::bit_cast<int64_t>(b);

        if ((bitsA < 0) != (bitsB < 0))
        {
            // Different signs means they are not close enough to be considered equal
            return false;
        }

        // DBL_EPSILON does not scale with the magnitude of the numbers being compared.
        // Use ULP (Units in the Last Place) to compare floating point numbers.
        return Abs(bitsA - bitsB) <= Detail::UlpToleranceDouble;
    }

    template <typename T>
    bool IsNearEqual(T x, T y, T epsilon)
        requires(std::is_floating_point_v<T>)
    {
        T const delta = x - y;
        return (-epsilon <= delta) and (delta <= epsilon);
    }

    template <typename T>
    bool IsNearEqual(T x, T y)
        requires(std::is_floating_point_v<T>)
    {
        return IsNearEqual(x, y, std::numeric_limits<T>::epsilon());
    }

    template <typename T>
    constexpr bool IsNearEqualSquared(T a, T b, T epsilon)
        requires(std::is_floating_point_v<T>)
    {
        T const delta = a - b;
        return (delta * delta) <= (epsilon * epsilon);
    }

    template <typename T>
    constexpr bool IsNearEqualSquared(T a, T b)
        requires(std::is_floating_point_v<T>)
    {
        return IsNearEqualSquared(a, b, std::numeric_limits<T>::epsilon());
    }
}

namespace Anemone::Math
{
    template <typename T>
    int QuadricEquation(T a, T b, T c, T& out_x1, T& out_x2)
        requires(std::is_floating_point_v<T>)
    {
        T delta = (b * b) - (T(4) * a * c);

        if (delta < T(0))
        {
            out_x1 = out_x2 = std::numeric_limits<T>::infinity();
            return 0;
        }

        if (IsZero<T>(delta))
        {
            out_x1 = out_x2 = (-b / (T(2) * a));
            return 1;
        }

        T denominator = T(1) / (T(2) * a);
        T delta_sqrt = Sqrt<T>(delta);
        out_x1 = (-b - delta_sqrt) * denominator;
        out_x2 = (-b + delta_sqrt) * denominator;
        return 2;
    }

    template <typename T>
    T SnapToGrid(T value, T grid_size)
        requires(std::is_floating_point_v<T>)
    {
        if (grid_size == T(0))
        {
            return value;
        }

        return Floor<T>((value + T(0.5) * grid_size) / grid_size) * grid_size;
    }

    template <typename T>
    T PingPong(T t, T length)
        requires(std::is_floating_point_v<T>)
    {
        t = Repeat(t, length * T(2));
        return length - Abs(t - length);
    }
}

namespace Anemone::Math
{
    constexpr uint32_t UNormToUInt(float value, size_t bits)
    {
        if (value <= 0.0f)
        {
            return 0;
        }

        if (value >= 1.0f)
        {
            return (1 << bits) - 1;
        }

        return static_cast<uint32_t>(value * static_cast<float>(1u << bits));
    }

    constexpr uint32_t SNormToUInt(float value, size_t bits)
    {
        return UNormToUInt((value + 1.0f) * 0.5f, bits);
    }

    constexpr float UIntToUNorm(uint32_t value, size_t bits)
    {
        return static_cast<float>(value) / static_cast<float>((1u << bits) - 1);
    }

    constexpr float UIntToSNorm(uint32_t value, size_t bits)
    {
        return UIntToUNorm(value, bits) * 2.0f - 1.0f;
    }

    template <typename T>
    T SrgbToLinear(T t)
        requires(std::is_floating_point_v<T>)
    {
        if (t <= T(0.04045))
        {
            return t / T(12.92);
        }

        return Power<T>((t * T(0.055)) / T(1.055), T(2.4));
    }

    template <typename T>
    T LinearToSrgb(T t)
        requires(std::is_floating_point_v<T>)
    {
        if (t < T(0.0031308))
        {
            return t * T(12.92);
        }

        return T(1.055) * Power<T>(t, T(1.0) / T(2.4)) - T(0.055);
    }
}

namespace Anemone::Math
{
    template <typename T>
    T UnwindRadians(T value)
        requires(std::is_floating_point_v<T>)
    {
        return Wrap(value, -Pi<T>, Pi<T>);
    }

    template <typename T>
    T UnwindDegrees(T value)
        requires(std::is_floating_point_v<T>)
    {
        return Wrap(value, -T(180), T(180));
    }

    template <typename T>
    T UnwindRevolutions(T value)
        requires(std::is_floating_point_v<T>)
    {
        return Repeat(value, T(1));
    }

    template <typename T>
    T AngleDifferenceDegrees(T a, T b)
        requires(std::is_floating_point_v<T>)
    {
        return UnwindDegrees(a - b);
    }

    template <typename T>
    T AngleDifferenceRadians(T a, T b)
        requires(std::is_floating_point_v<T>)
    {
        return UnwindRadians(a - b);
    }

    template <typename T>
    T AngleDifferenceRevolutions(T a, T b)
        requires(std::is_floating_point_v<T>)
    {
        return UnwindRevolutions(a - b);
    }

    template <typename T>
    constexpr T RevolutionsToDegrees(T value)
        requires(std::is_floating_point_v<T>)
    {
        return value * Detail::Factor_RevolutionsToDegrees<T>;
    }

    template <typename T>
    constexpr T RevolutionsToRadians(T value)
        requires(std::is_floating_point_v<T>)
    {
        return value * Detail::Factor_RevolutionsToRadians<T>;
    }

    template <typename T>
    constexpr T DegreesToRevolutions(T value)
        requires(std::is_floating_point_v<T>)
    {
        return value * Detail::Factor_DegreesToRevolutions<T>;
    }

    template <typename T>
    constexpr T RadiansToRevolutions(T value)
        requires(std::is_floating_point_v<T>)
    {
        return value * Detail::Factor_RadiansToRevolutions<T>;
    }

    template <typename T>
    constexpr T RadiansToDegrees(T value)
        requires(std::is_floating_point_v<T>)
    {
        return value * Detail::Factor_RadiansToDegrees<T>;
    }

    template <typename T>
    constexpr T DegreesToRadians(T value)
        requires(std::is_floating_point_v<T>)
    {
        return value * Detail::Factor_DegreesToRadians<T>;
    }

}

namespace Anemone::Math
{
    template <typename T>
    T MoveTowards(T current, T target, T maxDelta)
        requires(std::is_floating_point_v<T>)
    {
        T const delta = (target - current);
        if (Abs(delta) <= maxDelta)
        {
            return target;
        }

        return MultiplyAdd(Sign(delta), maxDelta, current);
    }

    template <typename T>
    T MoveTowardsAngle(T current, T target, T maxDelta)
        requires(std::is_floating_point_v<T>)
    {
        T const delta = AngleDifferenceRadians(current, target);

        if ((-maxDelta < delta) and (delta < maxDelta))
        {
            return target;
        }

        target = current + delta;

        return MoveTowards(current, target, maxDelta);
    }
}

namespace Anemone::Math
{
    template <typename T>
    T Gauss(T amplitude, T x, T y, T centerX, T centerY, T sigmaX, T sigmaY)
        requires(std::is_floating_point_v<T>)
    {
        T const cx = x - centerX;
        T const cy = y - centerY;

        T const componentX = cx * cx / (T(2) * sigmaX * sigmaX);
        T const componentY = cy * cy / (T(2) * sigmaY * sigmaY);
        return amplitude * Exp<T>(-(componentX + componentY));
    }

    template <typename T>
    T Bias(T value, T base)
        requires(std::is_floating_point_v<T>)
    {
        return Power<T>(value, -Log2<T>(base));
    }

    template <typename T>
    T Gain(T value, T gain)
        requires(std::is_floating_point_v<T>)
    {
        T const g = -Log2<T>(T(1) - gain);
        T const c = Power<T>(value, gain);
        return c / (c + Power<T>(T(1) - value, g));
    }

    template <typename T>
    T WaveSine(T time, T base, T amplitude, T frequency, T phase)
        requires(std::is_floating_point_v<T>)
    {
        T const offset = time + phase;
        T const angular = (offset * frequency) * Pi2<T>;
        T const sinAngular = Sin<T>(angular);
        T const sinOffset = sinAngular + T(1);
        T const phased = sinOffset * T(0.5) * amplitude;
        return base + phased;
    }

    template <typename T>
    T WaveSine(T time)
        requires(std::is_floating_point_v<T>)
    {
        return (Sin<T>(time * Pi2<T>) + T(1)) * T(0.5);
    }

    template <typename T>
    T WaveTriangle(T time, T base, T amplitude, T frequency, T phase)
        requires(std::is_floating_point_v<T>)
    {
        T const input = Fraction<T>((time + phase) * frequency);

        T output;

        if (input < T(0.25))
        {
            output = input * T(4);
        }
        else if (input < T(0.75))
        {
            output = T(1) - (input - T(0.25)) * T(4);
        }
        else
        {
            output = (input - T(0.75)) * T(4) - T(1);
        }

        return base + (output + T(1)) * T(0.5) * amplitude;
    }

    template <typename T>
    T WaveTriangle(T time)
        requires(std::is_floating_point_v<T>)
    {
        T const input = Fraction<T>(time);

        T output;

        if (input < T(0.25))
        {
            output = input * T(4);
        }
        else if (input < T(0.75))
        {
            output = T(1) - (input - T(0.25)) * T(4);
        }
        else
        {
            output = (input - T(0.75)) * T(4) - T(1);
        }

        return (output + T(1)) * T(0.5);
    }

    template <typename T>
    T WaveSquare(T time, T base, T amplitude, T frequency, T phase)
        requires(std::is_floating_point_v<T>)
    {
        T const t = (time + phase) * frequency;
        T const duty = (Fraction<T>(t) <= T(0.5)) ? T(1) : T(0);
        return base + (duty * amplitude);
    }

    template <typename T>
    T WaveSquare(T time)
        requires(std::is_floating_point_v<T>)
    {
        return (Fraction<T>(time) <= T(0.5)) ? T(1) : T(0);
    }

    template <typename T>
    T WaveSawtooth(T time, T base, T amplitude, T frequency, T phase)
        requires(std::is_floating_point_v<T>)
    {
        return base + Fraction<T>((time + phase) * frequency) * amplitude;
    }

    template <typename T>
    T WaveSawtooth(T time)
        requires(std::is_floating_point_v<T>)
    {
        return Fraction<T>(time);
    }

    template <typename T>
    T WaveInvSawtooth(T time, T base, T amplitude, T frequency, T phase)
        requires(std::is_floating_point_v<T>)
    {
        return base + (T(1) - Fraction<T>((time + phase) * frequency)) * amplitude;
    }

    template <typename T>
    T WaveInvSawtooth(T time)
        requires(std::is_floating_point_v<T>)
    {
        return T(1) - Fraction<T>(time);
    }

    template <typename T>
    T WavePulseWidth(T time, T duty, T base, T amplitude, T frequency, T phase)
        requires(std::is_floating_point_v<T>)
    {
        return base + ((Fraction<T>((time + phase) * frequency) <= duty) ? T(1) : T(0)) * amplitude;
    }

    template <typename T>
    T WavePulseWidth(T time, T duty)
        requires(std::is_floating_point_v<T>)
    {
        return (Fraction<T>(time) <= duty) ? T(1) : T(0);
    }
}

namespace Anemone::Math
{
    template <typename T>
    T SmoothDamp(T current, T target, T& currentVelocity, T smoothTime, T maxSpeed, T deltaTime)
        requires(std::is_floating_point_v<T>)
    {
        smoothTime = Max<T>(T(0.0001), smoothTime);
        T a = T(2) / smoothTime;
        T b = a * deltaTime;
        T c = T(1) / (T(1) + b + T(0.48) * b * b * T(0.235) * b * b * b);
        T d = current - target;
        T e = target;
        T f = maxSpeed * smoothTime;

        d = Clamp<T>(d, -f, f);
        target = current - d;
        T g = (currentVelocity + a * d) * deltaTime;
        currentVelocity = (currentVelocity - a * g) * c;
        T h = target + (d + g) * c;

        if ((e - current > T(0)) == (h > e))
        {
            h = e;
            currentVelocity = (h - e) / deltaTime;
        }

        return h;
    }

    template <typename T>
    T SmoothDampAngleRadians(T current, T target, T& currentVelocity, T smoothTime, T maxSpeed, T deltaTime)
        requires(std::is_floating_point_v<T>)
    {
        target = current + AngleDifferenceRadians<T>(current, target);
        return SmoothDamp<T>(current, target, currentVelocity, smoothTime, maxSpeed, deltaTime);
    }

    template <typename T>
    void SpringDamp(T& position, T& velocity, T target, T dampingRatio, T angularFrequency, T deltaTime)
        requires(std::is_floating_point_v<T>)
    {
        T const a = T(1) + T(2) * deltaTime * dampingRatio * angularFrequency;
        T const b = angularFrequency * angularFrequency;
        T const c = deltaTime * b;
        T const d = deltaTime * c;
        T const e = T(1) / (a + d);
        T const f = (a * position) + (deltaTime * velocity) + (d * target);
        T const g = velocity + c * (target - position);

        position = f * e;
        velocity = g * e;
    }
}

//
// ODE
//

namespace Anemone::Math
{
    template <typename T, typename Fn>
    constexpr void RungeKutta4Step(Fn& dydx, T& x, T& y, T dt)
    {
        T const k1 = dt * dydx(x, y);
        T const k2 = dt * dydx(x + (T(0.5) * dt), y + (T(0.5) * k1));
        T const k3 = dt * dydx(x + (T(0.5) * dt), y + (T(0.5) * k2));
        T const k4 = dt * dydx(x + dt, y + k3);

        y += (k1 + (T(2) * k2) + (T(2) * k3) + k4) / T(6);
        x += dt;
    }

    template <typename T, typename Fn>
    constexpr T RungeKutta4(Fn& dydx, T x0, T y0, T dt, size_t steps)
        requires(std::is_floating_point_v<T>)
    {
        T x = x0;
        T y = y0;

        for (size_t i = 0; i < steps; ++i)
        {
            RungeKutta4Step(dydx, x, y, dt);
        }

        return y;
    }
}
