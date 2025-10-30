#pragma once
#include "AnemoneInterop/Headers.hxx"

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

#include "AnemoneMath/Functions.hxx"
#include "AnemoneMath/Numbers.hxx"
#include "AnemoneDiagnostics/Debug.hxx"

namespace Anemone::Internal
{
#if ANEMONE_BUILD_DISABLE_SIMD

    struct SimdVector4D final
    {
        double Inner[4];
    };

    struct SimdMask4D final
    {
        bool Inner[4];
    };

    struct SimdMatrix4x4D final
    {
        SimdVector4D Inner[4];
    };

#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2

    using SimdVector4D = __m256d;

    using SimdMask4D = __m256d;

    struct SimdMatrix4x4D final
    {
        __m256d Inner[4];
    };

#elif ANEMONE_FEATURE_NEON

    struct SimdVector4D
    {
        float64x2x2_t Inner;
    };

    struct SimdMask4D
    {
        uint64x2x2_t Inner;
    };

    struct SimdMatrix4x4D final
    {
        float64x2x4_t Inner[2];
    };

#else
#error "Not implemented"
#endif
}

namespace Anemone::Internal
{
    SimdVector4D anemone_vectorcall Vector4D_LoadAlignedDouble4(double const* source);
    SimdVector4D anemone_vectorcall Vector4D_LoadAlignedDouble3(double const* source);
    SimdVector4D anemone_vectorcall Vector4D_LoadAlignedDouble2(double const* source);
    SimdVector4D anemone_vectorcall Vector4D_LoadAlignedDouble1(double const* source);
    SimdVector4D anemone_vectorcall Vector4D_LoadUnalignedDouble4(double const* source);
    SimdVector4D anemone_vectorcall Vector4D_LoadUnalignedDouble3(double const* source);
    SimdVector4D anemone_vectorcall Vector4D_LoadUnalignedDouble2(double const* source);
    SimdVector4D anemone_vectorcall Vector4D_LoadUnalignedDouble1(double const* source);
    
    void anemone_vectorcall Vector4D_StoreAlignedDouble4(double* destination, SimdVector4D source);
    void anemone_vectorcall Vector4D_StoreAlignedDouble3(double* destination, SimdVector4D source);
    void anemone_vectorcall Vector4D_StoreAlignedDouble2(double* destination, SimdVector4D source);
    void anemone_vectorcall Vector4D_StoreAlignedDouble1(double* destination, SimdVector4D source);
    void anemone_vectorcall Vector4D_StoreUnalignedDouble4(double* destination, SimdVector4D source);
    void anemone_vectorcall Vector4D_StoreUnalignedDouble3(double* destination, SimdVector4D source);
    void anemone_vectorcall Vector4D_StoreUnalignedDouble2(double* destination, SimdVector4D source);
    void anemone_vectorcall Vector4D_StoreUnalignedDouble1(double* destination, SimdVector4D source);

    SimdVector4D anemone_vectorcall Vector4D_Create(double x, double y, double z, double w);
    SimdVector4D anemone_vectorcall Vector4D_Replicate(double f);
    SimdVector4D anemone_vectorcall Vector4D_Zero();
    SimdVector4D anemone_vectorcall Vector4D_NaN();
    SimdVector4D anemone_vectorcall Vector4D_PositiveInfinity();
    SimdVector4D anemone_vectorcall Vector4D_NegativeInfinity();
    SimdVector4D anemone_vectorcall Vector4D_Epsilon();

    SimdVector4D anemone_vectorcall Vector4D_PositiveUnitX();
    SimdVector4D anemone_vectorcall Vector4D_PositiveUnitY();
    SimdVector4D anemone_vectorcall Vector4D_PositiveUnitZ();
    SimdVector4D anemone_vectorcall Vector4D_PositiveUnitW();
    SimdVector4D anemone_vectorcall Vector4D_NegativeUnitX();
    SimdVector4D anemone_vectorcall Vector4D_NegativeUnitY();
    SimdVector4D anemone_vectorcall Vector4D_NegativeUnitZ();
    SimdVector4D anemone_vectorcall Vector4D_NegativeUnitW();

    template <size_t N>
    double anemone_vectorcall Vector4D_Extract(SimdVector4D v)
        requires(N < 4);

    template <size_t N>
    void anemone_vectorcall Vector4D_Extract(SimdVector4D v, double* f)
        requires(N < 4);

    template <size_t N>
    SimdVector4D anemone_vectorcall Vector4D_Insert(SimdVector4D v, double f)
        requires(N < 4);

    template <size_t N>
    SimdVector4D anemone_vectorcall Vector4D_Insert(SimdVector4D v, double const* f)
        requires(N < 4);

    template <size_t N>
    SimdVector4D anemone_vectorcall Vector4D_Broadcast(SimdVector4D v)
        requires(N < 4);

    template <size_t X, size_t Y, size_t Z, size_t W>
    SimdVector4D anemone_vectorcall Vector4D_Permute(SimdVector4D v)
        requires((X < 4) and (Y < 4) and (Z < 4) and (W < 4));

    template <size_t X, size_t Y, size_t Z, size_t W>
    SimdVector4D anemone_vectorcall Vector4D_Permute(SimdVector4D a, SimdVector4D b)
        requires((X < 8) and (Y < 8) and (Z < 8) and (W < 8));

    SimdVector4D anemone_vectorcall Vector4D_Select(SimdMask4D mask, SimdVector4D whenFalse, SimdVector4D whenTrue);

    template <bool X, bool Y, bool Z, bool W>
    SimdVector4D anemone_vectorcall Vector4D_Select(SimdVector4D whenFalse, SimdVector4D whenTrue);

    SimdVector4D anemone_vectorcall Vector4D_MultiplyAdd(SimdVector4D a, SimdVector4D b, SimdVector4D c);
    SimdVector4D anemone_vectorcall Vector4D_MultiplySubtract(SimdVector4D a, SimdVector4D b, SimdVector4D c);
    SimdVector4D anemone_vectorcall Vector4D_NegateMultiplyAdd(SimdVector4D a, SimdVector4D b, SimdVector4D c);
    SimdVector4D anemone_vectorcall Vector4D_NegateMultiplySubtract(SimdVector4D a, SimdVector4D b, SimdVector4D c);
    SimdVector4D anemone_vectorcall Vector4D_Add(SimdVector4D a, SimdVector4D b);
    SimdVector4D anemone_vectorcall Vector4D_Subtract(SimdVector4D a, SimdVector4D b);
    SimdVector4D anemone_vectorcall Vector4D_Multiply(SimdVector4D a, SimdVector4D b);
    SimdVector4D anemone_vectorcall Vector4D_Divide(SimdVector4D a, SimdVector4D b);
    SimdVector4D anemone_vectorcall Vector4D_Negate(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Reciprocal(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_ReciprocalEst(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Min(SimdVector4D a, SimdVector4D b);
    SimdVector4D anemone_vectorcall Vector4D_Max(SimdVector4D a, SimdVector4D b);
    SimdVector4D anemone_vectorcall Vector4D_Abs(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Square(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_SignedSquare(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Clamp(SimdVector4D v, SimdVector4D lower, SimdVector4D upper);
    SimdVector4D anemone_vectorcall Vector4D_Saturate(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_SquareRoot(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_SquareRootEst(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_ReciprocalSquareRoot(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_ReciprocalSquareRootEst(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Ceil(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Floor(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Truncate(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Round(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Fraction(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Remainder(SimdVector4D x, SimdVector4D y);
    SimdVector4D anemone_vectorcall Vector4D_Repeat(SimdVector4D v, SimdVector4D length);
    SimdVector4D anemone_vectorcall Vector4D_Wrap(SimdVector4D v, SimdVector4D lower, SimdVector4D upper);
    SimdVector4D anemone_vectorcall Vector4D_Power(SimdVector4D x, SimdVector4D y);
    SimdVector4D anemone_vectorcall Vector4D_Exp(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Exp2(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Exp10(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Log(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Log(SimdVector4D x, SimdVector4D base);
    SimdVector4D anemone_vectorcall Vector4D_Log2(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Log10(SimdVector4D v);
    void anemone_vectorcall Vector4D_SinCos(SimdVector4D& sin, SimdVector4D& cos, SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Sin(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Cos(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Tan(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Asin(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Acos(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Atan(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Atan2(SimdVector4D y, SimdVector4D x);
    SimdVector4D anemone_vectorcall Vector4D_PreciseLerp(SimdVector4D a, SimdVector4D b, SimdVector4D t);
    SimdVector4D anemone_vectorcall Vector4D_PreciseLerp(SimdVector4D a, SimdVector4D b, double t);
    SimdVector4D anemone_vectorcall Vector4D_Lerp(SimdVector4D a, SimdVector4D b, SimdVector4D t);
    SimdVector4D anemone_vectorcall Vector4D_Lerp(SimdVector4D a, SimdVector4D b, double t);
    SimdVector4D anemone_vectorcall Vector4D_PreciseBilinearLerp(SimdVector4D a, SimdVector4D b, SimdVector4D c, SimdVector4D d, SimdVector4D u, SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_PreciseBilinearLerp(SimdVector4D a, SimdVector4D b, SimdVector4D c, SimdVector4D d, double u, double v);
    SimdVector4D anemone_vectorcall Vector4D_BilinearLerp(SimdVector4D a, SimdVector4D b, SimdVector4D c, SimdVector4D d, SimdVector4D u, SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_BilinearLerp(SimdVector4D a, SimdVector4D b, SimdVector4D c, SimdVector4D d, double u, double v);
    SimdVector4D anemone_vectorcall Vector4D_Barycentric(SimdVector4D a, SimdVector4D b, SimdVector4D c, SimdVector4D u, SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Barycentric(SimdVector4D a, SimdVector4D b, SimdVector4D c, double u, double v);
    SimdVector4D anemone_vectorcall Vector4D_Unlerp(SimdVector4D value, SimdVector4D lower, SimdVector4D upper);
    SimdVector4D anemone_vectorcall Vector4D_Map(SimdVector4D value, SimdVector4D sourceLower, SimdVector4D sourceUpper, SimdVector4D targetLower, SimdVector4D targetUpper);
    SimdVector4D anemone_vectorcall Vector4D_PreciseMap(SimdVector4D value, SimdVector4D sourceLower, SimdVector4D sourceUpper, SimdVector4D targetLower, SimdVector4D targetUpper);
    SimdVector4D anemone_vectorcall Vector4D_BezierPosition(SimdVector4D p0, SimdVector4D p1, SimdVector4D p2, SimdVector4D t);
    SimdVector4D anemone_vectorcall Vector4D_BezierPosition(SimdVector4D p0, SimdVector4D p1, SimdVector4D p2, double t);
    SimdVector4D anemone_vectorcall Vector4D_BezierTangent(SimdVector4D p0, SimdVector4D p1, SimdVector4D p2, SimdVector4D t);
    SimdVector4D anemone_vectorcall Vector4D_BezierTangent(SimdVector4D p0, SimdVector4D p1, SimdVector4D p2, double t);
    SimdVector4D anemone_vectorcall Vector4D_BezierAcceleration(SimdVector4D p0, SimdVector4D p1, SimdVector4D p2, SimdVector4D t);
    SimdVector4D anemone_vectorcall Vector4D_BezierAcceleration(SimdVector4D p0, SimdVector4D p1, SimdVector4D p2, double t);
    SimdVector4D anemone_vectorcall Vector4D_BezierPosition(SimdVector4D p0, SimdVector4D p1, SimdVector4D p2, SimdVector4D p3, SimdVector4D t);
    SimdVector4D anemone_vectorcall Vector4D_BezierPosition(SimdVector4D p0, SimdVector4D p1, SimdVector4D p2, SimdVector4D p3, double t);
    SimdVector4D anemone_vectorcall Vector4D_BezierTangent(SimdVector4D p0, SimdVector4D p1, SimdVector4D p2, SimdVector4D p3, SimdVector4D t);
    SimdVector4D anemone_vectorcall Vector4D_BezierTangent(SimdVector4D p0, SimdVector4D p1, SimdVector4D p2, SimdVector4D p3, double t);
    SimdVector4D anemone_vectorcall Vector4D_BezierAcceleration(SimdVector4D p0, SimdVector4D p1, SimdVector4D p2, SimdVector4D p3, SimdVector4D t);
    SimdVector4D anemone_vectorcall Vector4D_BezierAcceleration(SimdVector4D p0, SimdVector4D p1, SimdVector4D p2, SimdVector4D p3, double t);
    SimdVector4D anemone_vectorcall Vector4D_Hermite(SimdVector4D p0, SimdVector4D m0, SimdVector4D p1, SimdVector4D m1, SimdVector4D t);
    SimdVector4D anemone_vectorcall Vector4D_Hermite(SimdVector4D p0, SimdVector4D m0, SimdVector4D p1, SimdVector4D m1, double t);
    SimdVector4D anemone_vectorcall Vector4D_CatmullRom(SimdVector4D p0, SimdVector4D p1, SimdVector4D p2, SimdVector4D p3, SimdVector4D t);
    SimdVector4D anemone_vectorcall Vector4D_CatmullRom(SimdVector4D p0, SimdVector4D p1, SimdVector4D p2, SimdVector4D p3, double t);

    SimdVector4D anemone_vectorcall Vector4D_Midpoint(SimdVector4D p0, SimdVector4D p1);
    SimdVector4D anemone_vectorcall Vector4D_DeCasteljau(SimdVector4D p0, SimdVector4D p1, SimdVector4D p2, SimdVector4D p3, SimdVector4D t);

    SimdVector4D anemone_vectorcall Vector4D_UnwindRadians(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_UnwindDegrees(SimdVector4D v);

    SimdVector4D anemone_vectorcall Vector4D_AngleDifferenceRadians(SimdVector4D a, SimdVector4D b);
    SimdVector4D anemone_vectorcall Vector4D_AngleDifferenceDegrees(SimdVector4D a, SimdVector4D b);

    SimdVector4D anemone_vectorcall Vector4D_RevolutionsToDegrees(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_RevolutionsToRadians(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_DegreesToRevolutions(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_RadiansToRevolutions(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_RadiansToDegrees(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_DegreesToRadians(SimdVector4D v);

    SimdVector4D anemone_vectorcall Vector4D_Dot4(SimdVector4D a, SimdVector4D b);
    SimdVector4D anemone_vectorcall Vector4D_Dot3(SimdVector4D a, SimdVector4D b);
    SimdVector4D anemone_vectorcall Vector4D_Dot2(SimdVector4D a, SimdVector4D b);
    SimdVector4D anemone_vectorcall Vector4D_Cross4(SimdVector4D a, SimdVector4D b, SimdVector4D c);
    SimdVector4D anemone_vectorcall Vector4D_Cross3(SimdVector4D a, SimdVector4D b);
    SimdVector4D anemone_vectorcall Vector4D_Cross2(SimdVector4D a, SimdVector4D b);

    SimdVector4D anemone_vectorcall Vector4D_Wedge3(SimdVector4D a, SimdVector4D b);

    SimdVector4D anemone_vectorcall Vector4D_LengthSquared4(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_LengthSquared3(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_LengthSquared2(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Length4(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Length3(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Length2(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_ReciprocalLength4(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_ReciprocalLength3(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_ReciprocalLength2(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_ReciprocalLengthEst4(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_ReciprocalLengthEst3(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_ReciprocalLengthEst2(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Normalize4(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Normalize3(SimdVector4D v);
    SimdVector4D anemone_vectorcall Vector4D_Normalize2(SimdVector4D v);

    SimdVector4D anemone_vectorcall Vector4D_Reflect4(SimdVector4D incident, SimdVector4D normal);
    SimdVector4D anemone_vectorcall Vector4D_Reflect3(SimdVector4D incident, SimdVector4D normal);
    SimdVector4D anemone_vectorcall Vector4D_Reflect2(SimdVector4D incident, SimdVector4D normal);

    // TODO: Assert that index is not zero
    SimdVector4D anemone_vectorcall Vector4D_Refract4(SimdVector4D incident, SimdVector4D normal, SimdVector4D index);
    SimdVector4D anemone_vectorcall Vector4D_Refract3(SimdVector4D incident, SimdVector4D normal, SimdVector4D index);
    SimdVector4D anemone_vectorcall Vector4D_Refract2(SimdVector4D incident, SimdVector4D normal, SimdVector4D index);

    SimdVector4D anemone_vectorcall Vector4D_Refract4(SimdVector4D incident, SimdVector4D normal, double index);
    SimdVector4D anemone_vectorcall Vector4D_Refract3(SimdVector4D incident, SimdVector4D normal, double index);
    SimdVector4D anemone_vectorcall Vector4D_Refract2(SimdVector4D incident, SimdVector4D normal, double index);

    SimdVector4D anemone_vectorcall Vector4D_Transform4(SimdVector4D v, SimdMatrix4x4D m);
    SimdVector4D anemone_vectorcall Vector4D_Transform3(SimdVector4D v, SimdMatrix4x4D m);
    SimdVector4D anemone_vectorcall Vector4D_Transform2(SimdVector4D v, SimdMatrix4x4D m);

    SimdMask4D anemone_vectorcall Vector4D_CompareEqual(SimdVector4D a, SimdVector4D b);
    bool anemone_vectorcall Vector4D_IsEqual4(SimdVector4D a, SimdVector4D b);
    bool anemone_vectorcall Vector4D_IsEqual3(SimdVector4D a, SimdVector4D b);
    bool anemone_vectorcall Vector4D_IsEqual2(SimdVector4D a, SimdVector4D b);

    SimdMask4D anemone_vectorcall Vector4D_CompareNotEqual(SimdVector4D a, SimdVector4D b);
    bool anemone_vectorcall Vector4D_IsNotEqual4(SimdVector4D a, SimdVector4D b);
    bool anemone_vectorcall Vector4D_IsNotEqual3(SimdVector4D a, SimdVector4D b);
    bool anemone_vectorcall Vector4D_IsNotEqual2(SimdVector4D a, SimdVector4D b);

    SimdMask4D anemone_vectorcall Vector4D_CompareLessThan(SimdVector4D a, SimdVector4D b);
    bool anemone_vectorcall Vector4D_IsLessThan4(SimdVector4D a, SimdVector4D b);
    bool anemone_vectorcall Vector4D_IsLessThan3(SimdVector4D a, SimdVector4D b);
    bool anemone_vectorcall Vector4D_IsLessThan2(SimdVector4D a, SimdVector4D b);

    SimdMask4D anemone_vectorcall Vector4D_CompareLessEqual(SimdVector4D a, SimdVector4D b);
    bool anemone_vectorcall Vector4D_IsLessEqual4(SimdVector4D a, SimdVector4D b);
    bool anemone_vectorcall Vector4D_IsLessEqual3(SimdVector4D a, SimdVector4D b);
    bool anemone_vectorcall Vector4D_IsLessEqual2(SimdVector4D a, SimdVector4D b);

    SimdMask4D anemone_vectorcall Vector4D_CompareGreaterThan(SimdVector4D a, SimdVector4D b);
    bool anemone_vectorcall Vector4D_IsGreaterThan4(SimdVector4D a, SimdVector4D b);
    bool anemone_vectorcall Vector4D_IsGreaterThan3(SimdVector4D a, SimdVector4D b);
    bool anemone_vectorcall Vector4D_IsGreaterThan2(SimdVector4D a, SimdVector4D b);

    SimdMask4D anemone_vectorcall Vector4D_CompareGreaterEqual(SimdVector4D a, SimdVector4D b);
    bool anemone_vectorcall Vector4D_IsGreaterEqual4(SimdVector4D a, SimdVector4D b);
    bool anemone_vectorcall Vector4D_IsGreaterEqual3(SimdVector4D a, SimdVector4D b);
    bool anemone_vectorcall Vector4D_IsGreaterEqual2(SimdVector4D a, SimdVector4D b);

    SimdMask4D anemone_vectorcall Vector4D_CompareNaN(SimdVector4D v);
    bool anemone_vectorcall Vector4D_IsNaN4(SimdVector4D v);
    bool anemone_vectorcall Vector4D_IsNaN3(SimdVector4D v);
    bool anemone_vectorcall Vector4D_IsNaN2(SimdVector4D v);

    SimdMask4D anemone_vectorcall Vector4D_CompareInfinite(SimdVector4D v);
    bool anemone_vectorcall Vector4D_IsInfinite4(SimdVector4D v);
    bool anemone_vectorcall Vector4D_IsInfinite3(SimdVector4D v);
    bool anemone_vectorcall Vector4D_IsInfinite2(SimdVector4D v);

    SimdMask4D anemone_vectorcall Vector4D_CompareInRange(SimdVector4D v, SimdVector4D lower, SimdVector4D upper);
    bool anemone_vectorcall Vector4D_InRange4(SimdVector4D v, SimdVector4D lower, SimdVector4D upper);
    bool anemone_vectorcall Vector4D_InRange3(SimdVector4D v, SimdVector4D lower, SimdVector4D upper);
    bool anemone_vectorcall Vector4D_InRange2(SimdVector4D v, SimdVector4D lower, SimdVector4D upper);

    SimdMask4D anemone_vectorcall Vector4D_CompareNearZero(SimdVector4D v, SimdVector4D tolerance);
    SimdMask4D anemone_vectorcall Vector4D_CompareNearZero(SimdVector4D v, double tolerance);
    SimdMask4D anemone_vectorcall Vector4D_CompareNearZero(SimdVector4D v);
    SimdMask4D anemone_vectorcall Vector4D_CompareNearZeroPrecise(SimdVector4D v);

    SimdMask4D anemone_vectorcall Vector4D_CompareNearEqual(SimdVector4D a, SimdVector4D b, SimdVector4D tolerance);
    SimdMask4D anemone_vectorcall Vector4D_CompareNearEqual(SimdVector4D a, SimdVector4D b, double tolerance);
    SimdMask4D anemone_vectorcall Vector4D_CompareNearEqual(SimdVector4D a, SimdVector4D b);
    SimdMask4D anemone_vectorcall Vector4D_CompareNearEqualPrecise(SimdVector4D a, SimdVector4D b);
}

// Mask4D
namespace Anemone::Internal
{
    SimdMask4D anemone_vectorcall Mask4D_True();
    SimdMask4D anemone_vectorcall Mask4D_False();

    SimdMask4D anemone_vectorcall Mask4D_Create(bool x, bool y, bool z, bool w);
    SimdMask4D anemone_vectorcall Mask4D_Create(bool x, bool y, bool z);
    SimdMask4D anemone_vectorcall Mask4D_Create(bool x, bool y);
    SimdMask4D anemone_vectorcall Mask4D_Replicate(bool value);

    template <size_t N>
    uint32_t anemone_vectorcall Mask4D_Extract(SimdMask4D v)
        requires(N < 4);

    template <size_t N>
    SimdMask4D anemone_vectorcall Mask4D_Insert(SimdMask4D v, uint32_t b)
        requires(N < 4);

    SimdMask4D anemone_vectorcall Mask4D_And(SimdMask4D a, SimdMask4D b);
    SimdMask4D anemone_vectorcall Mask4D_AndNot(SimdMask4D a, SimdMask4D b);
    SimdMask4D anemone_vectorcall Mask4D_Or(SimdMask4D a, SimdMask4D b);
    SimdMask4D anemone_vectorcall Mask4D_Xor(SimdMask4D a, SimdMask4D b);
    SimdMask4D anemone_vectorcall Mask4D_Not(SimdMask4D mask);
    SimdMask4D anemone_vectorcall Mask4D_Select(SimdMask4D mask, SimdMask4D whenFalse, SimdMask4D whenTrue);

    template <bool X, bool Y, bool Z, bool W>
    SimdMask4D anemone_vectorcall Mask4D_Select(SimdMask4D whenFalse, SimdMask4D whenTrue);

    bool anemone_vectorcall Mask4D_All4(SimdMask4D mask);
    bool anemone_vectorcall Mask4D_All3(SimdMask4D mask);
    bool anemone_vectorcall Mask4D_All2(SimdMask4D mask);

    bool anemone_vectorcall Mask4D_Any4(SimdMask4D mask);
    bool anemone_vectorcall Mask4D_Any3(SimdMask4D mask);
    bool anemone_vectorcall Mask4D_Any2(SimdMask4D mask);

    bool anemone_vectorcall Mask4D_None4(SimdMask4D mask);
    bool anemone_vectorcall Mask4D_None3(SimdMask4D mask);
    bool anemone_vectorcall Mask4D_None2(SimdMask4D mask);

    SimdMask4D anemone_vectorcall Mask4D_CompareEqual(SimdMask4D a, SimdMask4D b);
    SimdMask4D anemone_vectorcall Mask4D_CompareNotEqual(SimdMask4D a, SimdMask4D b);
}

// QuaternionF
namespace Anemone::Internal
{
    SimdVector4D anemone_vectorcall QuaternionD_Identity();

    SimdVector4D anemone_vectorcall QuaternionD_CreateFromNormalAngle(SimdVector4D normal, double angle);
    SimdVector4D anemone_vectorcall QuaternionD_CreateFromAxisAngle(SimdVector4D axis, double angle);

    SimdVector4D anemone_vectorcall QuaternionD_CreateFromEulerAngles(SimdVector4D pitchYawRoll);
    SimdVector4D anemone_vectorcall QuaternionD_CreateFromEulerAngles(double pitch, double yaw, double roll);

    SimdVector4D anemone_vectorcall QuaternionD_Rotate3(SimdVector4D q, SimdVector4D v);
    SimdVector4D anemone_vectorcall QuaternionD_InverseRotate3(SimdVector4D q, SimdVector4D v);

    SimdVector4D anemone_vectorcall QuaternionD_Length(SimdVector4D q);
    SimdVector4D anemone_vectorcall QuaternionD_LengthSquared(SimdVector4D q);
    SimdVector4D anemone_vectorcall QuaternionD_Normalize(SimdVector4D q);
    SimdVector4D anemone_vectorcall QuaternionD_Dot(SimdVector4D a, SimdVector4D b);
    SimdVector4D anemone_vectorcall QuaternionD_Multiply(SimdVector4D a, SimdVector4D b);
    SimdVector4D anemone_vectorcall QuaternionD_Conjugate(SimdVector4D q);
    SimdVector4D anemone_vectorcall QuaternionD_Inverse(SimdVector4D q);
    void anemone_vectorcall QuaternionD_ToAxisAngle(SimdVector4D& axis, double& angle, SimdVector4D q);

    SimdVector4D anemone_vectorcall QuaternionD_Nlerp(SimdVector4D from, SimdVector4D to, SimdVector4D t);
    SimdVector4D anemone_vectorcall QuaternionD_Nlerp(SimdVector4D from, SimdVector4D to, double t);

    SimdVector4D anemone_vectorcall QuaternionD_Slerp(SimdVector4D from, SimdVector4D to, SimdVector4D t);
    SimdVector4D anemone_vectorcall QuaternionD_Slerp(SimdVector4D from, SimdVector4D to, double t);

    // SimdVector4D anemone_vectorcall QuaternionD_Barycentric(SimdVector4D a, SimdVector4D b, SimdVector4D c, SimdVector4D u, SimdVector4D v);
    // SimdVector4D anemone_vectorcall QuaternionD_Barycentric(SimdVector4D a, SimdVector4D b, SimdVector4D c, double u, double v);

    SimdMask4D anemone_vectorcall QuaternionD_CompareEqual(SimdVector4D a, SimdVector4D b);
    bool anemone_vectorcall QuaternionD_IsEqual(SimdVector4D a, SimdVector4D b);

    SimdMask4D anemone_vectorcall QuaternionD_CompareNotEqual(SimdVector4D a, SimdVector4D b);
    bool anemone_vectorcall QuaternionD_IsNotEqual(SimdVector4D a, SimdVector4D b);

    SimdMask4D anemone_vectorcall QuaternionD_CompareNaN(SimdVector4D q);
    bool anemone_vectorcall QuaternionD_IsNaN(SimdVector4D q);

    SimdMask4D anemone_vectorcall QuaternionD_CompareInfinite(SimdVector4D q);
    bool anemone_vectorcall QuaternionD_IsInfinite(SimdVector4D q);

    SimdMask4D anemone_vectorcall QuaternionD_CompareIdentity(SimdVector4D q);
    bool anemone_vectorcall QuaternionD_IsIdentity(SimdVector4D q);
}

// RotorF
namespace Anemone::Internal
{
    SimdVector4D anemone_vectorcall RotorD_Identity();

    SimdVector4D anemone_vectorcall RotorD_Create(double xy, double xz, double yz, double scalar);
    SimdVector4D anemone_vectorcall RotorD_Create(SimdVector4D bv, double scalar);
    SimdVector4D anemone_vectorcall RotorD_CreateFromAxisAngle(SimdVector4D axis, double angle);
    SimdVector4D anemone_vectorcall RotorD_CreateFromNormalAngle(SimdVector4D normal, double angle);
    SimdVector4D anemone_vectorcall RotorD_RotationBetween(SimdVector4D from, SimdVector4D to);

    SimdVector4D anemone_vectorcall RotorD_Rotate3(SimdVector4D rotor, SimdVector4D v);
    SimdVector4D anemone_vectorcall RotorD_InverseRotate3(SimdVector4D rotor, SimdVector4D v);

    SimdVector4D anemone_vectorcall RotorD_Dot(SimdVector4D a, SimdVector4D b);
    SimdVector4D anemone_vectorcall RotorD_Multiply(SimdVector4D a, SimdVector4D b);
    SimdVector4D anemone_vectorcall RotorD_RotateRotor(SimdVector4D rotor, SimdVector4D rotation);
    SimdVector4D anemone_vectorcall RotorD_Reverse(SimdVector4D rotor);
    SimdVector4D anemone_vectorcall RotorD_Length(SimdVector4D rotor);
    SimdVector4D anemone_vectorcall RotorD_LengthSquared(SimdVector4D rotor);
    SimdVector4D anemone_vectorcall RotorD_Normalize(SimdVector4D rotor);

    SimdMatrix4x4D anemone_vectorcall RotorD_ToMatrix4x4D(SimdVector4D rotor);

    SimdVector4D anemone_vectorcall RotorD_Nlerp(SimdVector4D from, SimdVector4D to, SimdVector4D t);
    SimdVector4D anemone_vectorcall RotorD_Nlerp(SimdVector4D from, SimdVector4D to, double t);

    SimdVector4D anemone_vectorcall RotorD_Slerp(SimdVector4D from, SimdVector4D to, SimdVector4D t);
    SimdVector4D anemone_vectorcall RotorD_Slerp(SimdVector4D from, SimdVector4D to, double t);

    // SimdVector4D anemone_vectorcall RotorD_Barycentric(SimdVector4D a, SimdVector4D b, SimdVector4D c, SimdVector4D u, SimdVector4D v);
    // SimdVector4D anemone_vectorcall RotorD_Barycentric(SimdVector4D a, SimdVector4D b, SimdVector4D c, double u, double v);

    SimdMask4D anemone_vectorcall RotorD_CompareEqual(SimdVector4D a, SimdVector4D b);
    bool anemone_vectorcall RotorD_IsEqual(SimdVector4D a, SimdVector4D b);

    SimdMask4D anemone_vectorcall RotorD_CompareNotEqual(SimdVector4D a, SimdVector4D b);
    bool anemone_vectorcall RotorD_IsNotEqual(SimdVector4D a, SimdVector4D b);

    SimdMask4D anemone_vectorcall RotorD_CompareNaN(SimdVector4D q);
    bool anemone_vectorcall RotorD_IsNaN(SimdVector4D q);

    SimdMask4D anemone_vectorcall RotorD_CompareInfinite(SimdVector4D q);
    bool anemone_vectorcall RotorD_IsInfinite(SimdVector4D q);

    SimdMask4D anemone_vectorcall RotorD_CompareIdentity(SimdVector4D q);
    bool anemone_vectorcall RotorD_IsIdentity(SimdVector4D q);
}

// Matrix4x4D
namespace Anemone::Internal
{
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_LoadAlignedDouble4x4(double const* source);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_LoadAlignedDouble4x3(double const* source);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_LoadAlignedDouble3x4(double const* source);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_LoadUnalignedDouble4x4(double const* source);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_LoadUnalignedDouble4x3(double const* source);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_LoadUnalignedDouble3x4(double const* source);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_LoadUnalignedDouble3x3(double const* source);

    void anemone_vectorcall Matrix4x4D_StoreAlignedDouble4x4(double* destination, SimdMatrix4x4D source);
    void anemone_vectorcall Matrix4x4D_StoreAlignedDouble4x3(double* destination, SimdMatrix4x4D source);
    void anemone_vectorcall Matrix4x4D_StoreAlignedDouble3x4(double* destination, SimdMatrix4x4D source);
    void anemone_vectorcall Matrix4x4D_StoreUnalignedDouble4x4(double* destination, SimdMatrix4x4D source);
    void anemone_vectorcall Matrix4x4D_StoreUnalignedDouble4x3(double* destination, SimdMatrix4x4D source);
    void anemone_vectorcall Matrix4x4D_StoreUnalignedDouble3x4(double* destination, SimdMatrix4x4D source);
    void anemone_vectorcall Matrix4x4D_StoreUnalignedDouble3x3(double* destination, SimdMatrix4x4D source);

    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_Identity();
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_NaN();
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_Infinity();

    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_CreateTranslation(double x, double y, double z);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_CreateTranslation(SimdVector4D translation);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_CreateScale(double x, double y, double z);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_CreateScale(SimdVector4D scale);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_CreateRotationX(double angle);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_CreateRotationY(double angle);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_CreateRotationZ(double angle);

    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_CreateFromQuaternion(SimdVector4D q);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_CreateFromPitchYawRoll(double pitch, double yaw, double roll);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_CreateFromPitchYawRoll(SimdVector4D pitchYawRoll);

    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_CreateAffineTransform2(
        SimdVector4D scaling,
        SimdVector4D rotationOrigin,
        double rotation,
        SimdVector4D translation);

    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_CreateAffineTransform3(
        SimdVector4D scaling,
        SimdVector4D rotationOrigin,
        SimdVector4D rotationQuaternion,
        SimdVector4D translation);

    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_CreateLookAtLH(SimdVector4D eye, SimdVector4D focus, SimdVector4D up);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_CreateLookAtRH(SimdVector4D eye, SimdVector4D focus, SimdVector4D up);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_CreateLookToLH(SimdVector4D eye, SimdVector4D direction, SimdVector4D up);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_CreateLookToRH(SimdVector4D eye, SimdVector4D direction, SimdVector4D up);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_CreatePerspectiveLH(double width, double height, double zNear, double zFar);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_CreatePerspectiveRH(double width, double height, double zNear, double zFar);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_CreatePerspectiveFovLH(double fov, double aspect, double zNear, double zFar);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_CreatePerspectiveFovRH(double fov, double aspect, double zNear, double zFar);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_CreateOrthographicLH(double width, double height, double zNear, double zFar);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_CreateOrthographicRH(double width, double height, double zNear, double zFar);

    template <size_t N>
    SimdVector4D anemone_vectorcall Matrix4x4D_Extract(SimdMatrix4x4D m)
        requires(N < 4);

    template <size_t N>
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_Insert(SimdMatrix4x4D m, SimdVector4D v)
        requires(N < 4);

    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_Multiply(SimdMatrix4x4D a, SimdMatrix4x4D b);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_MultiplyTranspose(SimdMatrix4x4D a, SimdMatrix4x4D b);

    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_Transpose(SimdMatrix4x4D m);
    SimdVector4D anemone_vectorcall Matrix4x4D_Determinant(SimdMatrix4x4D matrix);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_Inverse(SimdMatrix4x4D matrix, SimdVector4D& determinant);

    SimdVector4D anemone_vectorcall Matrix4x4D_Diagonal(SimdMatrix4x4D m);
    SimdVector4D anemone_vectorcall Matrix4x4D_Trace(SimdMatrix4x4D m);
    SimdMatrix4x4D anemone_vectorcall Matrix4x4D_TensorProduct(SimdVector4D a, SimdVector4D b);

    bool anemone_vectorcall Matrix4x4D_IsIdentity(SimdMatrix4x4D m);
    bool anemone_vectorcall Matrix4x4D_IsNaN(SimdMatrix4x4D m);
    bool anemone_vectorcall Matrix4x4D_IsInfinite(SimdMatrix4x4D m);
}

// PlaneF
namespace Anemone::Internal
{
    SimdVector4D anemone_vectorcall PlaneD_Create(double a, double b, double c, double d);
    SimdVector4D anemone_vectorcall PlaneD_CreateFromPointNormal(SimdVector4D point, SimdVector4D normal);
    SimdVector4D anemone_vectorcall PlaneD_CreateFromPoints(SimdVector4D point1, SimdVector4D point2, SimdVector4D point3);
    SimdVector4D anemone_vectorcall PlaneD_Dot(SimdVector4D plane, SimdVector4D point);
    SimdVector4D anemone_vectorcall PlaneD_DotCoord(SimdVector4D plane, SimdVector4D point);
    SimdVector4D anemone_vectorcall PlaneD_DotNormal(SimdVector4D plane, SimdVector4D point);
    SimdVector4D anemone_vectorcall PlaneD_Normalize(SimdVector4D plane);
    SimdVector4D anemone_vectorcall PlaneD_IntersectLine(SimdVector4D plane, SimdVector4D linePoint1, SimdVector4D linePoint2);
    void anemone_vectorcall PlaneD_IntersectPlane(SimdVector4D& linePoint1, SimdVector4D& linePoint2, SimdVector4D plane1, SimdVector4D plane2);
    SimdVector4D anemone_vectorcall PlaneD_Transform(SimdVector4D plane, SimdMatrix4x4D matrix);

    SimdMask4D anemone_vectorcall PlaneD_CompareEqual(SimdVector4D a, SimdVector4D b);
    bool anemone_vectorcall PlaneD_IsEqual(SimdVector4D a, SimdVector4D b);
    SimdMask4D anemone_vectorcall PlaneD_CompareNearEqual(SimdVector4D a, SimdVector4D b, SimdVector4D tolerance);
    SimdMask4D anemone_vectorcall PlaneD_CompareNotEqual(SimdVector4D a, SimdVector4D b);
    bool anemone_vectorcall PlaneD_IsNotEqual(SimdVector4D a, SimdVector4D b);
    SimdMask4D anemone_vectorcall PlaneD_CompareNaN(SimdVector4D q);
    bool anemone_vectorcall PlaneD_IsNaN(SimdVector4D q);
    SimdMask4D anemone_vectorcall PlaneD_CompareInfinite(SimdVector4D q);
    bool anemone_vectorcall PlaneD_IsInfinite(SimdVector4D q);
}

// Bezier4D
namespace Anemone::Internal
{
    struct Bezier4DA
    {
        SimdVector4D Point0;
        SimdVector4D Point1;
        SimdVector4D Point2;
        SimdVector4D Point3;
    };

    // TODO: This should be implemented using special structs instead of arrays
    inline void anemone_vectorcall Vector4D_SplitBezier(
        Bezier4DA& left,
        Bezier4DA& right,
        Bezier4DA input,
        double t)
    {
        SimdVector4D const p01 = Vector4D_Lerp(input.Point0, input.Point1, t);
        SimdVector4D const p12 = Vector4D_Lerp(input.Point1, input.Point2, t);
        SimdVector4D const p23 = Vector4D_Lerp(input.Point2, input.Point3, t);

        SimdVector4D const p012 = Vector4D_Lerp(p01, p12, t);
        SimdVector4D const p123 = Vector4D_Lerp(p12, p23, t);

        SimdVector4D const p0123 = Vector4D_Lerp(p012, p123, t);

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
        double Distance;
        double Value;
    };

    inline SimdVector4D anemone_vectorcall Bezier4_CalculateLengthSegments(
        SimdVector4D p0,
        SimdVector4D p1,
        SimdVector4D p2,
        SimdVector4D p3,
        DistanceToInterpolation* lookup,
        size_t resolution)
    {
        SimdVector4D length = Vector4D_Zero();
        SimdVector4D previous = Vector4D_BezierPosition(p0, p1, p2, p3, 0.0f);
        lookup[0] = DistanceToInterpolation{0.0f, 0.0f};

        for (size_t i = 1; i < resolution; ++i)
        {
            double const t = static_cast<double>(i) / static_cast<double>(resolution - 1);
            SimdVector4D const point = Vector4D_BezierPosition(p0, p1, p2, p3, t);
            SimdVector4D const direction = Vector4D_Subtract(point, previous);
            length = Vector4D_Add(length, Vector4D_Length3(direction));
            lookup[i] = DistanceToInterpolation{Vector4D_Extract<0>(length), t};
            previous = point;
        }

        return length;
    }

    anemone_noinline inline SimdVector4D anemone_vectorcall Bezier4_CalculateLength(
        SimdVector4D p0,
        SimdVector4D p1,
        SimdVector4D p2,
        SimdVector4D p3,
        size_t resolution)
    {
        SimdVector4D length = Vector4D_Zero();
        SimdVector4D previous = Vector4D_BezierPosition(p0, p1, p2, p3, 0.0f);

        for (size_t i = 1; i < resolution; ++i)
        {
            double const t = static_cast<double>(i) / static_cast<double>(resolution - 1);
            SimdVector4D const point = Vector4D_BezierPosition(p0, p1, p2, p3, t);
            SimdVector4D const direction = Vector4D_Subtract(point, previous);
            length = Vector4D_Add(length, Vector4D_Length3(direction));
            previous = point;
        }

        return length;
    }
}


#include "AnemoneMath/Detail/SimdDouble.inl"
