#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"

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
#include "AnemoneRuntime/Diagnostics/Debug.hxx"

namespace Anemone::Math::Detail
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

namespace Anemone::Math::Detail
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
}

#include "AnemoneRuntime/Math/Detail/SimdDouble.inl"
