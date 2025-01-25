#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#include <utility>

#if !ANEMONE_BUILD_DISABLE_SIMD && !ANEMONE_FEATURE_AVX && !ANEMONE_FEATURE_AVX2 && !ANEMONE_FEATURE_NEON && !ANEMONE_FEATURE_ASIMD
#error "No SIMD feature set"
#endif

// TODO: Verify these includes
#if ANEMONE_PLATFORM_WINDOWS

#if ANEMONE_FEATURE_AVX
#include <immintrin.h>
#endif

#if ANEMONE_FEATURE_AESNI
#include <wmmintrin.h>
#endif

#if ANEMONE_FEATURE_NEON
#include <arm64_neon.h>
#include <arm64intr.h>
#endif

#else

#if ANEMONE_FEATURE_AVX
#include <xmmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>
#include <tmmintrin.h>
#include <smmintrin.h>
#include <nmmintrin.h>
#include <immintrin.h>
#endif

#if ANEMONE_FEATURE_AESNI
#include <wmmintrin.h>
#endif

#if ANEMONE_FEATURE_NEON
#include <arm_neon.h>
#endif

#endif

#include "AnemoneRuntime/Math/Functions.hxx"
#include "AnemoneRuntime/Math/Numbers.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

namespace Anemone::Math::Detail
{
#if ANEMONE_BUILD_DISABLE_SIMD

    struct SimdVector4F final
    {
        float Inner[4];
    };

    struct SimdMask4F final
    {
        uint32_t Inner[4];
    };

    struct SimdMatrix4x4F final
    {
        SimdVector4F Inner[4];
    };

#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2

    using SimdVector4F = __m128;

    using SimdMask4F = __m128;

    struct SimdMatrix4x4F final
    {
        __m128 Inner[4];
    };

#elif ANEMONE_FEATURE_NEON

    using SimdVector4F = float32x4_t;

    using SimdMask4F = uint32x4_t;

    using SimdMatrix4x4F = float32x4x4_t;

#else
#error "Not implemented"
#endif
}

// Vector4F
namespace Anemone::Math::Detail
{
    SimdVector4F anemone_vectorcall Vector4F_LoadAlignedFloat4(float const* source);
    SimdVector4F anemone_vectorcall Vector4F_LoadAlignedFloat3(float const* source);
    SimdVector4F anemone_vectorcall Vector4F_LoadAlignedFloat2(float const* source);
    SimdVector4F anemone_vectorcall Vector4F_LoadAlignedFloat1(float const* source);
    SimdVector4F anemone_vectorcall Vector4F_LoadUnalignedFloat4(float const* source);
    SimdVector4F anemone_vectorcall Vector4F_LoadUnalignedFloat3(float const* source);
    SimdVector4F anemone_vectorcall Vector4F_LoadUnalignedFloat2(float const* source);
    SimdVector4F anemone_vectorcall Vector4F_LoadUnalignedFloat1(float const* source);

    void anemone_vectorcall Vector4F_StoreAlignedFloat4(float* destination, SimdVector4F source);
    void anemone_vectorcall Vector4F_StoreAlignedFloat3(float* destination, SimdVector4F source);
    void anemone_vectorcall Vector4F_StoreAlignedFloat2(float* destination, SimdVector4F source);
    void anemone_vectorcall Vector4F_StoreAlignedFloat1(float* destination, SimdVector4F source);
    void anemone_vectorcall Vector4F_StoreUnalignedFloat4(float* destination, SimdVector4F source);
    void anemone_vectorcall Vector4F_StoreUnalignedFloat3(float* destination, SimdVector4F source);
    void anemone_vectorcall Vector4F_StoreUnalignedFloat2(float* destination, SimdVector4F source);
    void anemone_vectorcall Vector4F_StoreUnalignedFloat1(float* destination, SimdVector4F source);

    SimdVector4F anemone_vectorcall Vector4F_Create(float x, float y, float z, float w);
    SimdVector4F anemone_vectorcall Vector4F_Replicate(float f);
    SimdVector4F anemone_vectorcall Vector4F_Zero();
    SimdVector4F anemone_vectorcall Vector4F_NaN();
    SimdVector4F anemone_vectorcall Vector4F_PositiveInfinity();
    SimdVector4F anemone_vectorcall Vector4F_NegativeInfinity();
    SimdVector4F anemone_vectorcall Vector4F_Epsilon();

    SimdVector4F anemone_vectorcall Vector4F_PositiveUnitX();
    SimdVector4F anemone_vectorcall Vector4F_PositiveUnitY();
    SimdVector4F anemone_vectorcall Vector4F_PositiveUnitZ();
    SimdVector4F anemone_vectorcall Vector4F_PositiveUnitW();
    SimdVector4F anemone_vectorcall Vector4F_NegativeUnitX();
    SimdVector4F anemone_vectorcall Vector4F_NegativeUnitY();
    SimdVector4F anemone_vectorcall Vector4F_NegativeUnitZ();
    SimdVector4F anemone_vectorcall Vector4F_NegativeUnitW();

    template <size_t N>
    float anemone_vectorcall Vector4F_Extract(SimdVector4F v)
        requires(N < 4);

    template <size_t N>
    SimdVector4F anemone_vectorcall Vector4F_Insert(SimdVector4F v, float f)
        requires(N < 4);

    template <size_t N>
    SimdVector4F anemone_vectorcall Vector4F_Broadcast(SimdVector4F v)
        requires(N < 4);

    template <size_t X, size_t Y, size_t Z, size_t W>
    SimdVector4F anemone_vectorcall Vector4F_Permute(SimdVector4F v)
        requires((X < 4) and (Y < 4) and (Z < 4) and (W < 4));

    template <size_t X, size_t Y, size_t Z, size_t W>
    SimdVector4F anemone_vectorcall Vector4F_Permute(SimdVector4F a, SimdVector4F b)
        requires((X < 8) and (Y < 8) and (Z < 8) and (W < 8));

    SimdVector4F anemone_vectorcall Vector4F_Select(SimdMask4F mask, SimdVector4F whenFalse, SimdVector4F whenTrue);

    template <bool X, bool Y, bool Z, bool W>
    SimdVector4F anemone_vectorcall Vector4F_Select(SimdVector4F whenFalse, SimdVector4F whenTrue);

    SimdVector4F anemone_vectorcall Vector4F_MultiplyAdd(SimdVector4F a, SimdVector4F b, SimdVector4F c);
    SimdVector4F anemone_vectorcall Vector4F_MultiplySubtract(SimdVector4F a, SimdVector4F b, SimdVector4F c);
    SimdVector4F anemone_vectorcall Vector4F_NegateMultiplyAdd(SimdVector4F a, SimdVector4F b, SimdVector4F c);
    SimdVector4F anemone_vectorcall Vector4F_NegateMultiplySubtract(SimdVector4F a, SimdVector4F b, SimdVector4F c);

    SimdVector4F anemone_vectorcall Vector4F_Add(SimdVector4F a, SimdVector4F b);
    SimdVector4F anemone_vectorcall Vector4F_Subtract(SimdVector4F a, SimdVector4F b);
    SimdVector4F anemone_vectorcall Vector4F_Multiply(SimdVector4F a, SimdVector4F b);
    SimdVector4F anemone_vectorcall Vector4F_Divide(SimdVector4F a, SimdVector4F b);
    SimdVector4F anemone_vectorcall Vector4F_Negate(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Reciprocal(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_ReciprocalEst(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Min(SimdVector4F a, SimdVector4F b);
    SimdVector4F anemone_vectorcall Vector4F_Max(SimdVector4F a, SimdVector4F b);
    SimdVector4F anemone_vectorcall Vector4F_Abs(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Square(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_SignedSquare(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Clamp(SimdVector4F v, SimdVector4F lower, SimdVector4F upper);
    SimdVector4F anemone_vectorcall Vector4F_Saturate(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_SquareRoot(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_SquareRootEst(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_ReciprocalSquareRoot(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_ReciprocalSquareRootEst(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Ceil(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Floor(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Truncate(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Round(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Fraction(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Remainder(SimdVector4F x, SimdVector4F y);
    SimdVector4F anemone_vectorcall Vector4F_Repeat(SimdVector4F v, SimdVector4F length);
    SimdVector4F anemone_vectorcall Vector4F_Wrap(SimdVector4F v, SimdVector4F lower, SimdVector4F upper);
    SimdVector4F anemone_vectorcall Vector4F_Power(SimdVector4F x, SimdVector4F y);
    SimdVector4F anemone_vectorcall Vector4F_Exp(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Exp2(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Exp10(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Log(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Log(SimdVector4F x, SimdVector4F base);
    SimdVector4F anemone_vectorcall Vector4F_Log2(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Log10(SimdVector4F v);
    void anemone_vectorcall Vector4F_SinCos(SimdVector4F& sin, SimdVector4F& cos, SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Sin(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Cos(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Tan(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Asin(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Acos(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Atan(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Atan2(SimdVector4F y, SimdVector4F x);
    SimdVector4F anemone_vectorcall Vector4F_PreciseLerp(SimdVector4F a, SimdVector4F b, SimdVector4F t);
    SimdVector4F anemone_vectorcall Vector4F_PreciseLerp(SimdVector4F a, SimdVector4F b, float t);
    SimdVector4F anemone_vectorcall Vector4F_Lerp(SimdVector4F a, SimdVector4F b, SimdVector4F t);
    SimdVector4F anemone_vectorcall Vector4F_Lerp(SimdVector4F a, SimdVector4F b, float t);
    SimdVector4F anemone_vectorcall Vector4F_PreciseBilinearLerp(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F d, SimdVector4F u, SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_PreciseBilinearLerp(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F d, float u, float v);
    SimdVector4F anemone_vectorcall Vector4F_BilinearLerp(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F d, SimdVector4F u, SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_BilinearLerp(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F d, float u, float v);
    SimdVector4F anemone_vectorcall Vector4F_Barycentric(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F u, SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Barycentric(SimdVector4F a, SimdVector4F b, SimdVector4F c, float u, float v);
    SimdVector4F anemone_vectorcall Vector4F_Unlerp(SimdVector4F value, SimdVector4F lower, SimdVector4F upper);
    SimdVector4F anemone_vectorcall Vector4F_Map(SimdVector4F value, SimdVector4F sourceLower, SimdVector4F sourceUpper, SimdVector4F targetLower, SimdVector4F targetUpper);
    SimdVector4F anemone_vectorcall Vector4F_PreciseMap(SimdVector4F value, SimdVector4F sourceLower, SimdVector4F sourceUpper, SimdVector4F targetLower, SimdVector4F targetUpper);
    SimdVector4F anemone_vectorcall Vector4F_BezierPosition(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F t);
    SimdVector4F anemone_vectorcall Vector4F_BezierPosition(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, float t);
    SimdVector4F anemone_vectorcall Vector4F_BezierTangent(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F t);
    SimdVector4F anemone_vectorcall Vector4F_BezierTangent(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, float t);
    SimdVector4F anemone_vectorcall Vector4F_BezierAcceleration(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F t);
    SimdVector4F anemone_vectorcall Vector4F_BezierAcceleration(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, float t);
    SimdVector4F anemone_vectorcall Vector4F_BezierPosition(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, SimdVector4F t);
    SimdVector4F anemone_vectorcall Vector4F_BezierPosition(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, float t);
    SimdVector4F anemone_vectorcall Vector4F_BezierTangent(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, SimdVector4F t);
    SimdVector4F anemone_vectorcall Vector4F_BezierTangent(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, float t);
    SimdVector4F anemone_vectorcall Vector4F_BezierAcceleration(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, SimdVector4F t);
    SimdVector4F anemone_vectorcall Vector4F_BezierAcceleration(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, float t);
    SimdVector4F anemone_vectorcall Vector4F_Hermite(SimdVector4F p0, SimdVector4F m0, SimdVector4F p1, SimdVector4F m1, SimdVector4F t);
    SimdVector4F anemone_vectorcall Vector4F_Hermite(SimdVector4F p0, SimdVector4F m0, SimdVector4F p1, SimdVector4F m1, float t);
    SimdVector4F anemone_vectorcall Vector4F_CatmullRom(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, SimdVector4F t);
    SimdVector4F anemone_vectorcall Vector4F_CatmullRom(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, float t);

    SimdVector4F anemone_vectorcall Vector4F_DeCasteljau(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, SimdVector4F t);

    SimdVector4F anemone_vectorcall Vector4F_UnwindRadians(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_UnwindDegrees(SimdVector4F v);

    SimdVector4F anemone_vectorcall Vector4F_AngleDifferenceRadians(SimdVector4F a, SimdVector4F b);
    SimdVector4F anemone_vectorcall Vector4F_AngleDifferenceDegrees(SimdVector4F a, SimdVector4F b);

    SimdVector4F anemone_vectorcall Vector4F_RevolutionsToDegrees(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_RevolutionsToRadians(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_DegreesToRevolutions(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_RadiansToRevolutions(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_RadiansToDegrees(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_DegreesToRadians(SimdVector4F v);

    SimdVector4F anemone_vectorcall Vector4F_Dot4(SimdVector4F a, SimdVector4F b);
    SimdVector4F anemone_vectorcall Vector4F_Dot3(SimdVector4F a, SimdVector4F b);
    SimdVector4F anemone_vectorcall Vector4F_Dot2(SimdVector4F a, SimdVector4F b);
    SimdVector4F anemone_vectorcall Vector4F_Cross4(SimdVector4F a, SimdVector4F b, SimdVector4F c);
    SimdVector4F anemone_vectorcall Vector4F_Cross3(SimdVector4F a, SimdVector4F b);
    SimdVector4F anemone_vectorcall Vector4F_Cross2(SimdVector4F a, SimdVector4F b);

    SimdVector4F anemone_vectorcall Vector4F_Wedge3(SimdVector4F a, SimdVector4F b);

    SimdVector4F anemone_vectorcall Vector4F_LengthSquared4(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_LengthSquared3(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_LengthSquared2(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Length4(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Length3(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Length2(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_ReciprocalLength4(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_ReciprocalLength3(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_ReciprocalLength2(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_ReciprocalLengthEst4(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_ReciprocalLengthEst3(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_ReciprocalLengthEst2(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Normalize4(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Normalize3(SimdVector4F v);
    SimdVector4F anemone_vectorcall Vector4F_Normalize2(SimdVector4F v);

    SimdVector4F anemone_vectorcall Vector4F_Reflect4(SimdVector4F incident, SimdVector4F normal);
    SimdVector4F anemone_vectorcall Vector4F_Reflect3(SimdVector4F incident, SimdVector4F normal);
    SimdVector4F anemone_vectorcall Vector4F_Reflect2(SimdVector4F incident, SimdVector4F normal);

    // TODO: Assert that index is not zero
    SimdVector4F anemone_vectorcall Vector4F_Refract4(SimdVector4F incident, SimdVector4F normal, SimdVector4F index);
    SimdVector4F anemone_vectorcall Vector4F_Refract3(SimdVector4F incident, SimdVector4F normal, SimdVector4F index);
    SimdVector4F anemone_vectorcall Vector4F_Refract2(SimdVector4F incident, SimdVector4F normal, SimdVector4F index);

    SimdVector4F anemone_vectorcall Vector4F_Refract4(SimdVector4F incident, SimdVector4F normal, float index);
    SimdVector4F anemone_vectorcall Vector4F_Refract3(SimdVector4F incident, SimdVector4F normal, float index);
    SimdVector4F anemone_vectorcall Vector4F_Refract2(SimdVector4F incident, SimdVector4F normal, float index);

    SimdVector4F anemone_vectorcall Vector4F_Transform4(SimdVector4F v, SimdMatrix4x4F m);
    SimdVector4F anemone_vectorcall Vector4F_Transform3(SimdVector4F v, SimdMatrix4x4F m);
    SimdVector4F anemone_vectorcall Vector4F_Transform2(SimdVector4F v, SimdMatrix4x4F m);

    SimdMask4F anemone_vectorcall Vector4F_CompareEqual(SimdVector4F a, SimdVector4F b);
    bool anemone_vectorcall Vector4F_IsEqual4(SimdVector4F a, SimdVector4F b);
    bool anemone_vectorcall Vector4F_IsEqual3(SimdVector4F a, SimdVector4F b);
    bool anemone_vectorcall Vector4F_IsEqual2(SimdVector4F a, SimdVector4F b);

    SimdMask4F anemone_vectorcall Vector4F_CompareNotEqual(SimdVector4F a, SimdVector4F b);
    bool anemone_vectorcall Vector4F_IsNotEqual4(SimdVector4F a, SimdVector4F b);
    bool anemone_vectorcall Vector4F_IsNotEqual3(SimdVector4F a, SimdVector4F b);
    bool anemone_vectorcall Vector4F_IsNotEqual2(SimdVector4F a, SimdVector4F b);

    SimdMask4F anemone_vectorcall Vector4F_CompareLessThan(SimdVector4F a, SimdVector4F b);
    bool anemone_vectorcall Vector4F_IsLessThan4(SimdVector4F a, SimdVector4F b);
    bool anemone_vectorcall Vector4F_IsLessThan3(SimdVector4F a, SimdVector4F b);
    bool anemone_vectorcall Vector4F_IsLessThan2(SimdVector4F a, SimdVector4F b);

    SimdMask4F anemone_vectorcall Vector4F_CompareLessEqual(SimdVector4F a, SimdVector4F b);
    bool anemone_vectorcall Vector4F_IsLessEqual4(SimdVector4F a, SimdVector4F b);
    bool anemone_vectorcall Vector4F_IsLessEqual3(SimdVector4F a, SimdVector4F b);
    bool anemone_vectorcall Vector4F_IsLessEqual2(SimdVector4F a, SimdVector4F b);

    SimdMask4F anemone_vectorcall Vector4F_CompareGreaterThan(SimdVector4F a, SimdVector4F b);
    bool anemone_vectorcall Vector4F_IsGreaterThan4(SimdVector4F a, SimdVector4F b);
    bool anemone_vectorcall Vector4F_IsGreaterThan3(SimdVector4F a, SimdVector4F b);
    bool anemone_vectorcall Vector4F_IsGreaterThan2(SimdVector4F a, SimdVector4F b);

    SimdMask4F anemone_vectorcall Vector4F_CompareGreaterEqual(SimdVector4F a, SimdVector4F b);
    bool anemone_vectorcall Vector4F_IsGreaterEqual4(SimdVector4F a, SimdVector4F b);
    bool anemone_vectorcall Vector4F_IsGreaterEqual3(SimdVector4F a, SimdVector4F b);
    bool anemone_vectorcall Vector4F_IsGreaterEqual2(SimdVector4F a, SimdVector4F b);

    SimdMask4F anemone_vectorcall Vector4F_CompareNaN(SimdVector4F v);
    bool anemone_vectorcall Vector4F_IsNaN4(SimdVector4F v);
    bool anemone_vectorcall Vector4F_IsNaN3(SimdVector4F v);
    bool anemone_vectorcall Vector4F_IsNaN2(SimdVector4F v);

    SimdMask4F anemone_vectorcall Vector4F_CompareInfinite(SimdVector4F v);
    bool anemone_vectorcall Vector4F_IsInfinite4(SimdVector4F v);
    bool anemone_vectorcall Vector4F_IsInfinite3(SimdVector4F v);
    bool anemone_vectorcall Vector4F_IsInfinite2(SimdVector4F v);

    SimdMask4F anemone_vectorcall Vector4F_CompareInRange(SimdVector4F v, SimdVector4F lower, SimdVector4F upper);
    bool anemone_vectorcall Vector4F_InRange4(SimdVector4F v, SimdVector4F lower, SimdVector4F upper);
    bool anemone_vectorcall Vector4F_InRange3(SimdVector4F v, SimdVector4F lower, SimdVector4F upper);
    bool anemone_vectorcall Vector4F_InRange2(SimdVector4F v, SimdVector4F lower, SimdVector4F upper);

    SimdMask4F anemone_vectorcall Vector4F_CompareNearZero(SimdVector4F v, SimdVector4F tolerance);
    SimdMask4F anemone_vectorcall Vector4F_CompareNearZero(SimdVector4F v, float tolerance);
    SimdMask4F anemone_vectorcall Vector4F_CompareNearZero(SimdVector4F v);
    SimdMask4F anemone_vectorcall Vector4F_CompareNearZeroPrecise(SimdVector4F v);

    SimdMask4F anemone_vectorcall Vector4F_CompareNearEqual(SimdVector4F a, SimdVector4F b, SimdVector4F tolerance);
    SimdMask4F anemone_vectorcall Vector4F_CompareNearEqual(SimdVector4F a, SimdVector4F b, float tolerance);
    SimdMask4F anemone_vectorcall Vector4F_CompareNearEqual(SimdVector4F a, SimdVector4F b);
    SimdMask4F anemone_vectorcall Vector4F_CompareNearEqualPrecise(SimdVector4F a, SimdVector4F b);
}

// Mask4F
namespace Anemone::Math::Detail
{
    SimdMask4F anemone_vectorcall Mask4F_True();
    SimdMask4F anemone_vectorcall Mask4F_False();

    SimdMask4F anemone_vectorcall Mask4F_Create(bool x, bool y, bool z, bool w);
    SimdMask4F anemone_vectorcall Mask4F_Create(bool x, bool y, bool z);
    SimdMask4F anemone_vectorcall Mask4F_Create(bool x, bool y);
    SimdMask4F anemone_vectorcall Mask4F_Replicate(bool value);

    template <size_t N>
    uint32_t anemone_vectorcall Mask4F_Extract(SimdMask4F v)
        requires(N < 4);

    template <size_t N>
    SimdMask4F anemone_vectorcall Mask4F_Insert(SimdMask4F v, uint32_t b)
        requires(N < 4);

    SimdMask4F anemone_vectorcall Mask4F_And(SimdMask4F a, SimdMask4F b);
    SimdMask4F anemone_vectorcall Mask4F_AndNot(SimdMask4F a, SimdMask4F b);
    SimdMask4F anemone_vectorcall Mask4F_Or(SimdMask4F a, SimdMask4F b);
    SimdMask4F anemone_vectorcall Mask4F_Xor(SimdMask4F a, SimdMask4F b);
    SimdMask4F anemone_vectorcall Mask4F_Not(SimdMask4F mask);
    SimdMask4F anemone_vectorcall Mask4F_Select(SimdMask4F mask, SimdMask4F whenFalse, SimdMask4F whenTrue);

    template <bool X, bool Y, bool Z, bool W>
    SimdMask4F anemone_vectorcall Mask4F_Select(SimdMask4F whenFalse, SimdMask4F whenTrue);

    bool anemone_vectorcall Mask4F_All4(SimdMask4F mask);
    bool anemone_vectorcall Mask4F_All3(SimdMask4F mask);
    bool anemone_vectorcall Mask4F_All2(SimdMask4F mask);

    bool anemone_vectorcall Mask4F_Any4(SimdMask4F mask);
    bool anemone_vectorcall Mask4F_Any3(SimdMask4F mask);
    bool anemone_vectorcall Mask4F_Any2(SimdMask4F mask);

    bool anemone_vectorcall Mask4F_None4(SimdMask4F mask);
    bool anemone_vectorcall Mask4F_None3(SimdMask4F mask);
    bool anemone_vectorcall Mask4F_None2(SimdMask4F mask);

    SimdMask4F anemone_vectorcall Mask4F_CompareEqual(SimdMask4F a, SimdMask4F b);
    SimdMask4F anemone_vectorcall Mask4F_CompareNotEqual(SimdMask4F a, SimdMask4F b);
}

// QuaternionF
namespace Anemone::Math::Detail
{
    SimdVector4F anemone_vectorcall QuaternionF_Identity();

    SimdVector4F anemone_vectorcall QuaternionF_CreateFromNormalAngle(SimdVector4F normal, float angle);
    SimdVector4F anemone_vectorcall QuaternionF_CreateFromAxisAngle(SimdVector4F axis, float angle);

    SimdVector4F anemone_vectorcall QuaternionF_CreateFromEulerAngles(SimdVector4F pitchYawRoll);
    SimdVector4F anemone_vectorcall QuaternionF_CreateFromEulerAngles(float pitch, float yaw, float roll);

    SimdVector4F anemone_vectorcall QuaternionF_Rotate3(SimdVector4F q, SimdVector4F v);
    SimdVector4F anemone_vectorcall QuaternionF_InverseRotate3(SimdVector4F q, SimdVector4F v);

    SimdVector4F anemone_vectorcall QuaternionF_Length(SimdVector4F q);
    SimdVector4F anemone_vectorcall QuaternionF_LengthSquared(SimdVector4F q);
    SimdVector4F anemone_vectorcall QuaternionF_Normalize(SimdVector4F q);
    SimdVector4F anemone_vectorcall QuaternionF_Dot(SimdVector4F a, SimdVector4F b);
    SimdVector4F anemone_vectorcall QuaternionF_Multiply(SimdVector4F a, SimdVector4F b);
    SimdVector4F anemone_vectorcall QuaternionF_Conjugate(SimdVector4F q);
    SimdVector4F anemone_vectorcall QuaternionF_Inverse(SimdVector4F q);
    void anemone_vectorcall QuaternionF_ToAxisAngle(SimdVector4F& axis, float& angle, SimdVector4F q);

    SimdVector4F anemone_vectorcall QuaternionF_Nlerp(SimdVector4F from, SimdVector4F to, SimdVector4F t);
    SimdVector4F anemone_vectorcall QuaternionF_Nlerp(SimdVector4F from, SimdVector4F to, float t);

    SimdVector4F anemone_vectorcall QuaternionF_Slerp(SimdVector4F from, SimdVector4F to, SimdVector4F t);
    SimdVector4F anemone_vectorcall QuaternionF_Slerp(SimdVector4F from, SimdVector4F to, float t);

    // SimdVector4F anemone_vectorcall QuaternionF_Barycentric(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F u, SimdVector4F v);
    // SimdVector4F anemone_vectorcall QuaternionF_Barycentric(SimdVector4F a, SimdVector4F b, SimdVector4F c, float u, float v);

    SimdMask4F anemone_vectorcall QuaternionF_CompareEqual(SimdVector4F a, SimdVector4F b);
    bool anemone_vectorcall QuaternionF_IsEqual(SimdVector4F a, SimdVector4F b);

    SimdMask4F anemone_vectorcall QuaternionF_CompareNotEqual(SimdVector4F a, SimdVector4F b);
    bool anemone_vectorcall QuaternionF_IsNotEqual(SimdVector4F a, SimdVector4F b);

    SimdMask4F anemone_vectorcall QuaternionF_CompareNaN(SimdVector4F q);
    bool anemone_vectorcall QuaternionF_IsNaN(SimdVector4F q);

    SimdMask4F anemone_vectorcall QuaternionF_CompareInfinite(SimdVector4F q);
    bool anemone_vectorcall QuaternionF_IsInfinite(SimdVector4F q);

    SimdMask4F anemone_vectorcall QuaternionF_CompareIdentity(SimdVector4F q);
    bool anemone_vectorcall QuaternionF_IsIdentity(SimdVector4F q);
}

// RotorF
namespace Anemone::Math::Detail
{
    SimdVector4F anemone_vectorcall RotorF_Identity();

    SimdVector4F anemone_vectorcall RotorF_Create(float xy, float xz, float yz, float scalar);
    SimdVector4F anemone_vectorcall RotorF_Create(SimdVector4F bv, float scalar);
    SimdVector4F anemone_vectorcall RotorF_CreateFromAxisAngle(SimdVector4F axis, float angle);
    SimdVector4F anemone_vectorcall RotorF_CreateFromNormalAngle(SimdVector4F normal, float angle);
    SimdVector4F anemone_vectorcall RotorF_RotationBetween(SimdVector4F from, SimdVector4F to);

    SimdVector4F anemone_vectorcall RotorF_Rotate3(SimdVector4F rotor, SimdVector4F v);
    SimdVector4F anemone_vectorcall RotorF_InverseRotate3(SimdVector4F rotor, SimdVector4F v);

    SimdVector4F anemone_vectorcall RotorF_Dot(SimdVector4F a, SimdVector4F b);
    SimdVector4F anemone_vectorcall RotorF_Multiply(SimdVector4F a, SimdVector4F b);
    SimdVector4F anemone_vectorcall RotorF_RotateRotor(SimdVector4F rotor, SimdVector4F rotation);
    SimdVector4F anemone_vectorcall RotorF_Reverse(SimdVector4F rotor);
    SimdVector4F anemone_vectorcall RotorF_Length(SimdVector4F rotor);
    SimdVector4F anemone_vectorcall RotorF_LengthSquared(SimdVector4F rotor);
    SimdVector4F anemone_vectorcall RotorF_Normalize(SimdVector4F rotor);

    SimdMatrix4x4F anemone_vectorcall RotorF_ToMatrix4x4F(SimdVector4F rotor);

    SimdVector4F anemone_vectorcall RotorF_Nlerp(SimdVector4F from, SimdVector4F to, SimdVector4F t);
    SimdVector4F anemone_vectorcall RotorF_Nlerp(SimdVector4F from, SimdVector4F to, float t);

    SimdVector4F anemone_vectorcall RotorF_Slerp(SimdVector4F from, SimdVector4F to, SimdVector4F t);
    SimdVector4F anemone_vectorcall RotorF_Slerp(SimdVector4F from, SimdVector4F to, float t);

    // SimdVector4F anemone_vectorcall RotorF_Barycentric(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F u, SimdVector4F v);
    // SimdVector4F anemone_vectorcall RotorF_Barycentric(SimdVector4F a, SimdVector4F b, SimdVector4F c, float u, float v);

    SimdMask4F anemone_vectorcall RotorF_CompareEqual(SimdVector4F a, SimdVector4F b);
    bool anemone_vectorcall RotorF_IsEqual(SimdVector4F a, SimdVector4F b);

    SimdMask4F anemone_vectorcall RotorF_CompareNotEqual(SimdVector4F a, SimdVector4F b);
    bool anemone_vectorcall RotorF_IsNotEqual(SimdVector4F a, SimdVector4F b);

    SimdMask4F anemone_vectorcall RotorF_CompareNaN(SimdVector4F q);
    bool anemone_vectorcall RotorF_IsNaN(SimdVector4F q);

    SimdMask4F anemone_vectorcall RotorF_CompareInfinite(SimdVector4F q);
    bool anemone_vectorcall RotorF_IsInfinite(SimdVector4F q);

    SimdMask4F anemone_vectorcall RotorF_CompareIdentity(SimdVector4F q);
    bool anemone_vectorcall RotorF_IsIdentity(SimdVector4F q);
}

// Matrix4x4F
namespace Anemone::Math::Detail
{
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadAlignedFloat4x4(float const* source);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadAlignedFloat4x3(float const* source);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadAlignedFloat3x4(float const* source);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadUnalignedFloat4x4(float const* source);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadUnalignedFloat4x3(float const* source);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadUnalignedFloat3x4(float const* source);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadUnalignedFloat3x3(float const* source);

    void anemone_vectorcall Matrix4x4F_StoreAlignedFloat4x4(float* destination, SimdMatrix4x4F source);
    void anemone_vectorcall Matrix4x4F_StoreAlignedFloat4x3(float* destination, SimdMatrix4x4F source);
    void anemone_vectorcall Matrix4x4F_StoreAlignedFloat3x4(float* destination, SimdMatrix4x4F source);
    void anemone_vectorcall Matrix4x4F_StoreUnalignedFloat4x4(float* destination, SimdMatrix4x4F source);
    void anemone_vectorcall Matrix4x4F_StoreUnalignedFloat4x3(float* destination, SimdMatrix4x4F source);
    void anemone_vectorcall Matrix4x4F_StoreUnalignedFloat3x4(float* destination, SimdMatrix4x4F source);
    void anemone_vectorcall Matrix4x4F_StoreUnalignedFloat3x3(float* destination, SimdMatrix4x4F source);

    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Identity();
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_NaN();
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Infinity();

    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateTranslation(float x, float y, float z);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateTranslation(SimdVector4F translation);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateScale(float x, float y, float z);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateScale(SimdVector4F scale);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateRotationX(float angle);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateRotationY(float angle);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateRotationZ(float angle);

    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateFromQuaternion(SimdVector4F q);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateFromPitchYawRoll(float pitch, float yaw, float roll);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateFromPitchYawRoll(SimdVector4F pitchYawRoll);

    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateAffineTransform2(
        SimdVector4F scaling,
        SimdVector4F rotationOrigin,
        float rotation,
        SimdVector4F translation);

    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateAffineTransform3(
        SimdVector4F scaling,
        SimdVector4F rotationOrigin,
        SimdVector4F rotationQuaternion,
        SimdVector4F translation);

    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateLookAtLH(SimdVector4F eye, SimdVector4F focus, SimdVector4F up);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateLookAtRH(SimdVector4F eye, SimdVector4F focus, SimdVector4F up);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateLookToLH(SimdVector4F eye, SimdVector4F direction, SimdVector4F up);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateLookToRH(SimdVector4F eye, SimdVector4F direction, SimdVector4F up);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreatePerspectiveLH(float width, float height, float zNear, float zFar);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreatePerspectiveRH(float width, float height, float zNear, float zFar);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreatePerspectiveFovLH(float fov, float aspect, float zNear, float zFar);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreatePerspectiveFovRH(float fov, float aspect, float zNear, float zFar);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateOrthographicLH(float width, float height, float zNear, float zFar);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateOrthographicRH(float width, float height, float zNear, float zFar);

    template <size_t N>
    SimdVector4F anemone_vectorcall Matrix4x4F_Extract(SimdMatrix4x4F m)
        requires(N < 4);

    template <size_t N>
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Insert(SimdMatrix4x4F m, SimdVector4F v)
        requires(N < 4);

    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Multiply(SimdMatrix4x4F a, SimdMatrix4x4F b);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_MultiplyTranspose(SimdMatrix4x4F a, SimdMatrix4x4F b);

    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Transpose(SimdMatrix4x4F m);
    SimdVector4F anemone_vectorcall Matrix4x4F_Determinant(SimdMatrix4x4F matrix);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Inverse(SimdMatrix4x4F matrix, SimdVector4F& determinant);

    SimdVector4F anemone_vectorcall Matrix4x4F_Diagonal(SimdMatrix4x4F m);
    SimdVector4F anemone_vectorcall Matrix4x4F_Trace(SimdMatrix4x4F m);
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_TensorProduct(SimdVector4F a, SimdVector4F b);

    bool anemone_vectorcall Matrix4x4F_IsIdentity(SimdMatrix4x4F m);
    bool anemone_vectorcall Matrix4x4F_IsNaN(SimdMatrix4x4F m);
    bool anemone_vectorcall Matrix4x4F_IsInfinite(SimdMatrix4x4F m);
}

// PlaneF
namespace Anemone::Math::Detail
{
    SimdVector4F anemone_vectorcall PlaneF_Create(float a, float b, float c, float d);
    SimdVector4F anemone_vectorcall PlaneF_CreateFromPointNormal(SimdVector4F point, SimdVector4F normal);
    SimdVector4F anemone_vectorcall PlaneF_CreateFromPoints(SimdVector4F point1, SimdVector4F point2, SimdVector4F point3);
    SimdVector4F anemone_vectorcall PlaneF_Dot(SimdVector4F plane, SimdVector4F point);
    SimdVector4F anemone_vectorcall PlaneF_DotCoord(SimdVector4F plane, SimdVector4F point);
    SimdVector4F anemone_vectorcall PlaneF_DotNormal(SimdVector4F plane, SimdVector4F point);
    SimdVector4F anemone_vectorcall PlaneF_Normalize(SimdVector4F plane);
    SimdVector4F anemone_vectorcall PlaneF_IntersectLine(SimdVector4F plane, SimdVector4F linePoint1, SimdVector4F linePoint2);
    void anemone_vectorcall PlaneF_IntersectPlane(SimdVector4F& linePoint1, SimdVector4F& linePoint2, SimdVector4F plane1, SimdVector4F plane2);
    SimdVector4F anemone_vectorcall PlaneF_Transform(SimdVector4F plane, SimdMatrix4x4F matrix);

    SimdMask4F anemone_vectorcall PlaneF_CompareEqual(SimdVector4F a, SimdVector4F b);
    bool anemone_vectorcall PlaneF_IsEqual(SimdVector4F a, SimdVector4F b);
    SimdMask4F anemone_vectorcall PlaneF_CompareNearEqual(SimdVector4F a, SimdVector4F b, SimdVector4F tolerance);
    SimdMask4F anemone_vectorcall PlaneF_CompareNotEqual(SimdVector4F a, SimdVector4F b);
    bool anemone_vectorcall PlaneF_IsNotEqual(SimdVector4F a, SimdVector4F b);
    SimdMask4F anemone_vectorcall PlaneF_CompareNaN(SimdVector4F q);
    bool anemone_vectorcall PlaneF_IsNaN(SimdVector4F q);
    SimdMask4F anemone_vectorcall PlaneF_CompareInfinite(SimdVector4F q);
    bool anemone_vectorcall PlaneF_IsInfinite(SimdVector4F q);
}

// Bezier4F
namespace Anemone::Math::Detail
{
    struct Bezier4FA
    {
        SimdVector4F Point0;
        SimdVector4F Point1;
        SimdVector4F Point2;
        SimdVector4F Point3;
    };

    // TODO: This should be implemented using special structs instead of arrays
    inline void anemone_vectorcall Vector4F_SplitBezier(
        Bezier4FA& left,
        Bezier4FA& right,
        Bezier4FA input,
        float t)
    {
        SimdVector4F const p01 = Vector4F_Lerp(input.Point0, input.Point1, t);
        SimdVector4F const p12 = Vector4F_Lerp(input.Point1, input.Point2, t);
        SimdVector4F const p23 = Vector4F_Lerp(input.Point2, input.Point3, t);

        SimdVector4F const p012 = Vector4F_Lerp(p01, p12, t);
        SimdVector4F const p123 = Vector4F_Lerp(p12, p23, t);

        SimdVector4F const p0123 = Vector4F_Lerp(p012, p123, t);

        left.Point0 = input.Point0;
        left.Point1 = p01;
        left.Point2 = p012;
        left.Point3 = p0123;

        right.Point0 = p0123;
        right.Point1 = p123;
        right.Point2 = p23;
        right.Point3 = input.Point3;
    }

    struct DistanceToInterpolation
    {
        float Distance;
        float Value;
    };

    inline SimdVector4F anemone_vectorcall Bezier4_CalculateLengthSegments(
        SimdVector4F p0,
        SimdVector4F p1,
        SimdVector4F p2,
        SimdVector4F p3,
        DistanceToInterpolation* lookup,
        size_t resolution)
    {
        SimdVector4F length = Vector4F_Zero();
        SimdVector4F previous = Vector4F_BezierPosition(p0, p1, p2, p3, 0.0f);
        lookup[0] = DistanceToInterpolation{0.0f, 0.0f};

        for (size_t i = 1; i < resolution; ++i)
        {
            float const t = static_cast<float>(i) / static_cast<float>(resolution - 1);
            SimdVector4F const point = Vector4F_BezierPosition(p0, p1, p2, p3, t);
            SimdVector4F const direction = Vector4F_Subtract(point, previous);
            length = Vector4F_Add(length, Vector4F_Length3(direction));
            lookup[i] = DistanceToInterpolation{Vector4F_Extract<0>(length), t};
            previous = point;
        }

        return length;
    }

    anemone_noinline inline SimdVector4F anemone_vectorcall Bezier4_CalculateLength(
        SimdVector4F p0,
        SimdVector4F p1,
        SimdVector4F p2,
        SimdVector4F p3,
        size_t resolution)
    {
        SimdVector4F length = Vector4F_Zero();
        SimdVector4F previous = Vector4F_BezierPosition(p0, p1, p2, p3, 0.0f);

        for (size_t i = 1; i < resolution; ++i)
        {
            float const t = static_cast<float>(i) / static_cast<float>(resolution - 1);
            SimdVector4F const point = Vector4F_BezierPosition(p0, p1, p2, p3, t);
            SimdVector4F const direction = Vector4F_Subtract(point, previous);
            length = Vector4F_Add(length, Vector4F_Length3(direction));
            previous = point;
        }

        return length;
    }
}

#include "AnemoneRuntime/Math/Detail/SimdFloat.inl"
