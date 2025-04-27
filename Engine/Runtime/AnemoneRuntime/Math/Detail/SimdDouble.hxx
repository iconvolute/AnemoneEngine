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
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

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

    struct SimdVector4D final
    {
        float64x2_t Inner[2];
    };

    struct SimdMask4D final
    {
        uint64x2_t Inner[2];
    };

    struct SimdMatrix4x4D final
    {
        float64x2x4_t Inner[2];
    };

#else
#error "Not implemented"
#endif
}

#include "AnemoneRuntime/Math/Detail/SimdDouble.inl"
