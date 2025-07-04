// ReSharper disable CppClangTidyClangDiagnosticFloatEqual
// ReSharper disable CppPassValueParameterByConstReference
#pragma once
#include "AnemoneRuntime/Math/Detail/SimdFloat.hxx"

#if ANEMONE_BUILD_DISABLE_SIMD

namespace Anemone::Math::Detail
{
    inline constexpr uint32_t ScalarMaskComponentUInt32{0xFFFF'FFFF};

    inline float ScalarHelper_SelectBranchless(uint32_t mask, float whenFalse, float whenTrue)
    {
        uint32_t const bitsWhenFalse = std::bit_cast<uint32_t>(whenFalse);
        uint32_t const bitsWhenTrue = std::bit_cast<uint32_t>(whenTrue);
        uint32_t const bitsResult = (mask & bitsWhenTrue) | (~mask & bitsWhenFalse);
        return std::bit_cast<float>(bitsResult);
    }
}

#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2

namespace Anemone::Math::Detail
{
    inline constexpr int32_t AvxMaskComponentInt32{-1};

    anemone_forceinline __m128i AvxCompareLessEqualI32(__m128i a, __m128i b)
    {
        // = [a < b]
        __m128i const m0 = _mm_cmpgt_epi32(b, a);
        // = [a == b]
        __m128i const m1 = _mm_cmpeq_epi32(b, a);
        return _mm_xor_si128(m0, m1);
    }

    anemone_forceinline __m128i AvxCompareSignI32(__m128i a, __m128i b)
    {
        return _mm_cmpeq_epi32(_mm_srli_epi32(a, 31), _mm_srli_epi32(b, 31));
    }

    anemone_forceinline __m256i AvxCompareSignI64(__m256i a, __m256i b)
    {
        return _mm256_cmpeq_epi64(_mm256_srli_epi64(a, 63), _mm256_srli_epi64(b, 63));
    }

    anemone_forceinline __m128i AvxCompareNearZeroPreciseI32(__m128i v)
    {
        __m128i const ulpTolerance = _mm_set1_epi32(4);
        __m128i const ulpValue = _mm_abs_epi32(v);
        return AvxCompareLessEqualI32(ulpValue, ulpTolerance);
    }
}

#elif ANEMONE_FEATURE_NEON

namespace Anemone::Math::Detail
{
    template <size_t Lane0, size_t Lane1>
    float32x2_t anemone_vectorcall NeonShuffle(float32x2_t v)
        requires((Lane0 < 2) and (Lane1 < 2));

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<0, 1>(float32x2_t v)
    {
        return v;
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<1, 0>(float32x2_t v)
    {
        return vrev64_f32(v);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<0, 0>(float32x2_t v)
    {
        return vdup_lane_f32(v, 0);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<1, 1>(float32x2_t v)
    {
        return vdup_lane_f32(v, 1);
    }

    template <size_t X, size_t Y>
    inline float32x2_t anemone_vectorcall NeonShuffle(float32x2_t xy, float32x2_t zw)
        requires((X < 4) and (Y < 4))
    = delete;

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<0, 0>(float32x2_t xy, float32x2_t zw)
    {
        (void)zw;
        return vdup_lane_f32(xy, 0);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<0, 1>(float32x2_t xy, float32x2_t zw)
    {
        (void)zw;
        return xy;
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<0, 2>(float32x2_t xy, float32x2_t zw)
    {
        return vtrn1_f32(xy, zw);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<0, 3>(float32x2_t xy, float32x2_t zw)
    {
        return vcopy_lane_f32(xy, 1, zw, 1);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<1, 0>(float32x2_t xy, float32x2_t zw)
    {
        (void)zw;
        return vrev64_f32(xy);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<1, 1>(float32x2_t xy, float32x2_t zw)
    {
        (void)zw;
        return vdup_lane_f32(xy, 1);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<1, 2>(float32x2_t xy, float32x2_t zw)
    {
        return vext_f32(xy, zw, 1);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<1, 3>(float32x2_t xy, float32x2_t zw)
    {
        return vtrn2_f32(xy, zw);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<2, 0>(float32x2_t xy, float32x2_t zw)
    {
        return vtrn1_f32(zw, xy);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<2, 1>(float32x2_t xy, float32x2_t zw)
    {
        return vcopy_lane_f32(zw, 1, xy, 1);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<2, 2>(float32x2_t xy, float32x2_t zw)
    {
        (void)xy;
        return vdup_lane_f32(zw, 0);
    }
    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<2, 3>(float32x2_t xy, float32x2_t zw)
    {
        (void)xy;
        return zw;
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<3, 0>(float32x2_t xy, float32x2_t zw)
    {
        return vext_f32(zw, xy, 1);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<3, 1>(float32x2_t xy, float32x2_t zw)
    {
        return vtrn2_f32(zw, xy);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<3, 2>(float32x2_t xy, float32x2_t zw)
    {
        (void)xy;
        return vrev64_f32(zw);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<3, 3>(float32x2_t xy, float32x2_t zw)
    {
        (void)xy;
        return vdup_lane_f32(zw, 1);
    }

    template <size_t Lane0, size_t Lane1, size_t Lane2, size_t Lane3>
    float32x4_t NeonShuffle(float32x4_t v)
        requires((Lane0 < 4) and (Lane1 < 4) and (Lane2 < 4) and (Lane3 < 4))
    {
        float32x4_t const r0 = vmovq_n_f32(vgetq_lane_f32(v, Lane0));
        float32x4_t const r1 = vsetq_lane_f32(vgetq_lane_f32(v, Lane1), r0, 1);
        float32x4_t const r2 = vsetq_lane_f32(vgetq_lane_f32(v, Lane2), r1, 2);
        float32x4_t const result = vsetq_lane_f32(vgetq_lane_f32(v, Lane3), r2, 3);
        return result;
    }

    template <>
    inline float32x4_t NeonShuffle<0, 1, 2, 3>(float32x4_t v) { return v; }

    template <>
    inline float32x4_t NeonShuffle<0, 0, 0, 0>(float32x4_t v) { return vdupq_laneq_f32(v, 0); }

    template <>
    inline float32x4_t NeonShuffle<1, 1, 1, 1>(float32x4_t v) { return vdupq_laneq_f32(v, 1); }

    template <>
    inline float32x4_t NeonShuffle<2, 2, 2, 2>(float32x4_t v) { return vdupq_laneq_f32(v, 2); }

    template <>
    inline float32x4_t NeonShuffle<3, 3, 3, 3>(float32x4_t v) { return vdupq_laneq_f32(v, 3); }

    template <>
    inline float32x4_t NeonShuffle<0, 1, 0, 1>(float32x4_t v)
    {
        float32x2_t const vxy = vget_low_f32(v);
        return vcombine_f32(vxy, vxy);
    }

    template <>
    inline float32x4_t NeonShuffle<2, 3, 2, 3>(float32x4_t v)
    {
        float32x2_t const vzw = vget_high_f32(v);
        return vcombine_f32(vzw, vzw);
    }

    template <>
    inline float32x4_t NeonShuffle<1, 0, 3, 2>(float32x4_t v) { return vrev64q_f32(v); }

    template <>
    inline float32x4_t NeonShuffle<0, 2, 0, 2>(float32x4_t v) { return vuzp1q_f32(v, v); }

    template <>
    inline float32x4_t NeonShuffle<1, 3, 1, 3>(float32x4_t v) { return vuzp2q_f32(v, v); }


    template <>
    inline float32x4_t NeonShuffle<1, 0, 1, 0>(float32x4_t v)
    {
        float32x2_t vt = vrev64_f32(vget_low_f32(v));
        return vcombine_f32(vt, vt);
    }

    template <>
    inline float32x4_t NeonShuffle<3, 2, 3, 2>(float32x4_t v)
    {
        float32x2_t vt = vrev64_f32(vget_high_f32(v));
        return vcombine_f32(vt, vt);
    }

    template <>
    inline float32x4_t NeonShuffle<0, 1, 3, 2>(float32x4_t v) { return vcombine_f32(vget_low_f32(v), vrev64_f32(vget_high_f32(v))); }
    template <>
    inline float32x4_t NeonShuffle<1, 0, 2, 3>(float32x4_t v) { return vcombine_f32(vrev64_f32(vget_low_f32(v)), vget_high_f32(v)); }
    template <>
    inline float32x4_t NeonShuffle<2, 3, 1, 0>(float32x4_t v) { return vcombine_f32(vget_high_f32(v), vrev64_f32(vget_low_f32(v))); }
    template <>
    inline float32x4_t NeonShuffle<3, 2, 0, 1>(float32x4_t v) { return vcombine_f32(vrev64_f32(vget_high_f32(v)), vget_low_f32(v)); }
    template <>
    inline float32x4_t NeonShuffle<3, 2, 1, 0>(float32x4_t v) { return vcombine_f32(vrev64_f32(vget_high_f32(v)), vrev64_f32(vget_low_f32(v))); }

    template <>
    inline float32x4_t NeonShuffle<0, 0, 2, 2>(float32x4_t v) { return vtrn1q_f32(v, v); }
    template <>
    inline float32x4_t NeonShuffle<1, 1, 3, 3>(float32x4_t v) { return vtrn2q_f32(v, v); }

    template <>
    inline float32x4_t NeonShuffle<0, 0, 1, 1>(float32x4_t v) { return vzip1q_f32(v, v); }
    template <>
    inline float32x4_t NeonShuffle<2, 2, 3, 3>(float32x4_t v) { return vzip2q_f32(v, v); }

    template <>
    inline float32x4_t NeonShuffle<1, 2, 3, 0>(float32x4_t v) { return vextq_f32(v, v, 1); }
    template <>
    inline float32x4_t NeonShuffle<2, 3, 0, 1>(float32x4_t v) { return vextq_f32(v, v, 2); }
    template <>
    inline float32x4_t NeonShuffle<3, 0, 1, 2>(float32x4_t v) { return vextq_f32(v, v, 3); }

    template <>
    inline float32x4_t NeonShuffle<1, 0, 0, 0>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 1), vdupq_n_f32(vgetq_lane_f32(v, 0)), 0); }
    template <>
    inline float32x4_t NeonShuffle<2, 0, 0, 0>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 2), vdupq_n_f32(vgetq_lane_f32(v, 0)), 0); }
    template <>
    inline float32x4_t NeonShuffle<3, 0, 0, 0>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 3), vdupq_n_f32(vgetq_lane_f32(v, 0)), 0); }

    template <>
    inline float32x4_t NeonShuffle<0, 1, 1, 1>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 0), vdupq_n_f32(vgetq_lane_f32(v, 1)), 0); }
    template <>
    inline float32x4_t NeonShuffle<2, 1, 1, 1>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 2), vdupq_n_f32(vgetq_lane_f32(v, 1)), 0); }
    template <>
    inline float32x4_t NeonShuffle<3, 1, 1, 1>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 3), vdupq_n_f32(vgetq_lane_f32(v, 1)), 0); }

    template <>
    inline float32x4_t NeonShuffle<0, 2, 2, 2>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 0), vdupq_n_f32(vgetq_lane_f32(v, 2)), 0); }
    template <>
    inline float32x4_t NeonShuffle<1, 2, 2, 2>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 1), vdupq_n_f32(vgetq_lane_f32(v, 2)), 0); }
    template <>
    inline float32x4_t NeonShuffle<3, 2, 2, 2>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 3), vdupq_n_f32(vgetq_lane_f32(v, 2)), 0); }

    template <>
    inline float32x4_t NeonShuffle<0, 3, 3, 3>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 0), vdupq_n_f32(vgetq_lane_f32(v, 3)), 0); }
    template <>
    inline float32x4_t NeonShuffle<1, 3, 3, 3>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 1), vdupq_n_f32(vgetq_lane_f32(v, 3)), 0); }
    template <>
    inline float32x4_t NeonShuffle<2, 3, 3, 3>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 2), vdupq_n_f32(vgetq_lane_f32(v, 3)), 0); }

    template <>
    inline float32x4_t NeonShuffle<0, 1, 0, 0>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 1), vdupq_n_f32(vgetq_lane_f32(v, 0)), 1); }
    template <>
    inline float32x4_t NeonShuffle<0, 2, 0, 0>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 2), vdupq_n_f32(vgetq_lane_f32(v, 0)), 1); }
    template <>
    inline float32x4_t NeonShuffle<0, 3, 0, 0>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 3), vdupq_n_f32(vgetq_lane_f32(v, 0)), 1); }

    template <>
    inline float32x4_t NeonShuffle<1, 0, 1, 1>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 0), vdupq_n_f32(vgetq_lane_f32(v, 1)), 1); }
    template <>
    inline float32x4_t NeonShuffle<1, 2, 1, 1>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 2), vdupq_n_f32(vgetq_lane_f32(v, 1)), 1); }
    template <>
    inline float32x4_t NeonShuffle<1, 3, 1, 1>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 3), vdupq_n_f32(vgetq_lane_f32(v, 1)), 1); }

    template <>
    inline float32x4_t NeonShuffle<2, 0, 2, 2>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 0), vdupq_n_f32(vgetq_lane_f32(v, 2)), 1); }
    template <>
    inline float32x4_t NeonShuffle<2, 1, 2, 2>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 1), vdupq_n_f32(vgetq_lane_f32(v, 2)), 1); }
    template <>
    inline float32x4_t NeonShuffle<2, 3, 2, 2>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 3), vdupq_n_f32(vgetq_lane_f32(v, 2)), 1); }

    template <>
    inline float32x4_t NeonShuffle<3, 0, 3, 3>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 0), vdupq_n_f32(vgetq_lane_f32(v, 3)), 1); }
    template <>
    inline float32x4_t NeonShuffle<3, 1, 3, 3>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 1), vdupq_n_f32(vgetq_lane_f32(v, 3)), 1); }
    template <>
    inline float32x4_t NeonShuffle<3, 2, 3, 3>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 2), vdupq_n_f32(vgetq_lane_f32(v, 3)), 1); }

    template <>
    inline float32x4_t NeonShuffle<0, 0, 1, 0>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 1), vdupq_n_f32(vgetq_lane_f32(v, 0)), 2); }
    template <>
    inline float32x4_t NeonShuffle<0, 0, 2, 0>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 2), vdupq_n_f32(vgetq_lane_f32(v, 0)), 2); }
    template <>
    inline float32x4_t NeonShuffle<0, 0, 3, 0>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 3), vdupq_n_f32(vgetq_lane_f32(v, 0)), 2); }

    template <>
    inline float32x4_t NeonShuffle<1, 1, 0, 1>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 0), vdupq_n_f32(vgetq_lane_f32(v, 1)), 2); }
    template <>
    inline float32x4_t NeonShuffle<1, 1, 2, 1>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 2), vdupq_n_f32(vgetq_lane_f32(v, 1)), 2); }
    template <>
    inline float32x4_t NeonShuffle<1, 1, 3, 1>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 3), vdupq_n_f32(vgetq_lane_f32(v, 1)), 2); }

    template <>
    inline float32x4_t NeonShuffle<2, 2, 0, 2>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 0), vdupq_n_f32(vgetq_lane_f32(v, 2)), 2); }
    template <>
    inline float32x4_t NeonShuffle<2, 2, 1, 2>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 1), vdupq_n_f32(vgetq_lane_f32(v, 2)), 2); }
    template <>
    inline float32x4_t NeonShuffle<2, 2, 3, 2>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 3), vdupq_n_f32(vgetq_lane_f32(v, 2)), 2); }

    template <>
    inline float32x4_t NeonShuffle<3, 3, 0, 3>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 0), vdupq_n_f32(vgetq_lane_f32(v, 3)), 2); }
    template <>
    inline float32x4_t NeonShuffle<3, 3, 1, 3>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 1), vdupq_n_f32(vgetq_lane_f32(v, 3)), 2); }
    template <>
    inline float32x4_t NeonShuffle<3, 3, 2, 3>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 2), vdupq_n_f32(vgetq_lane_f32(v, 3)), 2); }

    template <>
    inline float32x4_t NeonShuffle<0, 0, 0, 1>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 1), vdupq_n_f32(vgetq_lane_f32(v, 0)), 3); }
    template <>
    inline float32x4_t NeonShuffle<0, 0, 0, 2>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 2), vdupq_n_f32(vgetq_lane_f32(v, 0)), 3); }
    template <>
    inline float32x4_t NeonShuffle<0, 0, 0, 3>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 3), vdupq_n_f32(vgetq_lane_f32(v, 0)), 3); }

    template <>
    inline float32x4_t NeonShuffle<1, 1, 1, 0>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 0), vdupq_n_f32(vgetq_lane_f32(v, 1)), 3); }
    template <>
    inline float32x4_t NeonShuffle<1, 1, 1, 2>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 2), vdupq_n_f32(vgetq_lane_f32(v, 1)), 3); }
    template <>
    inline float32x4_t NeonShuffle<1, 1, 1, 3>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 3), vdupq_n_f32(vgetq_lane_f32(v, 1)), 3); }

    template <>
    inline float32x4_t NeonShuffle<2, 2, 2, 0>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 0), vdupq_n_f32(vgetq_lane_f32(v, 2)), 3); }
    template <>
    inline float32x4_t NeonShuffle<2, 2, 2, 1>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 1), vdupq_n_f32(vgetq_lane_f32(v, 2)), 3); }
    template <>
    inline float32x4_t NeonShuffle<2, 2, 2, 3>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 3), vdupq_n_f32(vgetq_lane_f32(v, 2)), 3); }

    template <>
    inline float32x4_t NeonShuffle<3, 3, 3, 0>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 0), vdupq_n_f32(vgetq_lane_f32(v, 3)), 3); }
    template <>
    inline float32x4_t NeonShuffle<3, 3, 3, 1>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 1), vdupq_n_f32(vgetq_lane_f32(v, 3)), 3); }
    template <>
    inline float32x4_t NeonShuffle<3, 3, 3, 2>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 2), vdupq_n_f32(vgetq_lane_f32(v, 3)), 3); }

    inline float32x2_t anemone_vectorcall NeonDot2(float32x2_t v1, float32x2_t v2)
    {
        float32x2_t const vxy = vmul_f32(v1, v2);
        return vpadd_f32(vxy, vxy);
    }

    inline float32x2_t anemone_vectorcall NeonDot2(float32x2_t v)
    {
        float32x2_t const vxy = vmul_f32(v, v);
        return vpadd_f32(vxy, vxy);
    }

    inline float32x2_t anemone_vectorcall NeonDot3(float32x4_t v1, float32x4_t v2)
    {
        // = [xm, ym, zm, _]
        float32x4_t const vxyz = vmulq_f32(v1, v2);
        // = [xm, ym]
        float32x2_t const vxy = vget_low_f32(vxyz);
        // = [zm, _]
        float32x2_t const vzw = vget_high_f32(vxyz);
        // = [x+y, x+y]
        float32x2_t const vxy_xy = vpadd_f32(vxy, vxy);
        // = [z, z]
        float32x2_t const vzz_zz = vdup_lane_f32(vzw, 0);
        // = [x+y+z, x+y+z]
        float32x2_t const vr = vadd_f32(vxy_xy, vzz_zz);
        // = [x+y+z, ...]
        return vr;
    }

    inline float32x2_t anemone_vectorcall NeonDot3(float32x4_t v)
    {
        // = [xm, ym, zm, _]
        float32x4_t const vxyz = vmulq_f32(v, v);
        // = [xm, ym]
        float32x2_t const vxy = vget_low_f32(vxyz);
        // = [zm, _]
        float32x2_t const vzw = vget_high_f32(vxyz);
        // = [x+y, x+y]
        float32x2_t const vxy_xy = vpadd_f32(vxy, vxy);
        // = [z, z]
        float32x2_t const vzz_zz = vdup_lane_f32(vzw, 0);
        // = [x+y+z, x+y+z]
        float32x2_t const vr = vadd_f32(vxy_xy, vzz_zz);
        // = [x+y+z, ...]
        return vr;
    }

    inline float32x2_t anemone_vectorcall NeonDot4(float32x4_t v1, float32x4_t v2)
    {
        // = [x, y, z, w]
        float32x4_t const vxyzw = vmulq_f32(v1, v2);
        // = [x+y, z+w]
        float32x2_t const vxy = vget_low_f32(vpaddq_f32(vxyzw, vxyzw));
        // = [x+y+z+w, ...]
        float32x2_t const vr = vpadd_f32(vxy, vxy);
        return vr;
    }

    inline float32x2_t anemone_vectorcall NeonDot4(float32x4_t v)
    {
        // = [x, y, z, w]
        float32x4_t const vxyzw = vmulq_f32(v, v);
        // = [x+y, z+w]
        float32x2_t const vxy = vget_low_f32(vpaddq_f32(vxyzw, vxyzw));
        // = [x+y+z+w, ...]
        float32x2_t const vr = vpadd_f32(vxy, vxy);
        return vr;
    }

    inline float32x2_t anemone_vectorcall NeonSquareRoot2xNewtonRhapson(float32x2_t v)
    {
        // = [0, 0]
        float32x2_t const zero = vdup_n_f32(0.0f);
        // = [v.x == 0, v.y == 0]
        uint32x2_t const mask_zero = vceq_f32(v, zero);
        // = sqrt_est(v)
        float32x2_t const s0 = vrsqrte_f32(v);
        float32x2_t const p0 = vmul_f32(v, s0);
        float32x2_t const r0 = vrsqrts_f32(p0, s0);
        float32x2_t const s1 = vmul_f32(s0, r0);
        float32x2_t const p1 = vmul_f32(v, s1);
        float32x2_t const r1 = vrsqrts_f32(p1, s1);
        float32x2_t const s2 = vmul_f32(s1, r1);
        float32x2_t const result = vmul_f32(v, s2);
        return vbsl_f32(mask_zero, zero, result);
    }

    inline float32x4_t anemone_vectorcall NeonSquareRoot2xNewtonRhapson(float32x4_t v)
    {
        // = [0, 0]
        float32x4_t const zero = vdupq_n_f32(0.0f);
        // = [v.x == 0, v.y == 0]
        uint32x4_t const mask_zero = vceqq_f32(v, zero);
        // = sqrt_est(v)
        float32x4_t const s0 = vrsqrteq_f32(v);
        float32x4_t const p0 = vmulq_f32(v, s0);
        float32x4_t const r0 = vrsqrtsq_f32(p0, s0);
        float32x4_t const s1 = vmulq_f32(s0, r0);
        float32x4_t const p1 = vmulq_f32(v, s1);
        float32x4_t const r1 = vrsqrtsq_f32(p1, s1);
        float32x4_t const s2 = vmulq_f32(s1, r1);
        float32x4_t const result = vmulq_f32(v, s2);
        return vbslq_f32(mask_zero, zero, result);
    }

    inline float32x2_t anemone_vectorcall NeonReciprocalSquareRoot2xNewtonRhapson(float32x2_t v)
    {
        float32x2_t const s0 = vrsqrte_f32(v);
        float32x2_t const p0 = vmul_f32(v, s0);
        float32x2_t const r0 = vrsqrts_f32(p0, s0);

        float32x2_t const s1 = vmul_f32(s0, r0);
        float32x2_t const p1 = vmul_f32(v, s1);
        float32x2_t const r1 = vrsqrts_f32(p1, s1);
        return vmul_f32(s1, r1);
    }

    inline float32x4_t anemone_vectorcall NeonReciprocalSquareRoot2xNewtonRhapson(float32x4_t v)
    {
        float32x4_t const s0 = vrsqrteq_f32(v);
        float32x4_t const p0 = vmulq_f32(v, s0);
        float32x4_t const r0 = vrsqrtsq_f32(p0, s0);

        float32x4_t const s1 = vmulq_f32(s0, r0);
        float32x4_t const p1 = vmulq_f32(v, s1);
        float32x4_t const r1 = vrsqrtsq_f32(p1, s1);
        return vmulq_f32(s1, r1);
    }

    inline uint32_t anemone_vectorcall NeonExtractMask4(uint32x4_t m)
    {
        // = [v & 1 << 31]
        uint32x4_t const masked = vandq_u32(m, vld1q_u32(F32x4_SignMask_XXXX.As<uint32_t>()));
        // = [v >> 31]
        uint32x4_t const shifted = vshlq_u32(masked, vld1q_s32(I32x4_MoveMaskShifts.As<int32_t>()));
        return vaddvq_u32(shifted);
    }

#if ANEMONE_COMPILER_MSVC
#pragma float_control(push)
#pragma float_control(precise, on)
#endif

    inline uint32x4_t anemone_vectorcall NeonCompareNaN(float32x4_t v)
    {
#if false
        uint32x4_t const mask = vceqq_f32(v, v);
        return vmvnq_u32(mask);
#else
        // = [abs(x) as u32, ...]
        uint32x4_t const absValue = vandq_u32(vreinterpretq_u32_f32(v), vdupq_n_u32(~Float32::SignMask));
        // = [v > positiveInfinityBits, ...]
        return vcgtq_u32(absValue, vdupq_n_u32(Float32::PositiveInfinityBits));
#endif
    }

#if ANEMONE_COMPILER_MSVC
#pragma float_control(pop)
#endif

    inline uint32_t anemone_vectorcall NeonExtractMask2(uint32x2_t m)
    {
        // = [v & 1 << 31]
        uint32x2_t const masked = vand_u32(m, vld1_u32(F32x4_SignMask_XXXX.As<uint32_t>()));
        // = [v >> 31]
        uint32x2_t const shifted = vshl_u32(masked, vld1_s32(I32x4_MoveMaskShifts.As<int32_t>()));
        return vaddv_u32(shifted);
    }

    anemone_forceinline uint32x4_t anemone_vectorcall NeonCreateMask(bool x, bool y, bool z, bool w)
    {
        // = [x, y, z, w] as int32x4
        int32_t const fr[4]{
            static_cast<int>(x),
            static_cast<int>(y),
            static_cast<int>(z),
            static_cast<int>(w),
        };
        int32x4_t const r = vld1q_s32(fr);

        // = [r > 0]
        return vcgtzq_s32(r);
    }

    anemone_forceinline uint32x4_t anemone_vectorcall NeonCompareSignInt32x4(int32x4_t a, int32x4_t b)
    {
        return vceqq_s32(vshrq_n_s32(a, 31), vshrq_n_s32(b, 31));
    }

    anemone_forceinline uint32x2_t anemone_vectorcall NeonCompareSignInt32x2(int32x2_t a, int32x2_t b)
    {
        return vceq_s32(vshr_n_s32(a, 31), vshr_n_s32(b, 31));
    }

    anemone_forceinline uint64x2_t anemone_vectorcall NeonCompareSignInt64x2(int64x2_t a, int64x2_t b)
    {
        return vceqq_s64(vshrq_n_s64(a, 63), vshrq_n_s64(b, 63));
    }

    anemone_forceinline uint32x4_t anemone_vectorcall NeonCompareNearZeroPrecise(int32x4_t v)
    {
        int32x4_t const ulpTolerance = vdupq_n_s32(4);
        int32x4_t const ulpValue = vabsq_s32(v);
        return vcleq_s32(ulpValue, ulpTolerance);
    }

    anemone_forceinline uint32x2_t anemone_vectorcall NeonCompareNearZeroPreciseInt32x2(int32x2_t v)
    {
        int32x2_t const ulpTolerance = vdup_n_s32(4);
        int32x2_t const ulpValue = vabs_s32(v);
        return vcle_s32(ulpValue, ulpTolerance);
    }

    anemone_forceinline uint64x2_t anemone_vectorcall NeonCompareNearZeroPreciseInt64x2(int64x2_t v)
    {
        int64x2_t const ulpTolerance = vdupq_n_s64(4);
        int64x2_t const ulpValue = vabsq_s64(v);
        return vcleq_s64(ulpValue, ulpTolerance);
    }

    anemone_forceinline uint32x4_t anemone_vectorcall NeonCompareInBoundsFloat32x4(float32x4_t v, float32x4_t lower, float32x4_t upper)
    {
        uint32x4_t const maskLower = vcleq_f32(lower, v);
        uint32x4_t const maskUpper = vcleq_f32(v, upper);
        return vandq_u32(maskLower, maskUpper);
    }

    anemone_forceinline uint32x2_t anemone_vectorcall NeonCompareInBoundsFloat32x2(float32x2_t v, float32x2_t lower, float32x2_t upper)
    {
        uint32x2_t const maskLower = vcle_f32(lower, v);
        uint32x2_t const maskUpper = vcle_f32(v, upper);
        return vand_u32(maskLower, maskUpper);
    }

    anemone_forceinline uint32x4_t anemone_vectorcall NeonCompareInBoundsFloat32x4(float32x4_t v, float32x4_t bounds)
    {
        uint32x4_t const maskUpper = vcleq_f32(v, bounds);
        uint32x4_t const maskLower = vcleq_f32(vnegq_f32(bounds), v);
        return vandq_u32(maskLower, maskUpper);
    }

    anemone_forceinline uint32x2_t anemone_vectorcall NeonCompareInBoundsFloat32x2(float32x2_t v, float32x2_t bounds)
    {
        uint32x2_t const maskUpper = vcle_f32(v, bounds);
        uint32x2_t const maskLower = vcle_f32(vneg_f32(bounds), v);
        return vand_u32(maskLower, maskUpper);
    }

    inline constexpr uint32_t NeonMaskComponentUInt32{0xFFFF'FFFFu};
}

#endif

// Vector4F
namespace Anemone::Math::Detail
{
    inline SimdVector4F anemone_vectorcall Vector4F_LoadAlignedFloat4(float const* source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{source[0], source[1], source[2], source[3]};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_load_ps(source);
#elif ANEMONE_FEATURE_NEON
        return vld1q_f32(source);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LoadAlignedFloat3(float const* source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{source[0], source[1], source[2], 0.0f};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_load_ps(source);
#elif ANEMONE_FEATURE_NEON
        return vld1q_f32(source);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LoadAlignedFloat2(float const* source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{source[0], source[1], 0.0f, 0.0f};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_load_ps(source);
#elif ANEMONE_FEATURE_NEON
        return vld1q_f32(source);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LoadAlignedFloat1(float const* source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{source[0], 0.0f, 0.0f, 0.0f};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_load_ps(source);
#elif ANEMONE_FEATURE_NEON
        return vld1q_f32(source);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LoadUnalignedFloat4(float const* source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{source[0], source[1], source[2], source[3]};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_loadu_ps(source);
#elif ANEMONE_FEATURE_NEON
        return vld1q_f32(source);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LoadUnalignedFloat3(float const* source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{source[0], source[1], source[2], 0.0f};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // = [x, y, 0, 0]
        __m128 const v_xy00 = _mm_castsi128_ps(_mm_loadl_epi64(reinterpret_cast<__m128i const*>(source)));
        // = [z, 0, 0, 0]
        __m128 const v_z000 = _mm_load_ss(source + 2);
        return _mm_movelh_ps(v_xy00, v_z000);
#elif ANEMONE_FEATURE_NEON
        // = [x, y]
        float32x2_t const xy = vld1_f32(source);
        // = [z, 0]
        float32x2_t const z0 = vld1_lane_f32(source + 2, vdup_n_f32(0.0f), 0);
        return vcombine_f32(xy, z0);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LoadUnalignedFloat2(float const* source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{source[0], source[1], 0.0f, 0.0f};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // = [x, y, 0, 0]
        return _mm_castsi128_ps(_mm_loadl_epi64(reinterpret_cast<__m128i const*>(source)));
#elif ANEMONE_FEATURE_NEON
        // = [x, y]
        float32x2_t const xy = vld1_f32(source);
        // = [0, 0]
        float32x2_t const zero = vdup_n_f32(0.0f);
        return vcombine_f32(xy, zero);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LoadUnalignedFloat1(float const* source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{source[0], 0.0f, 0.0f, 0.0f};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_load_ss(source);
#elif ANEMONE_FEATURE_NEON
        // = [x, 0, 0, 0]
        return vld1q_lane_f32(source, vdupq_n_f32(0.0f), 0);
#endif
    }

    inline void anemone_vectorcall Vector4F_StoreAlignedFloat4(float* destination, SimdVector4F source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        destination[0] = source.Inner[0];
        destination[1] = source.Inner[1];
        destination[2] = source.Inner[2];
        destination[3] = source.Inner[3];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        _mm_store_ps(destination, source);
#elif ANEMONE_FEATURE_NEON
        vst1q_f32(destination, source);
#endif
    }

    inline void anemone_vectorcall Vector4F_StoreAlignedFloat3(float* destination, SimdVector4F source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        destination[0] = source.Inner[0];
        destination[1] = source.Inner[1];
        destination[2] = source.Inner[2];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        _mm_store_ps(destination, source);
#elif ANEMONE_FEATURE_NEON
        vst1q_f32(destination, source);
#endif
    }

    inline void anemone_vectorcall Vector4F_StoreAlignedFloat2(float* destination, SimdVector4F source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        destination[0] = source.Inner[0];
        destination[1] = source.Inner[1];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        _mm_store_ps(destination, source);
#elif ANEMONE_FEATURE_NEON
        vst1q_f32(destination, source);
#endif
    }

    inline void anemone_vectorcall Vector4F_StoreAlignedFloat1(float* destination, SimdVector4F source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        destination[0] = source.Inner[0];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        _mm_store_ps(destination, source);
#elif ANEMONE_FEATURE_NEON
        vst1q_f32(destination, source);
#endif
    }

    inline void anemone_vectorcall Vector4F_StoreUnalignedFloat4(float* destination, SimdVector4F source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        destination[0] = source.Inner[0];
        destination[1] = source.Inner[1];
        destination[2] = source.Inner[2];
        destination[3] = source.Inner[3];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        _mm_storeu_ps(destination, source);
#elif ANEMONE_FEATURE_NEON
        vst1q_f32(destination, source);
#endif
    }

    inline void anemone_vectorcall Vector4F_StoreUnalignedFloat3(float* destination, SimdVector4F source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        destination[0] = source.Inner[0];
        destination[1] = source.Inner[1];
        destination[2] = source.Inner[2];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // = [x, y, ...]
        _mm_storel_epi64(reinterpret_cast<__m128i*>(destination), _mm_castps_si128(source));
        // = [z, ...]
        _mm_store_ss(destination + 2, _mm_movehl_ps(source, source));
#elif ANEMONE_FEATURE_NEON
        // = [x, y]
        vst1_f32(destination, vget_low_f32(source));
        // = [z]
        vst1q_lane_f32(destination + 2, source, 2);
#endif
    }

    inline void anemone_vectorcall Vector4F_StoreUnalignedFloat2(float* destination, SimdVector4F source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        destination[0] = source.Inner[0];
        destination[1] = source.Inner[1];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // target[0] = source.0
        // target[1] = source.1
        _mm_storel_epi64(reinterpret_cast<__m128i*>(destination), _mm_castps_si128(source));
#elif ANEMONE_FEATURE_NEON
        // = [x, y]
        vst1_f32(destination, vget_low_f32(source));
#endif
    }

    inline void anemone_vectorcall Vector4F_StoreUnalignedFloat1(float* destination, SimdVector4F source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        destination[0] = source.Inner[0];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        _mm_store_ss(destination, source);
#elif ANEMONE_FEATURE_NEON
        // = [x]
        vst1q_lane_f32(destination, source, 0);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Create(float x, float y, float z, float w)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{x, y, z, w};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_setr_ps(x, y, z, w);
#elif ANEMONE_FEATURE_NEON
        float const r[4]{x, y, z, w};
        return vld1q_f32(r);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Replicate(float f)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{f, f, f, f};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_set1_ps(f);
#elif ANEMONE_FEATURE_NEON
        return vdupq_n_f32(f);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Zero()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4F>(F32x4_PositiveZero_XXXX);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_setzero_ps();
#elif ANEMONE_FEATURE_NEON
        return vdupq_n_f32(0.0f);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NaN()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4F>(F32x4_PositiveQNaN_XXXX);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_load_ps(F32x4_PositiveQNaN_XXXX.As<float>());
#elif ANEMONE_FEATURE_NEON
        return vld1q_f32(F32x4_PositiveQNaN_XXXX.As<float>());
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PositiveInfinity()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4F>(F32x4_PositiveInfinity_XXXX);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_load_ps(F32x4_PositiveInfinity_XXXX.As<float>());
#elif ANEMONE_FEATURE_NEON
        return vld1q_f32(F32x4_PositiveInfinity_XXXX.As<float>());
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegativeInfinity()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4F>(F32x4_NegativeInfinity_XXXX);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_load_ps(F32x4_NegativeInfinity_XXXX.As<float>());
#elif ANEMONE_FEATURE_NEON
        return vld1q_f32(F32x4_NegativeInfinity_XXXX.As<float>());
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Epsilon()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4F>(F32x4_Epsilon_XXXX);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_load_ps(F32x4_Epsilon_XXXX.As<float>());
#elif ANEMONE_FEATURE_NEON
        return vld1q_f32(F32x4_Epsilon_XXXX.As<float>());
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PositiveUnitX()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4F>(F32x4_PositiveUnitX);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_load_ps(F32x4_PositiveUnitX.As<float>());
#elif ANEMONE_FEATURE_NEON
        return vld1q_f32(F32x4_PositiveUnitX.Elements);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PositiveUnitY()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4F>(F32x4_PositiveUnitY);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_load_ps(F32x4_PositiveUnitY.As<float>());
#elif ANEMONE_FEATURE_NEON
        return vld1q_f32(F32x4_PositiveUnitY.Elements);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PositiveUnitZ()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4F>(F32x4_PositiveUnitZ);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_load_ps(F32x4_PositiveUnitZ.As<float>());
#elif ANEMONE_FEATURE_NEON
        return vld1q_f32(F32x4_PositiveUnitZ.Elements);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PositiveUnitW()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4F>(F32x4_PositiveUnitW);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_load_ps(F32x4_PositiveUnitW.As<float>());
#elif ANEMONE_FEATURE_NEON
        return vld1q_f32(F32x4_PositiveUnitW.Elements);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegativeUnitX()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4F>(F32x4_NegativeUnitX);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_load_ps(F32x4_NegativeUnitX.As<float>());
#elif ANEMONE_FEATURE_NEON
        return vld1q_f32(F32x4_NegativeUnitX.Elements);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegativeUnitY()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4F>(F32x4_NegativeUnitY);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_load_ps(F32x4_NegativeUnitY.As<float>());
#elif ANEMONE_FEATURE_NEON
        return vld1q_f32(F32x4_NegativeUnitY.Elements);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegativeUnitZ()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4F>(F32x4_NegativeUnitZ);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_load_ps(F32x4_NegativeUnitZ.As<float>());
#elif ANEMONE_FEATURE_NEON
        return vld1q_f32(F32x4_NegativeUnitZ.Elements);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegativeUnitW()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4F>(F32x4_NegativeUnitW);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_load_ps(F32x4_NegativeUnitW.As<float>());
#elif ANEMONE_FEATURE_NEON
        return vld1q_f32(F32x4_NegativeUnitW.Elements);
#endif
    }

    template <size_t N>
    float anemone_vectorcall Vector4F_Extract(SimdVector4F v)
        requires(N < 4)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return v.Inner[N];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        if constexpr (N == 0)
        {
            return _mm_cvtss_f32(v);
        }
        else
        {
            // TODO: Reported MSVC bug, replace with `_mm_permute_ps` when fixed
            // https://developercommunity.visualstudio.com/t/MSVC-1940-vs-1939:-AVX-optimized-code/10698105
            return _mm_cvtss_f32(_mm_shuffle_ps(v, v, _MM_SHUFFLE(N, N, N, N)));
        }
#elif ANEMONE_FEATURE_NEON
        return vgetq_lane_f32(v, N);
#endif
    }

    template <size_t N>
    SimdVector4F anemone_vectorcall Vector4F_Insert(SimdVector4F v, float f)
        requires(N < 4)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        v.Inner[N] = f;
        return v;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        if constexpr (N == 0)
        {
            return _mm_move_ss(v, _mm_set_ss(f));
        }
        else
        {
            return _mm_insert_ps(v, _mm_set_ss(f), N << 4);
        }
#elif ANEMONE_FEATURE_NEON
        return vsetq_lane_f32(f, v, N);
#endif
    }

    template <size_t N>
    SimdVector4F anemone_vectorcall Vector4F_Broadcast(SimdVector4F v)
        requires(N < 4)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{v.Inner[N], v.Inner[N], v.Inner[N], v.Inner[N]};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_shuffle_ps(v, v, _MM_SHUFFLE(N, N, N, N));
#elif ANEMONE_FEATURE_NEON
        return vdupq_laneq_f32(v, N);
#endif
    }

    template <size_t X, size_t Y, size_t Z, size_t W>
    SimdVector4F anemone_vectorcall Vector4F_Permute(SimdVector4F v)
        requires((X < 4) and (Y < 4) and (Z < 4) and (W < 4))
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{v.Inner[X], v.Inner[Y], v.Inner[Z], v.Inner[W]};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_shuffle_ps(v, v, _MM_SHUFFLE(W, Z, Y, X));
#elif ANEMONE_FEATURE_NEON
        return NeonShuffle<X, Y, Z, W>(v);
#endif
    }

    template <size_t X, size_t Y, size_t Z, size_t W>
    SimdVector4F anemone_vectorcall Vector4F_Permute(SimdVector4F a, SimdVector4F b)
        requires((X < 8) and (Y < 8) and (Z < 8) and (W < 8))
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        SimdVector4F result;

        if constexpr (X < 4)
        {
            result.Inner[0] = a.Inner[X];
        }
        else
        {
            result.Inner[0] = b.Inner[X - 4];
        }

        if constexpr (Y < 4)
        {
            result.Inner[1] = a.Inner[Y];
        }
        else
        {
            result.Inner[1] = b.Inner[Y - 4];
        }

        if constexpr (Z < 4)
        {
            result.Inner[2] = a.Inner[Z];
        }
        else
        {
            result.Inner[2] = b.Inner[Z - 4];
        }

        if constexpr (W < 4)
        {
            result.Inner[3] = a.Inner[W];
        }
        else
        {
            result.Inner[3] = b.Inner[W - 4];
        }

        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // Blending cases first
        if constexpr ((X == 0) and (Y == 1) and (Z == 2) and (W == 3))
        {
            return a;
        }
        else if constexpr ((X == 4) and (Y == 1) and (Z == 2) and (W == 3))
        {
            return _mm_blend_ps(a, b, 0b0001);
        }
        else if constexpr ((X == 0) and (Y == 5) and (Z == 2) and (W == 3))
        {
            return _mm_blend_ps(a, b, 0b0010);
        }
        else if constexpr ((X == 4) and (Y == 5) and (Z == 2) and (W == 3))
        {
            return _mm_blend_ps(a, b, 0b0011);
        }
        else if constexpr ((X == 0) and (Y == 1) and (Z == 6) and (W == 3))
        {
            return _mm_blend_ps(a, b, 0b0100);
        }
        else if constexpr ((X == 4) and (Y == 1) and (Z == 6) and (W == 3))
        {
            return _mm_blend_ps(a, b, 0b0101);
        }
        else if constexpr ((X == 0) and (Y == 5) and (Z == 6) and (W == 3))
        {
            return _mm_blend_ps(a, b, 0b0110);
        }
        else if constexpr ((X == 4) and (Y == 5) and (Z == 6) and (W == 3))
        {
            return _mm_blend_ps(a, b, 0b0111);
        }
        else if constexpr ((X == 0) and (Y == 1) and (Z == 2) and (W == 7))
        {
            return _mm_blend_ps(a, b, 0b1000);
        }
        else if constexpr ((X == 4) and (Y == 1) and (Z == 2) and (W == 7))
        {
            return _mm_blend_ps(a, b, 0b1001);
        }
        else if constexpr ((X == 0) and (Y == 5) and (Z == 2) and (W == 7))
        {
            return _mm_blend_ps(a, b, 0b1010);
        }
        else if constexpr ((X == 4) and (Y == 5) and (Z == 2) and (W == 7))
        {
            return _mm_blend_ps(a, b, 0b1011);
        }
        else if constexpr ((X == 0) and (Y == 1) and (Z == 2) and (W == 7))
        {
            return _mm_blend_ps(a, b, 0b1100);
        }
        else if constexpr ((X == 4) and (Y == 1) and (Z == 6) and (W == 7))
        {
            return _mm_blend_ps(a, b, 0b1101);
        }
        else if constexpr ((X == 0) and (Y == 5) and (Z == 6) and (W == 7))
        {
            return _mm_blend_ps(a, b, 0b1110);
        }
        else if constexpr ((X == 4) and (Y == 5) and (Z == 6) and (W == 7))
        {
            return b;
        }
        else
        {
            // Determine if we have more generic permute/shuffle case available here.
            static constexpr bool which_x = X >= 4;
            static constexpr bool which_y = Y >= 4;
            static constexpr bool which_z = Z >= 4;
            static constexpr bool which_w = W >= 4;

            static constexpr int shuffle_mask = _MM_SHUFFLE(W & 0b11, Z & 0b11, Y & 0b11, X & 0b11);

            if constexpr (!which_x and !which_y and !which_z and !which_w)
            {
                return _mm_permute_ps(a, shuffle_mask);
            }
            else if constexpr (!which_x and !which_y and which_z and which_w)
            {
                return _mm_shuffle_ps(a, b, shuffle_mask);
            }
            else if constexpr (which_x and which_y and !which_z and !which_w)
            {
                return _mm_shuffle_ps(b, a, shuffle_mask);
            }
            else if constexpr (which_x and which_y and which_z and which_w)
            {
                return _mm_permute_ps(b, shuffle_mask);
            }
            else
            {
                // Non-standard shuffle. Need to mix components from two vectors, but no pattern matched here.
                // It would be great to have this as an intrinsic instead of this slow path.
                //
                // Almost every combination of component can be achieved by proper usage of blend, shuffle, permute, insert intrinsics.

                // On Intel i9 13900K, `_mm_permute_ps` + 3x `_mm_insert_ps` is slower than this code.
                // Similarly, `_mm_blend_ps` is slower thant bit masking values.

                static constexpr int32_t mask[4]{
                    which_x ? AvxMaskComponentInt32 : 0,
                    which_y ? AvxMaskComponentInt32 : 0,
                    which_z ? AvxMaskComponentInt32 : 0,
                    which_w ? AvxMaskComponentInt32 : 0,
                };

                __m128 const v_a = _mm_permute_ps(a, shuffle_mask);
                __m128 const v_b = _mm_permute_ps(b, shuffle_mask);

                __m128 const which_mask = _mm_load_ps(reinterpret_cast<float const*>(&mask));
                __m128 const selected_a = _mm_andnot_ps(which_mask, v_a);
                __m128 const selected_b = _mm_and_ps(which_mask, v_b);
                return _mm_or_ps(selected_a, selected_b);
            }
        }
#elif ANEMONE_FEATURE_NEON
        float32x4_t r;

        if constexpr (X < 4)
        {
            r = vmovq_n_f32(vgetq_lane_f32(a, X));
        }
        else
        {
            r = vmovq_n_f32(vgetq_lane_f32(b, X - 4));
        }

        if constexpr (Y < 4)
        {
            r = vcopyq_laneq_f32(r, 1, a, Y);
        }
        else
        {
            r = vcopyq_laneq_f32(r, 1, b, Y - 4);
        }

        if constexpr (Z < 4)
        {
            r = vcopyq_laneq_f32(r, 2, a, Z);
        }
        else
        {
            r = vcopyq_laneq_f32(r, 2, b, Z - 4);
        }

        if constexpr (W < 4)
        {
            r = vcopyq_laneq_f32(r, 3, a, W);
        }
        else
        {
            r = vcopyq_laneq_f32(r, 3, b, W - 4);
        }

        return r;
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Select(SimdMask4F mask, SimdVector4F whenFalse, SimdVector4F whenTrue)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            ScalarHelper_SelectBranchless(mask.Inner[0], whenFalse.Inner[0], whenTrue.Inner[0]),
            ScalarHelper_SelectBranchless(mask.Inner[1], whenFalse.Inner[1], whenTrue.Inner[1]),
            ScalarHelper_SelectBranchless(mask.Inner[2], whenFalse.Inner[2], whenTrue.Inner[2]),
            ScalarHelper_SelectBranchless(mask.Inner[3], whenFalse.Inner[3], whenTrue.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_blendv_ps(whenFalse, whenTrue, mask);
#elif ANEMONE_FEATURE_NEON
        return vbslq_f32(mask, whenTrue, whenFalse);
#endif
    }

    template <bool X, bool Y, bool Z, bool W>
    SimdVector4F anemone_vectorcall Vector4F_Select(SimdVector4F whenFalse, SimdVector4F whenTrue)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            X ? whenTrue.Inner[0] : whenFalse.Inner[0],
            Y ? whenTrue.Inner[1] : whenFalse.Inner[1],
            Z ? whenTrue.Inner[2] : whenFalse.Inner[2],
            W ? whenTrue.Inner[3] : whenFalse.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_blend_ps(whenFalse, whenTrue, (X ? 0b0001 : 0) | (Y ? 0b0010 : 0) | (Z ? 0b0100 : 0) | (W ? 0b1000 : 0));
#elif ANEMONE_FEATURE_NEON
        static constexpr uint32_t mask[4]{
            X ? NeonMaskComponentUInt32 : 0,
            Y ? NeonMaskComponentUInt32 : 0,
            Z ? NeonMaskComponentUInt32 : 0,
            W ? NeonMaskComponentUInt32 : 0,
        };
        return vbslq_f32(vld1q_u32(mask), whenTrue, whenFalse);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_MultiplyAdd(SimdVector4F a, SimdVector4F b, SimdVector4F c)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            (a.Inner[0] * b.Inner[0]) + c.Inner[0],
            (a.Inner[1] * b.Inner[1]) + c.Inner[1],
            (a.Inner[2] * b.Inner[2]) + c.Inner[2],
            (a.Inner[3] * b.Inner[3]) + c.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX2
        return _mm_fmadd_ps(a, b, c);
#elif ANEMONE_FEATURE_AVX
        // = (a * b) + c
        return _mm_add_ps(_mm_mul_ps(a, b), c);
#elif ANEMONE_FEATURE_NEON
        return vmlaq_f32(c, a, b);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_MultiplySubtract(SimdVector4F a, SimdVector4F b, SimdVector4F c)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            (a.Inner[0] * b.Inner[0]) - c.Inner[0],
            (a.Inner[1] * b.Inner[1]) - c.Inner[1],
            (a.Inner[2] * b.Inner[2]) - c.Inner[2],
            (a.Inner[3] * b.Inner[3]) - c.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX2
        return _mm_fmsub_ps(a, b, c);
#elif ANEMONE_FEATURE_AVX
        // (a * b) - c
        return _mm_sub_ps(_mm_mul_ps(a, b), c);
#elif ANEMONE_FEATURE_NEON
        return vmlaq_f32(vnegq_f32(c), a, b);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegateMultiplyAdd(SimdVector4F a, SimdVector4F b, SimdVector4F c)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            -(a.Inner[0] * b.Inner[0]) + c.Inner[0],
            -(a.Inner[1] * b.Inner[1]) + c.Inner[1],
            -(a.Inner[2] * b.Inner[2]) + c.Inner[2],
            -(a.Inner[3] * b.Inner[3]) + c.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX2
        return _mm_fnmadd_ps(a, b, c);
#elif ANEMONE_FEATURE_AVX
        // -(a * b) + c
        return _mm_sub_ps(c, _mm_mul_ps(a, b));
#elif ANEMONE_FEATURE_NEON
        return vmlsq_f32(c, a, b);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegateMultiplySubtract(SimdVector4F a, SimdVector4F b, SimdVector4F c)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            -(a.Inner[0] * b.Inner[0]) - c.Inner[0],
            -(a.Inner[1] * b.Inner[1]) - c.Inner[1],
            -(a.Inner[2] * b.Inner[2]) - c.Inner[2],
            -(a.Inner[3] * b.Inner[3]) - c.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX2
        return _mm_fnmsub_ps(a, b, c);
#elif ANEMONE_FEATURE_AVX
        // -(a * b) - c
        return _mm_sub_ps(Vector4F_Negate(_mm_mul_ps(a, b)), c);
#elif ANEMONE_FEATURE_NEON
        return vmlsq_f32(vnegq_f32(c), a, b);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Add(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            a.Inner[0] + b.Inner[0],
            a.Inner[1] + b.Inner[1],
            a.Inner[2] + b.Inner[2],
            a.Inner[3] + b.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_add_ps(a, b);
#elif ANEMONE_FEATURE_NEON
        return vaddq_f32(a, b);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Subtract(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            a.Inner[0] - b.Inner[0],
            a.Inner[1] - b.Inner[1],
            a.Inner[2] - b.Inner[2],
            a.Inner[3] - b.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_sub_ps(a, b);
#elif ANEMONE_FEATURE_NEON
        return vsubq_f32(a, b);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Multiply(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            a.Inner[0] * b.Inner[0],
            a.Inner[1] * b.Inner[1],
            a.Inner[2] * b.Inner[2],
            a.Inner[3] * b.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_mul_ps(a, b);
#elif ANEMONE_FEATURE_NEON
        return vmulq_f32(a, b);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Divide(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            a.Inner[0] / b.Inner[0],
            a.Inner[1] / b.Inner[1],
            a.Inner[2] / b.Inner[2],
            a.Inner[3] / b.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_div_ps(a, b);
#elif ANEMONE_FEATURE_NEON
        return vdivq_f32(a, b);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Negate(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            -v.Inner[0],
            -v.Inner[1],
            -v.Inner[2],
            -v.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const mask = _mm_load_ps(F32x4_SignMask_XXXX.As<float>());
        return _mm_xor_ps(v, mask);
#elif ANEMONE_FEATURE_NEON
        return vnegq_f32(v);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Reciprocal(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Reciprocal<float>(v.Inner[0]),
            Reciprocal<float>(v.Inner[1]),
            Reciprocal<float>(v.Inner[2]),
            Reciprocal<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_div_ps(_mm_load_ps(F32x4_PositiveOne.As<float>()), v);
#elif ANEMONE_FEATURE_NEON
        return vdivq_f32(vdupq_n_f32(1.0f), v);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_ReciprocalEst(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Reciprocal<float>(v.Inner[0]),
            Reciprocal<float>(v.Inner[1]),
            Reciprocal<float>(v.Inner[2]),
            Reciprocal<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_rcp_ps(v);
#elif ANEMONE_FEATURE_NEON
        return vrecpeq_f32(v);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Min(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Min<float>(a.Inner[0], b.Inner[0]),
            Min<float>(a.Inner[1], b.Inner[1]),
            Min<float>(a.Inner[2], b.Inner[2]),
            Min<float>(a.Inner[3], b.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_min_ps(a, b);
#elif ANEMONE_FEATURE_NEON
        return vminq_f32(a, b);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Max(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Max<float>(a.Inner[0], b.Inner[0]),
            Max<float>(a.Inner[1], b.Inner[1]),
            Max<float>(a.Inner[2], b.Inner[2]),
            Max<float>(a.Inner[3], b.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_max_ps(a, b);
#elif ANEMONE_FEATURE_NEON
        return vmaxq_f32(a, b);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Abs(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Abs<float>(v.Inner[0]),
            Abs<float>(v.Inner[1]),
            Abs<float>(v.Inner[2]),
            Abs<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const mask = _mm_load_ps(F32x4_AbsMask_XXXX.As<float>());
        return _mm_and_ps(v, mask);
#elif ANEMONE_FEATURE_NEON
        return vabsq_f32(v);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Square(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Square<float>(v.Inner[0]),
            Square<float>(v.Inner[1]),
            Square<float>(v.Inner[2]),
            Square<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_mul_ps(v, v);
#elif ANEMONE_FEATURE_NEON
        return vmulq_f32(v, v);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_SignedSquare(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            SignedSquare<float>(v.Inner[0]),
            SignedSquare<float>(v.Inner[1]),
            SignedSquare<float>(v.Inner[2]),
            SignedSquare<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_mul_ps(v, Vector4F_Abs(v));
#elif ANEMONE_FEATURE_NEON
        return vmulq_f32(v, vabsq_f32(v));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Clamp(SimdVector4F v, SimdVector4F lower, SimdVector4F upper)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Clamp<float>(v.Inner[0], lower.Inner[0], upper.Inner[0]),
            Clamp<float>(v.Inner[1], lower.Inner[1], upper.Inner[1]),
            Clamp<float>(v.Inner[2], lower.Inner[2], upper.Inner[2]),
            Clamp<float>(v.Inner[3], lower.Inner[3], upper.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_min_ps(upper, _mm_max_ps(lower, v));
#elif ANEMONE_FEATURE_NEON
        return vminq_f32(upper, vmaxq_f32(lower, v));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Saturate(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Saturate<float>(v.Inner[0]),
            Saturate<float>(v.Inner[1]),
            Saturate<float>(v.Inner[2]),
            Saturate<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const zero = _mm_setzero_ps();
        __m128 const one = _mm_load_ps(F32x4_PositiveOne.As<float>());
        return _mm_min_ps(one, _mm_max_ps(zero, v));
#elif ANEMONE_FEATURE_NEON
        float32x4_t const lower = vdupq_n_f32(0.0f);
        float32x4_t const upper = vdupq_n_f32(1.0f);
        return vminq_f32(upper, vmaxq_f32(lower, v));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_SquareRoot(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Sqrt<float>(v.Inner[0]),
            Sqrt<float>(v.Inner[1]),
            Sqrt<float>(v.Inner[2]),
            Sqrt<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_sqrt_ps(v);
#elif ANEMONE_FEATURE_NEON
        return vsqrtq_f32(v);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_SquareRootEst(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            SqrtEst<float>(v.Inner[0]),
            SqrtEst<float>(v.Inner[1]),
            SqrtEst<float>(v.Inner[2]),
            SqrtEst<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_rcp_ps(_mm_rsqrt_ps(v));
#elif ANEMONE_FEATURE_NEON
        // Use NeonSquareRoot2xNewtonRhapson(v.Inner);?
        return vsqrtq_f32(v);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_ReciprocalSquareRoot(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            ReciprocalSqrt<float>(v.Inner[0]),
            ReciprocalSqrt<float>(v.Inner[1]),
            ReciprocalSqrt<float>(v.Inner[2]),
            ReciprocalSqrt<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_div_ps(_mm_load_ps(F32x4_PositiveOne.As<float>()), _mm_sqrt_ps(v));
#elif ANEMONE_FEATURE_NEON
        return NeonReciprocalSquareRoot2xNewtonRhapson(v);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_ReciprocalSquareRootEst(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            ReciprocalSqrtEst<float>(v.Inner[0]),
            ReciprocalSqrtEst<float>(v.Inner[1]),
            ReciprocalSqrtEst<float>(v.Inner[2]),
            ReciprocalSqrtEst<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_rsqrt_ps(v);
#elif ANEMONE_FEATURE_NEON
        return vrsqrteq_f32(v);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Ceil(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Ceil<float>(v.Inner[0]),
            Ceil<float>(v.Inner[1]),
            Ceil<float>(v.Inner[2]),
            Ceil<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_round_ps(v, _MM_FROUND_TO_POS_INF | _MM_FROUND_NO_EXC);
#elif ANEMONE_FEATURE_NEON
        return vrndpq_f32(v);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Floor(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Floor<float>(v.Inner[0]),
            Floor<float>(v.Inner[1]),
            Floor<float>(v.Inner[2]),
            Floor<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_round_ps(v, _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC);
#elif ANEMONE_FEATURE_NEON
        return vrndmq_f32(v);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Truncate(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Truncate<float>(v.Inner[0]),
            Truncate<float>(v.Inner[1]),
            Truncate<float>(v.Inner[2]),
            Truncate<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_round_ps(v, _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC);
#elif ANEMONE_FEATURE_NEON
        return vrndq_f32(v);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Round(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Round<float>(v.Inner[0]),
            Round<float>(v.Inner[1]),
            Round<float>(v.Inner[2]),
            Round<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_round_ps(v, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
#elif ANEMONE_FEATURE_NEON
        return vrndaq_f32(v);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Fraction(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Fraction<float>(v.Inner[0]),
            Fraction<float>(v.Inner[1]),
            Fraction<float>(v.Inner[2]),
            Fraction<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_sub_ps(v, _mm_round_ps(v, _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC));
#elif ANEMONE_FEATURE_NEON
        return vsubq_f32(v, vrndmq_f32(v));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Remainder(SimdVector4F x, SimdVector4F y)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Remainder<float>(x.Inner[0], y.Inner[0]),
            Remainder<float>(x.Inner[1], y.Inner[1]),
            Remainder<float>(x.Inner[2], y.Inner[2]),
            Remainder<float>(x.Inner[3], y.Inner[3]),
        };
#elif ANEMONE_FEATURE_SVML
        return _mm_fmod_ps(x, y);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // = [x / y]
        __m128 const r0 = _mm_div_ps(x, y);
        // = [trunc(x / y)]
        __m128 const r1 = _mm_round_ps(r0, _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC);
        // = [x - (trunc(x / y) * y)]
        return _mm_sub_ps(x, _mm_mul_ps(r1, y));
#elif ANEMONE_FEATURE_NEON
        // = [x / y]
        float32x4_t const r0 = vdivq_f32(x, y);
        // = [trunc(x / y)]
        float32x4_t const r1 = vrndq_f32(r0);
        // = [x - (trunc(x / y) * y)]
        return vfmsq_f32(x, r1, y);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Repeat(SimdVector4F v, SimdVector4F length)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Repeat<float>(v.Inner[0], length.Inner[0]),
            Repeat<float>(v.Inner[1], length.Inner[1]),
            Repeat<float>(v.Inner[2], length.Inner[2]),
            Repeat<float>(v.Inner[3], length.Inner[3]),
        };
#else
        SimdVector4F const scaled = Vector4F_Fraction(Vector4F_Divide(v, length));
        return Vector4F_Multiply(scaled, length);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Wrap(SimdVector4F v, SimdVector4F lower, SimdVector4F upper)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Wrap<float>(v.Inner[0], lower.Inner[0], upper.Inner[0]),
            Wrap<float>(v.Inner[1], lower.Inner[1], upper.Inner[1]),
            Wrap<float>(v.Inner[2], lower.Inner[2], upper.Inner[2]),
            Wrap<float>(v.Inner[3], lower.Inner[3], upper.Inner[3]),
        };
#else
        SimdVector4F const range = Vector4F_Subtract(upper, lower);
        SimdVector4F const wrapped = Vector4F_Subtract(v, lower);
        SimdVector4F const scaled = Vector4F_Fraction(Vector4F_Divide(wrapped, range));
        return Vector4F_MultiplyAdd(scaled, range, lower);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Power(SimdVector4F x, SimdVector4F y)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Power<float>(x.Inner[0], y.Inner[0]),
            Power<float>(x.Inner[1], y.Inner[1]),
            Power<float>(x.Inner[2], y.Inner[2]),
            Power<float>(x.Inner[3], y.Inner[3]),
        };
#elif ANEMONE_FEATURE_SVML
        return _mm_pow_ps(x, y);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        alignas(16) float xs[4];
        alignas(16) float ys[4];

        _mm_store_ps(xs, x);
        _mm_store_ps(ys, y);

        return _mm_setr_ps(
            Power<float>(xs[0], ys[0]),
            Power<float>(xs[1], ys[1]),
            Power<float>(xs[2], ys[2]),
            Power<float>(xs[3], ys[3]));
#elif ANEMONE_FEATURE_NEON
        float const r[4]{
            Power<float>(vgetq_lane_f32(x, 0), vgetq_lane_f32(y, 0)),
            Power<float>(vgetq_lane_f32(x, 1), vgetq_lane_f32(y, 1)),
            Power<float>(vgetq_lane_f32(x, 2), vgetq_lane_f32(y, 2)),
            Power<float>(vgetq_lane_f32(x, 3), vgetq_lane_f32(y, 3)),
        };
        return vld1q_f32(r);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Exp(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Exp<float>(v.Inner[0]),
            Exp<float>(v.Inner[1]),
            Exp<float>(v.Inner[2]),
            Exp<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_SVML
        return _mm_exp_ps(v);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        alignas(16) float xs[4];
        _mm_store_ps(xs, v);

        return _mm_setr_ps(
            Exp<float>(xs[0]),
            Exp<float>(xs[1]),
            Exp<float>(xs[2]),
            Exp<float>(xs[3]));
#elif ANEMONE_FEATURE_NEON
        float const r[4]{
            Exp<float>(vgetq_lane_f32(v, 0)),
            Exp<float>(vgetq_lane_f32(v, 1)),
            Exp<float>(vgetq_lane_f32(v, 2)),
            Exp<float>(vgetq_lane_f32(v, 3)),
        };
        return vld1q_f32(r);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Exp2(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Exp2<float>(v.Inner[0]),
            Exp2<float>(v.Inner[1]),
            Exp2<float>(v.Inner[2]),
            Exp2<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_SVML
        return _mm_exp2_ps(v);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        alignas(16) float xs[4];
        _mm_store_ps(xs, v);

        return _mm_setr_ps(
            Exp2<float>(xs[0]),
            Exp2<float>(xs[1]),
            Exp2<float>(xs[2]),
            Exp2<float>(xs[3]));
#elif ANEMONE_FEATURE_NEON
        float const r[4]{
            Exp2<float>(vgetq_lane_f32(v, 0)),
            Exp2<float>(vgetq_lane_f32(v, 1)),
            Exp2<float>(vgetq_lane_f32(v, 2)),
            Exp2<float>(vgetq_lane_f32(v, 3)),
        };
        return vld1q_f32(r);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Exp10(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Exp10<float>(v.Inner[0]),
            Exp10<float>(v.Inner[1]),
            Exp10<float>(v.Inner[2]),
            Exp10<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_SVML
        return _mm_exp10_ps(v);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        alignas(16) float xs[4];
        _mm_store_ps(xs, v);

        return _mm_setr_ps(
            Exp10<float>(xs[0]),
            Exp10<float>(xs[1]),
            Exp10<float>(xs[2]),
            Exp10<float>(xs[3]));
#elif ANEMONE_FEATURE_NEON
        float const r[4]{
            Exp10<float>(vgetq_lane_f32(v, 0)),
            Exp10<float>(vgetq_lane_f32(v, 1)),
            Exp10<float>(vgetq_lane_f32(v, 2)),
            Exp10<float>(vgetq_lane_f32(v, 3)),
        };
        return vld1q_f32(r);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Log(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Log<float>(v.Inner[0]),
            Log<float>(v.Inner[1]),
            Log<float>(v.Inner[2]),
            Log<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_SVML
        return _mm_log_ps(v);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        alignas(16) float xs[4];
        _mm_store_ps(xs, v);

        return _mm_setr_ps(
            Log<float>(xs[0]),
            Log<float>(xs[1]),
            Log<float>(xs[2]),
            Log<float>(xs[3]));
#elif ANEMONE_FEATURE_NEON
        float const r[4]{
            Log<float>(vgetq_lane_f32(v, 0)),
            Log<float>(vgetq_lane_f32(v, 1)),
            Log<float>(vgetq_lane_f32(v, 2)),
            Log<float>(vgetq_lane_f32(v, 3)),
        };
        return vld1q_f32(r);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Log(SimdVector4F x, SimdVector4F base)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Log<float>(x.Inner[0], base.Inner[0]),
            Log<float>(x.Inner[1], base.Inner[1]),
            Log<float>(x.Inner[2], base.Inner[2]),
            Log<float>(x.Inner[3], base.Inner[3]),
        };
#elif ANEMONE_FEATURE_SVML
        return _mm_div_ps(_mm_log_ps(x), _mm_log_ps(base));
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        alignas(16) float xs[4];
        alignas(16) float ys[4];
        _mm_store_ps(xs, x);
        _mm_store_ps(ys, base);

        return _mm_setr_ps(
            Log<float>(xs[0], ys[0]),
            Log<float>(xs[1], ys[1]),
            Log<float>(xs[2], ys[2]),
            Log<float>(xs[3], ys[3]));
#elif ANEMONE_FEATURE_NEON
        float const r[4]{
            Log<float>(vgetq_lane_f32(x, 0), vgetq_lane_f32(base, 0)),
            Log<float>(vgetq_lane_f32(x, 1), vgetq_lane_f32(base, 1)),
            Log<float>(vgetq_lane_f32(x, 2), vgetq_lane_f32(base, 2)),
            Log<float>(vgetq_lane_f32(x, 3), vgetq_lane_f32(base, 3)),
        };
        return vld1q_f32(r);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Log2(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Log2<float>(v.Inner[0]),
            Log2<float>(v.Inner[1]),
            Log2<float>(v.Inner[2]),
            Log2<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_SVML
        return _mm_log2_ps(v);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        alignas(16) float xs[4];
        _mm_store_ps(xs, v);

        return _mm_setr_ps(
            Log2<float>(xs[0]),
            Log2<float>(xs[1]),
            Log2<float>(xs[2]),
            Log2<float>(xs[3]));
#elif ANEMONE_FEATURE_NEON
        float const r[4]{
            Log2<float>(vgetq_lane_f32(v, 0)),
            Log2<float>(vgetq_lane_f32(v, 1)),
            Log2<float>(vgetq_lane_f32(v, 2)),
            Log2<float>(vgetq_lane_f32(v, 3)),
        };
        return vld1q_f32(r);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Log10(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Log10<float>(v.Inner[0]),
            Log10<float>(v.Inner[1]),
            Log10<float>(v.Inner[2]),
            Log10<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_SVML
        return _mm_log10_ps(v);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        alignas(16) float xs[4];
        _mm_store_ps(xs, v);

        return _mm_setr_ps(
            Log10<float>(xs[0]),
            Log10<float>(xs[1]),
            Log10<float>(xs[2]),
            Log10<float>(xs[3]));
#elif ANEMONE_FEATURE_NEON
        float const r[4]{
            Log10<float>(vgetq_lane_f32(v, 0)),
            Log10<float>(vgetq_lane_f32(v, 1)),
            Log10<float>(vgetq_lane_f32(v, 2)),
            Log10<float>(vgetq_lane_f32(v, 3)),
        };
        return vld1q_f32(r);
#endif
    }

    inline void anemone_vectorcall Vector4F_SinCos(SimdVector4F& sin, SimdVector4F& cos, SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        sin = Vector4F_Sin(v);
        cos = Vector4F_Cos(v);
#elif ANEMONE_FEATURE_SVML
        sin = _mm_sincos_ps(&cos, v);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
#if ANEMONE_FEATURE_FAST_TRIGONOMETRIC_FUNCTIONS
        // Reduce range to [-pi..pi]
        __m128 const quotient = _mm_round_ps(_mm_mul_ps(v, _mm_set_ps1(InvPi2<float>)), _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);

        __m128 y = Vector4F_NegateMultiplyAdd(quotient, _mm_set_ps1(Pi2<float>), v);

        __m128 const maskAbovePositivePiOver2 = _mm_cmpgt_ps(y, _mm_set_ps1(PiOver2<float>));
        __m128 const maskBelowNegativePiOver2 = _mm_cmplt_ps(y, _mm_set_ps1(-PiOver2<float>));

        y = _mm_blendv_ps(y, _mm_sub_ps(_mm_set_ps1(Pi<float>), y), maskAbovePositivePiOver2);
        y = _mm_blendv_ps(y, _mm_sub_ps(_mm_set_ps1(-Pi<float>), y), maskBelowNegativePiOver2);

        __m128 const maskNegativeSign = _mm_or_ps(maskAbovePositivePiOver2, maskBelowNegativePiOver2);
        __m128 const sign = _mm_blendv_ps(_mm_set_ps1(+1.0f), _mm_set_ps1(-1.0f), maskNegativeSign);

        __m128 const y2 = _mm_mul_ps(y, y);

        // 11-deg polynomial
        __m128 const s0123 = _mm_load_ps(Detail::F32x4_SinC0123.Elements);
        __m128 const s45nn = _mm_load_ps(Detail::F32x4_SinC45nn.Elements);

        __m128 resultSin = Vector4F_MultiplyAdd(
            _mm_permute_ps(s45nn, _MM_SHUFFLE(1, 1, 1, 1)),
            y2,
            _mm_permute_ps(s45nn, _MM_SHUFFLE(0, 0, 0, 0)));

        resultSin = Vector4F_MultiplyAdd(resultSin, y2, _mm_permute_ps(s0123, _MM_SHUFFLE(3, 3, 3, 3)));
        resultSin = Vector4F_MultiplyAdd(resultSin, y2, _mm_permute_ps(s0123, _MM_SHUFFLE(2, 2, 2, 2)));
        resultSin = Vector4F_MultiplyAdd(resultSin, y2, _mm_permute_ps(s0123, _MM_SHUFFLE(1, 1, 1, 1)));
        resultSin = Vector4F_MultiplyAdd(resultSin, y2, _mm_permute_ps(s0123, _MM_SHUFFLE(0, 0, 0, 0)));

        sin = _mm_mul_ps(resultSin, y);
        // 10-deg polynomial
        __m128 const c0123 = _mm_load_ps(Detail::F32x4_CosC0123.Elements);
        __m128 const c45nn = _mm_load_ps(Detail::F32x4_CosC45nn.Elements);

        __m128 resultCos = Vector4F_MultiplyAdd(
            _mm_permute_ps(c45nn, _MM_SHUFFLE(1, 1, 1, 1)),
            y2,
            _mm_permute_ps(c45nn, _MM_SHUFFLE(0, 0, 0, 0)));

        resultCos = Vector4F_MultiplyAdd(resultCos, y2, _mm_permute_ps(c0123, _MM_SHUFFLE(3, 3, 3, 3)));
        resultCos = Vector4F_MultiplyAdd(resultCos, y2, _mm_permute_ps(c0123, _MM_SHUFFLE(2, 2, 2, 2)));
        resultCos = Vector4F_MultiplyAdd(resultCos, y2, _mm_permute_ps(c0123, _MM_SHUFFLE(1, 1, 1, 1)));
        resultCos = Vector4F_MultiplyAdd(resultCos, y2, _mm_permute_ps(c0123, _MM_SHUFFLE(0, 0, 0, 0)));

        cos = _mm_mul_ps(resultCos, sign);
#else
        alignas(16) float vs[4];
        _mm_store_ps(vs, v);

        sin = _mm_setr_ps(
            Sin<float>(vs[0]),
            Sin<float>(vs[1]),
            Sin<float>(vs[2]),
            Sin<float>(vs[3]));
        cos = _mm_setr_ps(
            Cos<float>(vs[0]),
            Cos<float>(vs[1]),
            Cos<float>(vs[2]),
            Cos<float>(vs[3]));
#endif
#elif ANEMONE_FEATURE_NEON
        float const fsin[4]{
            Sin<float>(vgetq_lane_f32(v, 0)),
            Sin<float>(vgetq_lane_f32(v, 1)),
            Sin<float>(vgetq_lane_f32(v, 2)),
            Sin<float>(vgetq_lane_f32(v, 3)),
        };
        sin = vld1q_f32(fsin);
        float const fcos[4]{
            Cos<float>(vgetq_lane_f32(v, 0)),
            Cos<float>(vgetq_lane_f32(v, 1)),
            Cos<float>(vgetq_lane_f32(v, 2)),
            Cos<float>(vgetq_lane_f32(v, 3)),
        };
        cos = vld1q_f32(fcos);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Sin(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Sin<float>(v.Inner[0]),
            Sin<float>(v.Inner[1]),
            Sin<float>(v.Inner[2]),
            Sin<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_SVML
        return _mm_sin_ps(v);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
#if ANEMONE_FEATURE_FAST_TRIGONOMETRIC_FUNCTIONS
        // Reduce range to [-pi..pi]
        __m128 const quotient = _mm_round_ps(_mm_mul_ps(v, _mm_set_ps1(InvPi2<float>)), _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);

        __m128 y = Vector4F_NegateMultiplyAdd(quotient, _mm_set_ps1(Pi2<float>), v);

        __m128 const maskAbovePositivePiOver2 = _mm_cmpgt_ps(y, _mm_set_ps1(PiOver2<float>));
        __m128 const maskBelowNegativePiOver2 = _mm_cmplt_ps(y, _mm_set_ps1(-PiOver2<float>));

        y = _mm_blendv_ps(y, _mm_sub_ps(_mm_set_ps1(Pi<float>), y), maskAbovePositivePiOver2);
        y = _mm_blendv_ps(y, _mm_sub_ps(_mm_set_ps1(-Pi<float>), y), maskBelowNegativePiOver2);

        __m128 const y2 = _mm_mul_ps(y, y);

        // 11-deg polynomial
        __m128 const c0123 = _mm_load_ps(Detail::F32x4_SinC0123.Elements);
        __m128 const c45nn = _mm_load_ps(Detail::F32x4_SinC45nn.Elements);

        __m128 resultSin = Vector4F_MultiplyAdd(
            _mm_permute_ps(c45nn, _MM_SHUFFLE(1, 1, 1, 1)),
            y2,
            _mm_permute_ps(c45nn, _MM_SHUFFLE(0, 0, 0, 0)));

        resultSin = Vector4F_MultiplyAdd(resultSin, y2, _mm_permute_ps(c0123, _MM_SHUFFLE(3, 3, 3, 3)));
        resultSin = Vector4F_MultiplyAdd(resultSin, y2, _mm_permute_ps(c0123, _MM_SHUFFLE(2, 2, 2, 2)));
        resultSin = Vector4F_MultiplyAdd(resultSin, y2, _mm_permute_ps(c0123, _MM_SHUFFLE(1, 1, 1, 1)));
        resultSin = Vector4F_MultiplyAdd(resultSin, y2, _mm_permute_ps(c0123, _MM_SHUFFLE(0, 0, 0, 0)));
        return _mm_mul_ps(resultSin, y);
#else
        alignas(16) float xs[4];
        _mm_store_ps(xs, v);
        return _mm_setr_ps(
            Sin<float>(xs[0]),
            Sin<float>(xs[1]),
            Sin<float>(xs[2]),
            Sin<float>(xs[3]));
#endif
#elif ANEMONE_FEATURE_NEON
        float const r[4]{
            Sin<float>(vgetq_lane_f32(v, 0)),
            Sin<float>(vgetq_lane_f32(v, 1)),
            Sin<float>(vgetq_lane_f32(v, 2)),
            Sin<float>(vgetq_lane_f32(v, 3)),
        };
        return vld1q_f32(r);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Cos(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Cos<float>(v.Inner[0]),
            Cos<float>(v.Inner[1]),
            Cos<float>(v.Inner[2]),
            Cos<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_SVML
        return _mm_cos_ps(v);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
#if ANEMONE_FEATURE_FAST_TRIGONOMETRIC_FUNCTIONS
        // Reduce range to [-pi..pi]
        __m128 const quotient = _mm_round_ps(_mm_mul_ps(v, _mm_set_ps1(InvPi2<float>)), _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);

        __m128 y = Vector4F_NegateMultiplyAdd(quotient, _mm_set_ps1(Pi2<float>), v);

        __m128 const maskAbovePositivePiOver2 = _mm_cmpgt_ps(y, _mm_set_ps1(PiOver2<float>));
        __m128 const maskBelowNegativePiOver2 = _mm_cmplt_ps(y, _mm_set_ps1(-PiOver2<float>));
        
        y = _mm_blendv_ps(y, _mm_sub_ps(_mm_set_ps1(Pi<float>), y), maskAbovePositivePiOver2);
        y = _mm_blendv_ps(y, _mm_sub_ps(_mm_set_ps1(-Pi<float>), y), maskBelowNegativePiOver2);

        __m128 const y2 = _mm_mul_ps(y, y);

        __m128 const maskNegativeSign = _mm_or_ps(maskAbovePositivePiOver2, maskBelowNegativePiOver2);
        __m128 const sign = _mm_blendv_ps(_mm_set_ps1(+1.0f), _mm_set_ps1(-1.0f), maskNegativeSign);

        // 10-deg polynomial
        __m128 const c0123 = _mm_load_ps(Detail::F32x4_CosC0123.Elements);
        __m128 const c45nn = _mm_load_ps(Detail::F32x4_CosC45nn.Elements);

        __m128 resultCos = Vector4F_MultiplyAdd(
            _mm_permute_ps(c45nn, _MM_SHUFFLE(1, 1, 1, 1)),
            y2,
            _mm_permute_ps(c45nn, _MM_SHUFFLE(0, 0, 0, 0)));

        resultCos = Vector4F_MultiplyAdd(resultCos, y2, _mm_permute_ps(c0123, _MM_SHUFFLE(3, 3, 3, 3)));
        resultCos = Vector4F_MultiplyAdd(resultCos, y2, _mm_permute_ps(c0123, _MM_SHUFFLE(2, 2, 2, 2)));
        resultCos = Vector4F_MultiplyAdd(resultCos, y2, _mm_permute_ps(c0123, _MM_SHUFFLE(1, 1, 1, 1)));
        resultCos = Vector4F_MultiplyAdd(resultCos, y2, _mm_permute_ps(c0123, _MM_SHUFFLE(0, 0, 0, 0)));
        return _mm_mul_ps(resultCos, sign);
#else
        alignas(16) float xs[4];
        _mm_store_ps(xs, v);
        return _mm_setr_ps(
            Cos<float>(xs[0]),
            Cos<float>(xs[1]),
            Cos<float>(xs[2]),
            Cos<float>(xs[3]));
#endif
#elif ANEMONE_FEATURE_NEON
        float const r[4]{
            Cos<float>(vgetq_lane_f32(v, 0)),
            Cos<float>(vgetq_lane_f32(v, 1)),
            Cos<float>(vgetq_lane_f32(v, 2)),
            Cos<float>(vgetq_lane_f32(v, 3)),
        };
        return vld1q_f32(r);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Tan(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Tan<float>(v.Inner[0]),
            Tan<float>(v.Inner[1]),
            Tan<float>(v.Inner[2]),
            Tan<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_SVML
        return _mm_tan_ps(v);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        alignas(16) float xs[4];
        _mm_store_ps(xs, v);
        return _mm_setr_ps(
            Tan<float>(xs[0]),
            Tan<float>(xs[1]),
            Tan<float>(xs[2]),
            Tan<float>(xs[3]));
#elif ANEMONE_FEATURE_NEON
        float const r[4]{
            Tan<float>(vgetq_lane_f32(v, 0)),
            Tan<float>(vgetq_lane_f32(v, 1)),
            Tan<float>(vgetq_lane_f32(v, 2)),
            Tan<float>(vgetq_lane_f32(v, 3)),
        };
        return vld1q_f32(r);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Asin(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Asin<float>(v.Inner[0]),
            Asin<float>(v.Inner[1]),
            Asin<float>(v.Inner[2]),
            Asin<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_SVML
        return _mm_asin_ps(v);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        alignas(16) float xs[4];
        _mm_store_ps(xs, v);
        return _mm_setr_ps(
            Asin<float>(xs[0]),
            Asin<float>(xs[1]),
            Asin<float>(xs[2]),
            Asin<float>(xs[3]));
#elif ANEMONE_FEATURE_NEON
        float const r[4]{
            Asin<float>(vgetq_lane_f32(v, 0)),
            Asin<float>(vgetq_lane_f32(v, 1)),
            Asin<float>(vgetq_lane_f32(v, 2)),
            Asin<float>(vgetq_lane_f32(v, 3)),
        };
        return vld1q_f32(r);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Acos(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Acos<float>(v.Inner[0]),
            Acos<float>(v.Inner[1]),
            Acos<float>(v.Inner[2]),
            Acos<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_SVML
        return _mm_acos_ps(v);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        alignas(16) float xs[4];
        _mm_store_ps(xs, v);
        return _mm_setr_ps(
            Acos<float>(xs[0]),
            Acos<float>(xs[1]),
            Acos<float>(xs[2]),
            Acos<float>(xs[3]));
#elif ANEMONE_FEATURE_NEON
        float const r[4]{
            Acos<float>(vgetq_lane_f32(v, 0)),
            Acos<float>(vgetq_lane_f32(v, 1)),
            Acos<float>(vgetq_lane_f32(v, 2)),
            Acos<float>(vgetq_lane_f32(v, 3)),
        };
        return vld1q_f32(r);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Atan(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Atan<float>(v.Inner[0]),
            Atan<float>(v.Inner[1]),
            Atan<float>(v.Inner[2]),
            Atan<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_SVML
        return _mm_atan_ps(v);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        alignas(16) float xs[4];
        _mm_store_ps(xs, v);
        return _mm_setr_ps(
            Atan<float>(xs[0]),
            Atan<float>(xs[1]),
            Atan<float>(xs[2]),
            Atan<float>(xs[3]));
#elif ANEMONE_FEATURE_NEON
        float const r[4]{
            Atan<float>(vgetq_lane_f32(v, 0)),
            Atan<float>(vgetq_lane_f32(v, 1)),
            Atan<float>(vgetq_lane_f32(v, 2)),
            Atan<float>(vgetq_lane_f32(v, 3)),
        };
        return vld1q_f32(r);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Atan2(SimdVector4F y, SimdVector4F x)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Atan2<float>(y.Inner[0], x.Inner[0]),
            Atan2<float>(y.Inner[1], x.Inner[1]),
            Atan2<float>(y.Inner[2], x.Inner[2]),
            Atan2<float>(y.Inner[3], x.Inner[3]),
        };
#elif ANEMONE_FEATURE_SVML
        return _mm_atan2_ps(y, x);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        alignas(16) float ys[4];
        alignas(16) float xs[4];
        _mm_store_ps(ys, y);
        _mm_store_ps(xs, x);
        return _mm_setr_ps(
            Atan2<float>(ys[0], xs[0]),
            Atan2<float>(ys[1], xs[1]),
            Atan2<float>(ys[2], xs[2]),
            Atan2<float>(ys[3], xs[3]));
#elif ANEMONE_FEATURE_NEON
        float const r[4]{
            Atan2<float>(vgetq_lane_f32(y, 0), vgetq_lane_f32(x, 0)),
            Atan2<float>(vgetq_lane_f32(y, 1), vgetq_lane_f32(x, 1)),
            Atan2<float>(vgetq_lane_f32(y, 2), vgetq_lane_f32(x, 2)),
            Atan2<float>(vgetq_lane_f32(y, 3), vgetq_lane_f32(x, 3)),
        };
        return vld1q_f32(r);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PreciseLerp(SimdVector4F a, SimdVector4F b, SimdVector4F t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            PreciseLerp<float>(a.Inner[0], b.Inner[0], t.Inner[0]),
            PreciseLerp<float>(a.Inner[1], b.Inner[1], t.Inner[1]),
            PreciseLerp<float>(a.Inner[2], b.Inner[2], t.Inner[2]),
            PreciseLerp<float>(a.Inner[3], b.Inner[3], t.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return Vector4F_MultiplyAdd(a, _mm_sub_ps(_mm_load_ps(F32x4_PositiveOne.As<float>()), t), _mm_mul_ps(b, t));
#elif ANEMONE_FEATURE_NEON
        float32x4_t const one = vdupq_n_f32(1.0f);
        float32x4_t const nt = vsubq_f32(one, t);
        return vmlaq_f32(vmulq_f32(b, t), a, nt);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PreciseLerp(SimdVector4F a, SimdVector4F b, float t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            PreciseLerp<float>(a.Inner[0], b.Inner[0], t),
            PreciseLerp<float>(a.Inner[1], b.Inner[1], t),
            PreciseLerp<float>(a.Inner[2], b.Inner[2], t),
            PreciseLerp<float>(a.Inner[3], b.Inner[3], t),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return Vector4F_MultiplyAdd(a, _mm_set1_ps(1.0f - t), _mm_mul_ps(b, _mm_set1_ps(t)));
#elif ANEMONE_FEATURE_NEON
        float const nt = 1.0f - t;
        return vmlaq_n_f32(vmulq_n_f32(b, t), a, nt);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Lerp(SimdVector4F a, SimdVector4F b, SimdVector4F t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Lerp<float>(a.Inner[0], b.Inner[0], t.Inner[0]),
            Lerp<float>(a.Inner[1], b.Inner[1], t.Inner[1]),
            Lerp<float>(a.Inner[2], b.Inner[2], t.Inner[2]),
            Lerp<float>(a.Inner[3], b.Inner[3], t.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const ba = _mm_sub_ps(b, a);
        return Vector4F_MultiplyAdd(ba, t, a);
#elif ANEMONE_FEATURE_NEON
        float32x4_t const ba = vsubq_f32(b, a);
        return vmlaq_f32(a, ba, t);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Lerp(SimdVector4F a, SimdVector4F b, float t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Lerp<float>(a.Inner[0], b.Inner[0], t),
            Lerp<float>(a.Inner[1], b.Inner[1], t),
            Lerp<float>(a.Inner[2], b.Inner[2], t),
            Lerp<float>(a.Inner[3], b.Inner[3], t),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const ba = _mm_sub_ps(b, a);
        return Vector4F_MultiplyAdd(ba, _mm_set1_ps(t), a);
#elif ANEMONE_FEATURE_NEON
        float32x4_t const ba = vsubq_f32(b, a);
        return vmlaq_n_f32(a, ba, t);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PreciseBilinearLerp(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F d, SimdVector4F u, SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            PreciseBilinearLerp<float>(a.Inner[0], b.Inner[0], c.Inner[0], d.Inner[0], u.Inner[0], v.Inner[0]),
            PreciseBilinearLerp<float>(a.Inner[1], b.Inner[1], c.Inner[1], d.Inner[1], u.Inner[1], v.Inner[1]),
            PreciseBilinearLerp<float>(a.Inner[2], b.Inner[2], c.Inner[2], d.Inner[2], u.Inner[2], v.Inner[2]),
            PreciseBilinearLerp<float>(a.Inner[3], b.Inner[3], c.Inner[3], d.Inner[3], u.Inner[3], v.Inner[3]),
        };
#else
        SimdVector4F const ab = Vector4F_PreciseLerp(a, b, u);
        SimdVector4F const cd = Vector4F_PreciseLerp(c, d, u);
        return Vector4F_PreciseLerp(ab, cd, v);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PreciseBilinearLerp(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F d, float u, float v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            PreciseBilinearLerp<float>(a.Inner[0], b.Inner[0], c.Inner[0], d.Inner[0], u, v),
            PreciseBilinearLerp<float>(a.Inner[1], b.Inner[1], c.Inner[1], d.Inner[1], u, v),
            PreciseBilinearLerp<float>(a.Inner[2], b.Inner[2], c.Inner[2], d.Inner[2], u, v),
            PreciseBilinearLerp<float>(a.Inner[3], b.Inner[3], c.Inner[3], d.Inner[3], u, v),
        };
#else
        SimdVector4F const ab = Vector4F_PreciseLerp(a, b, u);
        SimdVector4F const cd = Vector4F_PreciseLerp(c, d, u);
        return Vector4F_PreciseLerp(ab, cd, v);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BilinearLerp(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F d, SimdVector4F u, SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            BilinearLerp<float>(a.Inner[0], b.Inner[0], c.Inner[0], d.Inner[0], u.Inner[0], v.Inner[0]),
            BilinearLerp<float>(a.Inner[1], b.Inner[1], c.Inner[1], d.Inner[1], u.Inner[1], v.Inner[1]),
            BilinearLerp<float>(a.Inner[2], b.Inner[2], c.Inner[2], d.Inner[2], u.Inner[2], v.Inner[2]),
            BilinearLerp<float>(a.Inner[3], b.Inner[3], c.Inner[3], d.Inner[3], u.Inner[3], v.Inner[3]),
        };
#else
        SimdVector4F const x1 = Vector4F_Lerp(a, b, u);
        SimdVector4F const x2 = Vector4F_Lerp(c, d, u);
        return Vector4F_Lerp(x1, x2, v);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BilinearLerp(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F d, float u, float v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            BilinearLerp<float>(a.Inner[0], b.Inner[0], c.Inner[0], d.Inner[0], u, v),
            BilinearLerp<float>(a.Inner[1], b.Inner[1], c.Inner[1], d.Inner[1], u, v),
            BilinearLerp<float>(a.Inner[2], b.Inner[2], c.Inner[2], d.Inner[2], u, v),
            BilinearLerp<float>(a.Inner[3], b.Inner[3], c.Inner[3], d.Inner[3], u, v),
        };
#else
        SimdVector4F const x1 = Vector4F_Lerp(a, b, u);
        SimdVector4F const x2 = Vector4F_Lerp(c, d, u);
        return Vector4F_Lerp(x1, x2, v);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Barycentric(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F u, SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Barycentric<float>(a.Inner[0], b.Inner[0], c.Inner[0], u.Inner[0], v.Inner[0]),
            Barycentric<float>(a.Inner[1], b.Inner[1], c.Inner[1], u.Inner[1], v.Inner[1]),
            Barycentric<float>(a.Inner[2], b.Inner[2], c.Inner[2], u.Inner[2], v.Inner[2]),
            Barycentric<float>(a.Inner[3], b.Inner[3], c.Inner[3], u.Inner[3], v.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const ba = _mm_sub_ps(b, a);
        __m128 const ca = _mm_sub_ps(c, a);

        __m128 const r = Vector4F_MultiplyAdd(ba, u, a);
        return Vector4F_MultiplyAdd(ca, v, r);
#elif ANEMONE_FEATURE_NEON
        float32x4_t const ba = vsubq_f32(b, a);
        float32x4_t const ca = vsubq_f32(c, a);
        float32x4_t const r = vmlaq_f32(a, ba, u);
        return vmlaq_f32(r, ca, v);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Barycentric(SimdVector4F a, SimdVector4F b, SimdVector4F c, float u, float v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Barycentric<float>(a.Inner[0], b.Inner[0], c.Inner[0], u, v),
            Barycentric<float>(a.Inner[1], b.Inner[1], c.Inner[1], u, v),
            Barycentric<float>(a.Inner[2], b.Inner[2], c.Inner[2], u, v),
            Barycentric<float>(a.Inner[3], b.Inner[3], c.Inner[3], u, v),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const ba = _mm_sub_ps(b, a);
        __m128 const ca = _mm_sub_ps(c, a);

        __m128 const r = Vector4F_MultiplyAdd(ba, _mm_set1_ps(u), a);
        return Vector4F_MultiplyAdd(ca, _mm_set1_ps(v), r);
#elif ANEMONE_FEATURE_NEON
        float32x4_t const ba = vsubq_f32(b, a);
        float32x4_t const ca = vsubq_f32(c, a);

        float32x4_t const r = vmlaq_n_f32(a, ba, u);
        return vmlaq_n_f32(r, ca, v);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Unlerp(SimdVector4F value, SimdVector4F lower, SimdVector4F upper)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Unlerp<float>(value.Inner[0], lower.Inner[0], upper.Inner[0]),
            Unlerp<float>(value.Inner[1], lower.Inner[1], upper.Inner[1]),
            Unlerp<float>(value.Inner[2], lower.Inner[2], upper.Inner[2]),
            Unlerp<float>(value.Inner[3], lower.Inner[3], upper.Inner[3]),
        };
#else
        SimdVector4F const progress = Vector4F_Subtract(value, lower);
        SimdVector4F const range = Vector4F_Subtract(upper, lower);
        return Vector4F_Divide(progress, range);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Map(SimdVector4F value, SimdVector4F sourceLower, SimdVector4F sourceUpper, SimdVector4F targetLower, SimdVector4F targetUpper)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Map<float>(value.Inner[0], sourceLower.Inner[0], sourceUpper.Inner[0], targetLower.Inner[0], targetUpper.Inner[0]),
            Map<float>(value.Inner[1], sourceLower.Inner[1], sourceUpper.Inner[1], targetLower.Inner[1], targetUpper.Inner[1]),
            Map<float>(value.Inner[2], sourceLower.Inner[2], sourceUpper.Inner[2], targetLower.Inner[2], targetUpper.Inner[2]),
            Map<float>(value.Inner[3], sourceLower.Inner[3], sourceUpper.Inner[3], targetLower.Inner[3], targetUpper.Inner[3]),
        };
#else
        SimdVector4F const progress = Vector4F_Unlerp(value, sourceLower, sourceUpper);
        return Vector4F_Lerp(targetLower, targetUpper, progress);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PreciseMap(SimdVector4F value, SimdVector4F sourceLower, SimdVector4F sourceUpper, SimdVector4F targetLower, SimdVector4F targetUpper)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            PreciseMap<float>(value.Inner[0], sourceLower.Inner[0], sourceUpper.Inner[0], targetLower.Inner[0], targetUpper.Inner[0]),
            PreciseMap<float>(value.Inner[1], sourceLower.Inner[1], sourceUpper.Inner[1], targetLower.Inner[1], targetUpper.Inner[1]),
            PreciseMap<float>(value.Inner[2], sourceLower.Inner[2], sourceUpper.Inner[2], targetLower.Inner[2], targetUpper.Inner[2]),
            PreciseMap<float>(value.Inner[3], sourceLower.Inner[3], sourceUpper.Inner[3], targetLower.Inner[3], targetUpper.Inner[3]),
        };
#else
        SimdVector4F const progress = Vector4F_Unlerp(value, sourceLower, sourceUpper);
        return Vector4F_PreciseLerp(targetLower, targetUpper, progress);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierPosition(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            BezierPosition<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], t.Inner[0]),
            BezierPosition<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], t.Inner[1]),
            BezierPosition<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], t.Inner[2]),
            BezierPosition<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], t.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Cubic_B%C3%A9zier_curves
        // Note: This one uses matrix representation to compress the computation.

        // B(t) = ( 1 * t^2 - 2 * t + 1) * p0
        //      + (-2 * t^2 + 2 * t    ) * p1
        //      + ( 1 * t^2            ) * p2

        // = [t^2, t^2, t^2, t^2]
        __m128 const t2 = _mm_mul_ps(t, t);

        // = [1, -2, 1, 0]
        __m128 const c2 = _mm_setr_ps(1.0f, -2.0f, 1.0f, 0.0f);
        // = [-2, 2, 0, 0]
        __m128 const c1 = _mm_setr_ps(-2.0f, 2.0f, 0.0f, 0.0f);
        // = [1, 0, 0, 0]
        __m128 const c0 = _mm_set_ss(1.0f);

        // Multiply by coefficients
        // = (c2 * t2 + (c1 * t + c0))
        __m128 cc = Vector4F_MultiplyAdd(c1, t, c0);
        cc = Vector4F_MultiplyAdd(c2, t2, cc);

        // Evaluate
        __m128 r = _mm_mul_ps(p0, _mm_permute_ps(cc, _MM_SHUFFLE(0, 0, 0, 0)));
        r = Vector4F_MultiplyAdd(p1, _mm_permute_ps(cc, _MM_SHUFFLE(1, 1, 1, 1)), r);
        r = Vector4F_MultiplyAdd(p2, _mm_permute_ps(cc, _MM_SHUFFLE(2, 2, 2, 2)), r);
        return r;
#elif ANEMONE_FEATURE_NEON
        // https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Cubic_B%C3%A9zier_curves
        // Note: This one uses matrix representation to compress the computation.

        // B(t) = ( 1 * t^2 - 2 * t + 1) * p0
        //      + (-2 * t^2 + 2 * t    ) * p1
        //      + ( 1 * t^2            ) * p2

        // = [t^2, t^2, t^2, t^2]
        float32x4_t const t2 = vmulq_f32(t, t);

        // = [1, -2, 1, 0]
        static constexpr float fc2[4]{1.0f, -2.0f, 1.0f, 0.0f};
        float32x4_t const c2 = vld1q_f32(fc2);
        // = [-2, 2, 0, 0]
        static constexpr float fc1[4]{-2.0f, 2.0f, 0.0f, 0.0f};
        float32x4_t const c1 = vld1q_f32(fc1);
        // = [1, 0, 0, 0]
        static constexpr float fc0[4]{1.0f, 0.0f, 0.0f, 0.0f};
        float32x4_t const c0 = vld1q_f32(fc0);

        // Multiply by coefficients
        // = (c2 * t2 + (c1 * t + c0))
        float32x4_t cc = vmlaq_f32(c0, c1, t);
        cc = vmlaq_f32(cc, c2, t2);

        // Evaluate
        float32x4_t r = vmulq_laneq_f32(p0, cc, 0);
        r = vmlaq_laneq_f32(r, p1, cc, 1);
        r = vmlaq_laneq_f32(r, p2, cc, 2);
        return r;
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierPosition(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, float t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            BezierPosition<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], t),
            BezierPosition<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], t),
            BezierPosition<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], t),
            BezierPosition<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], t),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return Vector4F_BezierPosition(p0, p1, p2, _mm_set1_ps(t));
#elif ANEMONE_FEATURE_NEON
        return Vector4F_BezierPosition(p0, p1, p2, vdupq_n_f32(t));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierTangent(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            BezierTangent<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], t.Inner[0]),
            BezierTangent<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], t.Inner[1]),
            BezierTangent<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], t.Inner[2]),
            BezierTangent<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], t.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Cubic_B%C3%A9zier_curves
        // Note: This one uses matrix representation to compress the computation.

        // B'(t) = ( 2 * t - 2) * p0
        //       + (-4 * t + 2) * p1
        //       + ( 2 * t    ) * p2

        // = [2, -4, 2, 0]
        __m128 const c1 = _mm_setr_ps(2.0f, -4.0f, 2.0f, 0.0f);
        // = [-2, 2, 0, 0]
        __m128 const c0 = _mm_setr_ps(-2.0f, 2.0f, 0.0f, 0.0f);

        // Multiply by coefficients
        __m128 const cc = Vector4F_MultiplyAdd(c1, t, c0);

        // Evaluate
        __m128 r = _mm_mul_ps(p0, _mm_permute_ps(cc, _MM_SHUFFLE(0, 0, 0, 0)));
        r = Vector4F_MultiplyAdd(p1, _mm_permute_ps(cc, _MM_SHUFFLE(1, 1, 1, 1)), r);
        r = Vector4F_MultiplyAdd(p2, _mm_permute_ps(cc, _MM_SHUFFLE(2, 2, 2, 2)), r);
        return r;
#elif ANEMONE_FEATURE_NEON
        // https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Cubic_B%C3%A9zier_curves
        // Note: This one uses matrix representation to compress the computation.

        // B'(t) = ( 2 * t - 2) * p0
        //       + (-4 * t + 2) * p1
        //       + ( 2 * t    ) * p2

        // = [2, -4, 2, 0]
        static constexpr float const fc1[4]{2.0f, -4.0f, 2.0f, 0.0f};
        float32x4_t const c1 = vld1q_f32(fc1);
        // = [-2, 2, 0, 0]
        static constexpr float const fc0[4]{-2.0f, 2.0f, 0.0f, 0.0f};
        float32x4_t const c0 = vld1q_f32(fc0);

        // Multiply by coefficients
        float32x4_t const cc = vmlaq_f32(c0, c1, t);

        // Evaluate
        float32x4_t r = vmulq_laneq_f32(p0, cc, 0);
        r = vmlaq_laneq_f32(r, p1, cc, 1);
        r = vmlaq_laneq_f32(r, p2, cc, 2);
        return r;
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierTangent(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, float t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            BezierTangent<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], t),
            BezierTangent<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], t),
            BezierTangent<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], t),
            BezierTangent<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], t),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return Vector4F_BezierTangent(p0, p1, p2, _mm_set1_ps(t));
#elif ANEMONE_FEATURE_NEON
        return Vector4F_BezierTangent(p0, p1, p2, vdupq_n_f32(t));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierAcceleration(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            BezierAcceleration<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], t.Inner[0]),
            BezierAcceleration<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], t.Inner[1]),
            BezierAcceleration<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], t.Inner[2]),
            BezierAcceleration<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], t.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // Time value is not used to compute acceleration at a specific point.
        (void)t;

        // https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Cubic_B%C3%A9zier_curves
        // Note: This one uses matrix representation to compress the computation.

        // B''(t) = ( 2) * p0
        //        + (-4) * p1
        //        + ( 2) * p2

        // = [2, 2, 2, 2]
        __m128 const c0 = _mm_set1_ps(2.0f);
        // = [-4, -4, -4, -4]
        __m128 const c1 = _mm_set1_ps(-4.0f);

        // Evaluate
        __m128 r = _mm_mul_ps(p0, c0);
        r = Vector4F_MultiplyAdd(p1, c1, r);
        r = Vector4F_MultiplyAdd(p2, c0, r);
        return r;
#elif ANEMONE_FEATURE_NEON
        // Time value is not used to compute acceleration at a specific point.
        (void)t;

        // https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Cubic_B%C3%A9zier_curves
        // Note: This one uses matrix representation to compress the computation.

        // B''(t) = ( 2) * p0
        //        + (-4) * p1
        //        + ( 2) * p2

        // = [2, 2, 2, 2]
        float32x4_t const c0 = vdupq_n_f32(2.0f);
        // = [-4, -4, -4, -4]
        float32x4_t const c1 = vdupq_n_f32(-4.0f);

        // Evaluate
        float32x4_t r = vmulq_f32(p0, c0);
        r = vmlaq_f32(r, p1, c1);
        r = vmlaq_f32(r, p2, c0);
        return r;
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierAcceleration(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, float t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            BezierAcceleration<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], t),
            BezierAcceleration<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], t),
            BezierAcceleration<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], t),
            BezierAcceleration<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], t),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return Vector4F_BezierAcceleration(p0, p1, p2, _mm_set1_ps(t));
#elif ANEMONE_FEATURE_NEON
        return Vector4F_BezierAcceleration(p0, p1, p2, vdupq_n_f32(t));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierPosition(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, SimdVector4F t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            BezierPosition<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], p3.Inner[0], t.Inner[0]),
            BezierPosition<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], p3.Inner[1], t.Inner[1]),
            BezierPosition<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], p3.Inner[2], t.Inner[2]),
            BezierPosition<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], p3.Inner[3], t.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Cubic_B%C3%A9zier_curves
        // Note: This one uses matrix representation to compress the computation.

        // B(t) = (-1 * t^3 + 3 * t^2 - 3 * t + 1) * p0
        //      + ( 3 * t^3 - 6 * t^2 + 3 * t    ) * p1
        //      + (-3 * t^3 + 3 * t^2            ) * p2
        //      + ( 1 * t^3                      ) * p3

        // = [t^2, t^2, t^2, t^2]
        __m128 const t2 = _mm_mul_ps(t, t);
        // = [t^3, t^3, t^3, t^3]
        __m128 const t3 = _mm_mul_ps(t2, t);

        // = [-1, 3, -3, 1]
        __m128 const c3 = _mm_setr_ps(-1.0f, 3.0f, -3.0f, 1.0f);
        // = [3, -6, 3, 0]
        __m128 const c2 = _mm_setr_ps(3.0f, -6.0f, 3.0f, 0.0f);
        // = [-3, 3, 0, 0]
        __m128 const c1 = _mm_setr_ps(-3.0f, 3.0f, 0.0f, 0.0f);
        // = [1, 0, 0, 0]
        __m128 const c0 = _mm_set_ss(1.0f);

        // Multiply by coefficients
        // = (c3 * t3 + (c2 * t2)) + (c1 * t + c0)

        __m128 const r0 = _mm_mul_ps(c2, t2);
        __m128 const r1 = Vector4F_MultiplyAdd(c3, t3, r0);
        __m128 const r2 = Vector4F_MultiplyAdd(c1, t, c0);
        __m128 const cc = _mm_add_ps(r1, r2);

        // Evaluate
        __m128 r = _mm_mul_ps(p0, _mm_permute_ps(cc, _MM_SHUFFLE(0, 0, 0, 0)));
        r = Vector4F_MultiplyAdd(p1, _mm_permute_ps(cc, _MM_SHUFFLE(1, 1, 1, 1)), r);
        r = Vector4F_MultiplyAdd(p2, _mm_permute_ps(cc, _MM_SHUFFLE(2, 2, 2, 2)), r);
        r = Vector4F_MultiplyAdd(p3, _mm_permute_ps(cc, _MM_SHUFFLE(3, 3, 3, 3)), r);
        return r;
#elif ANEMONE_FEATURE_NEON
        // https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Cubic_B%C3%A9zier_curves
        // Note: This one uses matrix representation to compress the computation.

        // B(t) = (-1 * t^3 + 3 * t^2 - 3 * t + 1) * p0
        //      + ( 3 * t^3 - 6 * t^2 + 3 * t    ) * p1
        //      + (-3 * t^3 + 3 * t^2            ) * p2
        //      + ( 1 * t^3                      ) * p3

        // = [t^2, t^2, t^2, t^2]
        float32x4_t const t2 = vmulq_f32(t, t);
        // = [t^3, t^3, t^3, t^3]
        float32x4_t const t3 = vmulq_f32(t2, t);

        // = [-1, 3, -3, 1]
        static constexpr float fc3[4]{-1.0f, 3.0f, -3.0f, 1.0f};
        float32x4_t const c3 = vld1q_f32(fc3);
        // = [3, -6, 3, 0]
        static constexpr float fc2[4]{3.0f, -6.0f, 3.0f, 0.0f};
        float32x4_t const c2 = vld1q_f32(fc2);
        // = [-3, 3, 0, 0]
        static constexpr float fc1[4]{-3.0f, 3.0f, 0.0f, 0.0f};
        float32x4_t const c1 = vld1q_f32(fc1);
        // = [1, 0, 0, 0]
        static constexpr float fc0[4]{1.0f, 0.0f, 0.0f, 0.0f};
        float32x4_t const c0 = vld1q_f32(fc0);

        // Multiply by coefficients
        // = (c3 * t3 + (c2 * t2)) + (c1 * t + c0)

        float32x4_t const r0 = vmulq_f32(c2, t2);
        float32x4_t const r1 = vmlaq_f32(r0, c3, t3);
        float32x4_t const r2 = vmlaq_f32(c0, c1, t);
        float32x4_t const cc = vaddq_f32(r1, r2);

        // Evaluate
        float32x4_t r = vmulq_laneq_f32(p0, cc, 0);
        r = vmlaq_laneq_f32(r, p1, cc, 1);
        r = vmlaq_laneq_f32(r, p2, cc, 2);
        r = vmlaq_laneq_f32(r, p3, cc, 3);
        return r;
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierPosition(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, float t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            BezierPosition<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], p3.Inner[0], t),
            BezierPosition<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], p3.Inner[1], t),
            BezierPosition<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], p3.Inner[2], t),
            BezierPosition<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], p3.Inner[3], t),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return Vector4F_BezierPosition(p0, p1, p2, p3, _mm_set1_ps(t));
#elif ANEMONE_FEATURE_NEON
        return Vector4F_BezierPosition(p0, p1, p2, p3, vdupq_n_f32(t));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierTangent(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, SimdVector4F t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            BezierTangent<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], p3.Inner[0], t.Inner[0]),
            BezierTangent<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], p3.Inner[1], t.Inner[1]),
            BezierTangent<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], p3.Inner[2], t.Inner[2]),
            BezierTangent<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], p3.Inner[3], t.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Cubic_B%C3%A9zier_curves
        // Note: This one uses matrix representation to compress the computation.

        // B'(t) = (-3 * t^2 +  6 * t - 3) * p0
        //       + ( 9 * t^2 - 12 * t + 3) * p1
        //       + (-9 * t^2 +  6 * t    ) * p2
        //       + ( 3 * t^2             ) * p3

        // = [t^2, t^2, t^2, t^2]
        __m128 const t2 = _mm_mul_ps(t, t);

        // = [-3, 9, -9, 3]
        __m128 const c2 = _mm_setr_ps(-3.0f, 9.0f, -9.0f, 3.0f);
        // = [6, -12, 6, 0]
        __m128 const c1 = _mm_setr_ps(6.0f, -12.0f, 6.0f, 0.0f);
        // = [-3, 3, 0, 0]
        __m128 const c0 = _mm_setr_ps(-3.0f, 3.0f, 0.0f, 0.0f);

        // Multiply by coefficients
        // = (c2 * t2 + (c1 * t + c0))
        __m128 cc = Vector4F_MultiplyAdd(c1, t, c0);
        cc = Vector4F_MultiplyAdd(c2, t2, cc);

        // Evaluate
        __m128 r = _mm_mul_ps(p0, _mm_permute_ps(cc, _MM_SHUFFLE(0, 0, 0, 0)));
        r = Vector4F_MultiplyAdd(p1, _mm_permute_ps(cc, _MM_SHUFFLE(1, 1, 1, 1)), r);
        r = Vector4F_MultiplyAdd(p2, _mm_permute_ps(cc, _MM_SHUFFLE(2, 2, 2, 2)), r);
        r = Vector4F_MultiplyAdd(p3, _mm_permute_ps(cc, _MM_SHUFFLE(3, 3, 3, 3)), r);
        return r;
#elif ANEMONE_FEATURE_NEON
        // https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Cubic_B%C3%A9zier_curves
        // Note: This one uses matrix representation to compress the computation.

        // B'(t) = (-3 * t^2 +  6 * t - 3) * p0
        //       + ( 9 * t^2 - 12 * t + 3) * p1
        //       + (-9 * t^2 +  6 * t    ) * p2
        //       + ( 3 * t^2             ) * p3

        // = [t^2, t^2, t^2, t^2]
        float32x4_t const t2 = vmulq_f32(t, t);

        // = [-3, 9, -9, 3]
        static constexpr float fc2[4]{-3.0f, 9.0f, -9.0f, 3.0f};
        float32x4_t const c2 = vld1q_f32(fc2);
        // = [6, -12, 6, 0]
        static constexpr float fc1[4]{6.0f, -12.0f, 6.0f, 0.0f};
        float32x4_t const c1 = vld1q_f32(fc1);
        // = [-3, 3, 0, 0]
        static constexpr float fc0[4]{-3.0f, 3.0f, 0.0f, 0.0f};
        float32x4_t const c0 = vld1q_f32(fc0);

        // Multiply by coefficients
        // = (c2 * t2 + (c1 * t + c0))
        float32x4_t cc = vmlaq_f32(c0, c1, t);
        cc = vmlaq_f32(cc, c2, t2);

        // Evaluate
        float32x4_t r = vmulq_laneq_f32(p0, cc, 0);
        r = vmlaq_laneq_f32(r, p1, cc, 1);
        r = vmlaq_laneq_f32(r, p2, cc, 2);
        r = vmlaq_laneq_f32(r, p3, cc, 3);
        return r;
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierTangent(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, float t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            BezierTangent<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], p3.Inner[0], t),
            BezierTangent<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], p3.Inner[1], t),
            BezierTangent<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], p3.Inner[2], t),
            BezierTangent<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], p3.Inner[3], t),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return Vector4F_BezierTangent(p0, p1, p2, p3, _mm_set1_ps(t));
#elif ANEMONE_FEATURE_NEON
        return Vector4F_BezierTangent(p0, p1, p2, p3, vdupq_n_f32(t));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierAcceleration(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, SimdVector4F t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            BezierAcceleration<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], p3.Inner[0], t.Inner[0]),
            BezierAcceleration<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], p3.Inner[1], t.Inner[1]),
            BezierAcceleration<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], p3.Inner[2], t.Inner[2]),
            BezierAcceleration<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], p3.Inner[3], t.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Cubic_B%C3%A9zier_curves
        // Note: This one uses matrix representation to compress the computation.

        // B''(t) = ( -6 * t +  6) * p0
        //        + ( 18 * t - 12) * p1
        //        + (-18 * t +  6) * p2
        //        + (  6 * t     ) * p3

        // = [-6, 18, -18, 6]
        __m128 const c1 = _mm_setr_ps(-6.0f, 18.0f, -18.0f, 6.0f);
        // = [6, -12, 6, 0]
        __m128 const c0 = _mm_setr_ps(6.0f, -12.0f, 6.0f, 0.0f);

        // Multiply by coefficients
        // = (c1 * t + c0)
        __m128 const cc = Vector4F_MultiplyAdd(c1, t, c0);

        // Evaluate
        __m128 r = _mm_mul_ps(p0, _mm_permute_ps(cc, _MM_SHUFFLE(0, 0, 0, 0)));
        r = Vector4F_MultiplyAdd(p1, _mm_permute_ps(cc, _MM_SHUFFLE(1, 1, 1, 1)), r);
        r = Vector4F_MultiplyAdd(p2, _mm_permute_ps(cc, _MM_SHUFFLE(2, 2, 2, 2)), r);
        r = Vector4F_MultiplyAdd(p3, _mm_permute_ps(cc, _MM_SHUFFLE(3, 3, 3, 3)), r);
        return r;
#elif ANEMONE_FEATURE_NEON
        // https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Cubic_B%C3%A9zier_curves
        // Note: This one uses matrix representation to compress the computation.

        // B''(t) = ( -6 * t +  6) * p0
        //        + ( 18 * t - 12) * p1
        //        + (-18 * t +  6) * p2
        //        + (  6 * t     ) * p3

        // = [-6, 18, -18, 6]
        static constexpr float fc1[4]{-6.0f, 18.0f, -18.0f, 6.0f};
        float32x4_t const c1 = vld1q_f32(fc1);
        // = [6, -12, 6, 0]
        static constexpr float fc0[4]{6.0f, -12.0f, 6.0f, 0.0f};
        float32x4_t const c0 = vld1q_f32(fc0);

        // Multiply by coefficients
        // = (c1 * t + c0)
        float32x4_t const cc = vmlaq_f32(c0, c1, t);

        // Evaluate
        float32x4_t r = vmulq_laneq_f32(p0, cc, 0);
        r = vmlaq_laneq_f32(r, p1, cc, 1);
        r = vmlaq_laneq_f32(r, p2, cc, 2);
        r = vmlaq_laneq_f32(r, p3, cc, 3);
        return r;
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierAcceleration(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, float t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            BezierAcceleration<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], p3.Inner[0], t),
            BezierAcceleration<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], p3.Inner[1], t),
            BezierAcceleration<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], p3.Inner[2], t),
            BezierAcceleration<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], p3.Inner[3], t),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return Vector4F_BezierAcceleration(p0, p1, p2, p3, _mm_set1_ps(t));
#elif ANEMONE_FEATURE_NEON
        return Vector4F_BezierAcceleration(p0, p1, p2, p3, vdupq_n_f32(t));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Hermite(SimdVector4F p0, SimdVector4F m0, SimdVector4F p1, SimdVector4F m1, SimdVector4F t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Hermite<float>(p0.Inner[0], m0.Inner[0], p1.Inner[0], m1.Inner[0], t.Inner[0]),
            Hermite<float>(p0.Inner[1], m0.Inner[1], p1.Inner[1], m1.Inner[1], t.Inner[1]),
            Hermite<float>(p0.Inner[2], m0.Inner[2], p1.Inner[2], m1.Inner[2], t.Inner[2]),
            Hermite<float>(p0.Inner[3], m0.Inner[3], p1.Inner[3], m1.Inner[3], t.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // https://en.wikipedia.org/wiki/Cubic_Hermite_spline
        // Note: This one uses matrix representation to compress the computation.

        // H(t) = ( 2 * t^3 - 3 * t^2         + 1) * p0
        //      + ( 1 * t^3 - 2 * t^2 + 1 * t    ) * m0
        //      + (-2 * t^3 + 3 * t^2            ) * p1
        //      + ( 1   t^3 - 1 * t^2            ) * m1

        // = [t^2, t^2, t^2, t^2]
        __m128 const t2 = _mm_mul_ps(t, t);
        // = [t^3, t^3, t^3, t^3]
        __m128 const t3 = _mm_mul_ps(t2, t);

        __m128 const c3 = _mm_setr_ps(2.0f, 1.0f, -2.0f, 1.0f);
        __m128 const c2 = _mm_setr_ps(-3.0f, -2.0f, 3.0f, -1.0f);
        __m128 const c0 = _mm_set_ss(1.0f);

        // Multiply by coefficients
        // = (c3 * t3 + (c2 * t2)) + ((c1 * t) + c0)
        __m128 const r0 = _mm_mul_ps(c2, t2);
        __m128 const r1 = Vector4F_MultiplyAdd(c3, t3, r0);
        // = t * [0, 1, 0, 0]
        __m128 const r2 = _mm_and_ps(t, _mm_load_ps(F32x4_SelectMask_nXnn.As<float>()));
        __m128 const r3 = _mm_add_ps(r2, c0);
        __m128 const cc = _mm_add_ps(r1, r3);

        // Evaluate
        __m128 r = _mm_mul_ps(p0, _mm_permute_ps(cc, _MM_SHUFFLE(0, 0, 0, 0)));
        r = Vector4F_MultiplyAdd(m0, _mm_permute_ps(cc, _MM_SHUFFLE(1, 1, 1, 1)), r);
        r = Vector4F_MultiplyAdd(p1, _mm_permute_ps(cc, _MM_SHUFFLE(2, 2, 2, 2)), r);
        r = Vector4F_MultiplyAdd(m1, _mm_permute_ps(cc, _MM_SHUFFLE(3, 3, 3, 3)), r);
        return r;
#elif ANEMONE_FEATURE_NEON
        // https://en.wikipedia.org/wiki/Cubic_Hermite_spline
        // Note: This one uses matrix representation to compress the computation.

        // H(t) = ( 2 * t^3 - 3 * t^2         + 1) * p0
        //      + ( 1 * t^3 - 2 * t^2 + 1 * t    ) * m0
        //      + (-2 * t^3 + 3 * t^2            ) * p1
        //      + ( 1   t^3 - 1 * t^2            ) * m1

        // = [t^2, t^2, t^2, t^2]
        float32x4_t const t2 = vmulq_f32(t, t);
        // = [t^3, t^3, t^3, t^3]
        float32x4_t const t3 = vmulq_f32(t2, t);

        static constexpr float fc3[4]{2.0f, 1.0f, -2.0f, 1.0f};
        float32x4_t const c3 = vld1q_f32(fc3);
        static constexpr float fc2[4]{-3.0f, -2.0f, 3.0f, -1.0f};
        float32x4_t const c2 = vld1q_f32(fc2);
        float32x4_t const c0 = vsetq_lane_f32(1.0f, vdupq_n_f32(0.0f), 0);

        // Multiply by coefficients
        // = (c3 * t3 + (c2 * t2)) + ((c1 * t) + c0)
        float32x4_t const r0 = vmulq_f32(c2, t2);
        float32x4_t const r1 = vmlaq_f32(r0, c3, t3);

        // = [0, t, 0, 0]
        float32x4_t const zero = vdupq_n_f32(0.0f);
        float32x4_t const r2 = vcopyq_laneq_f32(zero, 1, t, 0);

        float32x4_t const r3 = vaddq_f32(r2, c0);
        float32x4_t const cc = vaddq_f32(r1, r3);

        // Evaluate
        float32x4_t r = vmulq_laneq_f32(p0, cc, 0);
        r = vmlaq_laneq_f32(r, m0, cc, 1);
        r = vmlaq_laneq_f32(r, p1, cc, 2);
        r = vmlaq_laneq_f32(r, m1, cc, 3);
        return r;
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Hermite(SimdVector4F p0, SimdVector4F m0, SimdVector4F p1, SimdVector4F m1, float t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Hermite<float>(p0.Inner[0], m0.Inner[0], p1.Inner[0], m1.Inner[0], t),
            Hermite<float>(p0.Inner[1], m0.Inner[1], p1.Inner[1], m1.Inner[1], t),
            Hermite<float>(p0.Inner[2], m0.Inner[2], p1.Inner[2], m1.Inner[2], t),
            Hermite<float>(p0.Inner[3], m0.Inner[3], p1.Inner[3], m1.Inner[3], t),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return Vector4F_Hermite(p0, m0, p1, m1, _mm_set1_ps(t));
#elif ANEMONE_FEATURE_NEON
        return Vector4F_Hermite(p0, m0, p1, m1, vdupq_n_f32(t));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_CatmullRom(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, SimdVector4F t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            CatmullRom<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], p3.Inner[0], t.Inner[0]),
            CatmullRom<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], p3.Inner[1], t.Inner[1]),
            CatmullRom<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], p3.Inner[2], t.Inner[2]),
            CatmullRom<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], p3.Inner[3], t.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // https://en.wikipedia.org/wiki/Cubic_Hermite_spline#Catmull%E2%80%93Rom_spline

        // C(t) = 0.5 * (
        //         (( -t^3 + 2t^2 - t    ) * p0) +
        //         (( 3t^3 - 5t^2     + 2) * p1) +
        //         ((-3t^3 + 4t^2 + t    ) * p2) +
        //         ((  t^3 -  t^2        ) * p3)
        //      )

        __m128 const c3 = _mm_setr_ps(-1.0f, 3.0f, -3.0f, 1.0f);
        __m128 const c2 = _mm_setr_ps(2.0f, -5.0f, 4.0f, -1.0f);
        __m128 const c1 = _mm_setr_ps(-1.0f, 0.0f, 1.0f, 0.0f);
        __m128 const c0 = _mm_setr_ps(0.0f, 2.0f, 0.0f, 0.0f);

        // = [t^2, t^2, t^2, t^2]
        __m128 const t2 = _mm_mul_ps(t, t);

        // = [t^3, t^3, t^3, t^3]
        __m128 const t3 = _mm_mul_ps(t2, t);

        // Multiply [t^3, t^2, t^1, 1] by coefficients
        // = (c3 * t3 + (c2 * t2)) + (c1 * t + c0)

        __m128 const r0 = _mm_mul_ps(c2, t2);
        __m128 const r1 = Vector4F_MultiplyAdd(c3, t3, r0);
        __m128 const r2 = Vector4F_MultiplyAdd(c1, t, c0);
        __m128 const cc = _mm_add_ps(r1, r2);

        __m128 r = _mm_mul_ps(p0, _mm_permute_ps(cc, _MM_SHUFFLE(0, 0, 0, 0)));
        r = Vector4F_MultiplyAdd(p1, _mm_permute_ps(cc, _MM_SHUFFLE(1, 1, 1, 1)), r);
        r = Vector4F_MultiplyAdd(p2, _mm_permute_ps(cc, _MM_SHUFFLE(2, 2, 2, 2)), r);
        r = Vector4F_MultiplyAdd(p3, _mm_permute_ps(cc, _MM_SHUFFLE(3, 3, 3, 3)), r);

        return _mm_mul_ps(r, _mm_set1_ps(0.5f));
#elif ANEMONE_FEATURE_NEON
        // https://en.wikipedia.org/wiki/Cubic_Hermite_spline#Catmull%E2%80%93Rom_spline

        // C(t) = 0.5 * (
        //         (( -t^3 + 2t^2 - t    ) * p0) +
        //         (( 3t^3 - 5t^2     + 2) * p1) +
        //         ((-3t^3 + 4t^2 + t    ) * p2) +
        //         ((  t^3 -  t^2        ) * p3)
        //      )

        static constexpr float fc3[4]{-1.0f, 3.0f, -3.0f, 1.0f};
        float32x4_t const c3 = vld1q_f32(fc3);
        static constexpr float fc2[4]{2.0f, -5.0f, 4.0f, -1.0f};
        float32x4_t const c2 = vld1q_f32(fc2);
        static constexpr float fc1[4]{-1.0f, 0.0f, 1.0f, 0.0f};
        float32x4_t const c1 = vld1q_f32(fc1);
        static constexpr float fc0[4]{0.0f, 2.0f, 0.0f, 0.0f};
        float32x4_t const c0 = vld1q_f32(fc0);

        // = [t^2, t^2, t^2, t^2]
        float32x4_t const t2 = vmulq_f32(t, t);

        // = [t^3, t^3, t^3, t^3]
        float32x4_t const t3 = vmulq_f32(t2, t);

        // Multiply [t^3, t^2, t^1, 1] by coefficients
        // = (c3 * t3 + (c2 * t2)) + (c1 * t + c0)

        float32x4_t const r0 = vmulq_f32(c2, t2);
        float32x4_t const r1 = vmlaq_f32(r0, c3, t3);
        float32x4_t const r2 = vmlaq_f32(c0, c1, t);
        float32x4_t const cc = vaddq_f32(r1, r2);

        float32x4_t r = vmulq_laneq_f32(p0, cc, 0);
        r = vmlaq_laneq_f32(r, p1, cc, 1);
        r = vmlaq_laneq_f32(r, p2, cc, 2);
        r = vmlaq_laneq_f32(r, p3, cc, 3);

        return vmulq_f32(r, vdupq_n_f32(0.5f));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_CatmullRom(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, float t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            CatmullRom<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], p3.Inner[0], t),
            CatmullRom<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], p3.Inner[1], t),
            CatmullRom<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], p3.Inner[2], t),
            CatmullRom<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], p3.Inner[3], t),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return Vector4F_CatmullRom(p0, p1, p2, p3, _mm_set1_ps(t));
#elif ANEMONE_FEATURE_NEON
        return Vector4F_CatmullRom(p0, p1, p2, p3, vdupq_n_f32(t));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Midpoint(SimdVector4F p0, SimdVector4F p1)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            Midpoint<float>(p0.Inner[0], p1.Inner[0]),
            Midpoint<float>(p0.Inner[1], p1.Inner[1]),
            Midpoint<float>(p0.Inner[2], p1.Inner[2]),
            Midpoint<float>(p0.Inner[3], p1.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_mul_ps(_mm_add_ps(p0, p1), _mm_set1_ps(0.5f));
#elif ANEMONE_FEATURE_NEON
        return vmulq_f32(vaddq_f32(p0, p1), vdupq_n_f32(0.5f));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_DeCasteljau(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, SimdVector4F t)
    {
        // https://en.wikipedia.org/wiki/De_Casteljau%27s_algorithm

        SimdVector4F const a0 = Vector4F_Lerp(p0, p1, t);
        SimdVector4F const a1 = Vector4F_Lerp(p1, p2, t);
        SimdVector4F const a2 = Vector4F_Lerp(p2, p3, t);

        SimdVector4F const b0 = Vector4F_Lerp(a0, a1, t);
        SimdVector4F const b1 = Vector4F_Lerp(a1, a2, t);

        return Vector4F_Lerp(b0, b1, t);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_UnwindRadians(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            UnwindRadians<float>(v.Inner[0]),
            UnwindRadians<float>(v.Inner[1]),
            UnwindRadians<float>(v.Inner[2]),
            UnwindRadians<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const vToRevolutions = _mm_load_ps(F32x4_InvPi2.As<float>());
        __m128 const vFromRevolutions = _mm_load_ps(F32x4_Pi2.As<float>());

        // Convert radians to full revolutions
        __m128 r = _mm_mul_ps(v, vToRevolutions);
        r = Vector4F_Round(r);
        // Convert full revolutions back to radians and subtract from original
        return Vector4F_NegateMultiplyAdd(r, vFromRevolutions, v);
#elif ANEMONE_FEATURE_NEON
        float32x4_t const vToRevolutions = vld1q_f32(F32x4_InvPi2.As<float>());
        float32x4_t const vFromRevolutions = vld1q_f32(F32x4_Pi2.As<float>());

        // Convert radians to full revolutions
        float32x4_t r = vmulq_f32(v, vToRevolutions);
        r = Vector4F_Round(r);
        // Convert full revolutions back to radians and subtract from original
        return Vector4F_NegateMultiplyAdd(r, vFromRevolutions, v);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_UnwindDegrees(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            UnwindDegrees<float>(v.Inner[0]),
            UnwindDegrees<float>(v.Inner[1]),
            UnwindDegrees<float>(v.Inner[2]),
            UnwindDegrees<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const vToRevolutions = _mm_set1_ps(Factor_DegreesToRevolutions<float>);
        __m128 const vFromRevolutions = _mm_set1_ps(Factor_RevolutionsToDegrees<float>);

        // Convert degrees to full revolutions
        __m128 r = _mm_mul_ps(v, vToRevolutions);
        r = Vector4F_Round(r);
        // Convert full revolutions back to degrees and subtract from original
        return Vector4F_NegateMultiplyAdd(r, vFromRevolutions, v);
#elif ANEMONE_FEATURE_NEON
        float32x4_t const vToRevolutions = vdupq_n_f32(Factor_DegreesToRevolutions<float>);
        float32x4_t const vFromRevolutions = vdupq_n_f32(Factor_RevolutionsToDegrees<float>);

        // Convert degrees to full revolutions
        float32x4_t r = vmulq_f32(v, vToRevolutions);
        r = Vector4F_Round(r);
        // Convert full revolutions back to degrees and subtract from original
        return Vector4F_NegateMultiplyAdd(r, vFromRevolutions, v);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_AngleDifferenceRadians(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            AngleDifferenceRadians<float>(a.Inner[0], b.Inner[0]),
            AngleDifferenceRadians<float>(a.Inner[1], b.Inner[1]),
            AngleDifferenceRadians<float>(a.Inner[2], b.Inner[2]),
            AngleDifferenceRadians<float>(a.Inner[3], b.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return Vector4F_UnwindRadians(_mm_sub_ps(a, b));
#elif ANEMONE_FEATURE_NEON
        return Vector4F_UnwindRadians(vsubq_f32(a, b));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_AngleDifferenceDegrees(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            AngleDifferenceDegrees<float>(a.Inner[0], b.Inner[0]),
            AngleDifferenceDegrees<float>(a.Inner[1], b.Inner[1]),
            AngleDifferenceDegrees<float>(a.Inner[2], b.Inner[2]),
            AngleDifferenceDegrees<float>(a.Inner[3], b.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return Vector4F_UnwindDegrees(_mm_sub_ps(a, b));
#elif ANEMONE_FEATURE_NEON
        return Vector4F_UnwindDegrees(vsubq_f32(a, b));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_RevolutionsToDegrees(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            RevolutionsToDegrees<float>(v.Inner[0]),
            RevolutionsToDegrees<float>(v.Inner[1]),
            RevolutionsToDegrees<float>(v.Inner[2]),
            RevolutionsToDegrees<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_mul_ps(v, _mm_set1_ps(Factor_RevolutionsToDegrees<float>));
#elif ANEMONE_FEATURE_NEON
        return vmulq_f32(v, vdupq_n_f32(Factor_RevolutionsToDegrees<float>));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_RevolutionsToRadians(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            RevolutionsToRadians<float>(v.Inner[0]),
            RevolutionsToRadians<float>(v.Inner[1]),
            RevolutionsToRadians<float>(v.Inner[2]),
            RevolutionsToRadians<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_mul_ps(v, _mm_set1_ps(Factor_RevolutionsToRadians<float>));
#elif ANEMONE_FEATURE_NEON
        return vmulq_f32(v, vdupq_n_f32(Factor_RevolutionsToRadians<float>));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_DegreesToRevolutions(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            DegreesToRevolutions<float>(v.Inner[0]),
            DegreesToRevolutions<float>(v.Inner[1]),
            DegreesToRevolutions<float>(v.Inner[2]),
            DegreesToRevolutions<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_mul_ps(v, _mm_set1_ps(Factor_DegreesToRevolutions<float>));
#elif ANEMONE_FEATURE_NEON
        return vmulq_f32(v, vdupq_n_f32(Factor_DegreesToRevolutions<float>));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_RadiansToRevolutions(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            RadiansToRevolutions<float>(v.Inner[0]),
            RadiansToRevolutions<float>(v.Inner[1]),
            RadiansToRevolutions<float>(v.Inner[2]),
            RadiansToRevolutions<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_mul_ps(v, _mm_set1_ps(Factor_RadiansToRevolutions<float>));
#elif ANEMONE_FEATURE_NEON
        return vmulq_f32(v, vdupq_n_f32(Factor_RadiansToRevolutions<float>));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_RadiansToDegrees(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            RadiansToDegrees<float>(v.Inner[0]),
            RadiansToDegrees<float>(v.Inner[1]),
            RadiansToDegrees<float>(v.Inner[2]),
            RadiansToDegrees<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_mul_ps(v, _mm_set1_ps(Factor_RadiansToDegrees<float>));
#elif ANEMONE_FEATURE_NEON
        return vmulq_f32(v, vdupq_n_f32(Factor_RadiansToDegrees<float>));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_DegreesToRadians(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            DegreesToRadians<float>(v.Inner[0]),
            DegreesToRadians<float>(v.Inner[1]),
            DegreesToRadians<float>(v.Inner[2]),
            DegreesToRadians<float>(v.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_mul_ps(v, _mm_set1_ps(Factor_DegreesToRadians<float>));
#elif ANEMONE_FEATURE_NEON
        return vmulq_f32(v, vdupq_n_f32(Factor_DegreesToRadians<float>));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Dot4(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const r = (a.Inner[0] * b.Inner[0]) + (a.Inner[1] * b.Inner[1]) + (a.Inner[2] * b.Inner[2]) + (a.Inner[3] * b.Inner[3]);
        return SimdVector4F{r, r, r, r};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_dp_ps(a, b, 0b1111'1111);
#elif ANEMONE_FEATURE_NEON
        float32x2_t const r = NeonDot4(a, b);
        return vcombine_f32(r, r);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Dot3(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const r = (a.Inner[0] * b.Inner[0]) + (a.Inner[1] * b.Inner[1]) + (a.Inner[2] * b.Inner[2]);
        return SimdVector4F{r, r, r, r};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_dp_ps(a, b, 0b0111'1111);
#elif ANEMONE_FEATURE_NEON
        float32x2_t const r = NeonDot3(a, b);
        return vcombine_f32(r, r);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Dot2(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const r = (a.Inner[0] * b.Inner[0]) + (a.Inner[1] * b.Inner[1]);
        return SimdVector4F{r, r, r, r};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_dp_ps(a, b, 0b0011'1111);
#elif ANEMONE_FEATURE_NEON
        float32x2_t const r = NeonDot2(vget_low_f32(a), vget_low_f32(b));
        return vcombine_f32(r, r);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Cross4(SimdVector4F a, SimdVector4F b, SimdVector4F c)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const ax = a.Inner[0];
        float const ay = a.Inner[1];
        float const az = a.Inner[2];
        float const aw = a.Inner[3];

        float const bx = b.Inner[0];
        float const by = b.Inner[1];
        float const bz = b.Inner[2];
        float const bw = b.Inner[3];

        float const cx = c.Inner[0];
        float const cy = c.Inner[1];
        float const cz = c.Inner[2];
        float const cw = c.Inner[3];

        float const dx = (((bz * cw) - (bw * cz)) * ay) - (((by * cw) - (bw * cy)) * az) + (((by * cz) - (bz * cy)) * aw);
        float const dy = (((bw * cz) - (bz * cw)) * ax) - (((bw * cx) - (bx * cw)) * az) + (((bz * cx) - (bx * cz)) * aw);
        float const dz = (((by * cw) - (bw * cy)) * ax) - (((bx * cw) - (bw * cx)) * ay) + (((bx * cy) - (by * cx)) * aw);
        float const dw = (((bz * cy) - (by * cz)) * ax) - (((bz * cx) - (bx * cz)) * ay) + (((by * cx) - (bx * cy)) * az);

        return SimdVector4F{
            dx,
            dy,
            dz,
            dw,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // r0 = ((b.zwyz * c.wzwy) - (b.wzwy * c.zwyz)) * a.yxxx
        __m128 const b_zwyz = _mm_permute_ps(b, _MM_SHUFFLE(2, 1, 3, 2));
        __m128 const c_wzwy = _mm_permute_ps(c, _MM_SHUFFLE(1, 3, 2, 3));
        __m128 const m0_bc_0 = _mm_mul_ps(b_zwyz, c_wzwy);

        __m128 const b_wzwy = _mm_permute_ps(b, _MM_SHUFFLE(1, 3, 2, 3));
        __m128 const c_zwyz = _mm_permute_ps(c_wzwy, _MM_SHUFFLE(1, 3, 0, 1));
        __m128 const m0_bc_1 = _mm_mul_ps(b_wzwy, c_zwyz);

        __m128 const s0 = _mm_sub_ps(m0_bc_0, m0_bc_1);
        __m128 const a_yxxx = _mm_permute_ps(a, _MM_SHUFFLE(0, 0, 0, 1));

        __m128 const r0 = _mm_mul_ps(s0, a_yxxx);

        // r1 = ((b.ywxz * c.wxwx) - (b.wxwx * c.ywxz)) * a.zzyy
        __m128 const b_ywxz = _mm_permute_ps(b, _MM_SHUFFLE(2, 0, 3, 1));
        __m128 const c_wxwx = _mm_permute_ps(c, _MM_SHUFFLE(0, 3, 0, 3));
        __m128 const m1_bc_0 = _mm_mul_ps(b_ywxz, c_wxwx);

        __m128 const b_wxwx = _mm_permute_ps(b_ywxz, _MM_SHUFFLE(2, 1, 2, 1));
        __m128 const c_ywxz = _mm_permute_ps(c, _MM_SHUFFLE(2, 0, 3, 1));
        __m128 const m1_bc_1 = _mm_mul_ps(b_wxwx, c_ywxz);

        __m128 const s1 = _mm_sub_ps(m1_bc_0, m1_bc_1);
        __m128 const a_zzyy = _mm_permute_ps(a, _MM_SHUFFLE(1, 1, 2, 2));
        __m128 const r1 = _mm_mul_ps(s1, a_zzyy);

        // t0 = r0 - r1
        __m128 const t0 = _mm_sub_ps(r0, r1);

        // r2 = ((b.yzxy * c.zxyx) - (b.zxyx * c.yzxy)) * a.wwwz
        __m128 const b_yzxy = _mm_permute_ps(b, _MM_SHUFFLE(1, 0, 2, 1));
        __m128 const c_zxyx = _mm_permute_ps(c, _MM_SHUFFLE(0, 1, 0, 2));
        __m128 const m2_bc_0 = _mm_mul_ps(b_yzxy, c_zxyx);

        __m128 const b_zxyx = _mm_permute_ps(b_yzxy, _MM_SHUFFLE(2, 0, 2, 1));
        __m128 const c_yzxy = _mm_permute_ps(c, _MM_SHUFFLE(1, 0, 2, 1));
        __m128 const m2_bc_1 = _mm_mul_ps(b_zxyx, c_yzxy);

        __m128 const s2 = _mm_sub_ps(m2_bc_0, m2_bc_1);
        __m128 const a_wwwz = _mm_permute_ps(a, _MM_SHUFFLE(2, 3, 3, 3));
        __m128 const r2 = _mm_mul_ps(s2, a_wwwz);

        return _mm_add_ps(t0, r2);
#elif ANEMONE_FEATURE_NEON
        // Recombine vectors
        float32x2_t const a_xy = vget_low_f32(a);
        float32x2_t const a_zw = vget_high_f32(a);
        float32x2_t const a_yx = vrev64_f32(a_xy);
        float32x2_t const a_wz = vrev64_f32(a_zw);

        float32x2_t const b_xy = vget_low_f32(b);
        float32x2_t const b_zw = vget_high_f32(b);
        float32x2_t const b_yx = vrev64_f32(b_xy);
        float32x2_t const b_wz = vrev64_f32(b_zw);

        float32x2_t const c_xy = vget_low_f32(c);
        float32x2_t const c_zw = vget_high_f32(c);
        float32x2_t const c_yx = vrev64_f32(c_xy);
        float32x2_t const c_wz = vrev64_f32(c_zw);

        float32x2_t const a_xx = vdup_lane_f32(a_xy, 0);
        float32x2_t const a_yy = vdup_lane_f32(a_xy, 1);
        float32x2_t const a_zz = vdup_lane_f32(a_zw, 0);
        float32x2_t const a_ww = vdup_lane_f32(a_zw, 1);

        float32x2_t const b_yz = vext_f32(b_xy, b_zw, 1);
        float32x2_t const b_yw = vext_f32(b_xy, b_wz, 1);
        float32x2_t const b_xz = vext_f32(b_yx, b_zw, 1);
        float32x2_t const b_wy = vext_f32(b_zw, b_yx, 1);
        float32x2_t const b_wx = vext_f32(b_zw, b_xy, 1);
        float32x2_t const b_zx = vext_f32(b_wz, b_xy, 1);

        float32x2_t const c_yz = vext_f32(c_xy, c_zw, 1);
        float32x2_t const c_xz = vext_f32(c_yx, c_zw, 1);
        float32x2_t const c_wy = vext_f32(c_zw, c_yx, 1);
        float32x2_t const c_wx = vext_f32(c_zw, c_xy, 1);
        float32x2_t const c_yw = vext_f32(c_xy, c_wz, 1);
        float32x2_t const c_zx = vext_f32(c_wz, c_xy, 1);

        // t0 = (((b.zwyz * c.wzwy) - (b.wzwy * c.zwyz)) * a.yxxx)
        float32x4_t const b_zwyz = vcombine_f32(b_zw, b_yz);
        float32x4_t const c_wzwy = vcombine_f32(c_wz, c_wy);
        float32x4_t const b_wzwy = vcombine_f32(b_wz, b_wy);
        float32x4_t const c_zwyz = vcombine_f32(c_zw, c_yz);
        float32x4_t const a_yxxx = vcombine_f32(a_yx, a_xx);
        float32x4_t const t0 = vmulq_f32(vmlsq_f32(vmulq_f32(b_zwyz, c_wzwy), b_wzwy, c_zwyz), a_yxxx);

        // t1 = (((b.ywxz * c.wxwx) - (b.wxwx * c.ywxz)) * a.zzyy)
        float32x4_t const b_ywxz = vcombine_f32(b_yw, b_xz);
        float32x4_t const c_wxwx = vcombine_f32(c_wx, c_wx);
        float32x4_t const b_wxwx = vcombine_f32(b_wx, b_wx);
        float32x4_t const c_ywxz = vcombine_f32(c_yw, c_xz);
        float32x4_t const a_zzyy = vcombine_f32(a_zz, a_yy);
        float32x4_t const t1 = vmulq_f32(vmlsq_f32(vmulq_f32(b_ywxz, c_wxwx), b_wxwx, c_ywxz), a_zzyy);

        // t2 = (((b.yzxy * c.zxyx) - (b.zxyx * c.yzxy)) * a.wwwz)
        float32x4_t const b_yzxy = vcombine_f32(b_yz, b_xy);
        float32x4_t const c_zxyx = vcombine_f32(c_zx, c_yx);
        float32x4_t const b_zxyx = vcombine_f32(b_zx, b_yx);
        float32x4_t const c_yzxy = vcombine_f32(c_yz, c_xy);
        float32x4_t const a_wwwz = vcombine_f32(a_ww, a_wz);
        float32x4_t const t2 = vmulq_f32(vmlsq_f32(vmulq_f32(b_yzxy, c_zxyx), b_zxyx, c_yzxy), a_wwwz);

        // = t0 - t1 + t2
        float32x4_t const r = vaddq_f32(vsubq_f32(t0, t1), t2);
        return r;
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Cross3(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const rx = (a.Inner[1] * b.Inner[2]) - (a.Inner[2] * b.Inner[1]);
        float const ry = (a.Inner[2] * b.Inner[0]) - (a.Inner[0] * b.Inner[2]);
        float const rz = (a.Inner[0] * b.Inner[1]) - (a.Inner[1] * b.Inner[0]);

        return SimdVector4F{
            rx,
            ry,
            rz,
            0.0f,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // [z, x, y] = [
        //    (a.x * b.y) - (a.y * b.x),
        //    (a.y * b.z) - (a.z * b.y),
        //    (a.z * b.x) - (a.x * b.z),
        // ].yzx
        __m128 const a_yzx = _mm_permute_ps(a, _MM_SHUFFLE(3, 0, 2, 1));
        __m128 const b_yzx = _mm_permute_ps(b, _MM_SHUFFLE(3, 0, 2, 1));
        __m128 const d_zxy = Vector4F_NegateMultiplyAdd(a_yzx, b, _mm_mul_ps(a, b_yzx));
        __m128 const d_xyz = _mm_permute_ps(d_zxy, _MM_SHUFFLE(3, 0, 2, 1));
        __m128 const mask1110 = _mm_load_ps(F32x4_SelectMask_XXXn.As<float>());

        return _mm_and_ps(d_xyz, mask1110);
#elif ANEMONE_FEATURE_NEON
        float32x2_t const a_xy = vget_low_f32(a);
        float32x2_t const a_zz = vdup_lane_f32(vget_high_f32(a), 0);
        float32x2_t const b_xy = vget_low_f32(b);
        float32x2_t const b_zz = vdup_lane_f32(vget_high_f32(b), 0);
        float32x2_t const a_yx = vrev64_f32(a_xy);
        float32x2_t const b_yx = vrev64_f32(b_xy);

        // = [y, x, x, y]
        float32x4_t const a_yxxy = vcombine_f32(a_yx, a_xy);
        float32x4_t const b_yxxy = vcombine_f32(b_yx, b_xy);

        // = [z, z, y, x]
        float32x4_t const a_zzyx = vcombine_f32(a_zz, a_yx);
        float32x4_t const b_zzyx = vcombine_f32(b_zz, b_yx);

        // = [
        //  a.y * b.z,
        //  a.x * b.z,
        //  a.x * b.y,
        //  a.y * b.x,
        // ]
        float32x4_t const m0 = vmulq_f32(a_yxxy, b_zzyx);

        // = [
        //  (a.y * b.z) - (a.z * b.y),
        //  (a.x * b.z) - (a.z * b.x),
        //  (a.x * b.y) - (a.y * b.x),
        //  (a.y * b.x) - (a.x * b.y),
        // ]
        float32x4_t const m1 = vmlsq_f32(m0, a_zzyx, b_yxxy);

        // flip: m2.y = m1.y * -1
        // m1: (a.x * b.z) - (a.z * b.x)
        // m2: -(a.x * b.z) + (a.z * b.x) = (a.z * b.x) - (a.x * b.z)
        uint32x4_t const sign0100 = vld1q_u32(F32x4_SignMask_nXnn.As<uint32_t>());
        uint32x4_t const m2 = veorq_u32(vreinterpretq_u32_f32(m1), sign0100);
        uint32x4_t const mask1110 = vld1q_u32(F32x4_SelectMask_XXXn.As<uint32_t>());
        uint32x4_t const m3 = vandq_u32(m2, mask1110);

        return vreinterpretq_f32_u32(m3);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Cross2(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const r = (a.Inner[0] * b.Inner[1]) - (a.Inner[1] * b.Inner[0]);

        return SimdVector4F{
            r,
            r,
            0.0f,
            0.0f,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // = [b.y, b.x]
        __m128 const r0 = _mm_permute_ps(b, _MM_SHUFFLE(0, 1, 0, 1));
        // = [a.x * b.y, a.y * b.x]
        __m128 const r1 = _mm_mul_ps(a, r0);
        // = [a.y * b.x, ...]
        __m128 const r2 = _mm_permute_ps(r1, _MM_SHUFFLE(1, 1, 1, 1));
        // = [a.x * b.y - a.y * b.x, ...]
        __m128 const r3 = _mm_sub_ps(r1, r2);
        // = r1.xxxx
        return _mm_permute_ps(r3, _MM_SHUFFLE(0, 0, 0, 0));
#elif ANEMONE_FEATURE_NEON
        static constexpr float fnegate_y[2]{1.0f, -1.0f};
        float32x2_t const negate_y = vld1_f32(fnegate_y);

        float32x2_t const a_xy = vget_low_f32(a);
        float32x2_t const b_xy = vget_low_f32(b);
        float32x2_t const b_yx = vrev64_f32(b_xy);
        // = [(a.x * b.y), (a.y * b.x)]
        float32x2_t const r0 = vmul_f32(a_xy, b_yx);
        // = [(a.x * b.y), -(a.y * b.x)]
        float32x2_t const r1 = vmul_f32(r0, negate_y);
        // = [(a.x * b.y) - (a.y * b.x), ...]
        float32x2_t const r2 = vpadd_f32(r1, r1);
        return vcombine_f32(r2, r2);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Wedge3(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            a.Inner[0] * b.Inner[1] - a.Inner[1] * b.Inner[0], // XY
            a.Inner[0] * b.Inner[2] - a.Inner[2] * b.Inner[0], // XZ
            a.Inner[1] * b.Inner[2] - a.Inner[2] * b.Inner[1], // YZ
            0.0f,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // = [x,x,y,y]
        __m128 const a_xxyy = _mm_permute_ps(a, _MM_SHUFFLE(1, 1, 0, 0));
        __m128 const b_xxyy = _mm_permute_ps(b, _MM_SHUFFLE(1, 1, 0, 0));
        // = [y,z,z,z]
        __m128 const a_yzzz = _mm_permute_ps(a, _MM_SHUFFLE(2, 2, 2, 1));
        __m128 const b_yzzz = _mm_permute_ps(b, _MM_SHUFFLE(2, 2, 2, 1));

        return Vector4F_MultiplySubtract(a_xxyy, b_yzzz, _mm_mul_ps(a_yzzz, b_xxyy));
#elif ANEMONE_FEATURE_NEON
        // = [x,x,y,y]
        float32x2_t const a_xx = vdup_laneq_f32(a, 0);
        float32x2_t const a_yy = vdup_laneq_f32(a, 1);
        float32x2_t const b_xx = vdup_laneq_f32(b, 0);
        float32x2_t const b_yy = vdup_laneq_f32(b, 1);

        float32x4_t const a_xxyy = vcombine_f32(a_xx, a_yy);
        float32x4_t const b_xxyy = vcombine_f32(b_xx, b_yy);

        // = [z, z, z, z]
        float32x4_t const a_zzzz = vdupq_laneq_f32(a, 3);
        float32x4_t const a_yzzz = vcopyq_lane_f32(a_zzzz, 0, a_yy, 0);

        float32x4_t const b_zzzz = vdupq_laneq_f32(b, 3);
        float32x4_t const b_yzzz = vcopyq_lane_f32(b_zzzz, 0, b_yy, 0);

        return vmlsq_f32(vmulq_f32(a_xxyy, b_yzzz), a_yzzz, b_xxyy);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LengthSquared4(SimdVector4F v)
    {
        return Vector4F_Dot4(v, v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LengthSquared3(SimdVector4F v)
    {
        return Vector4F_Dot3(v, v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LengthSquared2(SimdVector4F v)
    {
        return Vector4F_Dot2(v, v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Length4(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const lengthSquared = (v.Inner[0] * v.Inner[0]) + (v.Inner[1] * v.Inner[1]) + (v.Inner[2] * v.Inner[2]) + (v.Inner[3] * v.Inner[3]);
        float const length = Sqrt<float>(lengthSquared);

        return SimdVector4F{
            length,
            length,
            length,
            length,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const dot = Vector4F_Dot4(v, v);
        return _mm_sqrt_ps(dot);
#elif ANEMONE_FEATURE_NEON
        float32x2_t const r0 = NeonDot4(v);
        float32x2_t const r1 = NeonSquareRoot2xNewtonRhapson(r0);
        return vcombine_f32(r1, r1);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Length3(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const lengthSquared = (v.Inner[0] * v.Inner[0]) + (v.Inner[1] * v.Inner[1]) + (v.Inner[2] * v.Inner[2]);
        float const length = Sqrt<float>(lengthSquared);

        return SimdVector4F{
            length,
            length,
            length,
            length,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const dot = Vector4F_Dot3(v, v);
        return _mm_sqrt_ps(dot);
#elif ANEMONE_FEATURE_NEON
        float32x2_t const r0 = NeonDot3(v);
        float32x2_t const r1 = NeonSquareRoot2xNewtonRhapson(r0);
        return vcombine_f32(r1, r1);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Length2(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const lengthSquared = (v.Inner[0] * v.Inner[0]) + (v.Inner[1] * v.Inner[1]);
        float const length = Sqrt<float>(lengthSquared);

        return SimdVector4F{
            length,
            length,
            length,
            length,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const dot = Vector4F_Dot2(v, v);
        return _mm_sqrt_ps(dot);
#elif ANEMONE_FEATURE_NEON
        float32x2_t const r0 = NeonDot2(vget_low_f32(v));
        float32x2_t const r1 = NeonSquareRoot2xNewtonRhapson(r0);
        return vcombine_f32(r1, r1);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_ReciprocalLength4(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const lengthSquared = (v.Inner[0] * v.Inner[0]) + (v.Inner[1] * v.Inner[1]) + (v.Inner[2] * v.Inner[2]) + (v.Inner[3] * v.Inner[3]);
        float const result = ReciprocalSqrt<float>(lengthSquared);

        return SimdVector4F{
            result,
            result,
            result,
            result,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const lengthSquared = Vector4F_LengthSquared4(v);
        return Vector4F_ReciprocalSquareRoot(lengthSquared);
#elif ANEMONE_FEATURE_NEON
        float32x2_t const r0 = NeonDot4(v);
        float32x2_t const r1 = NeonReciprocalSquareRoot2xNewtonRhapson(r0);
        return vcombine_f32(r1, r1);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_ReciprocalLength3(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const lengthSquared = (v.Inner[0] * v.Inner[0]) + (v.Inner[1] * v.Inner[1]) + (v.Inner[2] * v.Inner[2]);
        float const result = ReciprocalSqrt<float>(lengthSquared);

        return SimdVector4F{
            result,
            result,
            result,
            result,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const lengthSquared = Vector4F_LengthSquared3(v);
        return Vector4F_ReciprocalSquareRoot(lengthSquared);
#elif ANEMONE_FEATURE_NEON
        float32x2_t const r0 = NeonDot3(v);
        float32x2_t const r1 = NeonReciprocalSquareRoot2xNewtonRhapson(r0);
        return vcombine_f32(r1, r1);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_ReciprocalLength2(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const lengthSquared = (v.Inner[0] * v.Inner[0]) + (v.Inner[1] * v.Inner[1]);
        float const result = ReciprocalSqrt<float>(lengthSquared);

        return SimdVector4F{
            result,
            result,
            result,
            result,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const lengthSquared = Vector4F_LengthSquared2(v);
        return Vector4F_ReciprocalSquareRoot(lengthSquared);
#elif ANEMONE_FEATURE_NEON
        float32x2_t const r0 = NeonDot2(vget_low_f32(v));
        float32x2_t const r1 = NeonReciprocalSquareRoot2xNewtonRhapson(r0);
        return vcombine_f32(r1, r1);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_ReciprocalLengthEst4(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const lengthSquared = (v.Inner[0] * v.Inner[0]) + (v.Inner[1] * v.Inner[1]) + (v.Inner[2] * v.Inner[2]) + (v.Inner[3] * v.Inner[3]);
        float const result = ReciprocalSqrtEst<float>(lengthSquared);

        return SimdVector4F{
            result,
            result,
            result,
            result,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const lengthSquared = Vector4F_LengthSquared4(v);
        return Vector4F_ReciprocalSquareRootEst(lengthSquared);
#elif ANEMONE_FEATURE_NEON
        float32x2_t const r0 = NeonDot4(v);
        float32x2_t const r1 = vrsqrte_f32(r0);
        return vcombine_f32(r1, r1);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_ReciprocalLengthEst3(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const lengthSquared = (v.Inner[0] * v.Inner[0]) + (v.Inner[1] * v.Inner[1]) + (v.Inner[2] * v.Inner[2]);
        float const result = ReciprocalSqrtEst<float>(lengthSquared);

        return SimdVector4F{
            result,
            result,
            result,
            result,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const lengthSquared = Vector4F_LengthSquared3(v);
        return Vector4F_ReciprocalSquareRootEst(lengthSquared);
#elif ANEMONE_FEATURE_NEON
        float32x2_t const r0 = NeonDot3(v);
        float32x2_t const r1 = vrsqrte_f32(r0);
        return vcombine_f32(r1, r1);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_ReciprocalLengthEst2(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const lengthSquared = (v.Inner[0] * v.Inner[0]) + (v.Inner[1] * v.Inner[1]);
        float const result = ReciprocalSqrtEst<float>(lengthSquared);

        return SimdVector4F{
            result,
            result,
            result,
            result,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const lengthSquared = Vector4F_LengthSquared2(v);
        return Vector4F_ReciprocalSquareRootEst(lengthSquared);
#elif ANEMONE_FEATURE_NEON
        float32x2_t const r0 = NeonDot2(vget_low_f32(v));
        float32x2_t const r1 = vrsqrte_f32(r0);
        return vcombine_f32(r1, r1);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Normalize4(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const lengthSquared = (v.Inner[0] * v.Inner[0]) + (v.Inner[1] * v.Inner[1]) + (v.Inner[2] * v.Inner[2]) + (v.Inner[3] * v.Inner[3]);
        float length = Sqrt<float>(lengthSquared);

        if (length > 0.0f)
        {
            length = 1.0f / length;
        }

        return SimdVector4F{
            v.Inner[0] * length,
            v.Inner[1] * length,
            v.Inner[2] * length,
            v.Inner[3] * length,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const length_squared = Vector4F_Dot4(v, v);
        __m128 const length = _mm_sqrt_ps(length_squared);

        // where: [e != 0.0]
        __m128 const zero = _mm_setzero_ps();
        __m128 const mask_zero = _mm_cmpneq_ps(zero, length);

        // where: [e != inf]
        __m128 const infinity = _mm_load_ps(F32x4_PositiveInfinity_XXXX.As<float>());
        __m128 const mask_infinity = _mm_cmpneq_ps(length_squared, infinity);

        __m128 const normalized = _mm_and_ps(_mm_div_ps(v, length), mask_zero);

        __m128 const nan = _mm_load_ps(F32x4_PositiveQNaN_XXXX.As<float>());
        __m128 const select_nan = _mm_andnot_ps(mask_infinity, nan);
        __m128 const select_inf = _mm_and_ps(normalized, mask_infinity);

        return _mm_or_ps(select_nan, select_inf);
#elif ANEMONE_FEATURE_NEON
        // = length
        float32x2_t const length_squared = NeonDot4(v);
        // = 1 / sqrt(length)
        float32x2_t const length_rcp = NeonReciprocalSquareRoot2xNewtonRhapson(length_squared);

        uint32x2_t const mask_zero = vceq_f32(length_squared, vdup_n_f32(0.0f));
        uint32x2_t const mask_inf = vceq_f32(length_squared, vld1_f32(F32x4_PositiveInfinity_XXXX.As<float>()));

        // = v * length_rcp
        float32x4_t const r0 = vmulq_f32(v, vcombine_f32(length_rcp, length_rcp));
        float32x4_t const r1 = vbslq_f32(vcombine_u32(mask_zero, mask_zero), vdupq_n_f32(0.0f), r0);
        float32x4_t const r2 = vbslq_f32(vcombine_u32(mask_inf, mask_inf), vld1q_f32(F32x4_PositiveQNaN_XXXX.As<float>()), r1);
        return r2;
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Normalize3(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const lengthSquared = (v.Inner[0] * v.Inner[0]) + (v.Inner[1] * v.Inner[1]) + (v.Inner[2] * v.Inner[2]);
        float length = Sqrt<float>(lengthSquared);

        if (length > 0.0f)
        {
            length = 1.0f / length;
        }

        return SimdVector4F{
            v.Inner[0] * length,
            v.Inner[1] * length,
            v.Inner[2] * length,
            v.Inner[3] * length,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const length_squared = Vector4F_Dot3(v, v);
        __m128 const length = _mm_sqrt_ps(length_squared);

        // where: [e != 0.0]
        __m128 const zero = _mm_setzero_ps();
        __m128 const mask_zero = _mm_cmpneq_ps(zero, length);

        // where: [e != inf]
        __m128 const infinity = _mm_load_ps(F32x4_PositiveInfinity_XXXX.As<float>());
        __m128 const mask_infinity = _mm_cmpneq_ps(length_squared, infinity);

        __m128 const normalized = _mm_and_ps(_mm_div_ps(v, length), mask_zero);

        __m128 const nan = _mm_load_ps(F32x4_PositiveQNaN_XXXX.As<float>());
        __m128 const select_nan = _mm_andnot_ps(mask_infinity, nan);
        __m128 const select_inf = _mm_and_ps(normalized, mask_infinity);

        return _mm_or_ps(select_nan, select_inf);
#elif ANEMONE_FEATURE_NEON
        // = length
        float32x2_t const length_squared = NeonDot3(v);
        // = 1 / sqrt(length)
        float32x2_t const length_rcp = NeonReciprocalSquareRoot2xNewtonRhapson(length_squared);

        uint32x2_t const mask_zero = vceq_f32(length_squared, vdup_n_f32(0.0f));
        uint32x2_t const mask_inf = vceq_f32(length_squared, vld1_f32(F32x4_PositiveInfinity_XXXX.As<float>()));

        // = v * length_rcp
        float32x4_t const r0 = vmulq_f32(v, vcombine_f32(length_rcp, length_rcp));
        float32x4_t const r1 = vbslq_f32(vcombine_u32(mask_zero, mask_zero), vdupq_n_f32(0.0f), r0);
        float32x4_t const r2 = vbslq_f32(vcombine_u32(mask_inf, mask_inf), vld1q_f32(F32x4_PositiveQNaN_XXXX.As<float>()), r1);
        return r2;
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Normalize2(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const lengthSquared = (v.Inner[0] * v.Inner[0]) + (v.Inner[1] * v.Inner[1]);
        float length = Sqrt<float>(lengthSquared);

        if (length > 0.0f)
        {
            length = 1.0f / length;
        }

        return SimdVector4F{
            v.Inner[0] * length,
            v.Inner[1] * length,
            v.Inner[2] * length,
            v.Inner[3] * length,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const length_squared = Vector4F_Dot2(v, v);
        __m128 const length = _mm_sqrt_ps(length_squared);

        // where: [e != 0.0]
        __m128 const zero = _mm_setzero_ps();
        __m128 const mask_zero = _mm_cmpneq_ps(zero, length);

        // where: [e != inf]
        __m128 const infinity = _mm_load_ps(F32x4_PositiveInfinity_XXXX.As<float>());
        __m128 const mask_infinity = _mm_cmpneq_ps(length_squared, infinity);

        __m128 const normalized = _mm_and_ps(_mm_div_ps(v, length), mask_zero);

        __m128 const nan = _mm_load_ps(F32x4_PositiveQNaN_XXXX.As<float>());
        __m128 const select_nan = _mm_andnot_ps(mask_infinity, nan);
        __m128 const select_inf = _mm_and_ps(normalized, mask_infinity);

        return _mm_or_ps(select_nan, select_inf);
#elif ANEMONE_FEATURE_NEON
        float32x2_t const v_xy = vget_low_f32(v);
        // = length^2
        float32x2_t const length_squared = NeonDot2(v_xy);
        // = 1 / sqrt(length^2)
        float32x2_t const length_rcp = NeonReciprocalSquareRoot2xNewtonRhapson(length_squared);

        uint32x2_t const mask_zero = vceq_f32(length_squared, vdup_n_f32(0.0f));
        uint32x2_t const mask_inf = vceq_f32(length_squared, vld1_f32(F32x4_PositiveInfinity_XXXX.As<float>()));

        // = v * length_rcp
        float32x2_t const r0 = vmul_f32(v_xy, length_rcp);
        float32x2_t const r1 = vbsl_f32(mask_zero, vdup_n_f32(0.0f), r0);
        float32x2_t const r2 = vbsl_f32(mask_inf, vld1_f32(F32x4_PositiveQNaN_XXXX.As<float>()), r1);

        return vcombine_f32(r2, r2);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Reflect4(SimdVector4F incident, SimdVector4F normal)
    {
        // = 2 * dot4(incident, normal)
        SimdVector4F const d = Vector4F_Dot4(incident, normal);
        SimdVector4F const r = Vector4F_Add(d, d);

        // = incident - (r * normal)
        return Vector4F_NegateMultiplyAdd(r, normal, incident);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Reflect3(SimdVector4F incident, SimdVector4F normal)
    {
        // = 2 * dot3(incident, normal)
        SimdVector4F const d = Vector4F_Dot3(incident, normal);
        SimdVector4F const r = Vector4F_Add(d, d);

        // = incident - (r * normal)
        return Vector4F_NegateMultiplyAdd(r, normal, incident);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Reflect2(SimdVector4F incident, SimdVector4F normal)
    {
        // = 2 * dot2(incident, normal)
        SimdVector4F const d = Vector4F_Dot2(incident, normal);
        SimdVector4F const r = Vector4F_Add(d, d);

        // = incident - (r * normal)
        return Vector4F_NegateMultiplyAdd(r, normal, incident);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Refract4(SimdVector4F incident, SimdVector4F normal, SimdVector4F index)
    {
        //
        // k = 1.0 - index^2 * (1.0 - dot(I, N)^2);
        // if (k < 0.0)
        //     R = T(0.0);
        // else
        //     R = index * I - (index * dot(I, N) + sqrt(k)) * N;
        //

        // = dot(I, N)
        SimdVector4F const i_dot_n = Vector4F_Dot4(incident, normal);
        SimdVector4F const one = Vector4F_Replicate(1.0f);

        // = 1 - dot(I, N)^2
        SimdVector4F const t0 = Vector4F_NegateMultiplyAdd(i_dot_n, i_dot_n, one);

        // = index * (1 - dot(I, N)^2)
        SimdVector4F const t1 = Vector4F_Multiply(t0, index);

        // = 1 - (index^2 * (1 - dot(I, N)^2))
        SimdVector4F const k = Vector4F_NegateMultiplyAdd(t1, index, one);

        SimdVector4F const zero = Vector4F_Zero();

        if (Vector4F_IsLessEqual4(k, zero))
        {
            return zero;
        }

        // = sqrt(t3)
        SimdVector4F const r0 = Vector4F_SquareRoot(k);

        // = (index * dot(I, N)) + sqrt(k)
        SimdVector4F const r1 = Vector4F_MultiplyAdd(index, i_dot_n, r0);

        // = index * I
        SimdVector4F const r2 = Vector4F_Multiply(index, incident);

        // = (index * I) - (N * (index * dot(I, N)) + sqrt(k))
        return Vector4F_NegateMultiplyAdd(normal, r1, r2);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Refract3(SimdVector4F incident, SimdVector4F normal, SimdVector4F index)
    {
        //
        // k = 1.0 - index^2 * (1.0 - dot(I, N)^2);
        // if (k < 0.0)
        //     R = T(0.0);
        // else
        //     R = index * I - (index * dot(I, N) + sqrt(k)) * N;
        //

        // = dot(I, N)
        SimdVector4F const i_dot_n = Vector4F_Dot3(incident, normal);
        SimdVector4F const one = Vector4F_Replicate(1.0f);

        // = 1 - dot(I, N)^2
        SimdVector4F const t0 = Vector4F_NegateMultiplyAdd(i_dot_n, i_dot_n, one);

        // = index * (1 - dot(I, N)^2)
        SimdVector4F const t1 = Vector4F_Multiply(t0, index);

        // = 1 - (index^2 * (1 - dot(I, N)^2))
        SimdVector4F const k = Vector4F_NegateMultiplyAdd(t1, index, one);

        SimdVector4F const zero = Vector4F_Zero();

        if (Vector4F_IsLessEqual3(k, zero))
        {
            return zero;
        }

        // = sqrt(t3)
        SimdVector4F const r0 = Vector4F_SquareRoot(k);

        // = (index * dot(I, N)) + sqrt(k)
        SimdVector4F const r1 = Vector4F_MultiplyAdd(index, i_dot_n, r0);

        // = index * I
        SimdVector4F const r2 = Vector4F_Multiply(index, incident);

        // = (index * I) - (N * (index * dot(I, N)) + sqrt(k))
        return Vector4F_NegateMultiplyAdd(normal, r1, r2);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Refract2(SimdVector4F incident, SimdVector4F normal, SimdVector4F index)
    {
        //
        // k = 1.0 - index^2 * (1.0 - dot(I, N)^2);
        // if (k < 0.0)
        //     R = T(0.0);
        // else
        //     R = index * I - (index * dot(I, N) + sqrt(k)) * N;
        //

        // = dot(I, N)
        SimdVector4F const i_dot_n = Vector4F_Dot2(incident, normal);
        SimdVector4F const one = Vector4F_Replicate(1.0f);

        // = 1 - dot(I, N)^2
        SimdVector4F const t0 = Vector4F_NegateMultiplyAdd(i_dot_n, i_dot_n, one);

        // = index * (1 - dot(I, N)^2)
        SimdVector4F const t1 = Vector4F_Multiply(t0, index);

        // = 1 - (index^2 * (1 - dot(I, N)^2))
        SimdVector4F const k = Vector4F_NegateMultiplyAdd(t1, index, one);

        SimdVector4F const zero = Vector4F_Zero();

        if (Vector4F_IsLessEqual2(k, zero))
        {
            return zero;
        }

        // = sqrt(t3)
        SimdVector4F const r0 = Vector4F_SquareRoot(k);

        // = (index * dot(I, N)) + sqrt(k)
        SimdVector4F const r1 = Vector4F_MultiplyAdd(index, i_dot_n, r0);

        // = index * I
        SimdVector4F const r2 = Vector4F_Multiply(index, incident);

        // = (index * I) - (N * (index * dot(I, N)) + sqrt(k))
        return Vector4F_NegateMultiplyAdd(normal, r1, r2);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Refract4(SimdVector4F incident, SimdVector4F normal, float index)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return Vector4F_Refract4(incident, normal, SimdVector4F{index, index, index, index});
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return Vector4F_Refract4(incident, normal, _mm_set1_ps(index));
#elif ANEMONE_FEATURE_NEON
        return Vector4F_Refract4(incident, normal, vdupq_n_f32(index));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Refract3(SimdVector4F incident, SimdVector4F normal, float index)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return Vector4F_Refract3(incident, normal, SimdVector4F{index, index, index, index});
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return Vector4F_Refract3(incident, normal, _mm_set1_ps(index));
#elif ANEMONE_FEATURE_NEON
        return Vector4F_Refract3(incident, normal, vdupq_n_f32(index));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Refract2(SimdVector4F incident, SimdVector4F normal, float index)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return Vector4F_Refract2(incident, normal, SimdVector4F{index, index, index, index});
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return Vector4F_Refract2(incident, normal, _mm_set1_ps(index));
#elif ANEMONE_FEATURE_NEON
        return Vector4F_Refract2(incident, normal, vdupq_n_f32(index));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Transform4(SimdVector4F v, SimdMatrix4x4F m)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        SimdVector4F const vXXXX = Vector4F_Broadcast<0>(v);
        SimdVector4F const vYYYY = Vector4F_Broadcast<1>(v);
        SimdVector4F const vZZZZ = Vector4F_Broadcast<2>(v);
        SimdVector4F const vWWWW = Vector4F_Broadcast<3>(v);

        SimdVector4F r = Vector4F_Multiply(vWWWW, m.Inner[3]);
        r = Vector4F_MultiplyAdd(vZZZZ, m.Inner[2], r);
        r = Vector4F_MultiplyAdd(vYYYY, m.Inner[1], r);
        r = Vector4F_MultiplyAdd(vXXXX, m.Inner[0], r);
        return r;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const v_xxxx = _mm_permute_ps(v, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 const v_yyyy = _mm_permute_ps(v, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 const v_zzzz = _mm_permute_ps(v, _MM_SHUFFLE(2, 2, 2, 2));
        __m128 const v_wwww = _mm_permute_ps(v, _MM_SHUFFLE(3, 3, 3, 3));

        __m128 r = Vector4F_Multiply(v_wwww, m.Inner[3]);
        r = Vector4F_MultiplyAdd(v_zzzz, m.Inner[2], r);
        r = Vector4F_MultiplyAdd(v_yyyy, m.Inner[1], r);
        r = Vector4F_MultiplyAdd(v_xxxx, m.Inner[0], r);
        return r;
#elif ANEMONE_FEATURE_NEON
        float32x4_t r = vmulq_laneq_f32(m.val[0], v, 0);
        r = vmlaq_laneq_f32(r, m.val[1], v, 1);
        r = vmlaq_laneq_f32(r, m.val[2], v, 2);
        r = vmlaq_laneq_f32(r, m.val[3], v, 3);
        return r;
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Transform3(SimdVector4F v, SimdMatrix4x4F m)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        SimdVector4F const vXXXX = Vector4F_Broadcast<0>(v);
        SimdVector4F const vYYYY = Vector4F_Broadcast<1>(v);
        SimdVector4F const vZZZZ = Vector4F_Broadcast<2>(v);

        SimdVector4F r = Vector4F_MultiplyAdd(vZZZZ, m.Inner[2], m.Inner[3]);
        r = Vector4F_MultiplyAdd(vYYYY, m.Inner[1], r);
        r = Vector4F_MultiplyAdd(vXXXX, m.Inner[0], r);
        return r;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const v_xxxx = _mm_permute_ps(v, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 const v_yyyy = _mm_permute_ps(v, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 const v_zzzz = _mm_permute_ps(v, _MM_SHUFFLE(2, 2, 2, 2));

        __m128 r = Vector4F_MultiplyAdd(v_zzzz, m.Inner[2], m.Inner[3]);
        r = Vector4F_MultiplyAdd(v_yyyy, m.Inner[1], r);
        r = Vector4F_MultiplyAdd(v_xxxx, m.Inner[0], r);
        return r;
#elif ANEMONE_FEATURE_NEON
        float32x4_t r = vmlaq_laneq_f32(m.val[3], m.val[0], v, 0);
        r = vmlaq_laneq_f32(r, m.val[1], v, 1);
        r = vmlaq_laneq_f32(r, m.val[2], v, 2);
        return r;
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Transform2(SimdVector4F v, SimdMatrix4x4F m)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        SimdVector4F const vXXXX = Vector4F_Broadcast<0>(v);
        SimdVector4F const vYYYY = Vector4F_Broadcast<1>(v);

        SimdVector4F r = Vector4F_MultiplyAdd(vYYYY, m.Inner[1], m.Inner[3]);
        r = Vector4F_MultiplyAdd(vXXXX, m.Inner[0], r);
        return r;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const v_xxxx = _mm_permute_ps(v, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 const v_yyyy = _mm_permute_ps(v, _MM_SHUFFLE(1, 1, 1, 1));

        __m128 r = Vector4F_MultiplyAdd(v_yyyy, m.Inner[1], m.Inner[3]);
        r = Vector4F_MultiplyAdd(v_xxxx, m.Inner[0], r);
        return r;
#elif ANEMONE_FEATURE_NEON
        float32x4_t r = vmlaq_laneq_f32(m.val[3], m.val[1], v, 1);
        r = vmlaq_laneq_f32(r, m.val[0], v, 0);
        return r;
#endif
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareEqual(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            (a.Inner[0] == b.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[1] == b.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[2] == b.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[3] == b.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_cmpeq_ps(a, b);
#elif ANEMONE_FEATURE_NEON
        return vceqq_f32(a, b);
#endif
    }

    inline bool anemone_vectorcall Vector4F_IsEqual4(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return (a.Inner[0] == b.Inner[0]) && (a.Inner[1] == b.Inner[1]) && (a.Inner[2] == b.Inner[2]) && (a.Inner[3] == b.Inner[3]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_movemask_ps(_mm_cmpeq_ps(a, b)) == 0b1111;
#elif ANEMONE_FEATURE_NEON
        return NeonExtractMask4(Vector4F_CompareEqual(a, b)) == 0b1111;
#endif
    }

    inline bool anemone_vectorcall Vector4F_IsEqual3(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return (a.Inner[0] == b.Inner[0]) && (a.Inner[1] == b.Inner[1]) && (a.Inner[2] == b.Inner[2]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return (_mm_movemask_ps(_mm_cmpeq_ps(a, b)) & 0b0111) == 0b0111;
#elif ANEMONE_FEATURE_NEON
        return (NeonExtractMask4(Vector4F_CompareEqual(a, b)) & 0b0111) == 0b0111;
#endif
    }

    inline bool anemone_vectorcall Vector4F_IsEqual2(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return (a.Inner[0] == b.Inner[0]) && (a.Inner[1] == b.Inner[1]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return (_mm_movemask_ps(_mm_cmpeq_ps(a, b)) & 0b0011) == 0b0011;
#elif ANEMONE_FEATURE_NEON
        return (NeonExtractMask4(Vector4F_CompareEqual(a, b)) & 0b0011) == 0b0011;
#endif
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNotEqual(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            (a.Inner[0] != b.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[1] != b.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[2] != b.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[3] != b.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_cmpneq_ps(a, b);
#elif ANEMONE_FEATURE_NEON
        return vmvnq_u32(vceqq_f32(a, b));
#endif
    }

    inline bool anemone_vectorcall Vector4F_IsNotEqual4(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return (a.Inner[0] != b.Inner[0]) || (a.Inner[1] != b.Inner[1]) || (a.Inner[2] != b.Inner[2]) || (a.Inner[3] != b.Inner[3]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_movemask_ps(_mm_cmpneq_ps(a, b)) == 0b1111;
#elif ANEMONE_FEATURE_NEON
        return NeonExtractMask4(Vector4F_CompareNotEqual(a, b)) == 0b1111;
#endif
    }

    inline bool anemone_vectorcall Vector4F_IsNotEqual3(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return (a.Inner[0] != b.Inner[0]) || (a.Inner[1] != b.Inner[1]) || (a.Inner[2] != b.Inner[2]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return (_mm_movemask_ps(_mm_cmpneq_ps(a, b)) & 0b0111) == 0b0111;
#elif ANEMONE_FEATURE_NEON
        return (NeonExtractMask4(Vector4F_CompareNotEqual(a, b)) & 0b0111) == 0b0111;
#endif
    }

    inline bool anemone_vectorcall Vector4F_IsNotEqual2(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return (a.Inner[0] != b.Inner[0]) || (a.Inner[1] != b.Inner[1]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return (_mm_movemask_ps(_mm_cmpneq_ps(a, b)) & 0b0011) == 0b0011;
#elif ANEMONE_FEATURE_NEON
        return (NeonExtractMask4(Vector4F_CompareNotEqual(a, b)) & 0b0011) == 0b0011;
#endif
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareLessThan(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            (a.Inner[0] < b.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[1] < b.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[2] < b.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[3] < b.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_cmplt_ps(a, b);
#elif ANEMONE_FEATURE_NEON
        return vcltq_f32(a, b);
#endif
    }

    inline bool anemone_vectorcall Vector4F_IsLessThan4(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return (a.Inner[0] < b.Inner[0]) && (a.Inner[1] < b.Inner[1]) && (a.Inner[2] < b.Inner[2]) && (a.Inner[3] < b.Inner[3]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_movemask_ps(_mm_cmplt_ps(a, b)) == 0b1111;
#elif ANEMONE_FEATURE_NEON
        return NeonExtractMask4(Vector4F_CompareLessThan(a, b)) == 0b1111;
#endif
    }

    inline bool anemone_vectorcall Vector4F_IsLessThan3(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return (a.Inner[0] < b.Inner[0]) && (a.Inner[1] < b.Inner[1]) && (a.Inner[2] < b.Inner[2]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return (_mm_movemask_ps(_mm_cmplt_ps(a, b)) & 0b0111) == 0b0111;
#elif ANEMONE_FEATURE_NEON
        return (NeonExtractMask4(Vector4F_CompareLessThan(a, b)) & 0b0111) == 0b0111;
#endif
    }

    inline bool anemone_vectorcall Vector4F_IsLessThan2(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return (a.Inner[0] < b.Inner[0]) && (a.Inner[1] < b.Inner[1]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return (_mm_movemask_ps(_mm_cmplt_ps(a, b)) & 0b0011) == 0b0011;
#elif ANEMONE_FEATURE_NEON
        return (NeonExtractMask4(Vector4F_CompareLessThan(a, b)) & 0b0011) == 0b0011;
#endif
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareLessEqual(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            (a.Inner[0] <= b.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[1] <= b.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[2] <= b.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[3] <= b.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_cmple_ps(a, b);
#elif ANEMONE_FEATURE_NEON
        return vcleq_f32(a, b);
#endif
    }

    inline bool anemone_vectorcall Vector4F_IsLessEqual4(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return (a.Inner[0] <= b.Inner[0]) && (a.Inner[1] <= b.Inner[1]) && (a.Inner[2] <= b.Inner[2]) && (a.Inner[3] <= b.Inner[3]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_movemask_ps(_mm_cmple_ps(a, b)) == 0b1111;
#elif ANEMONE_FEATURE_NEON
        return NeonExtractMask4(Vector4F_CompareLessEqual(a, b)) == 0b1111;
#endif
    }

    inline bool anemone_vectorcall Vector4F_IsLessEqual3(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return (a.Inner[0] <= b.Inner[0]) && (a.Inner[1] <= b.Inner[1]) && (a.Inner[2] <= b.Inner[2]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return (_mm_movemask_ps(_mm_cmple_ps(a, b)) & 0b0111) == 0b0111;
#elif ANEMONE_FEATURE_NEON
        return (NeonExtractMask4(Vector4F_CompareLessEqual(a, b)) & 0b0111) == 0b0111;
#endif
    }

    inline bool anemone_vectorcall Vector4F_IsLessEqual2(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return (a.Inner[0] <= b.Inner[0]) && (a.Inner[1] <= b.Inner[1]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return (_mm_movemask_ps(_mm_cmple_ps(a, b)) & 0b0011) == 0b0011;
#elif ANEMONE_FEATURE_NEON
        return (NeonExtractMask4(Vector4F_CompareLessEqual(a, b)) & 0b0011) == 0b0011;
#endif
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareGreaterThan(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            (a.Inner[0] > b.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[1] > b.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[2] > b.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[3] > b.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_cmpgt_ps(a, b);
#elif ANEMONE_FEATURE_NEON
        return vcgtq_f32(a, b);
#endif
    }

    inline bool anemone_vectorcall Vector4F_IsGreaterThan4(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return (a.Inner[0] > b.Inner[0]) && (a.Inner[1] > b.Inner[1]) && (a.Inner[2] > b.Inner[2]) && (a.Inner[3] > b.Inner[3]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_movemask_ps(_mm_cmpgt_ps(a, b)) == 0b1111;
#elif ANEMONE_FEATURE_NEON
        return NeonExtractMask4(Vector4F_CompareGreaterThan(a, b)) == 0b1111;
#endif
    }

    inline bool anemone_vectorcall Vector4F_IsGreaterThan3(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return (a.Inner[0] > b.Inner[0]) && (a.Inner[1] > b.Inner[1]) && (a.Inner[2] > b.Inner[2]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return (_mm_movemask_ps(_mm_cmpgt_ps(a, b)) & 0b0111) == 0b0111;
#elif ANEMONE_FEATURE_NEON
        return (NeonExtractMask4(Vector4F_CompareGreaterThan(a, b)) & 0b0111) == 0b0111;
#endif
    }

    inline bool anemone_vectorcall Vector4F_IsGreaterThan2(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return (a.Inner[0] > b.Inner[0]) && (a.Inner[1] > b.Inner[1]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return (_mm_movemask_ps(_mm_cmpgt_ps(a, b)) & 0b0011) == 0b0011;
#elif ANEMONE_FEATURE_NEON
        return (NeonExtractMask4(Vector4F_CompareGreaterThan(a, b)) & 0b0011) == 0b0011;
#endif
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareGreaterEqual(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            (a.Inner[0] >= b.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[1] >= b.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[2] >= b.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[3] >= b.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_cmpge_ps(a, b);
#elif ANEMONE_FEATURE_NEON
        return vcgeq_f32(a, b);
#endif
    }

    inline bool anemone_vectorcall Vector4F_IsGreaterEqual4(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return (a.Inner[0] >= b.Inner[0]) && (a.Inner[1] >= b.Inner[1]) && (a.Inner[2] >= b.Inner[2]) && (a.Inner[3] >= b.Inner[3]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_movemask_ps(_mm_cmpge_ps(a, b)) == 0b1111;
#elif ANEMONE_FEATURE_NEON
        return NeonExtractMask4(Vector4F_CompareGreaterEqual(a, b)) == 0b1111;
#endif
    }

    inline bool anemone_vectorcall Vector4F_IsGreaterEqual3(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return (a.Inner[0] >= b.Inner[0]) && (a.Inner[1] >= b.Inner[1]) && (a.Inner[2] >= b.Inner[2]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return (_mm_movemask_ps(_mm_cmpge_ps(a, b)) & 0b0111) == 0b0111;
#elif ANEMONE_FEATURE_NEON
        return (NeonExtractMask4(Vector4F_CompareGreaterEqual(a, b)) & 0b0111) == 0b0111;
#endif
    }

    inline bool anemone_vectorcall Vector4F_IsGreaterEqual2(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return (a.Inner[0] >= b.Inner[0]) && (a.Inner[1] >= b.Inner[1]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return (_mm_movemask_ps(_mm_cmpge_ps(a, b)) & 0b0011) == 0b0011;
#elif ANEMONE_FEATURE_NEON
        return (NeonExtractMask4(Vector4F_CompareGreaterEqual(a, b)) & 0b0011) == 0b0011;
#endif
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNaN(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            IsNaN<float>(v.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            IsNaN<float>(v.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            IsNaN<float>(v.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            IsNaN<float>(v.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_cmpunord_ps(v, v);
#elif ANEMONE_FEATURE_NEON
        return NeonCompareNaN(v);
#endif
    }

    inline bool anemone_vectorcall Vector4F_IsNaN4(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return IsNaN<float>(v.Inner[0]) && IsNaN<float>(v.Inner[1]) && IsNaN<float>(v.Inner[2]) && IsNaN<float>(v.Inner[3]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_movemask_ps(_mm_cmpunord_ps(v, v)) == 0b1111;
#elif ANEMONE_FEATURE_NEON
        return NeonExtractMask4(Vector4F_CompareNaN(v)) == 0b1111;
#endif
    }

    inline bool anemone_vectorcall Vector4F_IsNaN3(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return IsNaN<float>(v.Inner[0]) && IsNaN<float>(v.Inner[1]) && IsNaN<float>(v.Inner[2]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return (_mm_movemask_ps(_mm_cmpunord_ps(v, v)) & 0b0111) == 0b0111;
#elif ANEMONE_FEATURE_NEON
        return (NeonExtractMask4(Vector4F_CompareNaN(v)) & 0b0111) == 0b0111;
#endif
    }

    inline bool anemone_vectorcall Vector4F_IsNaN2(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return IsNaN<float>(v.Inner[0]) && IsNaN<float>(v.Inner[1]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return (_mm_movemask_ps(_mm_cmpunord_ps(v, v)) & 0b0011) == 0b0011;
#elif ANEMONE_FEATURE_NEON
        float32x2_t const v_xy = vget_low_f32(v);
        uint32x2_t const mask = vmvn_u32(vceq_f32(v_xy, v_xy));

        return (NeonExtractMask2(mask) & 0b0011) == 0b0011;
#endif
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareInfinite(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            IsInfinite<float>(v.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            IsInfinite<float>(v.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            IsInfinite<float>(v.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            IsInfinite<float>(v.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const mask = _mm_load_ps(F32x4_AbsMask_XXXX.As<float>());
        __m128 const infinity = _mm_load_ps(F32x4_PositiveInfinity_XXXX.As<float>());

        __m128 const abs_value = _mm_and_ps(v, mask);
        return _mm_cmpeq_ps(abs_value, infinity);
#elif ANEMONE_FEATURE_NEON
        float32x4_t const vAbs = vabsq_f32(v);
        return vceqq_f32(vAbs, vld1q_f32(F32x4_PositiveInfinity_XXXX.As<float>()));
#endif
    }

    inline bool anemone_vectorcall Vector4F_IsInfinite4(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return IsInfinite<float>(v.Inner[0]) && IsInfinite<float>(v.Inner[1]) && IsInfinite<float>(v.Inner[2]) && IsInfinite<float>(v.Inner[3]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_movemask_ps(Vector4F_CompareInfinite(v)) == 0b1111;
#elif ANEMONE_FEATURE_NEON
        return NeonExtractMask4(Vector4F_CompareInfinite(v)) == 0b1111;
#endif
    }

    inline bool anemone_vectorcall Vector4F_IsInfinite3(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return IsInfinite<float>(v.Inner[0]) && IsInfinite<float>(v.Inner[1]) && IsInfinite<float>(v.Inner[2]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return (_mm_movemask_ps(Vector4F_CompareInfinite(v)) & 0b0111) == 0b0111;
#elif ANEMONE_FEATURE_NEON
        return (NeonExtractMask4(Vector4F_CompareInfinite(v)) & 0b0111) == 0b0111;
#endif
    }

    inline bool anemone_vectorcall Vector4F_IsInfinite2(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return IsInfinite<float>(v.Inner[0]) && IsInfinite<float>(v.Inner[1]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return (_mm_movemask_ps(Vector4F_CompareInfinite(v)) & 0b0011) == 0b0011;
#elif ANEMONE_FEATURE_NEON
        return (NeonExtractMask4(Vector4F_CompareInfinite(v)) & 0b0011) == 0b0011;
#endif
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareInRange(SimdVector4F v, SimdVector4F lower, SimdVector4F upper)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            InRange<float>(v.Inner[0], lower.Inner[0], upper.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            InRange<float>(v.Inner[1], lower.Inner[1], upper.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            InRange<float>(v.Inner[2], lower.Inner[2], upper.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            InRange<float>(v.Inner[3], lower.Inner[3], upper.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const mask_lower = _mm_cmple_ps(lower, v);
        __m128 const mask_upper = _mm_cmple_ps(v, upper);
        return _mm_and_ps(mask_lower, mask_upper);
#elif ANEMONE_FEATURE_NEON
        uint32x4_t const maskLower = vcleq_f32(lower, v);
        uint32x4_t const maskUpper = vcleq_f32(v, upper);
        return vandq_u32(maskLower, maskUpper);
#endif
    }

    inline bool anemone_vectorcall Vector4F_InRange4(SimdVector4F v, SimdVector4F lower, SimdVector4F upper)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return InRange<float>(v.Inner[0], lower.Inner[0], upper.Inner[0]) &&
            InRange<float>(v.Inner[1], lower.Inner[1], upper.Inner[1]) &&
            InRange<float>(v.Inner[2], lower.Inner[2], upper.Inner[2]) &&
            InRange<float>(v.Inner[3], lower.Inner[3], upper.Inner[3]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_movemask_ps(Vector4F_CompareInRange(v, lower, upper)) == 0b1111;
#elif ANEMONE_FEATURE_NEON
        return NeonExtractMask4(Vector4F_CompareInRange(v, lower, upper)) == 0b1111;
#endif
    }

    inline bool anemone_vectorcall Vector4F_InRange3(SimdVector4F v, SimdVector4F lower, SimdVector4F upper)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return InRange<float>(v.Inner[0], lower.Inner[0], upper.Inner[0]) &&
            InRange<float>(v.Inner[1], lower.Inner[1], upper.Inner[1]) &&
            InRange<float>(v.Inner[2], lower.Inner[2], upper.Inner[2]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return (_mm_movemask_ps(Vector4F_CompareInRange(v, lower, upper)) & 0b0111) == 0b0111;
#elif ANEMONE_FEATURE_NEON
        return (NeonExtractMask4(Vector4F_CompareInRange(v, lower, upper)) & 0b0111) == 0b0111;
#endif
    }

    inline bool anemone_vectorcall Vector4F_InRange2(SimdVector4F v, SimdVector4F lower, SimdVector4F upper)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return InRange<float>(v.Inner[0], lower.Inner[0], upper.Inner[0]) &&
            InRange<float>(v.Inner[1], lower.Inner[1], upper.Inner[1]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return (_mm_movemask_ps(Vector4F_CompareInRange(v, lower, upper)) & 0b0011) == 0b0011;
#elif ANEMONE_FEATURE_NEON
        return (NeonExtractMask4(Vector4F_CompareInRange(v, lower, upper)) & 0b0011) == 0b0011;
#endif
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearZero(SimdVector4F v, SimdVector4F tolerance)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            IsNearZero<float>(v.Inner[0], tolerance.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            IsNearZero<float>(v.Inner[1], tolerance.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            IsNearZero<float>(v.Inner[2], tolerance.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            IsNearZero<float>(v.Inner[3], tolerance.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const abs = Vector4F_Abs(v);
        return _mm_cmple_ps(abs, tolerance);
#elif ANEMONE_FEATURE_NEON
        // = abs(v) <= bounds
        float32x4_t const abs_value = vabsq_f32(v);
        return vcleq_f32(abs_value, tolerance);
#endif
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearZero(SimdVector4F v, float tolerance)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            IsNearZero<float>(v.Inner[0], tolerance) ? ScalarMaskComponentUInt32 : 0,
            IsNearZero<float>(v.Inner[1], tolerance) ? ScalarMaskComponentUInt32 : 0,
            IsNearZero<float>(v.Inner[2], tolerance) ? ScalarMaskComponentUInt32 : 0,
            IsNearZero<float>(v.Inner[3], tolerance) ? ScalarMaskComponentUInt32 : 0,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return Vector4F_CompareNearZero(v, _mm_set1_ps(tolerance));
#elif ANEMONE_FEATURE_NEON
        return Vector4F_CompareNearZero(v, vdupq_n_f32(tolerance));
#endif
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearZero(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            IsNearZero<float>(v.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            IsNearZero<float>(v.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            IsNearZero<float>(v.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            IsNearZero<float>(v.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return Vector4F_CompareNearZero(v, _mm_set1_ps(std::numeric_limits<float>::epsilon()));
#elif ANEMONE_FEATURE_NEON
        return Vector4F_CompareNearZero(v, vdupq_n_f32(std::numeric_limits<float>::epsilon()));
#endif
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearZeroPrecise(SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            IsNearZeroPrecise(v.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            IsNearZeroPrecise(v.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            IsNearZeroPrecise(v.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            IsNearZeroPrecise(v.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_castsi128_ps(AvxCompareNearZeroPreciseI32(_mm_castps_si128(v)));
#elif ANEMONE_FEATURE_NEON
        return NeonCompareNearZeroPrecise(vreinterpretq_s32_f32(v));
#endif
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearEqual(SimdVector4F a, SimdVector4F b, SimdVector4F tolerance)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            IsNearEqual<float>(a.Inner[0], b.Inner[0], tolerance.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            IsNearEqual<float>(a.Inner[1], b.Inner[1], tolerance.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            IsNearEqual<float>(a.Inner[2], b.Inner[2], tolerance.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            IsNearEqual<float>(a.Inner[3], b.Inner[3], tolerance.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // Compute difference between two values
        __m128 const diff = _mm_sub_ps(a, b);
        // Compute absolute value
        __m128 const abs_mask = _mm_load_ps(F32x4_AbsMask_XXXX.As<float>());
        __m128 const abs_diff = _mm_and_ps(diff, abs_mask);
        return _mm_cmple_ps(abs_diff, tolerance);
#elif ANEMONE_FEATURE_NEON
        // = abs(a - b) <= tolerance
        float32x4_t const abs_diff = vabsq_f32(vsubq_f32(a, b));
        return vcleq_f32(abs_diff, tolerance);
#endif
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearEqual(SimdVector4F a, SimdVector4F b, float tolerance)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            IsNearEqual<float>(a.Inner[0], b.Inner[0], tolerance) ? ScalarMaskComponentUInt32 : 0,
            IsNearEqual<float>(a.Inner[1], b.Inner[1], tolerance) ? ScalarMaskComponentUInt32 : 0,
            IsNearEqual<float>(a.Inner[2], b.Inner[2], tolerance) ? ScalarMaskComponentUInt32 : 0,
            IsNearEqual<float>(a.Inner[3], b.Inner[3], tolerance) ? ScalarMaskComponentUInt32 : 0,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return Vector4F_CompareNearEqual(a, b, _mm_set1_ps(tolerance));
#elif ANEMONE_FEATURE_NEON
        return Vector4F_CompareNearEqual(a, b, vdupq_n_f32(tolerance));
#endif
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearEqual(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            IsNearEqual<float>(a.Inner[0], b.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            IsNearEqual<float>(a.Inner[1], b.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            IsNearEqual<float>(a.Inner[2], b.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            IsNearEqual<float>(a.Inner[3], b.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return Vector4F_CompareNearEqual(a, b, _mm_set1_ps(std::numeric_limits<float>::epsilon()));
#elif ANEMONE_FEATURE_NEON
        return Vector4F_CompareNearEqual(a, b, vdupq_n_f32(std::numeric_limits<float>::epsilon()));
#endif
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearEqualPrecise(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            IsNearEqualPrecise(a.Inner[0], b.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            IsNearEqualPrecise(a.Inner[1], b.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            IsNearEqualPrecise(a.Inner[2], b.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            IsNearEqualPrecise(a.Inner[3], b.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // = abs(a - b) < tolerance
        __m128 const mask_within_tolerance = Vector4F_CompareNearEqual(a, b);

        __m128i const a_ulp = _mm_castps_si128(a);
        __m128i const b_ulp = _mm_castps_si128(b);

        // = (sign(ulp(a)) == sign(ulp(b)))
        __m128 const mask_same_sign_ulp = _mm_castsi128_ps(AvxCompareSignI32(a_ulp, b_ulp));

        // = ulp(a - b) < ulp_tolerance
        __m128 const mask_within_ulp = _mm_castsi128_ps(AvxCompareNearZeroPreciseI32(_mm_sub_epi32(a_ulp, b_ulp)));

        return _mm_or_ps(mask_within_tolerance, _mm_and_ps(mask_same_sign_ulp, mask_within_ulp));
#elif ANEMONE_FEATURE_NEON
        // = abs(a - b) < tolerance
        uint32x4_t const maskWithinEpsilon = Vector4F_CompareNearEqual(a, b);

        // = bit_cast<int32>(...)
        int32x4_t const ulpA = vreinterpretq_s32_f32(a);
        int32x4_t const ulpB = vreinterpretq_s32_f32(b);

        // = (ulpA >> 31) == (ulpB >> 31)
        uint32x4_t const maskSameSignUlp = NeonCompareSignInt32x4(ulpA, ulpB);

        // = ulp(a - b) < ulpTolerance
        uint32x4_t const maskWithinUlp = NeonCompareNearZeroPrecise(vsubq_s32(ulpA, ulpB));

        return vorrq_u32(maskWithinEpsilon, vandq_u32(maskSameSignUlp, maskWithinUlp));
#endif
    }
}

// Mask4F
namespace Anemone::Math::Detail
{
    inline SimdMask4F anemone_vectorcall Mask4F_True()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            ScalarMaskComponentUInt32,
            ScalarMaskComponentUInt32,
            ScalarMaskComponentUInt32,
            ScalarMaskComponentUInt32,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_castsi128_ps(_mm_set1_epi32(AvxMaskComponentInt32));
#elif ANEMONE_FEATURE_NEON
        return vdupq_n_u32(NeonMaskComponentUInt32);
#endif
    }

    inline SimdMask4F anemone_vectorcall Mask4F_False()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            0,
            0,
            0,
            0,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_setzero_ps();
#elif ANEMONE_FEATURE_NEON
        return vdupq_n_u32(0);
#endif
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Create(bool x, bool y, bool z, bool w)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            x ? ScalarMaskComponentUInt32 : 0,
            y ? ScalarMaskComponentUInt32 : 0,
            z ? ScalarMaskComponentUInt32 : 0,
            w ? ScalarMaskComponentUInt32 : 0,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_castsi128_ps(
            _mm_setr_epi32(
                x ? AvxMaskComponentInt32 : 0,
                y ? AvxMaskComponentInt32 : 0,
                z ? AvxMaskComponentInt32 : 0,
                w ? AvxMaskComponentInt32 : 0));
#elif ANEMONE_FEATURE_NEON
        uint32_t const r[4]{
            x ? NeonMaskComponentUInt32 : 0,
            y ? NeonMaskComponentUInt32 : 0,
            z ? NeonMaskComponentUInt32 : 0,
            w ? NeonMaskComponentUInt32 : 0,
        };
        return vld1q_u32(r);
#endif
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Create(bool x, bool y, bool z)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            x ? ScalarMaskComponentUInt32 : 0,
            y ? ScalarMaskComponentUInt32 : 0,
            z ? ScalarMaskComponentUInt32 : 0,
            0,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_castsi128_ps(
            _mm_setr_epi32(
                x ? AvxMaskComponentInt32 : 0,
                y ? AvxMaskComponentInt32 : 0,
                z ? AvxMaskComponentInt32 : 0,
                0));
#elif ANEMONE_FEATURE_NEON
        uint32_t const r[4]{
            x ? NeonMaskComponentUInt32 : 0,
            y ? NeonMaskComponentUInt32 : 0,
            z ? NeonMaskComponentUInt32 : 0,
            0,
        };
        return vld1q_u32(r);
#endif
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Create(bool x, bool y)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            x ? ScalarMaskComponentUInt32 : 0,
            y ? ScalarMaskComponentUInt32 : 0,
            0,
            0,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_castsi128_ps(
            _mm_setr_epi32(
                x ? AvxMaskComponentInt32 : 0,
                y ? AvxMaskComponentInt32 : 0,
                0,
                0));
#elif ANEMONE_FEATURE_NEON
        uint32_t const r[4]{
            x ? NeonMaskComponentUInt32 : 0,
            y ? NeonMaskComponentUInt32 : 0,
            0,
            0,
        };
        return vld1q_u32(r);
#endif
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Replicate(bool value)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        uint32_t const mask = value ? ScalarMaskComponentUInt32 : 0;
        return SimdMask4F{mask, mask, mask, mask};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_castsi128_ps(_mm_set1_epi32(value ? AvxMaskComponentInt32 : 0));
#elif ANEMONE_FEATURE_NEON
        return vdupq_n_u32(value ? NeonMaskComponentUInt32 : 0);
#endif
    }

    template <size_t N>
    uint32_t anemone_vectorcall Mask4F_Extract(SimdMask4F v)
        requires(N < 4)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return v.Inner[N];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return static_cast<uint32_t>(_mm_extract_epi32(_mm_castps_si128(v), N));
#elif ANEMONE_FEATURE_NEON
        return vgetq_lane_u32(v, N);
#endif
    }

    template <size_t N>
    SimdMask4F anemone_vectorcall Mask4F_Insert(SimdMask4F v, uint32_t b)
        requires(N < 4)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        v.Inner[N] = b;
        return v;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_castsi128_ps(_mm_insert_epi32(_mm_castps_si128(v), static_cast<int>(b), N));
#elif ANEMONE_FEATURE_NEON
        return vsetq_lane_u32(b, v, N);
#endif
    }

    inline SimdMask4F anemone_vectorcall Mask4F_And(SimdMask4F a, SimdMask4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            a.Inner[0] & b.Inner[0],
            a.Inner[1] & b.Inner[1],
            a.Inner[2] & b.Inner[2],
            a.Inner[3] & b.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_and_ps(a, b);
#elif ANEMONE_FEATURE_NEON
        return vandq_u32(a, b);
#endif
    }

    inline SimdMask4F anemone_vectorcall Mask4F_AndNot(SimdMask4F a, SimdMask4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            a.Inner[0] & ~b.Inner[0],
            a.Inner[1] & ~b.Inner[1],
            a.Inner[2] & ~b.Inner[2],
            a.Inner[3] & ~b.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_andnot_ps(a, b);
#elif ANEMONE_FEATURE_NEON
        return vbicq_u32(a, b);
#endif
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Or(SimdMask4F a, SimdMask4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            a.Inner[0] | b.Inner[0],
            a.Inner[1] | b.Inner[1],
            a.Inner[2] | b.Inner[2],
            a.Inner[3] | b.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_or_ps(a, b);
#elif ANEMONE_FEATURE_NEON
        return vorrq_u32(a, b);
#endif
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Xor(SimdMask4F a, SimdMask4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            a.Inner[0] ^ b.Inner[0],
            a.Inner[1] ^ b.Inner[1],
            a.Inner[2] ^ b.Inner[2],
            a.Inner[3] ^ b.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_xor_ps(a, b);
#elif ANEMONE_FEATURE_NEON
        return veorq_u32(a, b);
#endif
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Not(SimdMask4F mask)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            ~mask.Inner[0],
            ~mask.Inner[1],
            ~mask.Inner[2],
            ~mask.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_xor_ps(mask, mask);
#elif ANEMONE_FEATURE_NEON
        return vmvnq_u32(mask);
#endif
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Select(SimdMask4F mask, SimdMask4F whenFalse, SimdMask4F whenTrue)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            (mask.Inner[0] & whenTrue.Inner[0]) | (~mask.Inner[0] & whenFalse.Inner[0]),
            (mask.Inner[1] & whenTrue.Inner[1]) | (~mask.Inner[1] & whenFalse.Inner[1]),
            (mask.Inner[2] & whenTrue.Inner[2]) | (~mask.Inner[2] & whenFalse.Inner[2]),
            (mask.Inner[3] & whenTrue.Inner[3]) | (~mask.Inner[3] & whenFalse.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_blendv_ps(whenFalse, whenTrue, mask);
#elif ANEMONE_FEATURE_NEON
        return vbslq_u32(mask, whenTrue, whenFalse);
#endif
    }

    template <bool X, bool Y, bool Z, bool W>
    SimdMask4F anemone_vectorcall Mask4F_Select(SimdMask4F whenFalse, SimdMask4F whenTrue)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            X ? whenTrue.Inner[0] : whenFalse.Inner[0],
            Y ? whenTrue.Inner[1] : whenFalse.Inner[1],
            Z ? whenTrue.Inner[2] : whenFalse.Inner[2],
            W ? whenTrue.Inner[3] : whenFalse.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_blend_ps(whenFalse, whenTrue, (X ? 0b0001 : 0) | (Y ? 0b0010 : 0) | (Z ? 0b0100 : 0) | (W ? 0b1000 : 0));
#elif ANEMONE_FEATURE_NEON
        static constexpr uint32_t mask[4]{
            X ? NeonMaskComponentUInt32 : 0,
            Y ? NeonMaskComponentUInt32 : 0,
            Z ? NeonMaskComponentUInt32 : 0,
            W ? NeonMaskComponentUInt32 : 0,
        };
        return vbslq_u32(vld1q_u32(mask), whenTrue, whenFalse);
#endif
    }

    inline bool anemone_vectorcall Mask4F_All4(SimdMask4F mask)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        uint32_t const reduced = mask.Inner[0] & mask.Inner[1] & mask.Inner[2] & mask.Inner[3];
        return reduced != 0;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_movemask_ps(mask) == 0b1111;
#elif ANEMONE_FEATURE_NEON
        return NeonExtractMask4(mask) == 0b1111;
#endif
    }

    inline bool anemone_vectorcall Mask4F_All3(SimdMask4F mask)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        uint32_t const reduced = mask.Inner[0] & mask.Inner[1] & mask.Inner[2];
        return reduced != 0;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return (_mm_movemask_ps(mask) & 0b0111) == 0b0111;
#elif ANEMONE_FEATURE_NEON
        return (NeonExtractMask4(mask) & 0b0111) == 0b0111;
#endif
    }

    inline bool anemone_vectorcall Mask4F_All2(SimdMask4F mask)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        uint32_t const reduced = mask.Inner[0] & mask.Inner[1];
        return reduced != 0;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return (_mm_movemask_ps(mask) & 0b0011) == 0b0011;
#elif ANEMONE_FEATURE_NEON
        return (NeonExtractMask4(mask) & 0b0011) == 0b0011;
#endif
    }

    inline bool anemone_vectorcall Mask4F_Any4(SimdMask4F mask)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        uint32_t const reduced = mask.Inner[0] | mask.Inner[1] | mask.Inner[2] | mask.Inner[3];
        return reduced != 0;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_movemask_ps(mask) != 0;
#elif ANEMONE_FEATURE_NEON
        return NeonExtractMask4(mask) != 0;
#endif
    }

    inline bool anemone_vectorcall Mask4F_Any3(SimdMask4F mask)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        uint32_t const reduced = mask.Inner[0] | mask.Inner[1] | mask.Inner[2];
        return reduced != 0;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return (_mm_movemask_ps(mask) & 0b0111) != 0;
#elif ANEMONE_FEATURE_NEON
        return (NeonExtractMask4(mask) & 0b0111) != 0;
#endif
    }

    inline bool anemone_vectorcall Mask4F_Any2(SimdMask4F mask)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        uint32_t const reduced = mask.Inner[0] | mask.Inner[1];
        return reduced != 0;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return (_mm_movemask_ps(mask) & 0b0011) != 0;
#elif ANEMONE_FEATURE_NEON
        return (NeonExtractMask4(mask) & 0b0011) != 0;
#endif
    }

    inline bool anemone_vectorcall Mask4F_None4(SimdMask4F mask)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        uint32_t const reduced = mask.Inner[0] | mask.Inner[1] | mask.Inner[2] | mask.Inner[3];
        return reduced == 0;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_movemask_ps(mask) == 0;
#elif ANEMONE_FEATURE_NEON
        return NeonExtractMask4(mask) == 0;
#endif
    }

    inline bool anemone_vectorcall Mask4F_None3(SimdMask4F mask)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        uint32_t const reduced = mask.Inner[0] | mask.Inner[1] | mask.Inner[2];
        return reduced == 0;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return (_mm_movemask_ps(mask) & 0b0111) == 0;
#elif ANEMONE_FEATURE_NEON
        return (NeonExtractMask4(mask) & 0b0111) == 0;
#endif
    }

    inline bool anemone_vectorcall Mask4F_None2(SimdMask4F mask)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        uint32_t const reduced = mask.Inner[0] | mask.Inner[1];
        return reduced == 0;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return (_mm_movemask_ps(mask) & 0b0011) == 0;
#elif ANEMONE_FEATURE_NEON
        return (NeonExtractMask4(mask) & 0b0011) == 0;
#endif
    }

    inline SimdMask4F anemone_vectorcall Mask4F_CompareEqual(SimdMask4F a, SimdMask4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            a.Inner[0] == b.Inner[0],
            a.Inner[1] == b.Inner[1],
            a.Inner[2] == b.Inner[2],
            a.Inner[3] == b.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_castsi128_ps(_mm_cmpeq_epi32(_mm_castps_si128(a), _mm_castps_si128(b)));
#elif ANEMONE_FEATURE_NEON
        return vceqq_u32(a, b);
#endif
    }

    inline SimdMask4F anemone_vectorcall Mask4F_CompareNotEqual(SimdMask4F a, SimdMask4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdMask4F{
            a.Inner[0] != b.Inner[0],
            a.Inner[1] != b.Inner[1],
            a.Inner[2] != b.Inner[2],
            a.Inner[3] != b.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const mask = _mm_castsi128_ps(_mm_cmpeq_epi32(_mm_castps_si128(a), _mm_castps_si128(b)));
        return _mm_xor_ps(mask, mask);
#elif ANEMONE_FEATURE_NEON
        return vmvnq_u32(vceqq_u32(a, b));
#endif
    }

}

// QuaternionF
namespace Anemone::Math::Detail
{
    inline SimdVector4F anemone_vectorcall QuaternionF_Identity()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4F>(F32x4_PositiveUnitW);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_load_ps(F32x4_PositiveUnitW.As<float>());
#elif ANEMONE_FEATURE_NEON
        return vld1q_f32(F32x4_PositiveUnitW.As<float>());
#endif
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_CreateFromNormalAngle(SimdVector4F normal, float angle)
    {
        auto [fSin, fCos] = SinCos<float>(0.5f * angle);

#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            fSin * normal.Inner[0],
            fSin * normal.Inner[1],
            fSin * normal.Inner[2],
            fCos,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // = [fSin, fSin, fSin, ...]
        __m128 const vSin = _mm_set1_ps(fSin);
        // = [fSin * n.x, fSin * n.y, fSin * n.z, ...]
        __m128 r = _mm_mul_ps(normal, vSin);
        // = [fSin * n.x, fSin * n.y, fSin * n.z, fCos]
        return _mm_insert_ps(r, _mm_set_ss(fCos), 3u << 4);
#elif ANEMONE_FEATURE_NEON

        // = [fSin, fSin, fSin, ...]
        float32x4_t const vSin = vdupq_n_f32(fSin);
        // = [fSin * n.x, fSin * n.y, fSin * n.z, ...]
        float32x4_t r = vmulq_f32(normal, vSin);
        // = [fSin * n.x, fSin * n.y, fSin * n.z, fCos]
        r = vsetq_lane_f32(fCos, r, 3);
        return r;
#endif
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_CreateFromAxisAngle(SimdVector4F axis, float angle)
    {
        // assert (axis is not zero)
        // assert (axis is not infinite)
        SimdVector4F const normal = Vector4F_Normalize3(axis);
        return QuaternionF_CreateFromNormalAngle(normal, angle);
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_CreateFromEulerAngles(SimdVector4F pitchYawRoll)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return QuaternionF_CreateFromEulerAngles(pitchYawRoll.Inner[0], pitchYawRoll.Inner[1], pitchYawRoll.Inner[2]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 vSin;
        __m128 vCos;
        Vector4F_SinCos(vSin, vCos, _mm_mul_ps(pitchYawRoll, _mm_set1_ps(0.5f)));

        __m128 const cp_cp_cp_cp = _mm_permute_ps(vCos, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 const cy_cy_cy_cy = _mm_permute_ps(vCos, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 const cr_cr_cr_cr = _mm_permute_ps(vCos, _MM_SHUFFLE(2, 2, 2, 2));

        __m128 const sp_sp_sp_sp = _mm_permute_ps(vSin, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 const sy_sy_sy_sy = _mm_permute_ps(vSin, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 const sr_sr_sr_sr = _mm_permute_ps(vSin, _MM_SHUFFLE(2, 2, 2, 2));

        // Note: Bitmasks for _mm_blend_ps are exact opposites. Maybe there is a better way to select the right values

        // cr * sp * cy + sr * sy * negate_yz * cp
        // cr * cp * sy + sr * cy * negate_yz * sp
        // sr * cp * cy + cr * sy * negate_yz * sp
        // cr * cp * cy + sr * sy * negate_yz * sp
        //
        // ( r0  )
        //                          (    r1      )
        //                ( r2  )
        //                (          r3          )
        // (              r4                     )

        __m128 const cr_cr_sr_cr = _mm_blend_ps(cr_cr_cr_cr, sr_sr_sr_sr, 0b0100);
        __m128 const sp_cp_cp_cp = _mm_blend_ps(cp_cp_cp_cp, sp_sp_sp_sp, 0b0001);
        __m128 const r0 = _mm_mul_ps(cr_cr_sr_cr, sp_cp_cp_cp);

        __m128 const negate_yz = _mm_setr_ps(1.0f, -1.0f, -1.0f, 1.0f);
        __m128 const cp_sp_sp_sp = _mm_blend_ps(cp_cp_cp_cp, sp_sp_sp_sp, 0b1110);
        __m128 const r1 = _mm_mul_ps(cp_sp_sp_sp, negate_yz);

        __m128 const sr_sr_cr_sr = _mm_blend_ps(cr_cr_cr_cr, sr_sr_sr_sr, 0b1011);
        __m128 const sy_cy_sy_sy = _mm_blend_ps(cy_cy_cy_cy, sy_sy_sy_sy, 0b1101);
        __m128 const r2 = _mm_mul_ps(sr_sr_cr_sr, sy_cy_sy_sy);

        __m128 const r3 = _mm_mul_ps(r1, r2);

        __m128 const cy_sy_cy_cy = _mm_blend_ps(cy_cy_cy_cy, sy_sy_sy_sy, 0b0010);
        __m128 const r4 = Vector4F_MultiplyAdd(r0, cy_sy_cy_cy, r3);
        return r4;
#elif ANEMONE_FEATURE_NEON
        float32x4_t vSin;
        float32x4_t vCos;
        Vector4F_SinCos(vSin, vCos, vmulq_n_f32(pitchYawRoll, 0.5f));

        float32x4_t const cp_cp_cp_cp = vdupq_laneq_f32(vCos, 0);
        float32x4_t const cy_cy_cy_cy = vdupq_laneq_f32(vCos, 1);
        float32x4_t const cr_cr_cr_cr = vdupq_laneq_f32(vCos, 2);

        float32x4_t const sp_sp_sp_sp = vdupq_laneq_f32(vSin, 0);
        float32x4_t const sy_sy_sy_sy = vdupq_laneq_f32(vSin, 1);
        float32x4_t const sr_sr_sr_sr = vdupq_laneq_f32(vSin, 2);

        // cr * sp * cy + sr * sy * negate_yz * cp
        // cr * cp * sy + sr * cy * negate_yz * sp
        // sr * cp * cy + cr * sy * negate_yz * sp
        // cr * cp * cy + sr * sy * negate_yz * sp
        //
        // ( r0  )
        //                          (    r1      )
        //                ( r2  )
        //                (          r3          )
        // (              r4                     )

        float32x4_t const cr_cr_sr_cr = vcopyq_laneq_f32(cr_cr_cr_cr, 2, sr_sr_sr_sr, 2);
        float32x4_t const sp_cp_cp_cp = vcopyq_laneq_f32(cp_cp_cp_cp, 0, sp_sp_sp_sp, 0);
        float32x4_t const r0 = vmulq_f32(cr_cr_sr_cr, sp_cp_cp_cp);

        static constexpr float fnegate_yz[4]{1.0f, -1.0f, -1.0f, 1.0f};
        float32x4_t const negate_yz = vld1q_f32(fnegate_yz);
        float32x4_t const cp_sp_sp_sp = vcopyq_laneq_f32(sp_sp_sp_sp, 0, cp_cp_cp_cp, 0);
        float32x4_t const r1 = vmulq_f32(cp_sp_sp_sp, negate_yz);

        float32x4_t const sr_sr_cr_sr = vcopyq_laneq_f32(sr_sr_sr_sr, 2, cr_cr_cr_cr, 2);
        float32x4_t const sy_cy_sy_sy = vcopyq_laneq_f32(sy_sy_sy_sy, 1, cy_cy_cy_cy, 1);
        float32x4_t const r2 = vmulq_f32(sr_sr_cr_sr, sy_cy_sy_sy);

        float32x4_t const r3 = vmulq_f32(r1, r2);

        float32x4_t const cy_sy_cy_cy = vcopyq_laneq_f32(cy_cy_cy_cy, 1, sy_sy_sy_sy, 1);
        float32x4_t const r4 = vmlaq_f32(r3, cy_sy_cy_cy, r0);
        return r4;
#endif
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_CreateFromEulerAngles(float pitch, float yaw, float roll)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        auto const [sp, cp] = SinCos<float>(pitch * 0.5f);
        auto const [sy, cy] = SinCos<float>(yaw * 0.5f);
        auto const [sr, cr] = SinCos<float>(roll * 0.5f);

        return SimdVector4F{
            cr * sp * cy + sr * cp * sy,
            cr * cp * sy - sr * sp * cy,
            sr * cp * cy - cr * sp * sy,
            cr * cp * cy + sr * sp * sy,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return QuaternionF_CreateFromEulerAngles(_mm_setr_ps(pitch, yaw, roll, 0.0f));
#elif ANEMONE_FEATURE_NEON
        float const angles[4]{pitch, yaw, roll, 0.0f};
        return QuaternionF_CreateFromEulerAngles(vld1q_f32(angles));
#endif
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Rotate3(SimdVector4F q, SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        // = [v.xyz, 0]
        SimdVector4F v_xyz = v;
        v_xyz.Inner[3] = 0.0f;

        // = q' * v.xyz * q
        SimdVector4F const q_conj = QuaternionF_Conjugate(q);
        return QuaternionF_Multiply(QuaternionF_Multiply(q_conj, v_xyz), q);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // = [v.xyz, 0]
        __m128 const v_xyz = _mm_and_ps(v, _mm_load_ps(F32x4_SelectMask_XXXn.As<float>()));
        __m128 const q_conj = QuaternionF_Conjugate(q);

        // = q' * v.xyz * q
        return QuaternionF_Multiply(QuaternionF_Multiply(q_conj, v_xyz), q);
#elif ANEMONE_FEATURE_NEON
        // = [v.xyz, 0]
        float32x4_t const v_xyz = vsetq_lane_f32(0.0f, v, 3);
        float32x4_t const q_conj = QuaternionF_Conjugate(q);

        // = q' * v.xyz * q
        return QuaternionF_Multiply(QuaternionF_Multiply(q_conj, v_xyz), q);
#endif
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_InverseRotate3(SimdVector4F q, SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        // = [v.xyz, 0]
        SimdVector4F v_xyz = v;
        v_xyz.Inner[3] = 0.0f;

        // = q * v.xyz * q'
        SimdVector4F const q_conj = QuaternionF_Conjugate(q);
        return QuaternionF_Multiply(QuaternionF_Multiply(q, v_xyz), q_conj);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // = [v.xyz, 0]
        __m128 const v_xyz = _mm_and_ps(v, _mm_load_ps(F32x4_SelectMask_XXXn.As<float>()));
        __m128 const q_conj = QuaternionF_Conjugate(q);

        // = q * v.xyz * q'
        return QuaternionF_Multiply(QuaternionF_Multiply(q, v_xyz), q_conj);
#elif ANEMONE_FEATURE_NEON
        // = [v.xyz, 0]
        float32x4_t const v_xyz = vsetq_lane_f32(0.0f, v, 3);
        float32x4_t const q_conj = QuaternionF_Conjugate(q);

        // = q * v.xyz * q'
        return QuaternionF_Multiply(QuaternionF_Multiply(q, v_xyz), q_conj);
#endif
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Length(SimdVector4F q)
    {
        return Vector4F_Length4(q);
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_LengthSquared(SimdVector4F q)
    {
        return Vector4F_LengthSquared4(q);
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Normalize(SimdVector4F q)
    {
        return Vector4F_Normalize4(q);
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Dot(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_Dot4(a, b);
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Multiply(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const ax = a.Inner[0];
        float const ay = a.Inner[1];
        float const az = a.Inner[2];
        float const aw = a.Inner[3];

        float const bx = b.Inner[0];
        float const by = b.Inner[1];
        float const bz = b.Inner[2];
        float const bw = b.Inner[3];

        return SimdVector4F{
            (bw * ax) + (bx * aw) + (by * az) - (bz * ay),
            (bw * ay) - (bx * az) + (by * aw) + (bz * ax),
            (bw * az) + (bx * ay) - (by * ax) + (bz * aw),
            (bw * aw) - (bx * ax) - (by * ay) - (bz * az),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const a_xyzw = a;
        __m128 const b_xyzw = b;

        __m128 const b_xxxx = _mm_permute_ps(b_xyzw, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 const b_yyyy = _mm_permute_ps(b_xyzw, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 const b_zzzz = _mm_permute_ps(b_xyzw, _MM_SHUFFLE(2, 2, 2, 2));
        __m128 const b_wwww = _mm_permute_ps(b_xyzw, _MM_SHUFFLE(3, 3, 3, 3));

        __m128 const negate_yw = _mm_setr_ps(1.0f, -1.0f, 1.0f, -1.0f);
        __m128 const negate_zw = _mm_setr_ps(1.0f, 1.0f, -1.0f, -1.0f);
        __m128 const negate_xw = _mm_setr_ps(-1.0f, 1.0f, 1.0f, -1.0f);

        __m128 const a_wzyx = _mm_permute_ps(a_xyzw, _MM_SHUFFLE(0, 1, 2, 3));
        __m128 const a_zwxy = _mm_permute_ps(a_wzyx, _MM_SHUFFLE(2, 3, 0, 1));
        __m128 const a_yxwz = _mm_permute_ps(a_zwxy, _MM_SHUFFLE(0, 1, 2, 3));

        // (bw * ax) + negate_yw * (bx * aw) + negate_zw * (by * az) + negate_xw * (bz * ay)
        // (bw * ay) + negate_yw * (bx * az) + negate_zw * (by * aw) + negate_xw * (bz * ax)
        // (bw * az) + negate_yw * (bx * ay) + negate_zw * (by * ax) + negate_xw * (bz * aw)
        // (bw * aw) + negate_yw * (bx * ax) + negate_zw * (by * ay) + negate_xw * (bz * az)
        //
        // (  t0   )   negate_yw   (  t1   )
        // (              r0               )
        // (              r0               )   negate_zw   (  t2   )
        // (              r0               )   (       r1          )
        // (              r0               )   (       r1          )   negate_xw   (  t3   )
        // (              r0               )   (                  r2                       )
        // (                                    r2                                         )

        __m128 const t0 = _mm_mul_ps(b_wwww, a_xyzw);
        __m128 const t1 = _mm_mul_ps(b_xxxx, a_wzyx);
        __m128 const r0 = Vector4F_MultiplyAdd(t1, negate_yw, t0);

        __m128 const t2 = _mm_mul_ps(b_yyyy, a_zwxy);
        __m128 const r1 = _mm_mul_ps(negate_zw, t2);
        __m128 const t3 = _mm_mul_ps(b_zzzz, a_yxwz);
        __m128 const r2 = Vector4F_MultiplyAdd(t3, negate_xw, r1);
        __m128 const r3 = _mm_add_ps(r0, r2);

        return r3;
#elif ANEMONE_FEATURE_NEON
        float32x4_t const a_xyzw = a;
        float32x4_t const b_xyzw = b;

        static constexpr float fnegate_yw[4]{1.0f, -1.0f, 1.0f, -1.0f};
        float32x4_t const negate_yw = vld1q_f32(fnegate_yw);
        static constexpr float fnegate_zw[4]{1.0f, 1.0f, -1.0f, -1.0f};
        float32x4_t const negate_zw = vld1q_f32(fnegate_zw);
        static constexpr float fnegate_xw[4]{-1.0f, 1.0f, 1.0f, -1.0f};
        float32x4_t const negate_xw = vld1q_f32(fnegate_xw);

        float32x2_t const a_xy = vget_low_f32(a_xyzw);
        float32x2_t const a_zw = vget_high_f32(a_xyzw);
        float32x2_t const a_yx = vrev64_f32(a_xy);
        float32x2_t const a_wz = vrev64_f32(a_zw);

        float32x4_t const a_wzyx = vcombine_f32(a_wz, a_yx);
        float32x4_t const a_zwxy = vcombine_f32(a_zw, a_xy);
        float32x4_t const a_yxwz = vcombine_f32(a_yx, a_wz);

        // (bw * ax) + negate_yw * (bx * aw) + negate_zw * (by * az) + negate_xw * (bz * ay)
        // (bw * ay) + negate_yw * (bx * az) + negate_zw * (by * aw) + negate_xw * (bz * ax)
        // (bw * az) + negate_yw * (bx * ay) + negate_zw * (by * ax) + negate_xw * (bz * aw)
        // (bw * aw) + negate_yw * (bx * ax) + negate_zw * (by * ay) + negate_xw * (bz * az)
        //
        // (  t0   )   negate_yw   (  t1   )
        // (              r0               )
        // (              r0               )   negate_zw   (  t2   )
        // (              r0               )   (       r1          )
        // (              r0               )   (       r1          )   negate_xw   (  t3   )
        // (              r0               )   (                  r2                       )
        // (                                    r2                                         )

        float32x4_t const t0 = vmulq_laneq_f32(a_xyzw, b_xyzw, 3);
        float32x4_t const t1 = vmulq_laneq_f32(a_wzyx, b_xyzw, 0);
        float32x4_t const r0 = vmlaq_f32(t0, negate_yw, t1);

        float32x4_t const t2 = vmulq_laneq_f32(a_zwxy, b_xyzw, 1);
        float32x4_t const r1 = vmulq_f32(negate_zw, t2);
        float32x4_t const t3 = vmulq_laneq_f32(a_yxwz, b_xyzw, 2);
        float32x4_t const r2 = vmlaq_f32(r1, negate_xw, t3);
        float32x4_t const r3 = vaddq_f32(r0, r2);

        return r3;
#endif
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Conjugate(SimdVector4F q)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            -q.Inner[0],
            -q.Inner[1],
            -q.Inner[2],
            q.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const negate_xyz = _mm_setr_ps(-1.0f, -1.0f, -1.0f, 1.0f);
        return _mm_mul_ps(q, negate_xyz);
#elif ANEMONE_FEATURE_NEON
        static constexpr float fnegate_xyz[4]{-1.0f, -1.0f, -1.0f, 1.0f};
        return vmulq_f32(q, vld1q_f32(fnegate_xyz));
#endif
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Inverse(SimdVector4F q)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        SimdVector4F const lengthSquared = Vector4F_LengthSquared4(q);
        SimdVector4F const conj = QuaternionF_Conjugate(q);
        SimdMask4F const mask = Vector4F_CompareNearZero(lengthSquared);

        SimdVector4F result = Vector4F_Divide(conj, lengthSquared);
        result = Vector4F_Select(mask, result, Vector4F_Zero());
        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const lengthSquared = Vector4F_LengthSquared4(q);
        __m128 const conj = QuaternionF_Conjugate(q);
        __m128 const mask = _mm_cmple_ps(lengthSquared, _mm_load_ps(F32x4_Epsilon_XXXX.As<float>()));
        __m128 const result = _mm_div_ps(conj, lengthSquared);
        return _mm_and_ps(result, mask);
#elif ANEMONE_FEATURE_NEON
        float32x4_t const lengthSquared = Vector4F_LengthSquared4(q);
        float32x4_t const conj = QuaternionF_Conjugate(q);
        uint32x4_t const mask = vcleq_f32(lengthSquared, vld1q_f32(F32x4_Epsilon_XXXX.As<float>()));
        float32x4_t const result = vdivq_f32(conj, lengthSquared);
        return vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(result), mask));
#endif
    }

    inline void anemone_vectorcall QuaternionF_ToAxisAngle(SimdVector4F& axis, float& angle, SimdVector4F q)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        // Axis
        axis.Inner[0] = q.Inner[0];
        axis.Inner[1] = q.Inner[1];
        axis.Inner[2] = q.Inner[2];
        axis.Inner[3] = 0.0f;

        // Angle
        angle = 2.0f * Acos<float>(q.Inner[3]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // Axis part
        axis = q;

        // Angle part
        angle = 2.0f * Acos<float>(Vector4F_Extract<3>(q));
#elif ANEMONE_FEATURE_NEON
        // Axis part
        axis = q;

        // Angle part
        angle = 2.0f * Acos<float>(vgetq_lane_f32(q, 3));
#endif
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Nlerp(SimdVector4F from, SimdVector4F to, SimdVector4F t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return QuaternionF_Nlerp(from, to, t.Inner[0]);
#else
        SimdVector4F const r = Vector4F_PreciseLerp(from, to, t);
        return Vector4F_Normalize4(r);
#endif
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Nlerp(SimdVector4F from, SimdVector4F to, float t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        SimdVector4F const r = Vector4F_PreciseLerp(from, to, t);
        return Vector4F_Normalize4(r);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return QuaternionF_Nlerp(from, to, _mm_set1_ps(t));
#elif ANEMONE_FEATURE_NEON
        return QuaternionF_Nlerp(from, to, vdupq_n_f32(t));
#endif
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Slerp(SimdVector4F from, SimdVector4F to, SimdVector4F t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return QuaternionF_Slerp(from, to, t.Inner[0]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // = from * sin((1 - t) * theta) / sin(theta) + to * sin(t * theta) / sin(theta)
        __m128 const positive_one = _mm_load_ps(F32x4_PositiveOne.As<float>());
        __m128 const negative_one = _mm_load_ps(F32x4_NegativeOne.As<float>());
        __m128 const zero = _mm_setzero_ps();

        // cos(theta) = dot(from, to)
        __m128 cos_theta = Vector4F_Dot4(from, to);
        // = [cos(theta) < 0].xxxx
        __m128 const mask = _mm_cmplt_ps(cos_theta, zero);
        // = [cos(theta) < 0 ? -1 : 1]
        __m128 const sign = _mm_blendv_ps(positive_one, negative_one, mask);

        // update sign of cos(theta)
        cos_theta = _mm_mul_ps(cos_theta, sign);

        // sin(theta) = sqrt(1 - cos(theta)^2)
        __m128 sin_theta = _mm_mul_ps(cos_theta, cos_theta);
        sin_theta = _mm_sub_ps(positive_one, sin_theta);
        sin_theta = _mm_sqrt_ps(sin_theta);

        // theta = atan2(sin(theta), cos(theta))
        __m128 const theta = Vector4F_Atan2(sin_theta, cos_theta);

        __m128 const negate_x = _mm_load_ps(F32x4_SignMask_Xnnn.As<float>());

        // = [-t, t, t, t]
        __m128 coefficient_linear = _mm_xor_ps(t, negate_x);
        // = [1-t, t, t, t]
        coefficient_linear = _mm_add_ps(coefficient_linear, _mm_load_ps(F32x4_PositiveUnitX.As<float>()));

        // = [
        //     sin((1 - t) * theta) / sin(theta),
        //     sin(t * theta) / sin(theta),
        // ]
        __m128 coefficient_spherical = _mm_mul_ps(coefficient_linear, theta);
        coefficient_spherical = Vector4F_Sin(coefficient_spherical);
        coefficient_spherical = _mm_div_ps(coefficient_spherical, sin_theta);

        // = [1 - eps].xxxx
        __m128 const one_minus_epsilon = _mm_set1_ps(1.0f - std::numeric_limits<float>::epsilon());
        // = [cos(theta) < (1 - eps)].xxxx
        __m128 const close_to_one = _mm_cmpge_ps(cos_theta, one_minus_epsilon);

        // = close_to_one ? linear : spherical
        __m128 const coefficient_result = _mm_blendv_ps(coefficient_spherical, coefficient_linear, close_to_one);
        __m128 const coefficient_from = _mm_permute_ps(coefficient_result, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 const coefficient_to = _mm_permute_ps(coefficient_result, _MM_SHUFFLE(1, 1, 1, 1));

        // interpolate
        __m128 r = _mm_mul_ps(to, coefficient_to);
        r = Vector4F_MultiplyAdd(from, coefficient_from, r);
        return r;
#elif ANEMONE_FEATURE_NEON
        // = from * sin((1 - t) * theta) / sin(theta) + to * sin(t * theta) / sin(theta)
        float32x4_t const positive_one = vld1q_f32(F32x4_PositiveOne.As<float>());
        float32x4_t const negative_one = vld1q_f32(F32x4_NegativeOne.As<float>());
        float32x4_t const zero = vdupq_n_f32(0.0f);

        // cos(theta) = dot(from, to)
        float32x4_t cos_theta = Vector4F_Dot4(from, to);
        // = [cos(theta) < 0].xxxx
        uint32x4_t const mask = vcltq_f32(cos_theta, zero);
        // = [cos(theta) < 0 ? -1 : 1]
        float32x4_t const sign = vbslq_f32(mask, positive_one, negative_one);

        // update sign of cos(theta)
        cos_theta = vmulq_f32(cos_theta, sign);

        // sin(theta) = sqrt(1 - cos(theta)^2)
        float32x4_t sin_theta = vmulq_f32(cos_theta, cos_theta);
        sin_theta = vsubq_f32(positive_one, sin_theta);
        sin_theta = vsqrtq_f32(sin_theta);

        // theta = atan2(sin(theta), cos(theta))
        float32x4_t const theta = Vector4F_Atan2(sin_theta, cos_theta);

        float32x4_t const negate_x = vld1q_f32(F32x4_Negate_Xnnn.As<float>());

        // = [-t, t, t, t]
        float32x4_t coefficient_linear = vmulq_f32(t, negate_x);
        // = [1-t, t, t, t]
        coefficient_linear = vaddq_f32(coefficient_linear, vld1q_f32(F32x4_PositiveUnitX.As<float>()));

        // = [
        //     sin((1 - t) * theta) / sin(theta),
        //     sin(t * theta) / sin(theta),
        // ]
        float32x4_t coefficient_spherical = vmulq_f32(coefficient_linear, theta);
        coefficient_spherical = Vector4F_Sin(coefficient_spherical);
        coefficient_spherical = vdivq_f32(coefficient_spherical, sin_theta);

        // = [1 - eps].xxxx
        float32x4_t const one_minus_epsilon = vdupq_n_f32(1.0f - std::numeric_limits<float>::epsilon());
        // = [cos(theta) < (1 - eps)].xxxx
        uint32x4_t const close_to_one = vcgeq_f32(cos_theta, one_minus_epsilon);

        // = close_to_one ? linear : spherical
        float32x4_t const coefficient = vbslq_f32(close_to_one, coefficient_linear, coefficient_spherical);

        // interpolate
        float32x4_t r = vmulq_laneq_f32(to, coefficient, 1);
        r = vmlaq_laneq_f32(r, from, coefficient, 0);
        return r;
#endif
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Slerp(SimdVector4F from, SimdVector4F to, float t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        // = from * sin((1 - t) * theta) / sin(theta) + to * sin(t * theta) / sin(theta)
        static constexpr float one_minus_epsilon = 1.0f - std::numeric_limits<float>::epsilon();

        float cos_theta = Vector4F_Dot4(from, to).Inner[0];

        if (cos_theta < 0.0f)
        {
            cos_theta = -cos_theta;
        }

        float const sin_theta = Sqrt<float>(1.0f - (cos_theta * cos_theta));
        float const theta = Atan2<float>(sin_theta, cos_theta);

        float const linear_from = 1.0f - t;
        float const linear_to = t;

        float const spherical_from = Sin<float>(linear_from * theta) / sin_theta;
        float const spherical_to = Sin<float>(linear_to * theta) / sin_theta;

        bool const use_spherical = (cos_theta < one_minus_epsilon);

        float const coefficient_from = use_spherical ? spherical_from : linear_from;
        float const coefficient_to = use_spherical ? spherical_to : linear_to;

        SimdVector4F const v_coefficient_from = Vector4F_Replicate(coefficient_from);
        SimdVector4F const v_coefficient_to = Vector4F_Replicate(coefficient_to);

        // interpolate
        SimdVector4F r = Vector4F_Multiply(to, v_coefficient_to);
        r = Vector4F_MultiplyAdd(from, v_coefficient_from, r);
        return r;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return QuaternionF_Slerp(from, to, _mm_set1_ps(t));
#elif ANEMONE_FEATURE_NEON
        return QuaternionF_Slerp(from, to, vdupq_n_f32(t));
#endif
    }

    // SimdVector4F anemone_vectorcall QuaternionF_Barycentric(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F u, SimdVector4F v);
    // SimdVector4F anemone_vectorcall QuaternionF_Barycentric(SimdVector4F a, SimdVector4F b, SimdVector4F c, float u, float v);

    inline SimdMask4F anemone_vectorcall QuaternionF_CompareEqual(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_CompareEqual(a, b);
    }

    inline bool anemone_vectorcall QuaternionF_IsEqual(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_IsEqual4(a, b);
    }

    inline SimdMask4F anemone_vectorcall QuaternionF_CompareNotEqual(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_CompareNotEqual(a, b);
    }

    inline bool anemone_vectorcall QuaternionF_IsNotEqual(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_IsNotEqual4(a, b);
    }

    inline SimdMask4F anemone_vectorcall QuaternionF_CompareNaN(SimdVector4F q)
    {
        return Vector4F_CompareNaN(q);
    }

    inline bool anemone_vectorcall QuaternionF_IsNaN(SimdVector4F q)
    {
        return Vector4F_IsNaN4(q);
    }

    inline SimdMask4F anemone_vectorcall QuaternionF_CompareInfinite(SimdVector4F q)
    {
        return Vector4F_CompareInfinite(q);
    }

    inline bool anemone_vectorcall QuaternionF_IsInfinite(SimdVector4F q)
    {
        return Vector4F_IsInfinite4(q);
    }

    inline SimdMask4F anemone_vectorcall QuaternionF_CompareIdentity(SimdVector4F q)
    {
        return Vector4F_CompareEqual(q, QuaternionF_Identity());
    }

    inline bool anemone_vectorcall QuaternionF_IsIdentity(SimdVector4F q)
    {
        return Vector4F_IsEqual4(q, QuaternionF_Identity());
    }

}

// RotorF
namespace Anemone::Math::Detail
{
    inline SimdVector4F anemone_vectorcall RotorF_Identity()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4F>(F32x4_PositiveUnitW);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm_load_ps(F32x4_PositiveUnitW.As<float>());
#elif ANEMONE_FEATURE_NEON
        return vld1q_f32(F32x4_PositiveUnitW.As<float>());
#endif
    }

    inline SimdVector4F anemone_vectorcall RotorF_Create(float xy, float xz, float yz, float scalar)
    {
        return Vector4F_Create(xy, xz, yz, scalar);
    }

    inline SimdVector4F anemone_vectorcall RotorF_Create(SimdVector4F bv, float scalar)
    {
        return Vector4F_Insert<3>(bv, scalar);
    }

    inline SimdVector4F anemone_vectorcall RotorF_CreateFromAxisAngle(SimdVector4F axis, float angle)
    {
        SimdVector4F const normal = Vector4F_Normalize3(axis);
        return RotorF_CreateFromNormalAngle(normal, angle);
    }

    inline SimdVector4F anemone_vectorcall RotorF_CreateFromNormalAngle(SimdVector4F normal, float angle)
    {
        auto [fSin, fCos] = SinCos<float>(angle * 0.5f);

#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            -fSin * normal.Inner[0],
            -fSin * normal.Inner[1],
            -fSin * normal.Inner[2],
            fCos,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // = [-sin, -sin, -sin, -sin]
        __m128 const vSin = _mm_set1_ps(-fSin);
        // = sin.xyz * normal.xyz
        __m128 const r = _mm_mul_ps(vSin, normal);
        return _mm_insert_ps(r, _mm_set_ss(fCos), 3 << 4);
#elif ANEMONE_FEATURE_NEON
        // = [-sin, -sin, -sin, -sin]
        float32x4_t r = vdupq_n_f32(-fSin);
        r = vmulq_f32(r, normal);
        r = vsetq_lane_f32(fCos, r, 3);
        return r;
#endif
    }

    inline SimdVector4F anemone_vectorcall RotorF_RotationBetween(SimdVector4F from, SimdVector4F to)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        SimdVector4F const dot = Vector4F_Dot3(to, from);
        SimdVector4F result = Vector4F_Wedge3(to, from);

        result.Inner[3] = 1.0f + dot.Inner[3];
        return RotorF_Normalize(result);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // = 1 + dot3(to, from)
        __m128 const t0 = Vector4F_Dot3(to, from);
        __m128 const t1 = _mm_add_ps(_mm_load_ps(F32x4_PositiveOne.As<float>()), t0);

        // = wedge(to, from)
        __m128 const r0 = Vector4F_Wedge3(to, from);
        // = [r0.xyz, t1.w]
        __m128 const r1 = _mm_blend_ps(r0, t1, 0b1000);
        return RotorF_Normalize(r1);
#elif ANEMONE_FEATURE_NEON
        // = 1 + dot3(to, from)
        float32x2_t const t0 = NeonDot3(to, from);
        float32x2_t const t1 = vadd_f32(vdup_n_f32(1.0f), t0);

        float32x4_t const r0 = Vector4F_Wedge3(to, from);
        float32x4_t const r1 = vcopyq_lane_f32(r0, 3, t1, 0);
        return RotorF_Normalize(r1);
#endif
    }

    inline SimdVector4F anemone_vectorcall RotorF_Rotate3(SimdVector4F rotor, SimdVector4F v)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const vx = v.Inner[0];
        float const vy = v.Inner[1];
        float const vz = v.Inner[2];

        float const rx = rotor.Inner[0];
        float const ry = rotor.Inner[1];
        float const rz = rotor.Inner[2];
        float const rw = rotor.Inner[3];

        float const qx = rw * vx + vy * rx + vz * ry;
        float const qy = rw * vy - vx * rx + vz * rz;
        float const qz = rw * vz - vx * ry - vy * rz;
        float const qw = rz * vx - vy * ry + vz * rx;

        SimdVector4F result;
        result.Inner[0] = rw * qx + qy * rx + qz * ry + qw * rz;
        result.Inner[1] = rw * qy - qx * rx - qw * ry + qz * rz;
        result.Inner[2] = rw * qz + qw * rx - qx * ry - qy * rz;
        result.Inner[3] = 0.0f;
        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const r_wwwz = _mm_permute_ps(rotor, _MM_SHUFFLE(2, 3, 3, 3));
        __m128 const r_xxyy = _mm_permute_ps(rotor, _MM_SHUFFLE(1, 1, 0, 0));
        __m128 const r_yzzx = _mm_permute_ps(rotor, _MM_SHUFFLE(0, 2, 2, 1));

        __m128 const v_xyzx = _mm_permute_ps(v, _MM_SHUFFLE(0, 2, 1, 0));
        __m128 const v_yxxy = _mm_permute_ps(v, _MM_SHUFFLE(1, 0, 0, 1));
        __m128 const v_zzyz = _mm_permute_ps(v, _MM_SHUFFLE(2, 1, 2, 2));

        // = [1, -1, -1, -1]
        __m128 const negate_yzw = _mm_setr_ps(1.0f, -1.0f, -1.0f, -1.0f);
        // = [1, 1, -1, 1]
        __m128 const negate_z = _mm_setr_ps(1.0f, 1.0f, -1.0f, 1.0f);
        // = [1, -1, 1, 1]
        __m128 const negate_y = _mm_setr_ps(1.0f, -1.0f, 1.0f, 1.0f);
        // = [1, -1, -1, 1]
        __m128 const negate_yz = _mm_setr_ps(1.0f, -1.0f, -1.0f, 1.0f);

        // qx = ((rw * vx) +  1.0f * (vy * rx)) +  1.0f * (vz * ry);
        // qy = ((rw * vy) + -1.0f * (vx * rx)) +  1.0f * (vz * rz);
        // qz = ((rw * vz) + -1.0f * (vx * ry)) + -1.0f * (vy * rz);
        // qw = ((rz * vx) + -1.0f * (vy * ry)) +  1.0f * (vz * rx);

        //    = ((t1 * negate_yzw) + t0) + ((t3 * negate_yz) + (t2 * negate_z))
        __m128 const t0 = _mm_mul_ps(r_wwwz, v_xyzx);
        __m128 const t1 = _mm_mul_ps(r_xxyy, v_yxxy);
        __m128 const r0 = Vector4F_MultiplyAdd(t1, negate_yzw, t0);

        __m128 const t2 = _mm_mul_ps(r_yzzx, v_zzyz);
        __m128 const q_xyzw = Vector4F_MultiplyAdd(negate_z, t2, r0);

        __m128 const r_wwww = _mm_permute_ps(rotor, _MM_SHUFFLE(3, 3, 3, 3));
        __m128 const r_zzzz = _mm_permute_ps(rotor, _MM_SHUFFLE(2, 2, 2, 2));
        __m128 const r_yyyy = _mm_permute_ps(rotor, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 const r_xxxx = _mm_permute_ps(rotor, _MM_SHUFFLE(0, 0, 0, 0));

        // x = ((rw * qx) +  1.0f * (qy * rx)) +  1.0f * (qz * ry) + ( 1.0f * (qw * rz));
        // y = ((rw * qy) + -1.0f * (qx * rx)) + -1.0f * (qw * ry) + ( 1.0f * (qz * rz));
        // z = ((rw * qz) +  1.0f * (qw * rx)) + -1.0f * (qx * ry) + (-1.0f * (qy * rz));
        // w = 0
        __m128 const q_yxww = _mm_permute_ps(q_xyzw, _MM_SHUFFLE(3, 3, 0, 1));
        __m128 const q_zwxx = _mm_permute_ps(q_xyzw, _MM_SHUFFLE(0, 0, 3, 2));
        __m128 const q_wzyy = _mm_permute_ps(q_xyzw, _MM_SHUFFLE(1, 1, 2, 3));

        __m128 const s0 = _mm_mul_ps(r_wwww, q_xyzw);
        __m128 const s1 = _mm_mul_ps(r_xxxx, q_yxww);
        __m128 const w0 = Vector4F_MultiplyAdd(s1, negate_y, s0);
        __m128 const s2 = _mm_mul_ps(r_yyyy, q_zwxx);
        __m128 const s3 = _mm_mul_ps(r_zzzz, q_wzyy);
        __m128 const s4 = _mm_mul_ps(negate_z, s3);
        __m128 const w1 = Vector4F_MultiplyAdd(s2, negate_yz, s4);

        __m128 const result = _mm_add_ps(w0, w1);
        return _mm_and_ps(result, _mm_load_ps(F32x4_SelectMask_XXXn.As<float>()));
#elif ANEMONE_FEATURE_NEON
        float32x2_t const r_xy = vget_low_f32(rotor);
        float32x2_t const r_zw = vget_high_f32(rotor);

        float32x2_t const r_ww = vdup_lane_f32(r_zw, 1);
        float32x2_t const r_xx = vdup_lane_f32(r_xy, 0);
        float32x2_t const r_yy = vdup_lane_f32(r_xy, 1);
        float32x2_t const r_yz = vext_f32(r_xy, r_zw, 1);
        float32x2_t const r_zx = vcopy_lane_f32(r_xx, 0, r_zw, 0);
        float32x2_t const r_wz = vrev64_f32(r_zw);

        float32x4_t const r_wwwz = vcombine_f32(r_ww, r_wz);
        float32x4_t const r_xxyy = vcombine_f32(r_xx, r_yy);
        float32x4_t const r_yzzx = vcombine_f32(r_yz, r_zx);

        float32x2_t const v_xy = vget_low_f32(v);
        float32x2_t const v_zw = vget_high_f32(v);

        float32x2_t const v_yx = vrev64_f32(v_xy);
        float32x2_t const v_zz = vdup_lane_f32(v_zw, 0);
        float32x2_t const v_yz = vext_f32(v_xy, v_zw, 1);
        float32x2_t const v_zx = vext_f32(v_zz, v_xy, 1);

        float32x4_t const v_xyzx = vcombine_f32(v_xy, v_zx);
        float32x4_t const v_yxxy = vcombine_f32(v_yx, v_xy);
        float32x4_t const v_zzyz = vcombine_f32(v_zz, v_yz);

        // = [1, -1, -1, -1]
        static constexpr float fnegate_yzw[4]{1.0f, -1.0f, -1.0f, -1.0f};
        float32x4_t const negate_yzw = vld1q_f32(fnegate_yzw);
        // = [1, 1, -1, 1]
        static constexpr float fnegate_z[4]{1.0f, 1.0f, -1.0f, 1.0f};
        float32x4_t const negate_z = vld1q_f32(fnegate_z);
        // = [1, -1, 1, 1]
        static constexpr float fnegate_y[4]{1.0f, -1.0f, 1.0f, 1.0f};
        float32x4_t const negate_y = vld1q_f32(fnegate_y);
        // = [1, -1, -1, 1]
        static constexpr float fnegate_yz[4]{1.0f, -1.0f, -1.0f, 1.0f};
        float32x4_t const negate_yz = vld1q_f32(fnegate_yz);

        // qx = ((rw * vx) +  1.0f * (vy * rx)) +  1.0f * (vz * ry);
        // qy = ((rw * vy) + -1.0f * (vx * rx)) +  1.0f * (vz * rz);
        // qz = ((rw * vz) + -1.0f * (vx * ry)) + -1.0f * (vy * rz);
        // qw = ((rz * vx) + -1.0f * (vy * ry)) +  1.0f * (vz * rx);

        //    = ((t1 * negate_yzw) + t0) + ((t3 * negate_yz) + (t2 * negate_z))
        float32x4_t const t0 = vmulq_f32(r_wwwz, v_xyzx);
        float32x4_t const t1 = vmulq_f32(r_xxyy, v_yxxy);
        float32x4_t const r0 = vmlaq_f32(t0, negate_yzw, t1);

        float32x4_t const t2 = vmulq_f32(r_yzzx, v_zzyz);
        float32x4_t const q_xyzw = vmlaq_f32(r0, negate_z, t2);

        // x = ((rw * qx) +  1.0f * (qy * rx)) +  1.0f * (qz * ry) + ( 1.0f * (qw * rz));
        // y = ((rw * qy) + -1.0f * (qx * rx)) + -1.0f * (qw * ry) + ( 1.0f * (qz * rz));
        // z = ((rw * qz) +  1.0f * (qw * rx)) + -1.0f * (qx * ry) + (-1.0f * (qy * rz));
        // w = 0

        float32x2_t const q_xy = vget_low_f32(q_xyzw);
        float32x2_t const q_zw = vget_high_f32(q_xyzw);
        float32x2_t const q_yx = vrev64_f32(q_xy);
        float32x2_t const q_wz = vrev64_f32(q_zw);
        float32x2_t const q_ww = vdup_lane_f32(q_zw, 1);
        float32x2_t const q_xx = vdup_lane_f32(q_xy, 0);
        float32x2_t const q_yy = vdup_lane_f32(q_xy, 1);

        float32x4_t const q_yxww = vcombine_f32(q_yx, q_ww);
        float32x4_t const q_zwxx = vcombine_f32(q_zw, q_xx);
        float32x4_t const q_wzyy = vcombine_f32(q_wz, q_yy);

        float32x4_t const s0 = vmulq_laneq_f32(q_xyzw, rotor, 3);
        float32x4_t const s1 = vmulq_laneq_f32(q_yxww, rotor, 0);
        float32x4_t const w0 = vmlaq_f32(s0, negate_y, s1);
        float32x4_t const s2 = vmulq_laneq_f32(q_zwxx, rotor, 1);
        float32x4_t const s3 = vmulq_laneq_f32(q_wzyy, rotor, 2);
        float32x4_t const s4 = vmulq_f32(negate_z, s3);
        float32x4_t const w1 = vmlaq_f32(s4, negate_yz, s2);

        float32x4_t const result = vaddq_f32(w0, w1);
        return vreinterpretq_f32_u32(
            vandq_u32(
                vreinterpretq_u32_f32(result),
                vld1q_u32(F32x4_SelectMask_XXXn.As<uint32_t>())));
#endif
    }

    inline SimdVector4F anemone_vectorcall RotorF_InverseRotate3(SimdVector4F rotor, SimdVector4F v)
    {
        return RotorF_Rotate3(RotorF_Reverse(rotor), v);
    }

    inline SimdVector4F anemone_vectorcall RotorF_Dot(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_Dot4(a, b);
    }

    inline SimdVector4F anemone_vectorcall RotorF_Multiply(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const ax = a.Inner[0];
        float const ay = a.Inner[1];
        float const az = a.Inner[2];
        float const aw = a.Inner[3];

        float const bx = b.Inner[0];
        float const by = b.Inner[1];
        float const bz = b.Inner[2];
        float const bw = b.Inner[3];

        SimdVector4F result;
        result.Inner[0] = ax * bw + aw * bx + az * by - ay * bz;
        result.Inner[1] = ay * bw + aw * by - az * bx + ax * bz;
        result.Inner[2] = az * bw + aw * bz + ay * bx - ax * by;
        result.Inner[3] = aw * bw - ax * bx - ay * by - az * bz;
        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // rx = (ax * bw) + (negate_w * aw * bx) + (negate_yw * az * by) + (negate_xzw * ay * bz);
        // ry = (ay * bw) + (negate_w * aw * by) + (negate_yw * az * bx) + (negate_xzw * ax * bz);
        // rz = (az * bw) + (negate_w * aw * bz) + (negate_yw * ay * bx) + (negate_xzw * ax * by);
        // rw = (aw * bw) + (negate_w * ax * bx) + (negate_yw * ay * by) + (negate_xzw * az * bz);

        __m128 const a_xyzw = a;
        __m128 const b_wwww = _mm_permute_ps(b, _MM_SHUFFLE(3, 3, 3, 3));
        __m128 const a_wwwx = _mm_permute_ps(a, _MM_SHUFFLE(0, 3, 3, 3));
        __m128 const b_xyzx = _mm_permute_ps(b, _MM_SHUFFLE(0, 2, 1, 0));
        __m128 const a_zzyy = _mm_permute_ps(a, _MM_SHUFFLE(1, 1, 2, 2));
        __m128 const b_yxxy = _mm_permute_ps(b_xyzx, _MM_SHUFFLE(1, 0, 0, 1));
        __m128 const a_yxxz = _mm_permute_ps(a, _MM_SHUFFLE(2, 0, 0, 1));
        __m128 const b_zzyz = _mm_permute_ps(b_xyzx, _MM_SHUFFLE(2, 2, 1, 2));

        __m128 const negate_w = _mm_setr_ps(1.0f, 1.0f, 1.0f, -1.0f);
        __m128 const negate_yw = _mm_setr_ps(1.0f, -1.0f, 1.0f, -1.0f);
        __m128 const negate_xzw = _mm_setr_ps(-1.0f, 1.0f, -1.0f, -1.0f);

        // = ((t1 * negate_w) + t0) + ((t3 * negate_xzw) + (t2 * negate_yw))
        __m128 const t0 = _mm_mul_ps(a_xyzw, b_wwww);
        __m128 const t1 = _mm_mul_ps(a_wwwx, b_xyzx);
        __m128 const r0 = Vector4F_MultiplyAdd(t1, negate_w, t0);

        __m128 const t2 = _mm_mul_ps(a_zzyy, b_yxxy);
        __m128 const r1 = _mm_mul_ps(negate_yw, t2);
        __m128 const t3 = _mm_mul_ps(a_yxxz, b_zzyz);
        __m128 const r2 = Vector4F_MultiplyAdd(t3, negate_xzw, r1);
        __m128 const r3 = _mm_add_ps(r0, r2);

        return r3;
#elif ANEMONE_FEATURE_NEON
        // rx = (ax * bw) + (negate_w * aw * bx) + (negate_yw * az * by) + (negate_xzw * ay * bz);
        // ry = (ay * bw) + (negate_w * aw * by) + (negate_yw * az * bx) + (negate_xzw * ax * bz);
        // rz = (az * bw) + (negate_w * aw * bz) + (negate_yw * ay * bx) + (negate_xzw * ax * by);
        // rw = (aw * bw) + (negate_w * ax * bx) + (negate_yw * ay * by) + (negate_xzw * az * bz);

        float32x2_t const a_xy = vget_low_f32(a);
        float32x2_t const a_yx = vrev64_f32(a_xy);
        float32x2_t const a_zw = vget_high_f32(a);
        float32x2_t const a_wx = vext_f32(a_zw, a_xy, 1);
        float32x2_t const a_xz = vext_f32(a_yx, a_zw, 1);
        float32x2_t const a_zz = vdup_lane_f32(a_zw, 0);
        float32x2_t const a_ww = vdup_lane_f32(a_zw, 1);
        float32x2_t const a_yy = vdup_lane_f32(a_xy, 1);

        float32x4_t const a_wwwx = vcombine_f32(a_ww, a_wx);
        float32x4_t const a_zzyy = vcombine_f32(a_zz, a_yy);
        float32x4_t const a_yxxz = vcombine_f32(a_yx, a_xz);

        float32x2_t const b_xy = vget_low_f32(b);
        float32x2_t const b_zw = vget_high_f32(b);
        float32x2_t const b_yx = vrev64_f32(b_xy);
        float32x2_t const b_yz = vext_f32(b_xy, b_zw, 1);
        float32x2_t const b_zz = vdup_lane_f32(b_zw, 0);
        float32x2_t const b_zx = vext_f32(b_yz, b_xy, 1);

        float32x4_t const b_xyzx = vcombine_f32(b_xy, b_zx);
        float32x4_t const b_yxxy = vcombine_f32(b_yx, b_xy);
        float32x4_t const b_zzyz = vcombine_f32(b_zz, b_yz);

        static constexpr float fnegate_w[4]{1.0f, 1.0f, 1.0f, -1.0f};
        float32x4_t const negate_w = vld1q_f32(fnegate_w);
        static constexpr float fnegate_yw[4]{1.0f, -1.0f, 1.0f, -1.0f};
        float32x4_t const negate_yw = vld1q_f32(fnegate_yw);
        static constexpr float fnegate_xzw[4]{-1.0f, 1.0f, -1.0f, -1.0f};
        float32x4_t const negate_xzw = vld1q_f32(fnegate_xzw);

        // = ((t1 * negate_w) + t0) + ((t3 * negate_xzw) + (t2 * negate_yw))
        // = a.xyzw * b.wwww
        float32x4_t const t0 = vmulq_laneq_f32(a, b, 3);
        float32x4_t const t1 = vmulq_f32(a_wwwx, b_xyzx);
        float32x4_t const r0 = vmlaq_f32(t0, negate_w, t1);

        float32x4_t const t2 = vmulq_f32(a_zzyy, b_yxxy);
        float32x4_t const r1 = vmulq_f32(negate_yw, t2);
        float32x4_t const t3 = vmulq_f32(a_yxxz, b_zzyz);
        float32x4_t const r2 = vmlaq_f32(r1, negate_xzw, t3);
        float32x4_t const r3 = vaddq_f32(r0, r2);

        return r3;
#endif
    }

    inline SimdVector4F anemone_vectorcall RotorF_RotateRotor(SimdVector4F rotor, SimdVector4F rotation)
    {
        SimdVector4F const t = RotorF_Multiply(rotor, rotation);
        return RotorF_Multiply(t, RotorF_Reverse(rotor));
    }

    inline SimdVector4F anemone_vectorcall RotorF_Reverse(SimdVector4F rotor)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{-rotor.Inner[0], -rotor.Inner[1], -rotor.Inner[2], rotor.Inner[3]};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const negate_xyz = _mm_setr_ps(-1.0f, -1.0f, -1.0f, 1.0f);
        return _mm_mul_ps(rotor, negate_xyz);
#elif ANEMONE_FEATURE_NEON
        static constexpr float fnegate_xyz[4]{-1.0f, -1.0f, -1.0f, 1.0f};
        return vmulq_f32(rotor, vld1q_f32(fnegate_xyz));
#endif
    }

    inline SimdVector4F anemone_vectorcall RotorF_Length(SimdVector4F rotor)
    {
        return Vector4F_Length4(rotor);
    }

    inline SimdVector4F anemone_vectorcall RotorF_LengthSquared(SimdVector4F rotor)
    {
        return Vector4F_LengthSquared4(rotor);
    }

    inline SimdVector4F anemone_vectorcall RotorF_Normalize(SimdVector4F rotor)
    {
        return Vector4F_Normalize4(rotor);
    }

    inline SimdMatrix4x4F anemone_vectorcall RotorF_ToMatrix4x4F(SimdVector4F rotor)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const rx = rotor.Inner[0];
        float const ry = rotor.Inner[1];
        float const rz = rotor.Inner[2];
        float const rw = rotor.Inner[3];

        float const rxx = rx * rx;
        float const ryy = ry * ry;
        float const rzz = rz * rz;
        float const rww = rw * rw;

        float const rxy = rx * ry;
        float const rxw = rx * rw;
        float const rxz = rx * rz;
        float const ryz = ry * rz;
        float const ryw = ry * rw;
        float const rzw = rz * rw;

        float const r2xw = 2.0f * rxw;
        float const r2yw = 2.0f * ryw;
        float const r2zw = 2.0f * rzw;

        float const r2yz = 2.0f * ryz;
        float const r2xz = 2.0f * rxz;
        float const r2xy = 2.0f * rxy;

        float const m11 = rww - rxx - ryy + rzz;
        float const m22 = rww - rxx + ryy - rzz;
        float const m33 = rww + rxx - ryy - rzz;

        float const m12 = -r2xw - r2yz;
        float const m13 = -r2yw + r2xz;
        float const m23 = -r2zw - r2xy;

        float const m21 = r2xw - r2yz;
        float const m31 = r2yw + r2xz;
        float const m32 = r2zw - r2xy;

        SimdMatrix4x4F result;
        result.Inner[0].Inner[0] = m11;
        result.Inner[0].Inner[1] = m12;
        result.Inner[0].Inner[2] = m13;
        result.Inner[0].Inner[3] = 0.0f;

        result.Inner[1].Inner[0] = m21;
        result.Inner[1].Inner[1] = m22;
        result.Inner[1].Inner[2] = m23;
        result.Inner[1].Inner[3] = 0.0f;

        result.Inner[2].Inner[0] = m31;
        result.Inner[2].Inner[1] = m32;
        result.Inner[2].Inner[2] = m33;
        result.Inner[2].Inner[3] = 0.0f;

        result.Inner[3].Inner[0] = 0.0f;
        result.Inner[3].Inner[1] = 0.0f;
        result.Inner[3].Inner[2] = 0.0f;
        result.Inner[3].Inner[3] = 1.0f;

        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const r_wwww = _mm_permute_ps(rotor, _MM_SHUFFLE(3, 3, 3, 3));

        // = [xw, yw, zw, ww]
        __m128 const r_xw_yw_zw_ww = _mm_mul_ps(rotor, r_wwww);
        // = [2xw, 2yw, 2zw, 2ww]
        __m128 const r_2xw_2yw_2zw_2ww = _mm_add_ps(r_xw_yw_zw_ww, r_xw_yw_zw_ww);

        // = [y, x, x, x]
        __m128 const r_yxxx = _mm_permute_ps(rotor, _MM_SHUFFLE(0, 0, 0, 1));
        // = [z, z, y, y]
        __m128 const r_zzyy = _mm_permute_ps(rotor, _MM_SHUFFLE(1, 1, 2, 2));
        // = [yz, xz, xy, xy]
        __m128 const r_yz_xz_xy_xy = _mm_mul_ps(r_yxxx, r_zzyy);
        // = [2yz, 2xz, 2xy, 2xy]
        __m128 const r_2yz_2xz_2xy_2xy = _mm_add_ps(r_yz_xz_xy_xy, r_yz_xz_xy_xy);

        // = [-1, -1, 1, 1]
        __m128 const negate_xy = _mm_setr_ps(-1.0f, -1.0f, 1.0f, 1.0f);
        // = [-1, 1, -1, -1]
        __m128 const negate_xz = _mm_setr_ps(-1.0f, 1.0f, -1.0f, -1.0f);
        // = [1, -1, -1, 1]
        __m128 const negate_yz = _mm_setr_ps(1.0f, -1.0f, -1.0f, 1.0f);

        // m11 = (rww + -1.0f * rxx) + (-1.0f * ryy + ( 1.0f * rzz));
        // m22 = (rww + -1.0f * rxx) + ( 1.0f * ryy + (-1.0f * rzz));
        // m33 = (rww +  1.0f * rxx) + (-1.0f * ryy + (-1.0f * rzz));

        __m128 const r_xx_yy_zz_ww = _mm_mul_ps(rotor, rotor);
        __m128 const r_xx_xx_xx_xx = _mm_permute_ps(r_xx_yy_zz_ww, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 const r_yy_yy_yy_yy = _mm_permute_ps(r_xx_yy_zz_ww, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 const r_zz_zz_zz_zz = _mm_permute_ps(r_xx_yy_zz_ww, _MM_SHUFFLE(2, 2, 2, 2));
        __m128 const r_ww_ww_ww_ww = _mm_permute_ps(r_xx_yy_zz_ww, _MM_SHUFFLE(3, 3, 3, 3));

        __m128 t0 = _mm_mul_ps(negate_yz, r_zz_zz_zz_zz);
        __m128 t1 = Vector4F_MultiplyAdd(negate_xz, r_yy_yy_yy_yy, t0);
        __m128 t2 = Vector4F_MultiplyAdd(negate_xy, r_xx_xx_xx_xx, r_ww_ww_ww_ww);
        __m128 const m11_m22_m33 = _mm_add_ps(t1, t2);

        t0 = _mm_mul_ps(negate_xz, r_2yz_2xz_2xy_2xy);

        // float const m12 = -1.0f * r2yz - r2xw;
        // float const m13 =  1.0f * r2xz - r2yw;
        // float const m23 = -1.0f * r2xy - r2zw;
        __m128 const m12_m13_m23 = _mm_sub_ps(t0, r_2xw_2yw_2zw_2ww);

        // float const m21 = -1.0f * r2yz + r2xw;
        // float const m31 =  1.0f * r2xz + r2yw;
        // float const m32 = -1.0f * r2xy + r2zw;
        __m128 const m21_m31_m32 = _mm_add_ps(t0, r_2xw_2yw_2zw_2ww);


        // = [m11, m11, m12, m13]
        t0 = _mm_shuffle_ps(m11_m22_m33, m12_m13_m23, _MM_SHUFFLE(1, 0, 0, 0));
        __m128 const m11_m12_m13 = _mm_permute_ps(t0, _MM_SHUFFLE(3, 3, 2, 0));

        // = [m21, m22, m33, ---]
        t0 = _mm_move_ss(m11_m22_m33, m21_m31_m32);
        __m128 const m21_m22_m23 = _mm_shuffle_ps(t0, m12_m13_m23, _MM_SHUFFLE(2, 2, 1, 0));

        // = [m31, m32, m33, m33]
        __m128 const m31_m32_m33 = _mm_shuffle_ps(m21_m31_m32, m11_m22_m33, _MM_SHUFFLE(2, 2, 2, 1));

        // = [true, true, true, false]
        __m128 const mask_select_xyz = _mm_load_ps(F32x4_SelectMask_XXXn.As<float>());

        SimdMatrix4x4F result;
        result.Inner[0] = _mm_and_ps(m11_m12_m13, mask_select_xyz);
        result.Inner[1] = _mm_and_ps(m21_m22_m23, mask_select_xyz);
        result.Inner[2] = _mm_and_ps(m31_m32_m33, mask_select_xyz);
        result.Inner[3] = _mm_load_ps(F32x4_PositiveUnitW.As<float>());

        return result;
#elif ANEMONE_FEATURE_NEON
        // = [xw, yw, zw, ww]
        float32x4_t const r_xw_yw_zw_ww = vmulq_laneq_f32(rotor, rotor, 3);
        // = [2xw, 2yw, 2zw, 2ww]
        float32x4_t const r_2xw_2yw_2zw_2ww = vaddq_f32(r_xw_yw_zw_ww, r_xw_yw_zw_ww);

        float32x2_t const r_xy = vget_low_f32(rotor);
        float32x2_t const r_zw = vget_high_f32(rotor);

        float32x2_t const r_yx = vrev64_f32(r_xy);
        float32x2_t const r_xx = vdup_lane_f32(r_xy, 0);
        float32x2_t const r_yy = vdup_lane_f32(r_xy, 1);
        float32x2_t const r_zz = vdup_lane_f32(r_zw, 0);

        // = [y, x, x, x]
        float32x4_t const r_yxxx = vcombine_f32(r_yx, r_xx);
        // = [z, z, y, y]
        float32x4_t const r_zzyy = vcombine_f32(r_zz, r_yy);
        // = [yz, xz, xy, xy]
        float32x4_t const r_yz_xz_xy_xy = vmulq_f32(r_yxxx, r_zzyy);
        // = [2yz, 2xz, 2xy, 2xy]
        float32x4_t const r_2yz_2xz_2xy_2xy = vaddq_f32(r_yz_xz_xy_xy, r_yz_xz_xy_xy);

        // = [-1, -1, 1, 1]
        static constexpr float fnegate_xy[4]{-1.0f, -1.0f, 1.0f, 1.0f};
        float32x4_t const negate_xy = vld1q_f32(fnegate_xy);
        // = [-1, 1, -1, -1]
        static constexpr float fnegate_xz[4]{-1.0f, 1.0f, -1.0f, -1.0f};
        float32x4_t const negate_xz = vld1q_f32(fnegate_xz);
        // = [1, -1, -1, 1]
        static constexpr float fnegate_yz[4]{1.0f, -1.0f, -1.0f, 1.0f};
        float32x4_t const negate_yz = vld1q_f32(fnegate_yz);

        // m11 = (rww + -1.0f * rxx) + (-1.0f * ryy + ( 1.0f * rzz));
        // m22 = (rww + -1.0f * rxx) + ( 1.0f * ryy + (-1.0f * rzz));
        // m33 = (rww +  1.0f * rxx) + (-1.0f * ryy + (-1.0f * rzz));

        float32x4_t const r_xx_yy_zz_ww = vmulq_f32(rotor, rotor);
        float32x4_t const r_ww_ww_ww_ww = vdupq_laneq_f32(r_xx_yy_zz_ww, 3);

        float32x4_t t0 = vmulq_laneq_f32(negate_yz, r_xx_yy_zz_ww, 2);
        float32x4_t t1 = vmlaq_laneq_f32(t0, negate_xz, r_xx_yy_zz_ww, 1);
        float32x4_t t2 = vmlaq_laneq_f32(r_ww_ww_ww_ww, negate_xy, r_xx_yy_zz_ww, 0);
        float32x4_t const m11_m22_m33 = vaddq_f32(t1, t2);

        t0 = vmulq_f32(negate_xz, r_2yz_2xz_2xy_2xy);

        // float const m12 = -1.0f * r2yz - r2xw;
        // float const m13 =  1.0f * r2xz - r2yw;
        // float const m23 = -1.0f * r2xy - r2zw;
        float32x4_t const m12_m13_m23 = vsubq_f32(t0, r_2xw_2yw_2zw_2ww);

        // float const m21 = -1.0f * r2yz + r2xw;
        // float const m31 =  1.0f * r2xz + r2yw;
        // float const m32 = -1.0f * r2xy + r2zw;
        float32x4_t const m21_m31_m32 = vaddq_f32(t0, r_2xw_2yw_2zw_2ww);

        float32x2_t const m11_m22 = vget_low_f32(m11_m22_m33);
        float32x2_t const m33_xxx = vget_high_f32(m11_m22_m33);
        float32x2_t const m12_m13 = vget_low_f32(m12_m13_m23);
        float32x2_t const m23_xxx = vget_high_f32(m12_m13_m23);
        float32x2_t const m21_m31 = vget_low_f32(m21_m31_m32);
        float32x2_t const m32_xxx = vget_high_f32(m21_m31_m32);

        float32x2_t const zero = vdup_n_f32(0.0f);
        float32x2_t const m11_m12 = vcopy_lane_f32(m11_m22, 1, m12_m13, 0);
        float32x2_t const m13_zzz = vcopy_lane_f32(zero, 0, m12_m13, 1);
        float32x2_t const m21_m22 = vcopy_lane_f32(m21_m31, 1, m11_m22, 1);
        float32x2_t const m23_zzz = vset_lane_f32(0.0f, m23_xxx, 1);
        float32x2_t const m31_m32 = vext_f32(m21_m31, m32_xxx, 1);
        float32x2_t const m33_zzz = vset_lane_f32(0.0f, m33_xxx, 1);

        float32x4_t const m11_m12_m13 = vcombine_f32(m11_m12, m13_zzz);
        float32x4_t const m21_m22_m23 = vcombine_f32(m21_m22, m23_zzz);
        float32x4_t const m31_m32_m33 = vcombine_f32(m31_m32, m33_zzz);

        SimdMatrix4x4F result;
        result.val[0] = m11_m12_m13;
        result.val[1] = m21_m22_m23;
        result.val[2] = m31_m32_m33;
        result.val[3] = vld1q_f32(F32x4_PositiveUnitW.As<float>());

        return result;
#endif
    }

    inline SimdVector4F anemone_vectorcall RotorF_Nlerp(SimdVector4F from, SimdVector4F to, SimdVector4F t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return RotorF_Nlerp(from, to, t.Inner[0]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // = dot(from, to)
        __m128 const dot = Vector4F_Dot4(from, to);
        // = [1, 1, 1, 1]
        __m128 const positive_one = _mm_load_ps(F32x4_PositiveOne.As<float>());
        // = [-1, -1, -1, -1]
        __m128 const negative_one = _mm_load_ps(F32x4_NegativeOne.As<float>());
        // = [0, 0, 0, 0]
        __m128 const zero = _mm_setzero_ps();

        // to = dot < 0 ? -to : to
        __m128 const mask = _mm_cmplt_ps(dot, zero);
        __m128 const sign = _mm_blendv_ps(positive_one, negative_one, mask);
        to = _mm_mul_ps(to, sign);

        __m128 const r = Vector4F_PreciseLerp(from, to, t);
        return Vector4F_Normalize4(r);
#elif ANEMONE_FEATURE_NEON
        // = dot(from, to)
        float32x4_t const dot = Vector4F_Dot4(from, to);
        // = [1, 1, 1, 1]
        float32x4_t const positive_one = vld1q_f32(F32x4_PositiveOne.As<float>());
        // = [-1, -1, -1, -1]
        float32x4_t const negative_one = vld1q_f32(F32x4_NegativeOne.As<float>());
        // = [0, 0, 0, 0]
        float32x4_t const zero = vdupq_n_f32(0.0f);

        // = [dot < 0].xxxx
        uint32x4_t const mask = vcltq_f32(dot, zero);
        // = [dot < 0 ? -1 : 1].xxxx
        float32x4_t const sign = vbslq_f32(mask, negative_one, positive_one);
        to = vmulq_f32(to, sign);

        float32x4_t const r = Vector4F_PreciseLerp(from, to, t);
        return Vector4F_Normalize4(r);
#endif
    }

    inline SimdVector4F anemone_vectorcall RotorF_Nlerp(SimdVector4F from, SimdVector4F to, float t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        SimdVector4F const dot = Vector4F_Dot4(from, to);

        if (dot.Inner[0] < 0.0f)
        {
            to = Vector4F_Negate(to);
        }

        SimdVector4F const r = Vector4F_PreciseLerp(from, to, t);
        return Vector4F_Normalize4(r);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return RotorF_Nlerp(from, to, _mm_set1_ps(t));
#elif ANEMONE_FEATURE_NEON
        return RotorF_Nlerp(from, to, vdupq_n_f32(t));
#endif
    }

    inline SimdVector4F anemone_vectorcall RotorF_Slerp(SimdVector4F from, SimdVector4F to, SimdVector4F t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return RotorF_Slerp(from, to, t.Inner[0]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // = from * sin((1 - t) * theta) / sin(theta) + to * sin(t * theta) / sin(theta)
        __m128 const positive_one = _mm_load_ps(F32x4_PositiveOne.As<float>());
        __m128 const negative_one = _mm_load_ps(F32x4_NegativeOne.As<float>());
        __m128 const zero = _mm_setzero_ps();

        // cos(theta) = dot(from, to)
        __m128 cos_theta = Vector4F_Dot4(from, to);
        // = [cos(theta) < 0].xxxx
        __m128 const mask = _mm_cmplt_ps(cos_theta, zero);
        // = [cos(theta) < 0 ? -1 : 1]
        __m128 const sign = _mm_blendv_ps(positive_one, negative_one, mask);

        // update sign of cos(theta)
        cos_theta = _mm_mul_ps(cos_theta, sign);
        // update sign of to rotor
        to = _mm_mul_ps(to, sign);

        // sin(theta) = sqrt(1 - cos(theta)^2)
        __m128 sin_theta = _mm_mul_ps(cos_theta, cos_theta);
        sin_theta = _mm_sub_ps(positive_one, sin_theta);
        sin_theta = _mm_sqrt_ps(sin_theta);

        // theta = atan2(sin(theta), cos(theta))
        __m128 const theta = Vector4F_Atan2(sin_theta, cos_theta);

        __m128 const negate_x = _mm_load_ps(F32x4_SignMask_Xnnn.As<float>());

        // = [-t, t, t, t]
        __m128 coefficient_linear = _mm_xor_ps(t, negate_x);
        // = [1-t, t, t, t]
        coefficient_linear = _mm_add_ps(coefficient_linear, _mm_load_ps(F32x4_PositiveUnitX.As<float>()));

        // = [
        //     sin((1 - t) * theta) / sin(theta),
        //     sin(t * theta) / sin(theta),
        // ]
        __m128 coefficient_spherical = _mm_mul_ps(coefficient_linear, theta);
        coefficient_spherical = Vector4F_Sin(coefficient_spherical);
        coefficient_spherical = _mm_div_ps(coefficient_spherical, sin_theta);

        // = [1 - eps].xxxx
        __m128 const one_minus_epsilon = _mm_set1_ps(1.0f - std::numeric_limits<float>::epsilon());
        // = [cos(theta) < (1 - eps)].xxxx
        __m128 const close_to_one = _mm_cmpge_ps(cos_theta, one_minus_epsilon);

        // = close_to_one ? linear : spherical
        __m128 const coefficient_result = _mm_blendv_ps(coefficient_spherical, coefficient_linear, close_to_one);
        __m128 const coefficient_from = _mm_permute_ps(coefficient_result, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 const coefficient_to = _mm_permute_ps(coefficient_result, _MM_SHUFFLE(1, 1, 1, 1));

        // interpolate
        __m128 r = _mm_mul_ps(to, coefficient_to);
        r = Vector4F_MultiplyAdd(from, coefficient_from, r);
        return r;
#elif ANEMONE_FEATURE_NEON
        // = from * sin((1 - t) * theta) / sin(theta) + to * sin(t * theta) / sin(theta)
        float32x4_t const positive_one = vld1q_f32(F32x4_PositiveOne.As<float>());
        float32x4_t const negative_one = vld1q_f32(F32x4_NegativeOne.As<float>());
        float32x4_t const zero = vdupq_n_f32(0.0f);

        // cos(theta) = dot(from, to)
        float32x4_t cos_theta = Vector4F_Dot4(from, to);
        // = [cos(theta) < 0].xxxx
        uint32x4_t const mask = vcltq_f32(cos_theta, zero);
        // = [cos(theta) < 0 ? -1 : 1]
        float32x4_t const sign = vbslq_f32(mask, negative_one, positive_one);

        // update sign of cos(theta)
        cos_theta = vmulq_f32(cos_theta, sign);
        // update sign of to rotor
        to = vmulq_f32(to, sign);

        // sin(theta) = sqrt(1 - cos(theta)^2)
        float32x4_t sin_theta = vmulq_f32(cos_theta, cos_theta);
        sin_theta = vsubq_f32(positive_one, sin_theta);
        sin_theta = vsqrtq_f32(sin_theta);

        // theta = atan2(sin(theta), cos(theta))
        float32x4_t const theta = Vector4F_Atan2(sin_theta, cos_theta);

        float32x4_t const negate_x = vld1q_f32(F32x4_Negate_Xnnn.As<float>());

        // = [-t, t, t, t]
        float32x4_t coefficient_linear = vmulq_f32(t, negate_x);
        // = [1-t, t, t, t]
        coefficient_linear = vaddq_f32(coefficient_linear, vld1q_f32(F32x4_PositiveUnitX.As<float>()));

        // = [
        //     sin((1 - t) * theta) / sin(theta),
        //     sin(t * theta) / sin(theta),
        // ]
        float32x4_t coefficient_spherical = vmulq_f32(coefficient_linear, theta);
        coefficient_spherical = Vector4F_Sin(coefficient_spherical);
        coefficient_spherical = vdivq_f32(coefficient_spherical, sin_theta);

        // = [1 - eps].xxxx
        float32x4_t const one_minus_epsilon = vdupq_n_f32(1.0f - std::numeric_limits<float>::epsilon());
        // = [cos(theta) < (1 - eps)].xxxx
        uint32x4_t const close_to_one = vcgeq_f32(cos_theta, one_minus_epsilon);

        // = close_to_one ? linear : spherical
        float32x4_t const coefficient = vbslq_f32(close_to_one, coefficient_linear, coefficient_spherical);

        // interpolate
        float32x4_t r = vmulq_laneq_f32(to, coefficient, 1);
        r = vmlaq_laneq_f32(r, from, coefficient, 0);
        return r;
#endif
    }

    inline SimdVector4F anemone_vectorcall RotorF_Slerp(SimdVector4F from, SimdVector4F to, float t)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        // = from * sin((1 - t) * theta) / sin(theta) + to * sin(t * theta) / sin(theta)
        static constexpr float one_minus_epsilon = 1.0f - std::numeric_limits<float>::epsilon();

        float cos_theta = Vector4F_Dot4(from, to).Inner[0];

        if (cos_theta < 0.0f)
        {
            cos_theta = -cos_theta;
            to = Vector4F_Negate(to);
        }

        float const sin_theta = Sqrt<float>(1.0f - (cos_theta * cos_theta));
        float const theta = Atan2<float>(sin_theta, cos_theta);

        float const linear_from = 1.0f - t;
        float const linear_to = t;

        float const spherical_from = Sin<float>(linear_from * theta) / sin_theta;
        float const spherical_to = Sin<float>(linear_to * theta) / sin_theta;

        bool const use_spherical = (cos_theta < one_minus_epsilon);

        float const coefficient_from = use_spherical ? spherical_from : linear_from;
        float const coefficient_to = use_spherical ? spherical_to : linear_to;

        SimdVector4F const v_coefficient_from = Vector4F_Replicate(coefficient_from);
        SimdVector4F const v_coefficient_to = Vector4F_Replicate(coefficient_to);

        // interpolate
        SimdVector4F r = Vector4F_Multiply(to, v_coefficient_to);
        r = Vector4F_MultiplyAdd(from, v_coefficient_from, r);
        return r;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return RotorF_Slerp(from, to, _mm_set1_ps(t));
#elif ANEMONE_FEATURE_NEON
        return RotorF_Slerp(from, to, vdupq_n_f32(t));
#endif
    }

    // SimdVector4F anemone_vectorcall RotorF_Barycentric(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F u, SimdVector4F v);
    // SimdVector4F anemone_vectorcall RotorF_Barycentric(SimdVector4F a, SimdVector4F b, SimdVector4F c, float u, float v);

    inline SimdMask4F anemone_vectorcall RotorF_CompareEqual(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_CompareEqual(a, b);
    }

    inline bool anemone_vectorcall RotorF_IsEqual(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_IsEqual4(a, b);
    }

    inline SimdMask4F anemone_vectorcall RotorF_CompareNotEqual(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_CompareNotEqual(a, b);
    }

    inline bool anemone_vectorcall RotorF_IsNotEqual(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_IsNotEqual4(a, b);
    }

    inline SimdMask4F anemone_vectorcall RotorF_CompareNaN(SimdVector4F q)
    {
        return Vector4F_CompareNaN(q);
    }

    inline bool anemone_vectorcall RotorF_IsNaN(SimdVector4F q)
    {
        return Vector4F_IsNaN4(q);
    }

    inline SimdMask4F anemone_vectorcall RotorF_CompareInfinite(SimdVector4F q)
    {
        return Vector4F_CompareInfinite(q);
    }

    inline bool anemone_vectorcall RotorF_IsInfinite(SimdVector4F q)
    {
        return Vector4F_IsInfinite4(q);
    }

    inline SimdMask4F anemone_vectorcall RotorF_CompareIdentity(SimdVector4F q)
    {
        return Vector4F_CompareEqual(q, RotorF_Identity());
    }

    inline bool anemone_vectorcall RotorF_IsIdentity(SimdVector4F q)
    {
        return Vector4F_IsEqual4(q, RotorF_Identity());
    }
}

// Matrix4x4F
namespace Anemone::Math::Detail
{
    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadAlignedFloat4x4(float const* source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        SimdMatrix4x4F result;

        result.Inner[0].Inner[0] = source[0 * 4 + 0];
        result.Inner[0].Inner[1] = source[0 * 4 + 1];
        result.Inner[0].Inner[2] = source[0 * 4 + 2];
        result.Inner[0].Inner[3] = source[0 * 4 + 3];

        result.Inner[1].Inner[0] = source[1 * 4 + 0];
        result.Inner[1].Inner[1] = source[1 * 4 + 1];
        result.Inner[1].Inner[2] = source[1 * 4 + 2];
        result.Inner[1].Inner[3] = source[1 * 4 + 3];

        result.Inner[2].Inner[0] = source[2 * 4 + 0];
        result.Inner[2].Inner[1] = source[2 * 4 + 1];
        result.Inner[2].Inner[2] = source[2 * 4 + 2];
        result.Inner[2].Inner[3] = source[2 * 4 + 3];

        result.Inner[3].Inner[0] = source[3 * 4 + 0];
        result.Inner[3].Inner[1] = source[3 * 4 + 1];
        result.Inner[3].Inner[2] = source[3 * 4 + 2];
        result.Inner[3].Inner[3] = source[3 * 4 + 3];

        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdMatrix4x4F{
            _mm_load_ps(source + 0),
            _mm_load_ps(source + 4),
            _mm_load_ps(source + 8),
            _mm_load_ps(source + 12),
        };
#elif ANEMONE_FEATURE_NEON
        return vld1q_f32_x4(source);
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadAlignedFloat4x3(float const* source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        SimdMatrix4x4F result;

        result.Inner[0].Inner[0] = source[0 * 3 + 0];
        result.Inner[0].Inner[1] = source[0 * 3 + 1];
        result.Inner[0].Inner[2] = source[0 * 3 + 2];
        result.Inner[0].Inner[3] = 0.0f;

        result.Inner[1].Inner[0] = source[1 * 3 + 0];
        result.Inner[1].Inner[1] = source[1 * 3 + 1];
        result.Inner[1].Inner[2] = source[1 * 3 + 2];
        result.Inner[1].Inner[3] = 0.0f;

        result.Inner[2].Inner[0] = source[2 * 3 + 0];
        result.Inner[2].Inner[1] = source[2 * 3 + 1];
        result.Inner[2].Inner[2] = source[2 * 3 + 2];
        result.Inner[2].Inner[3] = 0.0f;

        result.Inner[3].Inner[0] = source[3 * 3 + 0];
        result.Inner[3].Inner[1] = source[3 * 3 + 1];
        result.Inner[3].Inner[2] = source[3 * 3 + 2];
        result.Inner[3].Inner[3] = 1.0f;

        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const m11_m12_m13_m21 = _mm_load_ps(source + 0);
        __m128 const m22_m23_m31_m32 = _mm_load_ps(source + 4);
        __m128 const m33_m41_m42_m43 = _mm_load_ps(source + 8);

        __m128 const m31_m32_m33_m33 = _mm_shuffle_ps(m22_m23_m31_m32, m33_m41_m42_m43, _MM_SHUFFLE(0, 0, 3, 2));
        __m128 const m22_m23_m21_m21 = _mm_shuffle_ps(m22_m23_m31_m32, m11_m12_m13_m21, _MM_SHUFFLE(3, 3, 1, 0));
        __m128 const m21_m22_m23_m23 = _mm_permute_ps(m22_m23_m21_m21, _MM_SHUFFLE(1, 1, 0, 2));
        __m128 const mask = _mm_load_ps(F32x4_SelectMask_XXXn.As<float>());
        __m128 const m11_m12_m13_cc0 = _mm_and_ps(m11_m12_m13_m21, mask);
        __m128 const m21_m22_m23_cc0 = _mm_and_ps(m21_m22_m23_m23, mask);
        __m128 const m31_m32_m33_cc0 = _mm_and_ps(m31_m32_m33_m33, mask);

        __m128i const m41_m42_m43_cc0 = _mm_srli_si128(_mm_castps_si128(m33_m41_m42_m43), 4);
        __m128i const m41_m42_m43_cc1 = _mm_or_si128(m41_m42_m43_cc0, _mm_castps_si128(_mm_load_ps(F32x4_PositiveUnitW.As<float>())));

        return SimdMatrix4x4F{
            m11_m12_m13_cc0,
            m21_m22_m23_cc0,
            m31_m32_m33_cc0,
            _mm_castsi128_ps(m41_m42_m43_cc1),
        };
#elif ANEMONE_FEATURE_NEON
        float32x4_t const m11_m12_m13_m21 = vld1q_f32(source + 0);
        float32x4_t const m22_m23_m31_m32 = vld1q_f32(source + 4);
        float32x4_t const m33_m41_m42_m43 = vld1q_f32(source + 8);

        float32x4_t const m21_m22_m23 = vextq_f32(m11_m12_m13_m21, m22_m23_m31_m32, 3);
        float32x4_t const m31_m32_m33 = vcombine_f32(vget_high_f32(m22_m23_m31_m32), vget_low_f32(m33_m41_m42_m43));
        float32x4_t const m41_m42_m43 = vextq_f32(m33_m41_m42_m43, m33_m41_m42_m43, 1);

        uint32x4_t const mask = vld1q_u32(F32x4_SelectMask_XXXn.As<uint32_t>());

        SimdMatrix4x4F result;

        // = [m11, m12, m13, 0]
        result.val[0] = vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(m11_m12_m13_m21), mask));
        // = [m21, m22, m23, 0]
        result.val[1] = vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(m21_m22_m23), mask));
        // = [m31, m32, m33, 0]
        result.val[2] = vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(m31_m32_m33), mask));
        // = [m41, m42, m43, 1]
        result.val[3] = vsetq_lane_f32(1.0f, m41_m42_m43, 3);

        return result;
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadAlignedFloat3x4(float const* source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        SimdMatrix4x4F result;

        result.Inner[0].Inner[0] = source[0 * 4 + 0];
        result.Inner[0].Inner[1] = source[1 * 4 + 0];
        result.Inner[0].Inner[2] = source[2 * 4 + 0];
        result.Inner[0].Inner[3] = 0.0f;

        result.Inner[1].Inner[0] = source[0 * 4 + 1];
        result.Inner[1].Inner[1] = source[1 * 4 + 1];
        result.Inner[1].Inner[2] = source[2 * 4 + 1];
        result.Inner[1].Inner[3] = 0.0f;

        result.Inner[2].Inner[0] = source[0 * 4 + 2];
        result.Inner[2].Inner[1] = source[1 * 4 + 2];
        result.Inner[2].Inner[2] = source[2 * 4 + 2];
        result.Inner[2].Inner[3] = 0.0f;

        result.Inner[3].Inner[0] = source[0 * 4 + 3];
        result.Inner[3].Inner[1] = source[1 * 4 + 3];
        result.Inner[3].Inner[2] = source[2 * 4 + 3];
        result.Inner[3].Inner[3] = 1.0f;

        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const m11_m12_m13_m14 = _mm_load_ps(source + 0);
        __m128 const m21_m22_m23_m24 = _mm_load_ps(source + 4);
        __m128 const m31_m32_m33_m34 = _mm_load_ps(source + 8);
        __m128 const m41_m42_m43_m44 = _mm_load_ps(F32x4_PositiveUnitW.As<float>());

        __m128 const m11_m12_m21_m22 = _mm_shuffle_ps(m11_m12_m13_m14, m21_m22_m23_m24, _MM_SHUFFLE(1, 0, 1, 0));
        __m128 const m13_m14_m23_m24 = _mm_shuffle_ps(m11_m12_m13_m14, m21_m22_m23_m24, _MM_SHUFFLE(3, 2, 3, 2));
        __m128 const m31_m32_m41_m42 = _mm_shuffle_ps(m31_m32_m33_m34, m41_m42_m43_m44, _MM_SHUFFLE(1, 0, 1, 0));
        __m128 const m33_m34_m43_m44 = _mm_shuffle_ps(m31_m32_m33_m34, m41_m42_m43_m44, _MM_SHUFFLE(3, 2, 3, 2));

        __m128 const m11_m21_m31_m41 = _mm_shuffle_ps(m11_m12_m21_m22, m31_m32_m41_m42, _MM_SHUFFLE(2, 0, 2, 0));
        __m128 const m12_m22_m32_m42 = _mm_shuffle_ps(m11_m12_m21_m22, m31_m32_m41_m42, _MM_SHUFFLE(3, 1, 3, 1));
        __m128 const m13_m23_m33_m43 = _mm_shuffle_ps(m13_m14_m23_m24, m33_m34_m43_m44, _MM_SHUFFLE(2, 0, 2, 0));
        __m128 const m14_m24_m34_m44 = _mm_shuffle_ps(m13_m14_m23_m24, m33_m34_m43_m44, _MM_SHUFFLE(3, 1, 3, 1));

        return SimdMatrix4x4F{
            m11_m21_m31_m41,
            m12_m22_m32_m42,
            m13_m23_m33_m43,
            m14_m24_m34_m44,
        };
#elif ANEMONE_FEATURE_NEON
        uint32x4_t const mask = vld1q_u32(F32x4_SelectMask_XXXn.As<uint32_t>());

        float32x2x4_t r0_r1 = vld4_f32(source + 0);
        float32x4_t m31_m32_m33_m34 = vld1q_f32(source + 8);

        float32x2_t m31_m32 = vget_low_f32(m31_m32_m33_m34);
        float32x4_t m11_m21_m31_m32 = vcombine_f32(r0_r1.val[0], m31_m32);

        float32x2_t m32_m31 = vrev64_f32(m31_m32);
        float32x4_t m12_m22_m32_m31 = vcombine_f32(r0_r1.val[1], m32_m31);

        float32x2_t m33_m34 = vget_high_f32(m31_m32_m33_m34);
        float32x4_t m13_m23_m33_m34 = vcombine_f32(r0_r1.val[2], m33_m34);

        float32x2_t m34_m33 = vrev64_f32(m33_m34);
        float32x4_t m14_m24_m34_m33 = vcombine_f32(r0_r1.val[3], m34_m33);

        SimdMatrix4x4F result;

        // = [m11, m21, m31, 0]
        result.val[0] = vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(m11_m21_m31_m32), mask));
        // = [m12, m22, m32, 0
        result.val[1] = vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(m12_m22_m32_m31), mask));
        // = [m13, m23, m33, 0]
        result.val[2] = vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(m13_m23_m33_m34), mask));
        // = [m14, m24, m34, 1]
        result.val[3] = vsetq_lane_f32(1.f, m14_m24_m34_m33, 3);

        return result;
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadUnalignedFloat4x4(float const* source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        SimdMatrix4x4F result;

        result.Inner[0].Inner[0] = source[0 * 4 + 0];
        result.Inner[0].Inner[1] = source[0 * 4 + 1];
        result.Inner[0].Inner[2] = source[0 * 4 + 2];
        result.Inner[0].Inner[3] = source[0 * 4 + 3];

        result.Inner[1].Inner[0] = source[1 * 4 + 0];
        result.Inner[1].Inner[1] = source[1 * 4 + 1];
        result.Inner[1].Inner[2] = source[1 * 4 + 2];
        result.Inner[1].Inner[3] = source[1 * 4 + 3];

        result.Inner[2].Inner[0] = source[2 * 4 + 0];
        result.Inner[2].Inner[1] = source[2 * 4 + 1];
        result.Inner[2].Inner[2] = source[2 * 4 + 2];
        result.Inner[2].Inner[3] = source[2 * 4 + 3];

        result.Inner[3].Inner[0] = source[3 * 4 + 0];
        result.Inner[3].Inner[1] = source[3 * 4 + 1];
        result.Inner[3].Inner[2] = source[3 * 4 + 2];
        result.Inner[3].Inner[3] = source[3 * 4 + 3];

        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdMatrix4x4F{
            _mm_loadu_ps(source + 0),
            _mm_loadu_ps(source + 4),
            _mm_loadu_ps(source + 8),
            _mm_loadu_ps(source + 12),
        };
#elif ANEMONE_FEATURE_NEON
        return vld1q_f32_x4(source);
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadUnalignedFloat4x3(float const* source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        SimdMatrix4x4F result;

        result.Inner[0].Inner[0] = source[0 * 3 + 0];
        result.Inner[0].Inner[1] = source[0 * 3 + 1];
        result.Inner[0].Inner[2] = source[0 * 3 + 2];
        result.Inner[0].Inner[3] = 0.0f;

        result.Inner[1].Inner[0] = source[1 * 3 + 0];
        result.Inner[1].Inner[1] = source[1 * 3 + 1];
        result.Inner[1].Inner[2] = source[1 * 3 + 2];
        result.Inner[1].Inner[3] = 0.0f;

        result.Inner[2].Inner[0] = source[2 * 3 + 0];
        result.Inner[2].Inner[1] = source[2 * 3 + 1];
        result.Inner[2].Inner[2] = source[2 * 3 + 2];
        result.Inner[2].Inner[3] = 0.0f;

        result.Inner[3].Inner[0] = source[3 * 3 + 0];
        result.Inner[3].Inner[1] = source[3 * 3 + 1];
        result.Inner[3].Inner[2] = source[3 * 3 + 2];
        result.Inner[3].Inner[3] = 1.0f;

        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const m11_m12_m13_m21 = _mm_loadu_ps(source + 0);
        __m128 const m22_m23_m31_m32 = _mm_loadu_ps(source + 4);
        __m128 const m33_m41_m42_m43 = _mm_loadu_ps(source + 8);

        __m128 const m31_m32_m33_m33 = _mm_shuffle_ps(m22_m23_m31_m32, m33_m41_m42_m43, _MM_SHUFFLE(0, 0, 3, 2));
        __m128 const m22_m23_m21_m21 = _mm_shuffle_ps(m22_m23_m31_m32, m11_m12_m13_m21, _MM_SHUFFLE(3, 3, 1, 0));
        __m128 const m21_m22_m23_m23 = _mm_permute_ps(m22_m23_m21_m21, _MM_SHUFFLE(1, 1, 0, 2));
        __m128 const mask = _mm_load_ps(F32x4_SelectMask_XXXn.As<float>());
        __m128 const m11_m12_m13_cc0 = _mm_and_ps(m11_m12_m13_m21, mask);
        __m128 const m21_m22_m23_cc0 = _mm_and_ps(m21_m22_m23_m23, mask);
        __m128 const m31_m32_m33_cc0 = _mm_and_ps(m31_m32_m33_m33, mask);

        __m128i const m41_m42_m43_cc0 = _mm_srli_si128(_mm_castps_si128(m33_m41_m42_m43), 4);
        __m128i const m41_m42_m43_cc1 = _mm_or_si128(m41_m42_m43_cc0, _mm_castps_si128(_mm_load_ps(F32x4_PositiveUnitW.As<float>())));

        return SimdMatrix4x4F{
            m11_m12_m13_cc0,
            m21_m22_m23_cc0,
            m31_m32_m33_cc0,
            _mm_castsi128_ps(m41_m42_m43_cc1),
        };
#elif ANEMONE_FEATURE_NEON
        float32x4_t const m11_m12_m13_m21 = vld1q_f32(source + 0);
        float32x4_t const m22_m23_m31_m32 = vld1q_f32(source + 4);
        float32x4_t const m33_m41_m42_m43 = vld1q_f32(source + 8);

        float32x4_t const m21_m22_m23 = vextq_f32(m11_m12_m13_m21, m22_m23_m31_m32, 3);
        float32x4_t const m31_m32_m33 = vcombine_f32(vget_high_f32(m22_m23_m31_m32), vget_low_f32(m33_m41_m42_m43));
        float32x4_t const m41_m42_m43 = vextq_f32(m33_m41_m42_m43, m33_m41_m42_m43, 1);

        uint32x4_t const mask = vld1q_u32(F32x4_SelectMask_XXXn.As<uint32_t>());

        SimdMatrix4x4F result;

        // = [m11, m12, m13, 0]
        result.val[0] = vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(m11_m12_m13_m21), mask));
        // = [m21, m22, m23, 0]
        result.val[1] = vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(m21_m22_m23), mask));
        // = [m31, m32, m33, 0]
        result.val[2] = vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(m31_m32_m33), mask));
        // = [m41, m42, m43, 1]
        result.val[3] = vsetq_lane_f32(1.0f, m41_m42_m43, 3);

        return result;
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadUnalignedFloat3x4(float const* source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        SimdMatrix4x4F result;

        result.Inner[0].Inner[0] = source[0 * 4 + 0];
        result.Inner[0].Inner[1] = source[1 * 4 + 0];
        result.Inner[0].Inner[2] = source[2 * 4 + 0];
        result.Inner[0].Inner[3] = 0.0f;

        result.Inner[1].Inner[0] = source[0 * 4 + 1];
        result.Inner[1].Inner[1] = source[1 * 4 + 1];
        result.Inner[1].Inner[2] = source[2 * 4 + 1];
        result.Inner[1].Inner[3] = 0.0f;

        result.Inner[2].Inner[0] = source[0 * 4 + 2];
        result.Inner[2].Inner[1] = source[1 * 4 + 2];
        result.Inner[2].Inner[2] = source[2 * 4 + 2];
        result.Inner[2].Inner[3] = 0.0f;

        result.Inner[3].Inner[0] = source[0 * 4 + 3];
        result.Inner[3].Inner[1] = source[1 * 4 + 3];
        result.Inner[3].Inner[2] = source[2 * 4 + 3];
        result.Inner[3].Inner[3] = 1.0f;

        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const m11_m12_m13_m14 = _mm_loadu_ps(source + 0);
        __m128 const m21_m22_m23_m24 = _mm_loadu_ps(source + 4);
        __m128 const m31_m32_m33_m34 = _mm_loadu_ps(source + 8);
        __m128 const m41_m42_m43_m44 = _mm_load_ps(F32x4_PositiveUnitW.As<float>());

        __m128 const m11_m12_m21_m22 = _mm_shuffle_ps(m11_m12_m13_m14, m21_m22_m23_m24, _MM_SHUFFLE(1, 0, 1, 0));
        __m128 const m13_m14_m23_m24 = _mm_shuffle_ps(m11_m12_m13_m14, m21_m22_m23_m24, _MM_SHUFFLE(3, 2, 3, 2));
        __m128 const m31_m32_m41_m42 = _mm_shuffle_ps(m31_m32_m33_m34, m41_m42_m43_m44, _MM_SHUFFLE(1, 0, 1, 0));
        __m128 const m33_m34_m43_m44 = _mm_shuffle_ps(m31_m32_m33_m34, m41_m42_m43_m44, _MM_SHUFFLE(3, 2, 3, 2));

        __m128 const m11_m21_m31_m41 = _mm_shuffle_ps(m11_m12_m21_m22, m31_m32_m41_m42, _MM_SHUFFLE(2, 0, 2, 0));
        __m128 const m12_m22_m32_m42 = _mm_shuffle_ps(m11_m12_m21_m22, m31_m32_m41_m42, _MM_SHUFFLE(3, 1, 3, 1));
        __m128 const m13_m23_m33_m43 = _mm_shuffle_ps(m13_m14_m23_m24, m33_m34_m43_m44, _MM_SHUFFLE(2, 0, 2, 0));
        __m128 const m14_m24_m34_m44 = _mm_shuffle_ps(m13_m14_m23_m24, m33_m34_m43_m44, _MM_SHUFFLE(3, 1, 3, 1));

        return SimdMatrix4x4F{
            m11_m21_m31_m41,
            m12_m22_m32_m42,
            m13_m23_m33_m43,
            m14_m24_m34_m44,
        };
#elif ANEMONE_FEATURE_NEON
        uint32x4_t const mask = vld1q_u32(F32x4_SelectMask_XXXn.As<uint32_t>());

        float32x2x4_t r0_r1 = vld4_f32(source + 0);
        float32x4_t m31_m32_m33_m34 = vld1q_f32(source + 8);

        float32x2_t m31_m32 = vget_low_f32(m31_m32_m33_m34);
        float32x4_t m11_m21_m31_m32 = vcombine_f32(r0_r1.val[0], m31_m32);

        float32x2_t m32_m31 = vrev64_f32(m31_m32);
        float32x4_t m12_m22_m32_m31 = vcombine_f32(r0_r1.val[1], m32_m31);

        float32x2_t m33_m34 = vget_high_f32(m31_m32_m33_m34);
        float32x4_t m13_m23_m33_m34 = vcombine_f32(r0_r1.val[2], m33_m34);

        float32x2_t m34_m33 = vrev64_f32(m33_m34);
        float32x4_t m14_m24_m34_m33 = vcombine_f32(r0_r1.val[3], m34_m33);

        SimdMatrix4x4F result;

        // = [m11, m21, m31, 0]
        result.val[0] = vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(m11_m21_m31_m32), mask));
        // = [m12, m22, m32, 0
        result.val[1] = vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(m12_m22_m32_m31), mask));
        // = [m13, m23, m33, 0]
        result.val[2] = vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(m13_m23_m33_m34), mask));
        // = [m14, m24, m34, 1]
        result.val[3] = vsetq_lane_f32(1.f, m14_m24_m34_m33, 3);

        return result;
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadUnalignedFloat3x3(float const* source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        SimdMatrix4x4F result;

        result.Inner[0].Inner[0] = source[0 * 3 + 0];
        result.Inner[0].Inner[1] = source[0 * 3 + 1];
        result.Inner[0].Inner[2] = source[0 * 3 + 2];
        result.Inner[0].Inner[3] = 0.0f;

        result.Inner[1].Inner[0] = source[1 * 3 + 0];
        result.Inner[1].Inner[1] = source[1 * 3 + 1];
        result.Inner[1].Inner[2] = source[1 * 3 + 2];
        result.Inner[1].Inner[3] = 0.0f;

        result.Inner[2].Inner[0] = source[2 * 3 + 0];
        result.Inner[2].Inner[1] = source[2 * 3 + 1];
        result.Inner[2].Inner[2] = source[2 * 3 + 2];
        result.Inner[2].Inner[3] = 0.0f;

        result.Inner[3].Inner[0] = 0.0f;
        result.Inner[3].Inner[1] = 0.0f;
        result.Inner[3].Inner[2] = 0.0f;
        result.Inner[3].Inner[3] = 1.0f;

        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const m11_m12_m13_m21 = _mm_loadu_ps(source + 0);
        __m128 const m22_m23_m31_m32 = _mm_loadu_ps(source + 4);
        __m128 const m33_zzz_zzz_zzz = _mm_load_ss(source + 8);
        __m128 const zzz_zzz_zzz_zzz = _mm_setzero_ps();

        __m128 const m13_zzz_m21_zzz = _mm_unpackhi_ps(m11_m12_m13_m21, zzz_zzz_zzz_zzz);
        __m128 const m22_zzz_m23_zzz = _mm_unpacklo_ps(m22_m23_m31_m32, zzz_zzz_zzz_zzz);
        __m128 const m33_m33_zzz_m22 = _mm_shuffle_ps(m33_zzz_zzz_zzz, m22_zzz_m23_zzz, _MM_SHUFFLE(0, 1, 0, 0));
        __m128 const zzz_m22_m23_zzz = _mm_movehl_ps(m22_zzz_m23_zzz, m33_m33_zzz_m22);
        __m128 const m21_zzz_zzz_zzz = _mm_movehl_ps(zzz_zzz_zzz_zzz, m13_zzz_m21_zzz);

        __m128 const m11_m12_m13_zzz = _mm_movelh_ps(m11_m12_m13_m21, m13_zzz_m21_zzz);
        __m128 const m21_m22_m23_zzz = _mm_add_ps(zzz_m22_m23_zzz, m21_zzz_zzz_zzz);
        __m128 const m31_m32_m33_zzz = _mm_shuffle_ps(m22_m23_m31_m32, m33_zzz_zzz_zzz, _MM_SHUFFLE(1, 0, 3, 2));

        return SimdMatrix4x4F{
            m11_m12_m13_zzz,
            m21_m22_m23_zzz,
            m31_m32_m33_zzz,
            _mm_load_ps(F32x4_PositiveUnitW.As<float>()),
        };
#elif ANEMONE_FEATURE_NEON
        float32x4_t const m11_m12_m13_m21 = vld1q_f32(source + 0);
        float32x4_t const m22_m23_m31_m32 = vld1q_f32(source + 4);
        float32x2_t const m33_cc0 = vcreate_f32(static_cast<uint64_t>(*(reinterpret_cast<uint32_t const*>(source + 8))));
        float32x4_t const m21_m22_m23_m31 = vextq_f32(m11_m12_m13_m21, m22_m23_m31_m32, 3);

        uint32x4_t const mask = vld1q_u32(F32x4_SelectMask_XXXn.As<uint32_t>());

        SimdMatrix4x4F result;

        // = [m11, m12, m13, 0]
        result.val[0] = vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(m11_m12_m13_m21), mask));
        // = [m21, m22, m23, 0]
        result.val[1] = vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(m21_m22_m23_m31), mask));
        // = [m31, m32, m33, 0]
        result.val[2] = vcombine_f32(vget_high_f32(m22_m23_m31_m32), m33_cc0);
        // = [0, 0, 0, 1]
        result.val[3] = vld1q_f32(F32x4_PositiveUnitW.As<float>());

        return result;
#endif
    }

    inline void anemone_vectorcall Matrix4x4F_StoreAlignedFloat4x4(float* destination, SimdMatrix4x4F source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        destination[0 * 4 + 0] = source.Inner[0].Inner[0];
        destination[0 * 4 + 1] = source.Inner[0].Inner[1];
        destination[0 * 4 + 2] = source.Inner[0].Inner[2];
        destination[0 * 4 + 3] = source.Inner[0].Inner[3];

        destination[1 * 4 + 0] = source.Inner[1].Inner[0];
        destination[1 * 4 + 1] = source.Inner[1].Inner[1];
        destination[1 * 4 + 2] = source.Inner[1].Inner[2];
        destination[1 * 4 + 3] = source.Inner[1].Inner[3];

        destination[2 * 4 + 0] = source.Inner[2].Inner[0];
        destination[2 * 4 + 1] = source.Inner[2].Inner[1];
        destination[2 * 4 + 2] = source.Inner[2].Inner[2];
        destination[2 * 4 + 3] = source.Inner[2].Inner[3];

        destination[3 * 4 + 0] = source.Inner[3].Inner[0];
        destination[3 * 4 + 1] = source.Inner[3].Inner[1];
        destination[3 * 4 + 2] = source.Inner[3].Inner[2];
        destination[3 * 4 + 3] = source.Inner[3].Inner[3];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        _mm_store_ps(destination + 0, source.Inner[0]);
        _mm_store_ps(destination + 4, source.Inner[1]);
        _mm_store_ps(destination + 8, source.Inner[2]);
        _mm_store_ps(destination + 12, source.Inner[3]);
#elif ANEMONE_FEATURE_NEON
        vst1q_f32_x4(destination, source);
#endif
    }

    inline void anemone_vectorcall Matrix4x4F_StoreAlignedFloat4x3(float* destination, SimdMatrix4x4F source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        destination[0 * 3 + 0] = source.Inner[0].Inner[0];
        destination[0 * 3 + 1] = source.Inner[0].Inner[1];
        destination[0 * 3 + 2] = source.Inner[0].Inner[2];

        destination[1 * 3 + 0] = source.Inner[1].Inner[0];
        destination[1 * 3 + 1] = source.Inner[1].Inner[1];
        destination[1 * 3 + 2] = source.Inner[1].Inner[2];

        destination[2 * 3 + 0] = source.Inner[2].Inner[0];
        destination[2 * 3 + 1] = source.Inner[2].Inner[1];
        destination[2 * 3 + 2] = source.Inner[2].Inner[2];

        destination[3 * 3 + 0] = source.Inner[3].Inner[0];
        destination[3 * 3 + 1] = source.Inner[3].Inner[1];
        destination[3 * 3 + 2] = source.Inner[3].Inner[2];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const m11_m12_m13_m14 = source.Inner[0];
        __m128 const m21_m22_m23_m24 = source.Inner[1];
        __m128 const m31_m32_m33_m34 = source.Inner[2];
        __m128 const m41_m42_m43_m44 = source.Inner[3];

        __m128 const m22_m23_m31_m32 = _mm_shuffle_ps(m21_m22_m23_m24, m31_m32_m33_m34, _MM_SHUFFLE(1, 0, 2, 1));

        __m128 const m21_m21_m13_m13 = _mm_shuffle_ps(m21_m22_m23_m24, m11_m12_m13_m14, _MM_SHUFFLE(2, 2, 0, 0));
        __m128 const m11_m12_m13_m21 = _mm_shuffle_ps(m11_m12_m13_m14, m21_m21_m13_m13, _MM_SHUFFLE(0, 2, 1, 0));
        __m128 const m33_m33_m41_m41 = _mm_shuffle_ps(m31_m32_m33_m34, m41_m42_m43_m44, _MM_SHUFFLE(0, 0, 2, 2));
        __m128 const m33_m41_m42_m43 = _mm_shuffle_ps(m33_m33_m41_m41, m41_m42_m43_m44, _MM_SHUFFLE(2, 1, 2, 0));

        _mm_store_ps(destination + 0, m11_m12_m13_m21);
        _mm_store_ps(destination + 4, m22_m23_m31_m32);
        _mm_store_ps(destination + 8, m33_m41_m42_m43);
#elif ANEMONE_FEATURE_NEON
        uint32x4_t const mask = vld1q_u32(F32x4_SelectMask_XXXn.As<uint32_t>());

        // = [m11, m12, m13, m21]
        float32x4_t const m12_m13_m14_m21 = vextq_f32(source.val[0], source.val[1], 1);
        float32x4_t const m11_m12_m13_m21 = vbslq_f32(mask, source.val[0], m12_m13_m14_m21);
        vst1q_f32(destination + 0, m11_m12_m13_m21);

        // = [m22, m23, m31, m32]
        float32x4_t const m22_m23_m24_m21 = vextq_f32(source.val[1], source.val[1], 1);
        float32x4_t const m22_m23_m31_m32 = vcombine_f32(vget_low_f32(m22_m23_m24_m21), vget_low_f32(source.val[2]));
        vst1q_f32(destination + 4, m22_m23_m31_m32);

        // = [m33, m41, m42, m43]
        float32x4_t const m33_m33_m33_m33 = vdupq_lane_f32(vget_high_f32(source.val[2]), 0);
        float32x4_t const m33_m41_m42_m43 = vextq_f32(m33_m33_m33_m33, source.val[3], 3);
        vst1q_f32(destination + 8, m33_m41_m42_m43);
#endif
    }

    inline void anemone_vectorcall Matrix4x4F_StoreAlignedFloat3x4(float* destination, SimdMatrix4x4F source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        destination[0 * 4 + 0] = source.Inner[0].Inner[0];
        destination[0 * 4 + 1] = source.Inner[1].Inner[0];
        destination[0 * 4 + 2] = source.Inner[2].Inner[0];
        destination[0 * 4 + 3] = source.Inner[3].Inner[0];

        destination[1 * 4 + 0] = source.Inner[0].Inner[1];
        destination[1 * 4 + 1] = source.Inner[1].Inner[1];
        destination[1 * 4 + 2] = source.Inner[2].Inner[1];
        destination[1 * 4 + 3] = source.Inner[3].Inner[1];

        destination[2 * 4 + 0] = source.Inner[0].Inner[2];
        destination[2 * 4 + 1] = source.Inner[1].Inner[2];
        destination[2 * 4 + 2] = source.Inner[2].Inner[2];
        destination[2 * 4 + 3] = source.Inner[3].Inner[2];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const m11_m12_m13_m14 = source.Inner[0];
        __m128 const m21_m22_m23_m24 = source.Inner[1];
        __m128 const m31_m32_m33_m34 = source.Inner[2];
        __m128 const m41_m42_m43_m44 = source.Inner[3];

        __m128 const m11_m12_m21_m22 = _mm_shuffle_ps(m11_m12_m13_m14, m21_m22_m23_m24, _MM_SHUFFLE(1, 0, 1, 0));
        __m128 const m13_m14_m23_m24 = _mm_shuffle_ps(m11_m12_m13_m14, m21_m22_m23_m24, _MM_SHUFFLE(3, 2, 3, 2));
        __m128 const m31_m32_m41_m42 = _mm_shuffle_ps(m31_m32_m33_m34, m41_m42_m43_m44, _MM_SHUFFLE(1, 0, 1, 0));
        __m128 const m33_m34_m43_m44 = _mm_shuffle_ps(m31_m32_m33_m34, m41_m42_m43_m44, _MM_SHUFFLE(3, 2, 3, 2));

        __m128 const m11_m21_m31_m41 = _mm_shuffle_ps(m11_m12_m21_m22, m31_m32_m41_m42, _MM_SHUFFLE(2, 0, 2, 0));
        __m128 const m12_m22_m32_m42 = _mm_shuffle_ps(m11_m12_m21_m22, m31_m32_m41_m42, _MM_SHUFFLE(3, 1, 3, 1));
        __m128 const m13_m23_m33_m43 = _mm_shuffle_ps(m13_m14_m23_m24, m33_m34_m43_m44, _MM_SHUFFLE(2, 0, 2, 0));

        _mm_store_ps(destination + 0, m11_m21_m31_m41);
        _mm_store_ps(destination + 4, m12_m22_m32_m42);
        _mm_store_ps(destination + 8, m13_m23_m33_m43);
#elif ANEMONE_FEATURE_NEON
        float32x4x2_t const p0 = vzipq_f32(source.val[0], source.val[2]);
        float32x4x2_t const p1 = vzipq_f32(source.val[1], source.val[3]);

        float32x4x2_t const t0 = vzipq_f32(p0.val[0], p1.val[0]);
        float32x4x2_t const t1 = vzipq_f32(p0.val[1], p1.val[1]);

        vst1q_f32(destination + 0, t0.val[0]);
        vst1q_f32(destination + 4, t0.val[1]);
        vst1q_f32(destination + 8, t1.val[0]);
#endif
    }

    inline void anemone_vectorcall Matrix4x4F_StoreUnalignedFloat4x4(float* destination, SimdMatrix4x4F source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        destination[0 * 4 + 0] = source.Inner[0].Inner[0];
        destination[0 * 4 + 1] = source.Inner[0].Inner[1];
        destination[0 * 4 + 2] = source.Inner[0].Inner[2];
        destination[0 * 4 + 3] = source.Inner[0].Inner[3];

        destination[1 * 4 + 0] = source.Inner[1].Inner[0];
        destination[1 * 4 + 1] = source.Inner[1].Inner[1];
        destination[1 * 4 + 2] = source.Inner[1].Inner[2];
        destination[1 * 4 + 3] = source.Inner[1].Inner[3];

        destination[2 * 4 + 0] = source.Inner[2].Inner[0];
        destination[2 * 4 + 1] = source.Inner[2].Inner[1];
        destination[2 * 4 + 2] = source.Inner[2].Inner[2];
        destination[2 * 4 + 3] = source.Inner[2].Inner[3];

        destination[3 * 4 + 0] = source.Inner[3].Inner[0];
        destination[3 * 4 + 1] = source.Inner[3].Inner[1];
        destination[3 * 4 + 2] = source.Inner[3].Inner[2];
        destination[3 * 4 + 3] = source.Inner[3].Inner[3];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        _mm_storeu_ps(destination + 0, source.Inner[0]);
        _mm_storeu_ps(destination + 4, source.Inner[1]);
        _mm_storeu_ps(destination + 8, source.Inner[2]);
        _mm_storeu_ps(destination + 12, source.Inner[3]);
#elif ANEMONE_FEATURE_NEON
        vst1q_f32_x4(destination, source);
#endif
    }

    inline void anemone_vectorcall Matrix4x4F_StoreUnalignedFloat4x3(float* destination, SimdMatrix4x4F source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        destination[0 * 3 + 0] = source.Inner[0].Inner[0];
        destination[0 * 3 + 1] = source.Inner[0].Inner[1];
        destination[0 * 3 + 2] = source.Inner[0].Inner[2];

        destination[1 * 3 + 0] = source.Inner[1].Inner[0];
        destination[1 * 3 + 1] = source.Inner[1].Inner[1];
        destination[1 * 3 + 2] = source.Inner[1].Inner[2];

        destination[2 * 3 + 0] = source.Inner[2].Inner[0];
        destination[2 * 3 + 1] = source.Inner[2].Inner[1];
        destination[2 * 3 + 2] = source.Inner[2].Inner[2];

        destination[3 * 3 + 0] = source.Inner[3].Inner[0];
        destination[3 * 3 + 1] = source.Inner[3].Inner[1];
        destination[3 * 3 + 2] = source.Inner[3].Inner[2];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const m11_m12_m13_m14 = source.Inner[0];
        __m128 const m21_m22_m23_m24 = source.Inner[1];
        __m128 const m31_m32_m33_m34 = source.Inner[2];
        __m128 const m41_m42_m43_m44 = source.Inner[3];

        __m128 const m22_m23_m31_m32 = _mm_shuffle_ps(m21_m22_m23_m24, m31_m32_m33_m34, _MM_SHUFFLE(1, 0, 2, 1));

        __m128 const m21_m21_m13_m13 = _mm_shuffle_ps(m21_m22_m23_m24, m11_m12_m13_m14, _MM_SHUFFLE(2, 2, 0, 0));
        __m128 const m11_m12_m13_m21 = _mm_shuffle_ps(m11_m12_m13_m14, m21_m21_m13_m13, _MM_SHUFFLE(0, 2, 1, 0));
        __m128 const m33_m33_m41_m41 = _mm_shuffle_ps(m31_m32_m33_m34, m41_m42_m43_m44, _MM_SHUFFLE(0, 0, 2, 2));
        __m128 const m33_m41_m42_m43 = _mm_shuffle_ps(m33_m33_m41_m41, m41_m42_m43_m44, _MM_SHUFFLE(2, 1, 2, 0));

        _mm_storeu_ps(destination + 0, m11_m12_m13_m21);
        _mm_storeu_ps(destination + 4, m22_m23_m31_m32);
        _mm_storeu_ps(destination + 8, m33_m41_m42_m43);
#elif ANEMONE_FEATURE_NEON
        uint32x4_t const mask = vld1q_u32(F32x4_SelectMask_XXXn.As<uint32_t>());

        // = [m11, m12, m13, m21]
        float32x4_t const m12_m13_m14_m21 = vextq_f32(source.val[0], source.val[1], 1);
        float32x4_t const m11_m12_m13_m21 = vbslq_f32(mask, source.val[0], m12_m13_m14_m21);
        vst1q_f32(destination + 0, m11_m12_m13_m21);

        // = [m22, m23, m31, m32]
        float32x4_t const m22_m23_m24_m21 = vextq_f32(source.val[1], source.val[1], 1);
        float32x4_t const m22_m23_m31_m32 = vcombine_f32(vget_low_f32(m22_m23_m24_m21), vget_low_f32(source.val[2]));
        vst1q_f32(destination + 4, m22_m23_m31_m32);

        // = [m33, m41, m42, m43]
        float32x4_t const m33_m33_m33_m33 = vdupq_lane_f32(vget_high_f32(source.val[2]), 0);
        float32x4_t const m33_m41_m42_m43 = vextq_f32(m33_m33_m33_m33, source.val[3], 3);
        vst1q_f32(destination + 8, m33_m41_m42_m43);
#endif
    }

    inline void anemone_vectorcall Matrix4x4F_StoreUnalignedFloat3x4(float* destination, SimdMatrix4x4F source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        destination[0 * 4 + 0] = source.Inner[0].Inner[0];
        destination[0 * 4 + 1] = source.Inner[1].Inner[0];
        destination[0 * 4 + 2] = source.Inner[2].Inner[0];
        destination[0 * 4 + 3] = source.Inner[3].Inner[0];

        destination[1 * 4 + 0] = source.Inner[0].Inner[1];
        destination[1 * 4 + 1] = source.Inner[1].Inner[1];
        destination[1 * 4 + 2] = source.Inner[2].Inner[1];
        destination[1 * 4 + 3] = source.Inner[3].Inner[1];

        destination[2 * 4 + 0] = source.Inner[0].Inner[2];
        destination[2 * 4 + 1] = source.Inner[1].Inner[2];
        destination[2 * 4 + 2] = source.Inner[2].Inner[2];
        destination[2 * 4 + 3] = source.Inner[3].Inner[2];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const m11_m12_m13_m14 = source.Inner[0];
        __m128 const m21_m22_m23_m24 = source.Inner[1];
        __m128 const m31_m32_m33_m34 = source.Inner[2];
        __m128 const m41_m42_m43_m44 = source.Inner[3];

        __m128 const m11_m12_m21_m22 = _mm_shuffle_ps(m11_m12_m13_m14, m21_m22_m23_m24, _MM_SHUFFLE(1, 0, 1, 0));
        __m128 const m13_m14_m23_m24 = _mm_shuffle_ps(m11_m12_m13_m14, m21_m22_m23_m24, _MM_SHUFFLE(3, 2, 3, 2));
        __m128 const m31_m32_m41_m42 = _mm_shuffle_ps(m31_m32_m33_m34, m41_m42_m43_m44, _MM_SHUFFLE(1, 0, 1, 0));
        __m128 const m33_m34_m43_m44 = _mm_shuffle_ps(m31_m32_m33_m34, m41_m42_m43_m44, _MM_SHUFFLE(3, 2, 3, 2));

        __m128 const m11_m21_m31_m41 = _mm_shuffle_ps(m11_m12_m21_m22, m31_m32_m41_m42, _MM_SHUFFLE(2, 0, 2, 0));
        __m128 const m12_m22_m32_m42 = _mm_shuffle_ps(m11_m12_m21_m22, m31_m32_m41_m42, _MM_SHUFFLE(3, 1, 3, 1));
        __m128 const m13_m23_m33_m43 = _mm_shuffle_ps(m13_m14_m23_m24, m33_m34_m43_m44, _MM_SHUFFLE(2, 0, 2, 0));

        _mm_storeu_ps(destination + 0, m11_m21_m31_m41);
        _mm_storeu_ps(destination + 4, m12_m22_m32_m42);
        _mm_storeu_ps(destination + 8, m13_m23_m33_m43);
#elif ANEMONE_FEATURE_NEON
        float32x4x2_t const p0 = vzipq_f32(source.val[0], source.val[2]);
        float32x4x2_t const p1 = vzipq_f32(source.val[1], source.val[3]);

        float32x4x2_t const t0 = vzipq_f32(p0.val[0], p1.val[0]);
        float32x4x2_t const t1 = vzipq_f32(p0.val[1], p1.val[1]);

        vst1q_f32(destination + 0, t0.val[0]);
        vst1q_f32(destination + 4, t0.val[1]);
        vst1q_f32(destination + 8, t1.val[0]);
#endif
    }

    inline void anemone_vectorcall Matrix4x4F_StoreUnalignedFloat3x3(float* destination, SimdMatrix4x4F source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        destination[0 * 3 + 0] = source.Inner[0].Inner[0];
        destination[0 * 3 + 1] = source.Inner[0].Inner[1];
        destination[0 * 3 + 2] = source.Inner[0].Inner[2];

        destination[1 * 3 + 0] = source.Inner[1].Inner[0];
        destination[1 * 3 + 1] = source.Inner[1].Inner[1];
        destination[1 * 3 + 2] = source.Inner[1].Inner[2];

        destination[2 * 3 + 0] = source.Inner[2].Inner[0];
        destination[2 * 3 + 1] = source.Inner[2].Inner[1];
        destination[2 * 3 + 2] = source.Inner[2].Inner[2];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const m11_m12_m13_m14 = source.Inner[0];
        __m128 const m21_m22_m23_m24 = source.Inner[1];
        __m128 const m31_m32_m33_m34 = source.Inner[2];

        __m128 const m13_m13_m21_m21 = _mm_shuffle_ps(m11_m12_m13_m14, m21_m22_m23_m24, _MM_SHUFFLE(0, 0, 2, 2));

        __m128 const m11_m12_m13_m21 = _mm_shuffle_ps(m11_m12_m13_m14, m13_m13_m21_m21, _MM_SHUFFLE(2, 0, 1, 0));
        __m128 const m22_m23_m31_m32 = _mm_shuffle_ps(m21_m22_m23_m24, m31_m32_m33_m34, _MM_SHUFFLE(1, 0, 2, 1));
        __m128 const m33_m33_m33_m33 = _mm_permute_ps(m31_m32_m33_m34, _MM_SHUFFLE(2, 2, 2, 2));
        _mm_storeu_ps(destination + 0, m11_m12_m13_m21);
        _mm_storeu_ps(destination + 4, m22_m23_m31_m32);
        _mm_store_ss(destination + 8, m33_m33_m33_m33);
#elif ANEMONE_FEATURE_NEON
        uint32x4_t const mask = vld1q_u32(F32x4_SelectMask_XXXn.As<uint32_t>());

        // = [m12, m13, m14, m21]
        float32x4_t const m12_m13_m14_m21 = vextq_f32(source.val[0], source.val[1], 1);
        // = [m11, m12, m13, m21]
        float32x4_t const m11_m12_m13_m21 = vbslq_f32(mask, source.val[0], m12_m13_m14_m21);
        vst1q_f32(destination + 0, m11_m12_m13_m21);

        // = [m22, m23, m24, m21]
        float32x4_t const m22_m23_m24_m21 = vextq_f32(source.val[1], source.val[1], 1);
        // = [m22, m23, m31, m32]
        float32x4_t const m22_m23_m31_m32 = vcombine_f32(vget_low_f32(m22_m23_m24_m21), vget_low_f32(source.val[2]));
        vst1q_f32(destination + 4, m22_m23_m31_m32);
        // = [m33]
        vst1q_lane_f32(destination + 8, source.val[2], 2);
#endif
    }

#if ANEMONE_FEATURE_NEON
    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadTransposeFloat4x4(float const* source)
    {
        return vld4q_f32(source);
    }

    inline void anemone_vectorcall Matrix4x4F_StoreTransposeFloat4x4(float* destination, SimdMatrix4x4F source)
    {
        vst4q_f32(destination, source);
    }
#endif

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Identity()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        SimdMatrix4x4F result;

        result.Inner[0].Inner[0] = 1.0f;
        result.Inner[0].Inner[1] = 0.0f;
        result.Inner[0].Inner[2] = 0.0f;
        result.Inner[0].Inner[3] = 0.0f;

        result.Inner[1].Inner[0] = 0.0f;
        result.Inner[1].Inner[1] = 1.0f;
        result.Inner[1].Inner[2] = 0.0f;
        result.Inner[1].Inner[3] = 0.0f;

        result.Inner[2].Inner[0] = 0.0f;
        result.Inner[2].Inner[1] = 0.0f;
        result.Inner[2].Inner[2] = 1.0f;
        result.Inner[2].Inner[3] = 0.0f;

        result.Inner[3].Inner[0] = 0.0f;
        result.Inner[3].Inner[1] = 0.0f;
        result.Inner[3].Inner[2] = 0.0f;
        result.Inner[3].Inner[3] = 1.0f;

        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdMatrix4x4F{
            _mm_load_ps(F32x4_PositiveUnitX.As<float>()),
            _mm_load_ps(F32x4_PositiveUnitY.As<float>()),
            _mm_load_ps(F32x4_PositiveUnitZ.As<float>()),
            _mm_load_ps(F32x4_PositiveUnitW.As<float>()),
        };
#elif ANEMONE_FEATURE_NEON
        return SimdMatrix4x4F{
            vld1q_f32(F32x4_PositiveUnitX.As<float>()),
            vld1q_f32(F32x4_PositiveUnitY.As<float>()),
            vld1q_f32(F32x4_PositiveUnitZ.As<float>()),
            vld1q_f32(F32x4_PositiveUnitW.As<float>()),
        };
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_NaN()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        SimdVector4F const lane = Vector4F_LoadAlignedFloat4(F32x4_PositiveQNaN_XXXX.As<float>());
        return SimdMatrix4x4F{lane, lane, lane, lane};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const lane = _mm_load_ps(F32x4_PositiveQNaN_XXXX.As<float>());
        return SimdMatrix4x4F{lane, lane, lane, lane};
#elif ANEMONE_FEATURE_NEON
        float32x4_t const lane = vld1q_f32(F32x4_PositiveQNaN_XXXX.As<float>());
        return SimdMatrix4x4F{lane, lane, lane, lane};
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Infinity()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        SimdVector4F const lane = Vector4F_LoadAlignedFloat4(F32x4_PositiveInfinity_XXXX.As<float>());
        return SimdMatrix4x4F{lane, lane, lane, lane};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const lane = _mm_load_ps(F32x4_PositiveInfinity_XXXX.As<float>());
        return SimdMatrix4x4F{lane, lane, lane, lane};
#elif ANEMONE_FEATURE_NEON
        float32x4_t const lane = vld1q_f32(F32x4_PositiveInfinity_XXXX.As<float>());
        return SimdMatrix4x4F{lane, lane, lane, lane};
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateTranslation(float x, float y, float z)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        SimdMatrix4x4F result;

        result.Inner[0].Inner[0] = 1.0f;
        result.Inner[0].Inner[1] = 0.0f;
        result.Inner[0].Inner[2] = 0.0f;
        result.Inner[0].Inner[3] = 0.0f;

        result.Inner[1].Inner[0] = 0.0f;
        result.Inner[1].Inner[1] = 1.0f;
        result.Inner[1].Inner[2] = 0.0f;
        result.Inner[1].Inner[3] = 0.0f;

        result.Inner[2].Inner[0] = 0.0f;
        result.Inner[2].Inner[1] = 0.0f;
        result.Inner[2].Inner[2] = 1.0f;
        result.Inner[2].Inner[3] = 0.0f;

        result.Inner[3].Inner[0] = x;
        result.Inner[3].Inner[1] = y;
        result.Inner[3].Inner[2] = z;
        result.Inner[3].Inner[3] = 1.0f;

        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdMatrix4x4F{
            _mm_load_ps(F32x4_PositiveUnitX.As<float>()),
            _mm_load_ps(F32x4_PositiveUnitY.As<float>()),
            _mm_load_ps(F32x4_PositiveUnitZ.As<float>()),
            _mm_setr_ps(x, y, z, 1.0f),
        };
#elif ANEMONE_FEATURE_NEON
        float const last[4]{x, y, z, 1.0f};
        return SimdMatrix4x4F{
            vld1q_f32(F32x4_PositiveUnitX.As<float>()),
            vld1q_f32(F32x4_PositiveUnitY.As<float>()),
            vld1q_f32(F32x4_PositiveUnitZ.As<float>()),
            vld1q_f32(last),
        };
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateTranslation(SimdVector4F translation)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return Matrix4x4F_CreateTranslation(translation.Inner[0], translation.Inner[1], translation.Inner[2]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdMatrix4x4F{
            _mm_load_ps(F32x4_PositiveUnitX.As<float>()),
            _mm_load_ps(F32x4_PositiveUnitY.As<float>()),
            _mm_load_ps(F32x4_PositiveUnitZ.As<float>()),
            _mm_blend_ps(translation, _mm_load_ps(F32x4_PositiveOne.As<float>()), 0b1000),
        };
#elif ANEMONE_FEATURE_NEON
        return SimdMatrix4x4F{
            vld1q_f32(F32x4_PositiveUnitX.As<float>()),
            vld1q_f32(F32x4_PositiveUnitY.As<float>()),
            vld1q_f32(F32x4_PositiveUnitZ.As<float>()),
            vsetq_lane_f32(1.0f, translation, 3),
        };
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateScale(float x, float y, float z)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        SimdMatrix4x4F result;

        result.Inner[0].Inner[0] = x;
        result.Inner[0].Inner[1] = 0.0f;
        result.Inner[0].Inner[2] = 0.0f;
        result.Inner[0].Inner[3] = 0.0f;

        result.Inner[1].Inner[0] = 0.0f;
        result.Inner[1].Inner[1] = y;
        result.Inner[1].Inner[2] = 0.0f;
        result.Inner[1].Inner[3] = 0.0f;

        result.Inner[2].Inner[0] = 0.0f;
        result.Inner[2].Inner[1] = 0.0f;
        result.Inner[2].Inner[2] = z;
        result.Inner[2].Inner[3] = 0.0f;

        result.Inner[3].Inner[0] = 0.0f;
        result.Inner[3].Inner[1] = 0.0f;
        result.Inner[3].Inner[2] = 0.0f;
        result.Inner[3].Inner[3] = 1.0f;

        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdMatrix4x4F{
            _mm_setr_ps(x, 0.0f, 0.0f, 0.0f),
            _mm_setr_ps(0.0f, y, 0.0f, 0.0f),
            _mm_setr_ps(0.0f, 0.0f, z, 0.0f),
            _mm_load_ps(F32x4_PositiveUnitW.As<float>()),
        };
#elif ANEMONE_FEATURE_NEON
        float32x4_t const zero = vdupq_n_f32(0.0f);

        return SimdMatrix4x4F{
            vsetq_lane_f32(x, zero, 0),
            vsetq_lane_f32(y, zero, 1),
            vsetq_lane_f32(z, zero, 2),
            vld1q_f32(F32x4_PositiveUnitW.As<float>()),
        };
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateScale(SimdVector4F scale)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return Matrix4x4F_CreateScale(scale.Inner[0], scale.Inner[1], scale.Inner[2]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const zero = _mm_setzero_ps();
        return SimdMatrix4x4F{
            _mm_blend_ps(zero, scale, 0b0001),
            _mm_blend_ps(zero, scale, 0b0010),
            _mm_blend_ps(zero, scale, 0b0100),
            _mm_load_ps(F32x4_PositiveUnitW.As<float>()),
        };
#elif ANEMONE_FEATURE_NEON
        float32x4_t const zero = vdupq_n_f32(0.0f);

        return SimdMatrix4x4F{
            vcopyq_laneq_f32(zero, 0, scale, 0),
            vcopyq_laneq_f32(zero, 1, scale, 1),
            vcopyq_laneq_f32(zero, 2, scale, 2),
            vld1q_f32(F32x4_PositiveUnitW.As<float>()),
        };
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateRotationX(float angle)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        auto [sinAngle, cosAngle] = SinCos<float>(angle);

        SimdMatrix4x4F result;

        result.Inner[0].Inner[0] = 1.0f;
        result.Inner[0].Inner[1] = 0.0f;
        result.Inner[0].Inner[2] = 0.0f;
        result.Inner[0].Inner[3] = 0.0f;

        result.Inner[1].Inner[0] = 0.0f;
        result.Inner[1].Inner[1] = cosAngle;
        result.Inner[1].Inner[2] = sinAngle;
        result.Inner[1].Inner[3] = 0.0f;

        result.Inner[2].Inner[0] = 0.0f;
        result.Inner[2].Inner[1] = -sinAngle;
        result.Inner[2].Inner[2] = cosAngle;
        result.Inner[2].Inner[3] = 0.0f;

        result.Inner[3].Inner[0] = 0.0f;
        result.Inner[3].Inner[1] = 0.0f;
        result.Inner[3].Inner[2] = 0.0f;
        result.Inner[3].Inner[3] = 1.0f;

        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        auto [fSin, fCos] = SinCos<float>(angle);

        __m128 const vSin = _mm_set_ss(fSin);
        __m128 const vCos = _mm_set_ss(fCos);

        __m128 const negateY = _mm_load_ps(F32x4_Negate_nXnn.As<float>());

        __m128 r1 = _mm_shuffle_ps(vCos, vSin, _MM_SHUFFLE(3, 0, 0, 3));
        __m128 r2 = _mm_permute_ps(r1, _MM_SHUFFLE(3, 1, 2, 0));
        r2 = _mm_mul_ps(r2, negateY);

        SimdMatrix4x4F result;

        // = [1, 0, 0, 0]
        result.Inner[0] = _mm_load_ps(F32x4_PositiveUnitX.As<float>());
        // = [0, cos, sin, 0]
        result.Inner[1] = r1;
        // = [0, -sin, cos, 0]
        result.Inner[2] = r2;
        // = [0, 0, 0, 1]
        result.Inner[3] = _mm_load_ps(F32x4_PositiveUnitW.As<float>());

        return result;
#elif ANEMONE_FEATURE_NEON
        auto [fSin, fCos] = SinCos<float>(angle);

        float32x4_t const zero = vdupq_n_f32(0.0f);

        // = [0, fCos, fSin, 0]
        float32x4_t r1 = vsetq_lane_f32(fCos, zero, 1);
        r1 = vsetq_lane_f32(fSin, r1, 2);

        // = [0, -fSin, fCos, 0]
        float32x4_t r2 = vsetq_lane_f32(-fSin, zero, 1);
        r2 = vsetq_lane_f32(fCos, r2, 2);

        SimdMatrix4x4F result;

        result.val[0] = vld1q_f32(F32x4_PositiveUnitX.As<float>());
        result.val[1] = r1;
        result.val[2] = r2;
        result.val[3] = vld1q_f32(F32x4_PositiveUnitW.As<float>());

        return result;
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateRotationY(float angle)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        auto [sinAngle, cosAngle] = SinCos<float>(angle);

        SimdMatrix4x4F result;

        result.Inner[0].Inner[0] = cosAngle;
        result.Inner[0].Inner[1] = 0.0f;
        result.Inner[0].Inner[2] = -sinAngle;
        result.Inner[0].Inner[3] = 0.0f;

        result.Inner[1].Inner[0] = 0.0f;
        result.Inner[1].Inner[1] = 1.0f;
        result.Inner[1].Inner[2] = 0.0f;
        result.Inner[1].Inner[3] = 0.0f;

        result.Inner[2].Inner[0] = sinAngle;
        result.Inner[2].Inner[1] = 0.0f;
        result.Inner[2].Inner[2] = cosAngle;
        result.Inner[2].Inner[3] = 0.0f;

        result.Inner[3].Inner[0] = 0.0f;
        result.Inner[3].Inner[1] = 0.0f;
        result.Inner[3].Inner[2] = 0.0f;
        result.Inner[3].Inner[3] = 1.0f;

        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        auto [fSin, fCos] = SinCos<float>(angle);

        __m128 const vSin = _mm_set_ss(fSin);
        __m128 const vCos = _mm_set_ss(fCos);

        __m128 const negateZ = _mm_load_ps(F32x4_Negate_nnXn.As<float>());

        // = [sin, 0, cos, 0]
        __m128 r2 = _mm_shuffle_ps(vSin, vCos, _MM_SHUFFLE(3, 0, 3, 0));

        // = [cos, 0, -sin, 0]
        __m128 r0 = _mm_permute_ps(r2, _MM_SHUFFLE(3, 0, 1, 2));
        r0 = _mm_mul_ps(r0, negateZ);

        SimdMatrix4x4F result;
        result.Inner[0] = r0;
        result.Inner[1] = _mm_load_ps(F32x4_PositiveUnitY.As<float>());
        result.Inner[2] = r2;
        result.Inner[3] = _mm_load_ps(F32x4_PositiveUnitW.As<float>());

        return result;
#elif ANEMONE_FEATURE_NEON
        auto [fSin, fCos] = SinCos<float>(angle);

        float32x4_t const zero = vdupq_n_f32(0.0f);

        // = [fCos, 0, -fSin, 0]
        float32x4_t r0 = vsetq_lane_f32(fCos, zero, 0);
        r0 = vsetq_lane_f32(-fSin, r0, 2);

        // = [fSin, 0, fCos, 0]
        float32x4_t r2 = vsetq_lane_f32(fSin, zero, 0);
        r2 = vsetq_lane_f32(fCos, r2, 2);

        SimdMatrix4x4F result;

        result.val[0] = r0;
        result.val[1] = vld1q_f32(F32x4_PositiveUnitY.As<float>());
        result.val[2] = r2;
        result.val[3] = vld1q_f32(F32x4_PositiveUnitW.As<float>());

        return result;
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateRotationZ(float angle)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        auto [sinAngle, cosAngle] = SinCos<float>(angle);

        SimdMatrix4x4F result;

        result.Inner[0].Inner[0] = cosAngle;
        result.Inner[0].Inner[1] = sinAngle;
        result.Inner[0].Inner[2] = 0.0f;
        result.Inner[0].Inner[3] = 0.0f;

        result.Inner[1].Inner[0] = -sinAngle;
        result.Inner[1].Inner[1] = cosAngle;
        result.Inner[1].Inner[2] = 0.0f;
        result.Inner[1].Inner[3] = 0.0f;

        result.Inner[2].Inner[0] = 0.0f;
        result.Inner[2].Inner[1] = 0.0f;
        result.Inner[2].Inner[2] = 1.0f;
        result.Inner[2].Inner[3] = 0.0f;

        result.Inner[3].Inner[0] = 0.0f;
        result.Inner[3].Inner[1] = 0.0f;
        result.Inner[3].Inner[2] = 0.0f;
        result.Inner[3].Inner[3] = 1.0f;

        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        auto [fSin, fCos] = SinCos<float>(angle);

        __m128 const vSin = _mm_set_ss(fSin);
        __m128 const vCos = _mm_set_ss(fCos);

        __m128 const negateX = _mm_load_ps(F32x4_Negate_Xnnn.As<float>());

        // = [cos, sin, 0, 0]
        __m128 r0 = _mm_unpacklo_ps(vCos, vSin);

        // = [-sin, cos, 0, 0]
        __m128 r1 = _mm_permute_ps(r0, _MM_SHUFFLE(3, 2, 0, 1));
        r1 = _mm_mul_ps(r1, negateX);

        SimdMatrix4x4F result;
        result.Inner[0] = r0;
        result.Inner[1] = r1;
        result.Inner[2] = _mm_load_ps(F32x4_PositiveUnitZ.As<float>());
        result.Inner[3] = _mm_load_ps(F32x4_PositiveUnitW.As<float>());
        return result;
#elif ANEMONE_FEATURE_NEON
        auto [fSin, fCos] = SinCos<float>(angle);

        float32x4_t const zero = vdupq_n_f32(0.0f);

        // = [fCos, fSin, 0, 0]
        float32x4_t r0 = vsetq_lane_f32(fCos, zero, 0);
        r0 = vsetq_lane_f32(fSin, r0, 1);

        // = [-fSin, fCos, 0, 0]
        float32x4_t r1 = vsetq_lane_f32(-fSin, zero, 0);
        r1 = vsetq_lane_f32(fCos, r1, 1);

        SimdMatrix4x4F result;

        result.val[0] = r0;
        result.val[1] = r1;
        result.val[2] = vld1q_f32(F32x4_PositiveUnitZ.As<float>());
        result.val[3] = vld1q_f32(F32x4_PositiveUnitW.As<float>());

        return result;
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateFromQuaternion(SimdVector4F q)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const qx = q.Inner[0];
        float const qy = q.Inner[1];
        float const qz = q.Inner[2];
        float const qw = q.Inner[3];

        float const qxx = qx * qx;
        float const qyy = qy * qy;
        float const qzz = qz * qz;

        float const qxy = qx * qy;
        float const qzw = qz * qw;
        float const qxz = qx * qz;
        float const qyw = qy * qw;
        float const qyz = qy * qz;
        float const qxw = qx * qw;

        SimdMatrix4x4F result;

        result.Inner[0].Inner[0] = 1.0f - 2.0f * qyy - 2.0f * qzz;
        result.Inner[0].Inner[1] = 2.0f * qxy + 2.0f * qzw;
        result.Inner[0].Inner[2] = 2.0f * qxz - 2.0f * qyw;
        result.Inner[0].Inner[3] = 0.0f;

        result.Inner[1].Inner[0] = 2.0f * qxy - 2.0f * qzw;
        result.Inner[1].Inner[1] = 1.0f - 2.0f * qxx - 2.0f * qzz;
        result.Inner[1].Inner[2] = 2.0f * qyz + 2.0f * qxw;
        result.Inner[1].Inner[3] = 0.0f;

        result.Inner[2].Inner[0] = 2.0f * qxz + 2.0f * qyw;
        result.Inner[2].Inner[1] = 2.0f * qyz - 2.0f * qxw;
        result.Inner[2].Inner[2] = 1.0f - 2.0f * qxx - 2.0f * qyy;
        result.Inner[2].Inner[3] = 0.0f;

        result.Inner[3].Inner[0] = 0.0f;
        result.Inner[3].Inner[1] = 0.0f;
        result.Inner[3].Inner[2] = 0.0f;
        result.Inner[3].Inner[3] = 1.0f;

        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const maskXXXn = _mm_load_ps(F32x4_SelectMask_XXXn.As<float>());
        __m128 const const_1110 = _mm_setr_ps(1.0f, 1.0f, 1.0f, 0.0f);

        // 1. Precompute parts of 2 * qn * qm

        // = [q2x, q2y, q2z, q2w]
        __m128 const q2x_q2y_q2z_q2w = _mm_add_ps(q, q);

        // = [q2xx, q2yy, q2zz, q2ww]
        __m128 const q2xx_q2yy_q2zz_q2ww = _mm_mul_ps(q, q2x_q2y_q2z_q2w);

        // ---

        // 2. Compute values at diagonal of the matrix

        // = [q2yy, q2xx, q2xx, q2ww]
        __m128 const q2yy_q2xx_q2xx_q2ww = _mm_permute_ps(q2xx_q2yy_q2zz_q2ww, _MM_SHUFFLE(3, 0, 0, 1));
        // = [q2yy, q2xx, q2xx, 0]
        __m128 const q2yy_q2xx_q2xx = _mm_and_ps(q2yy_q2xx_q2xx_q2ww, maskXXXn);

        // = [q2zz, q2zz, q2yy, q2ww]
        __m128 const q2zz_q2zz_q2yy_q2ww = _mm_permute_ps(q2xx_q2yy_q2zz_q2ww, _MM_SHUFFLE(3, 1, 2, 2));

        // = [q2zz, q2zz, q2yy, 0]
        __m128 const q2zz_q2zz_q2yy = _mm_and_ps(q2zz_q2zz_q2yy_q2ww, maskXXXn);
        // = [1 - q2yy, 1 - q2xx, 1 - q2xx, 0]
        __m128 const one_sub_q2yy_q2xx_q2xx = _mm_sub_ps(const_1110, q2yy_q2xx_q2xx);

        // = [1 - q2yy -q2zz, 1 - q2xx - q2zz, 1 - q2xx - q2yy, 0]
        __m128 const m11_m22_m33_zzz = _mm_sub_ps(one_sub_q2yy_q2xx_q2xx, q2zz_q2zz_q2yy);

        // ---

        // 3. Compute rest of the matrix values

        // = [qx, qx, qy, qw]
        __m128 const qx_qx_qy_qw = _mm_permute_ps(q, _MM_SHUFFLE(3, 1, 0, 0));

        // = [q2z, q2y, q2z, q2w]
        __m128 const q2z_q2y_q2z_q2w = _mm_permute_ps(q2x_q2y_q2z_q2w, _MM_SHUFFLE(3, 2, 1, 2));

        // = [q2xz, q2xy, q2yz, q2ww]
        __m128 const q2xz_q2xy_q2yz_q2ww = _mm_mul_ps(qx_qx_qy_qw, q2z_q2y_q2z_q2w);

        // = [qw, qw, qw, qw]
        __m128 const qw_qw_qw_qw = _mm_permute_ps(q, _MM_SHUFFLE(3, 3, 3, 3));

        // = [q2y, q2z, q2x, q2w]
        __m128 const q2y_q2z_q2x_q2w = _mm_permute_ps(q2x_q2y_q2z_q2w, _MM_SHUFFLE(3, 0, 2, 1));

        // = [q2yw, q2zw, q2xw, q2ww]
        __m128 const q2yw_q2zw_q2x2_q2ww = _mm_mul_ps(qw_qw_qw_qw, q2y_q2z_q2x_q2w);

        // = [q2xz+q2yw, q2xy+q2zw, q2yz+q2xw]
        __m128 const m31_m12_m23 = _mm_add_ps(q2xz_q2xy_q2yz_q2ww, q2yw_q2zw_q2x2_q2ww);

        // = [q2xz-q2yw, q2xy-q2zw, q2yz-q2xw]
        __m128 const m13_m21_m32 = _mm_sub_ps(q2xz_q2xy_q2yz_q2ww, q2yw_q2zw_q2x2_q2ww);

        // = [q2xy+q2zw, q2yz+q2xw, q2xz-q2yw, q2xy-q2zw]
        __m128 const m12_m23_m13_m21 = _mm_shuffle_ps(m31_m12_m23, m13_m21_m32, _MM_SHUFFLE(1, 0, 2, 1));

        // = [q2xy+q2zw, q2xz-q2yw, q2xy-q2zw, q2yz+q2xw]
        __m128 const m12_m13_m21_m23 = _mm_permute_ps(m12_m23_m13_m21, _MM_SHUFFLE(1, 3, 2, 0));

        // = [q2xz+q2yw, q2xz+q2yw, q2yz-q2xw, q2yz-q2xw]
        __m128 const m31_m31_m32_m32 = _mm_shuffle_ps(m31_m12_m23, m13_m21_m32, _MM_SHUFFLE(2, 2, 0, 0));

        // = [q2xz+q2yw, q2yz-q2xw, q2xz+q2yw, q2yz-q2xw]
        __m128 const m31_m32_m13_m32 = _mm_permute_ps(m31_m31_m32_m32, _MM_SHUFFLE(2, 0, 2, 0));

        // = [1-q2yy-q2zz, 0, q2xy+q2zw, q2xz-q2yw]
        __m128 const m11_zzz_m12_m13 = _mm_shuffle_ps(m11_m22_m33_zzz, m12_m13_m21_m23, _MM_SHUFFLE(1, 0, 3, 0));

        // Compute row[0]
        // = [1-q2yy-q2zz, q2xy+q2zw, q2xz-q2yw, 0]
        __m128 const m11_m12_m13_zzz = _mm_permute_ps(m11_zzz_m12_m13, _MM_SHUFFLE(1, 3, 2, 0));

        // = [1-q2xx-q2zz, 0, q2xy-q2zw, q2yz+q2xw]
        __m128 const m22_zzz_m21_m23 = _mm_shuffle_ps(m11_m22_m33_zzz, m12_m13_m21_m23, _MM_SHUFFLE(3, 2, 3, 1));

        // = [q2xy-q2zw, 1-q2xx-q2zz, q2yz+q2xw, 0]
        __m128 const m21_m22_m23_zzz = _mm_permute_ps(m22_zzz_m21_m23, _MM_SHUFFLE(1, 3, 0, 2));

        // = [q2xz+q2yw, q2yz-q2xw, 1-q2xx-q2yy, 0]
        __m128 const m31_m32_m33_zzz = _mm_shuffle_ps(m31_m32_m13_m32, m11_m22_m33_zzz, _MM_SHUFFLE(3, 2, 1, 0));

        SimdMatrix4x4F result;
        result.Inner[0] = m11_m12_m13_zzz;
        result.Inner[1] = m21_m22_m23_zzz;
        result.Inner[2] = m31_m32_m33_zzz;
        result.Inner[3] = _mm_load_ps(F32x4_PositiveUnitW.As<float>());
        return result;
#elif ANEMONE_FEATURE_NEON
        // = [2x, 2y, 2z, 2w]
        float32x4_t const q2x_q2y_q2z_q2w = vaddq_f32(q, q);
        // = [2xx, 2yy, 2zz, 2ww]
        float32x4_t const q2xx_q2yy_q2zz_q2ww = vmulq_f32(q2x_q2y_q2z_q2w, q);

        // = [2xx, 2yy]
        float32x2_t const q2xx_q2yy = vget_low_f32(q2xx_q2yy_q2zz_q2ww);
        // = [2zz, 2ww]
        float32x2_t const q2zz_q2ww = vget_high_f32(q2xx_q2yy_q2zz_q2ww);
        // = [2zz, 2zz]
        float32x2_t const q2zz_q2zz = vdup_lane_f32(q2zz_q2ww, 0);
        // = [2yy, 2xx]
        float32x2_t const q2yy_q2xx = vrev64_f32(q2xx_q2yy);
        // = [2zz, 2zz, 2yy, 2xx]
        float32x4_t const q2zz_q2zz_q2yy_q2xx = vcombine_f32(q2zz_q2zz, q2yy_q2xx);
        // = [2xx, 2yy, 2xx, 2yy]
        float32x4_t const q2xx_q2yy_q2xx_q2yy = vcombine_f32(q2xx_q2yy, q2xx_q2yy);

        // result[1][1] = 1.0f - q2xx - q2zz;
        // result[0][0] = 1.0f - q2yy - q2zz;
        // result[2][2] = 1.0f - q2xx - q2yy;

        float32x4_t const m22_m11_m33 = vsubq_f32(vsubq_f32(vdupq_n_f32(1.0f), q2xx_q2yy_q2xx_q2yy), q2zz_q2zz_q2yy_q2xx);

        // = [w, w, w, w]
        float32x4_t const qw_qw_qw_qw = vdupq_laneq_f32(q, 3);
        // = [2xw, 2yw, 2zw, 2ww]
        float32x4_t const q2xw_q2yw_q2zw_q2ww = vmulq_f32(q2x_q2y_q2z_q2w, qw_qw_qw_qw);

        // = [x, y]
        float32x2_t const qx_qy = vget_low_f32(q);
        // = [z, w]
        float32x2_t const qz_qw = vget_high_f32(q);
        // = [y, x]
        float32x2_t const qy_qx = vrev64_f32(qx_qy);
        // = [z, z]
        float32x2_t const qz_qz = vdup_lane_f32(qz_qw, 0);
        // = [z, z, y, x]
        float32x4_t const qz_qz_qy_qx = vcombine_f32(qz_qz, qy_qx);
        // = [2x, 2y]
        float32x2_t const q2x_q2y = vget_low_f32(q2x_q2y_q2z_q2w);
        // = [2y, 2x]
        float32x2_t const q2y_q2x = vrev64_f32(q2x_q2y);
        // = [2y, 2x, 2x, 2y]
        float32x4_t const q2y_q2x_q2x_q2y = vcombine_f32(q2y_q2x, q2x_q2y);
        // = [2yz, 2xz, 2xy, 2xy]
        float32x4_t const q2yz_q2xz_q2xy_q2xy = vmulq_f32(q2y_q2x_q2x_q2y, qz_qz_qy_qx);

        // result[1][2] = q2yz + q2xw;
        // result[2][0] = q2xz + q2yw;
        // result[0][1] = q2xy + q2zw;
        float32x4_t const m23_m31_m12 = vaddq_f32(q2yz_q2xz_q2xy_q2xy, q2xw_q2yw_q2zw_q2ww);

        // result[2][1] = q2yz - q2xw;
        // result[0][2] = q2xz - q2yw;
        // result[1][0] = q2xy - q2zw;
        float32x4_t const m32_m13_m21 = vsubq_f32(q2yz_q2xz_q2xy_q2xy, q2xw_q2yw_q2zw_q2ww);

        // Swizzle to get the final result
        float32x2_t const m22_m11 = vget_low_f32(m22_m11_m33);
        float32x2_t const m33_xxx = vget_high_f32(m22_m11_m33);
        float32x2_t const m23_m31 = vget_low_f32(m23_m31_m12);
        float32x2_t const m12_xxx = vget_high_f32(m23_m31_m12);
        float32x2_t const m32_m13 = vget_low_f32(m32_m13_m21);
        float32x2_t const m21_xxx = vget_high_f32(m32_m13_m21);

        // = [0, 0]
        float32x2_t const zero = vdup_n_f32(0.0f);
        // = [m11, m12]
        float32x2_t const m11_m12 = vext_f32(m22_m11, m12_xxx, 1);
        // = [m13, 0]
        float32x2_t const m13_000 = vext_f32(m32_m13, zero, 1);
        // = [m21, m22]
        float32x2_t const m21_m22 = vcopy_lane_f32(m21_xxx, 1, m22_m11, 0);
        // = [m23, 0]
        float32x2_t const m23_000 = vcopy_lane_f32(m23_m31, 1, zero, 0);
        // = [m31, m32]
        float32x2_t const m31_m32 = vext_f32(m23_m31, m32_m13, 1);
        // = [m33, 0]
        float32x2_t const m33_000 = vcopy_lane_f32(m33_xxx, 1, zero, 0);

        // = [m11, m12, m13, 0]
        float32x4_t const m11_m12_m13_000 = vcombine_f32(m11_m12, m13_000);
        // = [m21, m22, m23, 0]
        float32x4_t const m21_m22_m23_000 = vcombine_f32(m21_m22, m23_000);
        // = [m31, m32, m33, 0]
        float32x4_t const m31_m32_m33_000 = vcombine_f32(m31_m32, m33_000);

        SimdMatrix4x4F result;

        result.val[0] = m11_m12_m13_000;
        result.val[1] = m21_m22_m23_000;
        result.val[2] = m31_m32_m33_000;
        result.val[3] = vld1q_f32(F32x4_PositiveUnitW.As<float>());

        return result;
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateFromPitchYawRoll(float pitch, float yaw, float roll)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        auto [sp, cp] = SinCos<float>(pitch);
        auto [sy, cy] = SinCos<float>(yaw);
        auto [sr, cr] = SinCos<float>(roll);

        SimdMatrix4x4F result;

        result.Inner[0].Inner[0] = cr * cy + sr * sp * sy;
        result.Inner[0].Inner[1] = sr * cp;
        result.Inner[0].Inner[2] = sr * sp * cy - cr * sy;
        result.Inner[0].Inner[3] = 0.0f;

        result.Inner[1].Inner[0] = cr * sp * sy - sr * cy;
        result.Inner[1].Inner[1] = cr * cp;
        result.Inner[1].Inner[2] = sr * sy + cr * sp * cy;
        result.Inner[1].Inner[3] = 0.0f;

        result.Inner[2].Inner[0] = cp * sy;
        result.Inner[2].Inner[1] = -sp;
        result.Inner[2].Inner[2] = cp * cy;
        result.Inner[2].Inner[3] = 0.0f;

        result.Inner[3].Inner[0] = 0.0f;
        result.Inner[3].Inner[1] = 0.0f;
        result.Inner[3].Inner[2] = 0.0f;
        result.Inner[3].Inner[3] = 1.0f;

        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return Matrix4x4F_CreateFromPitchYawRoll(_mm_setr_ps(pitch, yaw, roll, 0.0f));
#elif ANEMONE_FEATURE_NEON
        float const angles[4]{pitch, yaw, roll, 0.0f};
        return Matrix4x4F_CreateFromPitchYawRoll(vld1q_f32(angles));
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateFromPitchYawRoll(SimdVector4F pitchYawRoll)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return Matrix4x4F_CreateFromPitchYawRoll(pitchYawRoll.Inner[0], pitchYawRoll.Inner[1], pitchYawRoll.Inner[2]);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const sign = _mm_setr_ps(1.0f, -1.0f, -1.0f, 1.0f);

        // = [sp, sy, sr, _]
        __m128 sp_sy_sr;
        // = [cp, cy, cr, _]
        __m128 cp_cy_cr;
        Vector4F_SinCos(sp_sy_sr, cp_cy_cr, pitchYawRoll);

        // = [cp, cp, cr, cp]
        __m128 const cp_cp_cr_cp = _mm_permute_ps(cp_cy_cr, _MM_SHUFFLE(0, 2, 0, 0));
        // = [cp, sr, cr, cp]
        __m128 const cp_sr_cr_cp = _mm_insert_ps(cp_cp_cr_cp, sp_sy_sr, 0b10'01'0000);
        // = [cp, cp, cp, cy]
        __m128 const cp_cp_cp_cy = _mm_permute_ps(cp_cy_cr, _MM_SHUFFLE(1, 0, 0, 0));
        // = [sy, cp, cp, cy]
        __m128 const sy_cp_cp_cy = _mm_insert_ps(cp_cp_cp_cy, sp_sy_sr, 0b01'00'0000);
        // = [cr, sr, cr, sr]
        __m128 const cr_sr_cr_sr = _mm_permute_ps(cp_sr_cr_cp, _MM_SHUFFLE(1, 2, 1, 2));
        // = [cy, cy, sy, sy,]
        __m128 const cy_cy_sy_sy = _mm_permute_ps(sy_cp_cp_cy, _MM_SHUFFLE(0, 0, 3, 3));
        // = [sr, cr, sr, cr]
        __m128 const sr_cr_sr_cr = _mm_permute_ps(cp_sr_cr_cp, _MM_SHUFFLE(2, 1, 2, 1));
        // = [sy, sy, cy, cy]
        __m128 const sy_sy_cy_cy = _mm_permute_ps(sy_cp_cp_cy, _MM_SHUFFLE(3, 3, 0, 0));
        // = [sp, sp, sp, sp]
        __m128 const sp_sp_sp_sp = _mm_permute_ps(sp_sy_sr, _MM_SHUFFLE(0, 0, 0, 0));

        __m128 const m32 = Vector4F_Negate(sp_sy_sr);

        // = [
        //      cp * sy
        //      sr * cp
        //      cr * cp
        //      cp * cy
        // ]
        __m128 const m31_m12_m22_m33 = _mm_mul_ps(cp_sr_cr_cp, sy_cp_cp_cy);

        // = [
        //      cr
        //      -sr
        //      -cr
        //      sr
        // ]
        __m128 const t0 = _mm_mul_ps(cr_sr_cr_sr, sign);

        // = [
        //      cr * cy
        //      -sr * cy
        //      -cr * sy
        //      sr * sy
        // ]
        __m128 const t1 = _mm_mul_ps(t0, cy_cy_sy_sy);

        // = [
        //      sr * sp
        //      cr * sp
        //      sr * sp
        //      cr * sp
        // ]
        __m128 const t2 = _mm_mul_ps(sr_cr_sr_cr, sp_sp_sp_sp);

        // = [
        //      sr * sp * sy + cr * cy
        //      cr * sp * sy - sr * cy
        //      sr * sp * cy - cr * sy
        //      cr * sp * cy + sr * sy
        // ]
        __m128 const m11_m21_m13_m23 = Vector4F_MultiplyAdd(t2, sy_sy_cy_cy, t1);

        // = [m11, m12, m13, 0]
        __m128 const m11_m12_m13 = _mm_insert_ps(m11_m21_m13_m23, m31_m12_m22_m33, 0b01'01'1000);
        // = [m21, m13, m23, m23]
        __m128 const m21_m13_m23_m23 = _mm_permute_ps(m11_m21_m13_m23, _MM_SHUFFLE(3, 3, 2, 1));
        // = [m21, m22, m23, 0]
        __m128 const m21_m22_m23 = _mm_insert_ps(m21_m13_m23_m23, m31_m12_m22_m33, 0b10'01'1000);
        // = [m31, m33, m33, m33]
        __m128 const m31_m33_m33_m33 = _mm_permute_ps(m31_m12_m22_m33, _MM_SHUFFLE(3, 3, 3, 0));
        // = [m31, m32, m33, 0]
        __m128 const m31_m32_m33 = _mm_insert_ps(m31_m33_m33_m33, m32, 0b00'01'1000);

        SimdMatrix4x4F result;
        result.Inner[0] = m11_m12_m13;
        result.Inner[1] = m21_m22_m23;
        result.Inner[2] = m31_m32_m33;
        result.Inner[3] = _mm_load_ps(F32x4_PositiveUnitW.As<float>());
        return result;
#elif ANEMONE_FEATURE_NEON
        float32x4_t sp_sy_sr;
        float32x4_t cp_cy_cr;
        Vector4F_SinCos(sp_sy_sr, cp_cy_cr, pitchYawRoll);

        float32x2_t const sp_sy = vget_low_f32(sp_sy_sr);
        float32x2_t const sr = vget_high_f32(sp_sy_sr);
        float32x2_t const cp_cy = vget_low_f32(cp_cy_cr);
        float32x2_t const cr = vget_high_f32(cp_cy_cr);


        // = [sy, sy]
        float32x2_t const sy_sy = vdup_lane_f32(sp_sy, 1);
        // = [cy, cy]
        float32x2_t const cy_cy = vdup_lane_f32(cp_cy, 1);
        // = [cp, cp]
        float32x2_t const cp_cp = vdup_lane_f32(cp_cy, 0);
        // = [cp, sr]
        float32x2_t const cp_sr = vcopy_lane_f32(cp_cy, 1, sr, 0);
        // = [cr, cp]
        float32x2_t const cr_cp = vcopy_lane_f32(cp_cp, 0, cr, 0);
        // = [sy, cp]
        float32x2_t const sy_cp = vext_f32(sp_sy, cp_cy, 1);
        // = [sr, cr]
        float32x2_t const sr_cr = vext_f32(cp_sr, cr, 1);
        // = [cr, sr]
        float32x2_t const cr_sr = vrev64_f32(sr_cr);

        // = [cp, sr, cr, cp]
        float32x4_t const cp_sr_cr_cp = vcombine_f32(cp_sr, cr_cp);
        // = [sy, cp, cp, cy]
        float32x4_t const sy_cp_cp_cy = vcombine_f32(sy_cp, cp_cy);
        // = [cr, sr, cr, sr]
        float32x4_t const cr_sr_cr_sr = vcombine_f32(cr_sr, cr_sr);
        // = [cy, cy, sy, sy]
        float32x4_t const cy_cy_sy_sy = vcombine_f32(cy_cy, sy_sy);
        // = [sr, cr, sr, cr]
        float32x4_t const sr_cr_sr_cr = vcombine_f32(sr_cr, sr_cr);
        // = [sy, sy, cy, cy]
        float32x4_t const sy_sy_cy_cy = vcombine_f32(sy_sy, cy_cy);
        // = [sp, sp, sp, sp]
        float32x4_t const sp_sp_sp_sp = vdupq_lane_f32(sp_sy, 0);

        // = [m32, _, _, _]
        float32x4_t const m32 = vnegq_f32(sp_sy_sr);

        // = [
        //      cp * sy
        //      sr * cp
        //      cr * cp
        //      cp * cy
        // ]
        float32x4_t const m31_m12_m22_m33 = vmulq_f32(cp_sr_cr_cp, sy_cp_cp_cy);

        // = [
        //      cr
        //      -sr
        //      -cr
        //      sr
        // ]
        static constexpr float fsign[4]{1.0f, -1.0f, -1.0f, 1.0f};
        float32x4_t const sign = vld1q_f32(fsign);
        float32x4_t const t0 = vmulq_f32(cr_sr_cr_sr, sign);

        // = [
        //      cr * cy
        //      -sr * cy
        //      -cr * sy
        //      sr * sy
        // ]
        float32x4_t const t1 = vmulq_f32(t0, cy_cy_sy_sy);

        // = [
        //      sr * sp
        //      cr * sp
        //      sr * sp
        //      cr * sp
        // ]
        float32x4_t const t2 = vmulq_f32(sr_cr_sr_cr, sp_sp_sp_sp);

        // = [
        //      sr * sp * sy + cr * cy
        //      cr * sp * sy - sr * cy
        //      sr * sp * cy - cr * sy
        //      cr * sp * cy + sr * sy
        // ]
        float32x4_t const m11_m21_m13_m23 = vmlaq_f32(t1, sy_sy_cy_cy, t2);
        // Vector4F_MultiplyAdd(t2, sy_sy_cy_cy, t1);

        // m11_m21, m13_m23, m32_xxx, m31_m12, m22_m33

        float32x2_t const zero = vdup_n_f32(0.0f);
        float32x2_t const m11_m21 = vget_low_f32(m11_m21_m13_m23);
        float32x2_t const m13_m23 = vget_high_f32(m11_m21_m13_m23);
        float32x2_t const m31_m12 = vget_low_f32(m31_m12_m22_m33);
        float32x2_t const m22_m33 = vget_high_f32(m31_m12_m22_m33);

        // = [m11, m12, m13, 0]
        float32x2_t const m11_m12 = vcopy_lane_f32(m11_m21, 1, m31_m12, 1);
        float32x2_t const m13_000 = vset_lane_f32(0.0f, m13_m23, 1);
        float32x2_t const m21_m22 = vext_f32(m11_m21, m22_m33, 1);
        float32x2_t const m23_000 = vext_f32(m13_m23, zero, 1);
        float32x2_t const m31_m32 = vcopy_laneq_f32(m31_m12, 1, m32, 0);
        float32x2_t const m33_000 = vext_f32(m22_m33, zero, 1);

        SimdMatrix4x4F result;

        result.val[0] = vcombine_f32(m11_m12, m13_000);
        result.val[1] = vcombine_f32(m21_m22, m23_000);
        result.val[2] = vcombine_f32(m31_m32, m33_000);
        result.val[3] = vld1q_f32(F32x4_PositiveUnitW.As<float>());

        return result;
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateAffineTransform2(
        SimdVector4F scaling,
        SimdVector4F rotationOrigin,
        float rotation,
        SimdVector4F translation)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        // = [scaling.x, scaling.y, 1, 1]
        SimdVector4F const vScaling{
            scaling.Inner[0],
            scaling.Inner[1],
            1.0f,
            0.0f,
        };

        // = [rotationOrigin.x, rotationOrigin.y, 0, 0]
        SimdVector4F const vRotationOrigin{
            rotationOrigin.Inner[0],
            rotationOrigin.Inner[1],
            0.0f,
            0.0f,
        };

        // = [translation.x, translation.y, 0, 0]
        SimdVector4F const vTranslation{
            translation.Inner[0],
            translation.Inner[1],
            0.0f,
            0.0f,
        };

        // 1. Apply scaling
        SimdMatrix4x4F mResult = Matrix4x4F_CreateScale(vScaling);
        // 2. Move to rotation origin
        mResult.Inner[3] = Vector4F_Subtract(mResult.Inner[3], vRotationOrigin);
        // 3. Apply rotation
        mResult = Matrix4x4F_Multiply(mResult, Matrix4x4F_CreateRotationZ(rotation));
        // 4. Restore rotation origin
        mResult.Inner[3] = Vector4F_Add(mResult.Inner[3], vRotationOrigin);
        // 5. Apply translation
        mResult.Inner[3] = Vector4F_Add(mResult.Inner[3], vTranslation);
        return mResult;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // = [scaling.x, scaling.y, 1, 1]
        __m128 const vScaling = _mm_blend_ps(scaling, _mm_load_ps(F32x4_PositiveOne.As<float>()), 0b1100);
        __m128 const vZero = _mm_setzero_ps();
        // = [rotationOrigin.x, rotationOrigin.y, 0, 0]
        __m128 const vRotationOrigin = _mm_blend_ps(rotationOrigin, vZero, 0b1100);
        // = [translation.x, translation.y, 0, 0]
        __m128 const vTranslation = _mm_blend_ps(translation, vZero, 0b1100);

        // 1. Apply scaling
        SimdMatrix4x4F mResult = Matrix4x4F_CreateScale(vScaling);
        // 2. Move to rotation origin
        mResult.Inner[3] = _mm_sub_ps(mResult.Inner[3], vRotationOrigin);
        // 3. Apply rotation
        mResult = Matrix4x4F_Multiply(mResult, Matrix4x4F_CreateRotationZ(rotation));
        // 4. Restore rotation origin
        mResult.Inner[3] = _mm_add_ps(mResult.Inner[3], vRotationOrigin);
        // 5. Apply translation
        mResult.Inner[3] = _mm_add_ps(mResult.Inner[3], vTranslation);
        return mResult;
#elif ANEMONE_FEATURE_NEON
        float32x2_t const ones = vdup_n_f32(1.0f);
        float32x2_t const zero = vdup_n_f32(0.0f);
        // = [scaling.x, scaling.y, 1, 1]
        float32x4_t const vScaling = vcombine_f32(vget_low_f32(scaling), ones);
        // = [rotationOrigin.x, rotationOrigin.y, 0, 0]
        float32x4_t const vRotationOrigin = vcombine_f32(vget_low_f32(rotationOrigin), zero);
        // = [translation.x, translation.y, 0, 0]
        float32x4_t const vTranslation = vcombine_f32(vget_low_f32(translation), zero);

        // 1. Apply scaling
        SimdMatrix4x4F mResult = Matrix4x4F_CreateScale(vScaling);
        // 2. Move to rotation origin
        mResult.val[3] = vsubq_f32(mResult.val[3], vRotationOrigin);
        // 3. Apply rotation
        mResult = Matrix4x4F_Multiply(mResult, Matrix4x4F_CreateRotationZ(rotation));
        // 4. Restore rotation origin
        mResult.val[3] = vaddq_f32(mResult.val[3], vRotationOrigin);
        // 5. Apply translation
        mResult.val[3] = vaddq_f32(mResult.val[3], vTranslation);
        return mResult;
#endif
    }


    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateAffineTransform3(
        SimdVector4F scaling,
        SimdVector4F rotationOrigin,
        SimdVector4F rotationQuaternion,
        SimdVector4F translation)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        // = [rotationOrigin.x, rotationOrigin.y, rotationOrigin.z, 0]
        SimdVector4F const vRotationOrigin{
            rotationOrigin.Inner[0],
            rotationOrigin.Inner[1],
            rotationOrigin.Inner[2],
            0.0f,
        };

        // = [translation.x, translation.y, translation.z, 0]
        SimdVector4F const vTranslation{
            translation.Inner[0],
            translation.Inner[1],
            translation.Inner[2],
            0.0f,
        };

        SimdMatrix4x4F mRotation = Matrix4x4F_CreateFromQuaternion(rotationQuaternion);

        // 1. Apply scaling
        SimdMatrix4x4F mResult = Matrix4x4F_CreateScale(scaling);
        // 2. Move to rotation origin
        mResult.Inner[3] = Vector4F_Subtract(mResult.Inner[3], vRotationOrigin);
        // 3. Apply rotation
        mResult = Matrix4x4F_Multiply(mResult, mRotation);
        // 4. Restore rotation origin
        mResult.Inner[3] = Vector4F_Add(mResult.Inner[3], vRotationOrigin);
        // 5. Apply translation
        mResult.Inner[3] = Vector4F_Add(mResult.Inner[3], vTranslation);
        return mResult;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const vZero = _mm_setzero_ps();

        // = [rotationOrigin.x, rotationOrigin.y, rotationOrigin.z, 0]
        __m128 const vRotationOrigin = _mm_blend_ps(rotationOrigin, vZero, 0b1000);
        // = [translation.x, translation.y, translation.z, 0]
        __m128 const vTranslation = _mm_blend_ps(translation, vZero, 0b1000);

        SimdMatrix4x4F mRotation = Matrix4x4F_CreateFromQuaternion(rotationQuaternion);

        // 1. Apply scaling
        SimdMatrix4x4F mResult = Matrix4x4F_CreateScale(scaling);
        // 2. Move to rotation origin
        mResult.Inner[3] = _mm_sub_ps(mResult.Inner[3], vRotationOrigin);
        // 3. Apply rotation
        mResult = Matrix4x4F_Multiply(mResult, mRotation);
        // 4. Restore rotation origin
        mResult.Inner[3] = _mm_add_ps(mResult.Inner[3], vRotationOrigin);
        // 5. Apply translation
        mResult.Inner[3] = _mm_add_ps(mResult.Inner[3], vTranslation);
        return mResult;
#elif ANEMONE_FEATURE_NEON
        // = [rotationOrigin.x, rotationOrigin.y, rotationOrigin.z, 0]
        float32x4_t const vRotationOrigin = vsetq_lane_f32(0.0f, rotationOrigin, 3);
        // = [translation.x, translation.y, translation.z, 0]
        float32x4_t const vTranslation = vsetq_lane_f32(0.0f, translation, 3);

        SimdMatrix4x4F mRotation = Matrix4x4F_CreateFromQuaternion(rotationQuaternion);

        // 1. Apply scaling
        SimdMatrix4x4F mResult = Matrix4x4F_CreateScale(scaling);
        // 2. Move to rotation origin
        mResult.val[3] = vsubq_f32(mResult.val[3], vRotationOrigin);
        // 3. Apply rotation
        mResult = Matrix4x4F_Multiply(mResult, mRotation);
        // 4. Restore rotation origin
        mResult.val[3] = vaddq_f32(mResult.val[3], vRotationOrigin);
        // 5. Apply translation
        mResult.val[3] = vaddq_f32(mResult.val[3], vTranslation);
        return mResult;
#endif
    }


    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateLookAtLH(SimdVector4F eye, SimdVector4F focus, SimdVector4F up)
    {
        SimdVector4F const direction = Vector4F_Subtract(focus, eye);
        return Matrix4x4F_CreateLookToLH(eye, direction, up);
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateLookAtRH(SimdVector4F eye, SimdVector4F focus, SimdVector4F up)
    {
        SimdVector4F const direction = Vector4F_Subtract(eye, focus);
        return Matrix4x4F_CreateLookToRH(eye, direction, up);
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateLookToLH(SimdVector4F eye, SimdVector4F direction, SimdVector4F up)
    {
        SimdVector4F const r2 = Vector4F_Normalize3(direction);
        SimdVector4F const r0 = Vector4F_Normalize3(Vector4F_Cross3(up, r2));
        SimdVector4F const r1 = Vector4F_Cross3(r2, r0);
        SimdVector4F const negEye = Vector4F_Negate(eye);

        SimdVector4F const d0 = Vector4F_Dot3(r0, negEye);
        SimdVector4F const d1 = Vector4F_Dot3(r1, negEye);
        SimdVector4F const d2 = Vector4F_Dot3(r2, negEye);

        SimdMatrix4x4F result;

#if ANEMONE_FEATURE_NEON
        result.val[0] = Vector4F_Select<true, true, true, false>(d0, r0);
        result.val[1] = Vector4F_Select<true, true, true, false>(d1, r1);
        result.val[2] = Vector4F_Select<true, true, true, false>(d2, r2);
        result.val[3] = Vector4F_PositiveUnitW();
#else
        result.Inner[0] = Vector4F_Select<true, true, true, false>(d0, r0);
        result.Inner[1] = Vector4F_Select<true, true, true, false>(d1, r1);
        result.Inner[2] = Vector4F_Select<true, true, true, false>(d2, r2);
        result.Inner[3] = Vector4F_PositiveUnitW();
#endif

        return result;
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateLookToRH(SimdVector4F eye, SimdVector4F direction, SimdVector4F up)
    {
        SimdVector4F const negDirection = Vector4F_Negate(direction);
        return Matrix4x4F_CreateLookToLH(eye, negDirection, up);
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreatePerspectiveLH(float width, float height, float zNear, float zFar)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const fRange = zFar / (zFar - zNear);
        float const fTwoNearZ = zNear + zNear;

        SimdMatrix4x4F result;

        result.Inner[0].Inner[0] = fTwoNearZ / width;
        result.Inner[0].Inner[1] = 0.0f;
        result.Inner[0].Inner[2] = 0.0f;
        result.Inner[0].Inner[3] = 0.0f;

        result.Inner[1].Inner[0] = 0.0f;
        result.Inner[1].Inner[1] = fTwoNearZ / height;
        result.Inner[1].Inner[2] = 0.0f;
        result.Inner[1].Inner[3] = 0.0f;

        result.Inner[2].Inner[0] = 0.0f;
        result.Inner[2].Inner[1] = 0.0f;
        result.Inner[2].Inner[2] = fRange;
        result.Inner[2].Inner[3] = 1.0f;

        result.Inner[3].Inner[0] = 0.0f;
        result.Inner[3].Inner[1] = 0.0f;
        result.Inner[3].Inner[2] = -fRange * zNear;
        result.Inner[3].Inner[3] = 0.0f;

        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        float const fRange = zFar / (zFar - zNear);
        float const fTwoNearZ = zNear + zNear;

        __m128 const zero = _mm_setzero_ps();
        __m128 const unitW = _mm_load_ps(F32x4_PositiveUnitW.As<float>());
        __m128 const values = _mm_setr_ps(
            fTwoNearZ / width,
            fTwoNearZ / height,
            -fRange * zNear,
            fRange);

        SimdMatrix4x4F result;

        // = [fTwoNearZ / width, 0, 0, 0]
        result.Inner[0] = _mm_move_ss(zero, values);
        // = [0, fTwoNearZ / height, 0, 0]
        result.Inner[1] = _mm_blend_ps(zero, values, 0b0010);
        // = [0, 0, fRange, 1]
        result.Inner[2] = _mm_insert_ps(unitW, values, 0b11'10'0000);
        // = [0, 0, -fRange * zNear, 0]
        result.Inner[3] = _mm_blend_ps(zero, values, 0b0100);

        return result;
#elif ANEMONE_FEATURE_NEON
        float const fRange = zFar / (zFar - zNear);
        float const fTwoNearZ = zNear + zNear;

        float32x4_t const zero = vdupq_n_f32(0.0f);
        float32x4_t const unitW = vld1q_f32(F32x4_PositiveUnitW.As<float>());

        SimdMatrix4x4F result;

        // = [fTwoNearZ / width, 0, 0, 0]
        result.val[0] = vsetq_lane_f32(fTwoNearZ / width, zero, 0);
        // = [fTwoNearZ / height, 0, 0, 0]
        result.val[1] = vsetq_lane_f32(fTwoNearZ / height, zero, 1);
        // = [fRange, 0, 0, 1]
        result.val[2] = vsetq_lane_f32(fRange, unitW, 2);
        // = [0, 0, fRange * zNear, 0]
        result.val[3] = vsetq_lane_f32(-fRange * zNear, zero, 2);

        return result;
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreatePerspectiveRH(float width, float height, float zNear, float zFar)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const fRange = zFar / (zNear - zFar);
        float const fTwoNearZ = zNear + zNear;

        SimdMatrix4x4F result;

        result.Inner[0].Inner[0] = fTwoNearZ / width;
        result.Inner[0].Inner[1] = 0.0f;
        result.Inner[0].Inner[2] = 0.0f;
        result.Inner[0].Inner[3] = 0.0f;

        result.Inner[1].Inner[0] = 0.0f;
        result.Inner[1].Inner[1] = fTwoNearZ / height;
        result.Inner[1].Inner[2] = 0.0f;
        result.Inner[1].Inner[3] = 0.0f;

        result.Inner[2].Inner[0] = 0.0f;
        result.Inner[2].Inner[1] = 0.0f;
        result.Inner[2].Inner[2] = fRange;
        result.Inner[2].Inner[3] = -1.0f;

        result.Inner[3].Inner[0] = 0.0f;
        result.Inner[3].Inner[1] = 0.0f;
        result.Inner[3].Inner[2] = fRange * zNear;
        result.Inner[3].Inner[3] = 0.0f;

        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        float const fRange = zFar / (zNear - zFar);
        float const fTwoNearZ = zNear + zNear;

        __m128 const zero = _mm_setzero_ps();
        __m128 const unitW = _mm_load_ps(F32x4_NegativeUnitW.As<float>());
        __m128 const values = _mm_setr_ps(
            fTwoNearZ / width,
            fTwoNearZ / height,
            fRange * zNear,
            fRange);

        SimdMatrix4x4F result;

        // = [fTwoNearZ / width, 0, 0, 0]
        result.Inner[0] = _mm_move_ss(zero, values);

        // = [0, fTwoNearZ / height, 0, 0]
        result.Inner[1] = _mm_blend_ps(zero, values, 0b0010);

        // = [0, 0, fRange, -1]
        result.Inner[2] = _mm_insert_ps(unitW, values, 0b11'10'0000);

        // = [0, 0, fRange * zNear, 0]
        result.Inner[3] = _mm_blend_ps(zero, values, 0b0100);

        return result;
#elif ANEMONE_FEATURE_NEON
        float const fRange = zFar / (zNear - zFar);
        float const fTwoNearZ = zNear + zNear;

        float32x4_t const zero = vdupq_n_f32(0.0f);
        float32x4_t const unitW = vld1q_f32(F32x4_NegativeUnitW.As<float>());

        SimdMatrix4x4F result;

        // = [fTwoNearZ / width, 0, 0, 0]
        result.val[0] = vsetq_lane_f32(fTwoNearZ / width, zero, 0);
        // = [0, fTwoNearZ / height, 0, 0]
        result.val[1] = vsetq_lane_f32(fTwoNearZ / height, zero, 1);
        // = [0, 0, fRange, -1]
        result.val[2] = vsetq_lane_f32(fRange, unitW, 2);
        // = [0, 0, fRange * zNear, 0]
        result.val[3] = vsetq_lane_f32(fRange * zNear, zero, 2);

        return result;
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreatePerspectiveFovLH(float fov, float aspect, float zNear, float zFar)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        auto [fSin, fCos] = SinCos<float>(0.5f * fov);

        float const fRange = zFar / (zFar - zNear);
        float const height = fCos / fSin;
        float const width = height / aspect;

        SimdMatrix4x4F result;

        result.Inner[0].Inner[0] = width;
        result.Inner[0].Inner[1] = 0.0f;
        result.Inner[0].Inner[2] = 0.0f;
        result.Inner[0].Inner[3] = 0.0f;

        result.Inner[1].Inner[0] = 0.0f;
        result.Inner[1].Inner[1] = height;
        result.Inner[1].Inner[2] = 0.0f;
        result.Inner[1].Inner[3] = 0.0f;

        result.Inner[2].Inner[0] = 0.0f;
        result.Inner[2].Inner[1] = 0.0f;
        result.Inner[2].Inner[2] = fRange;
        result.Inner[2].Inner[3] = 1.0f;

        result.Inner[3].Inner[0] = 0.0f;
        result.Inner[3].Inner[1] = 0.0f;
        result.Inner[3].Inner[2] = -fRange * zNear;
        result.Inner[3].Inner[3] = 0.0f;

        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        auto [fSin, fCos] = SinCos<float>(0.5f * fov);

        float const fRange = zFar / (zFar - zNear);
        float const height = fCos / fSin;
        float const width = height / aspect;

        __m128 const zero = _mm_setzero_ps();
        __m128 const unitW = _mm_load_ps(F32x4_PositiveUnitW.As<float>());
        __m128 const values = _mm_setr_ps(
            width,
            height,
            -fRange * zNear,
            fRange);

        SimdMatrix4x4F result;

        // = [width, 0, 0, 0]
        result.Inner[0] = _mm_move_ss(zero, values);

        // = [0, height, 0, 0]
        result.Inner[1] = _mm_blend_ps(zero, values, 0b0010);

        // = [0, 0, fRange, 1]
        result.Inner[2] = _mm_insert_ps(unitW, values, 0b11'10'0000);

        // = [0, 0, -fRange * zNear, 0]
        result.Inner[3] = _mm_blend_ps(zero, values, 0b0100);

        return result;
#elif ANEMONE_FEATURE_NEON
        auto [fSin, fCos] = SinCos<float>(0.5f * fov);

        float const fRange = zFar / (zFar - zNear);
        float const height = fCos / fSin;
        float const width = height / aspect;

        float32x4_t const zero = vdupq_n_f32(0.0f);
        float32x4_t const unitW = vld1q_f32(F32x4_PositiveUnitW.As<float>());

        SimdMatrix4x4F result;

        // = [width, 0, 0, 0]
        result.val[0] = vsetq_lane_f32(width, zero, 0);

        // = [0, height, 0, 0]
        result.val[1] = vsetq_lane_f32(height, zero, 1);

        // = [0, 0, fRange, 1]
        result.val[2] = vsetq_lane_f32(fRange, unitW, 2);

        // = [0, 0, -fRange * zNear, 0]
        result.val[3] = vsetq_lane_f32(-fRange * zNear, zero, 2);

        return result;
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreatePerspectiveFovRH(float fov, float aspect, float zNear, float zFar)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        auto [fSin, fCos] = SinCos<float>(0.5f * fov);

        float const fRange = zFar / (zNear - zFar);
        float const height = fCos / fSin;
        float const width = height / aspect;

        SimdMatrix4x4F result;

        result.Inner[0].Inner[0] = width;
        result.Inner[0].Inner[1] = 0.0f;
        result.Inner[0].Inner[2] = 0.0f;
        result.Inner[0].Inner[3] = 0.0f;

        result.Inner[1].Inner[0] = 0.0f;
        result.Inner[1].Inner[1] = height;
        result.Inner[1].Inner[2] = 0.0f;
        result.Inner[1].Inner[3] = 0.0f;

        result.Inner[2].Inner[0] = 0.0f;
        result.Inner[2].Inner[1] = 0.0f;
        result.Inner[2].Inner[2] = fRange;
        result.Inner[2].Inner[3] = -1.0f;

        result.Inner[3].Inner[0] = 0.0f;
        result.Inner[3].Inner[1] = 0.0f;
        result.Inner[3].Inner[2] = fRange * zNear;
        result.Inner[3].Inner[3] = 0.0f;

        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        auto [fSin, fCos] = SinCos<float>(0.5f * fov);

        float const fRange = zFar / (zNear - zFar);
        float const height = fCos / fSin;
        float const width = height / aspect;

        __m128 const zero = _mm_setzero_ps();
        __m128 const unitW = _mm_load_ps(F32x4_NegativeUnitW.As<float>());
        __m128 const values = _mm_setr_ps(
            width,
            height,
            fRange * zNear,
            fRange);

        SimdMatrix4x4F result;

        // = [width, 0, 0, 0]
        result.Inner[0] = _mm_move_ss(zero, values);

        // = [0, height, 0, 0]
        result.Inner[1] = _mm_blend_ps(zero, values, 0b0010);

        // = [0, 0, fRange, -1]
        result.Inner[2] = _mm_insert_ps(unitW, values, 0b11'10'0000);

        // = [0, 0, fRange * zNear, 0]
        result.Inner[3] = _mm_blend_ps(zero, values, 0b0100);

        return result;
#elif ANEMONE_FEATURE_NEON
        auto [fSin, fCos] = SinCos<float>(0.5f * fov);

        float const fRange = zFar / (zNear - zFar);
        float const height = fCos / fSin;
        float const width = height / aspect;

        float32x4_t const zero = vdupq_n_f32(0.0f);
        float32x4_t const unitW = vld1q_f32(F32x4_NegativeUnitW.As<float>());

        SimdMatrix4x4F result;

        // = [width, 0, 0, 0]
        result.val[0] = vsetq_lane_f32(width, zero, 0);

        // = [0, height, 0, 0]
        result.val[1] = vsetq_lane_f32(height, zero, 1);

        // = [0, 0, fRange, -1]
        result.val[2] = vsetq_lane_f32(fRange, unitW, 2);

        // = [0, 0, fRange * zNear, 0]
        result.val[3] = vsetq_lane_f32(fRange * zNear, zero, 2);

        return result;
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateOrthographicLH(float width, float height, float zNear, float zFar)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const fRange = 1.0f / (zFar - zNear);

        SimdMatrix4x4F result;

        result.Inner[0].Inner[0] = 2.0f / width;
        result.Inner[0].Inner[1] = 0.0f;
        result.Inner[0].Inner[2] = 0.0f;
        result.Inner[0].Inner[3] = 0.0f;

        result.Inner[1].Inner[0] = 0.0f;
        result.Inner[1].Inner[1] = 2.0f / height;
        result.Inner[1].Inner[2] = 0.0f;
        result.Inner[1].Inner[3] = 0.0f;

        result.Inner[2].Inner[0] = 0.0f;
        result.Inner[2].Inner[1] = 0.0f;
        result.Inner[2].Inner[2] = fRange;
        result.Inner[2].Inner[3] = 0.0f;

        result.Inner[3].Inner[0] = 0.0f;
        result.Inner[3].Inner[1] = 0.0f;
        result.Inner[3].Inner[2] = -fRange * zNear;
        result.Inner[3].Inner[3] = 1.0f;

        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        float const fRange = 1.0f / (zFar - zNear);

        __m128 const zero = _mm_setzero_ps();
        __m128 const unitW = _mm_load_ps(F32x4_PositiveUnitW.As<float>());
        __m128 const values = _mm_setr_ps(
            2.0f / width,
            2.0f / height,
            -fRange * zNear,
            fRange);

        SimdMatrix4x4F result;

        // = [2.0f / width, 0, 0, 0]
        result.Inner[0] = _mm_move_ss(zero, values);

        // = [0, 2.0f / height, 0, 0]
        result.Inner[1] = _mm_blend_ps(zero, values, 0b0010);

        // = [0, 0, fRange, 0]
        result.Inner[2] = _mm_insert_ps(zero, values, 0b11'10'0000);

        // = [0, 0, -fRange * zNear, 1]
        result.Inner[3] = _mm_blend_ps(unitW, values, 0b0100);

        return result;
#elif ANEMONE_FEATURE_NEON
        float const fRange = 1.0f / (zFar - zNear);

        float32x4_t const zero = vdupq_n_f32(0.0f);
        float32x4_t const unitW = vld1q_f32(F32x4_PositiveUnitW.As<float>());

        SimdMatrix4x4F result;

        // = [2 / width, 0, 0, 0]
        result.val[0] = vsetq_lane_f32(2.0f / width, zero, 0);

        // = [0, 2 / height, 0, 0]
        result.val[1] = vsetq_lane_f32(2.0f / height, zero, 1);

        // = [0, 0, fRange, 0]
        result.val[2] = vsetq_lane_f32(fRange, zero, 2);

        // = [0, 0, -fRange * zNear, 1]
        result.val[3] = vsetq_lane_f32(-fRange * zNear, unitW, 2);

        return result;
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateOrthographicRH(float width, float height, float zNear, float zFar)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const fRange = 1.0f / (zNear - zFar);

        SimdMatrix4x4F result;

        result.Inner[0].Inner[0] = 2.0f / width;
        result.Inner[0].Inner[1] = 0.0f;
        result.Inner[0].Inner[2] = 0.0f;
        result.Inner[0].Inner[3] = 0.0f;

        result.Inner[1].Inner[0] = 0.0f;
        result.Inner[1].Inner[1] = 2.0f / height;
        result.Inner[1].Inner[2] = 0.0f;
        result.Inner[1].Inner[3] = 0.0f;

        result.Inner[2].Inner[0] = 0.0f;
        result.Inner[2].Inner[1] = 0.0f;
        result.Inner[2].Inner[2] = fRange;
        result.Inner[2].Inner[3] = 0.0f;

        result.Inner[3].Inner[0] = 0.0f;
        result.Inner[3].Inner[1] = 0.0f;
        result.Inner[3].Inner[2] = fRange * zNear;
        result.Inner[3].Inner[3] = 1.0f;

        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        float const fRange = 1.0f / (zNear - zFar);

        __m128 const zero = _mm_setzero_ps();
        __m128 const unitW = _mm_load_ps(F32x4_PositiveUnitW.As<float>());
        __m128 const values = _mm_setr_ps(
            2.0f / width,
            2.0f / height,
            fRange * zNear,
            fRange);

        SimdMatrix4x4F result;

        // = [2.0f / width, 0, 0, 0]
        result.Inner[0] = _mm_move_ss(zero, values);

        // = [0, 2.0f / height, 0, 0]
        result.Inner[1] = _mm_blend_ps(zero, values, 0b0010);

        // = [0, 0, fRange, 0]
        result.Inner[2] = _mm_insert_ps(zero, values, 0b11'10'0000);

        // = [0, 0, fRange * zNear, 1]
        result.Inner[3] = _mm_blend_ps(unitW, values, 0b0100);

        return result;
#elif ANEMONE_FEATURE_NEON
        float const fRange = 1.0f / (zNear - zFar);

        float32x4_t const zero = vdupq_n_f32(0.0f);
        float32x4_t const unitW = vld1q_f32(F32x4_PositiveUnitW.As<float>());

        SimdMatrix4x4F result;

        // = [2 / width, 0, 0, 0]
        result.val[0] = vsetq_lane_f32(2.0f / width, zero, 0);

        // = [0, 2 / height, 0, 0]
        result.val[1] = vsetq_lane_f32(2.0f / height, zero, 1);

        // = [0, 0, fRange, 0]
        result.val[2] = vsetq_lane_f32(fRange, zero, 2);

        // = [0, 0, fRange * zNear, 1]
        result.val[3] = vsetq_lane_f32(fRange * zNear, unitW, 2);

        return result;
#endif
    }


    template <size_t N>
    SimdVector4F anemone_vectorcall Matrix4x4F_Extract(SimdMatrix4x4F m)
        requires(N < 4)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return m.Inner[N];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return m.Inner[N];
#elif ANEMONE_FEATURE_NEON
        return m.val[N];
#endif
    }


    template <size_t N>
    SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Insert(SimdMatrix4x4F m, SimdVector4F v)
        requires(N < 4)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        m.Inner[N] = v;
        return m;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        m.Inner[N] = v;
        return m;
#elif ANEMONE_FEATURE_NEON
        m.val[N] = v;
        return m;
#endif
    }


    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Multiply(SimdMatrix4x4F a, SimdMatrix4x4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        SimdMatrix4x4F result;

        float const a00 = a.Inner[0].Inner[0];
        float const a01 = a.Inner[0].Inner[1];
        float const a02 = a.Inner[0].Inner[2];
        float const a03 = a.Inner[0].Inner[3];

        result.Inner[0].Inner[0] = (b.Inner[0].Inner[0] * a00) + (b.Inner[1].Inner[0] * a01) + (b.Inner[2].Inner[0] * a02) + (b.Inner[3].Inner[0] * a03);
        result.Inner[0].Inner[1] = (b.Inner[0].Inner[1] * a00) + (b.Inner[1].Inner[1] * a01) + (b.Inner[2].Inner[1] * a02) + (b.Inner[3].Inner[1] * a03);
        result.Inner[0].Inner[2] = (b.Inner[0].Inner[2] * a00) + (b.Inner[1].Inner[2] * a01) + (b.Inner[2].Inner[2] * a02) + (b.Inner[3].Inner[2] * a03);
        result.Inner[0].Inner[3] = (b.Inner[0].Inner[3] * a00) + (b.Inner[1].Inner[3] * a01) + (b.Inner[2].Inner[3] * a02) + (b.Inner[3].Inner[3] * a03);

        float const a10 = a.Inner[1].Inner[0];
        float const a11 = a.Inner[1].Inner[1];
        float const a12 = a.Inner[1].Inner[2];
        float const a13 = a.Inner[1].Inner[3];

        result.Inner[1].Inner[0] = (b.Inner[0].Inner[0] * a10) + (b.Inner[1].Inner[0] * a11) + (b.Inner[2].Inner[0] * a12) + (b.Inner[3].Inner[0] * a13);
        result.Inner[1].Inner[1] = (b.Inner[0].Inner[1] * a10) + (b.Inner[1].Inner[1] * a11) + (b.Inner[2].Inner[1] * a12) + (b.Inner[3].Inner[1] * a13);
        result.Inner[1].Inner[2] = (b.Inner[0].Inner[2] * a10) + (b.Inner[1].Inner[2] * a11) + (b.Inner[2].Inner[2] * a12) + (b.Inner[3].Inner[2] * a13);
        result.Inner[1].Inner[3] = (b.Inner[0].Inner[3] * a10) + (b.Inner[1].Inner[3] * a11) + (b.Inner[2].Inner[3] * a12) + (b.Inner[3].Inner[3] * a13);

        float const a20 = a.Inner[2].Inner[0];
        float const a21 = a.Inner[2].Inner[1];
        float const a22 = a.Inner[2].Inner[2];
        float const a23 = a.Inner[2].Inner[3];

        result.Inner[2].Inner[0] = (b.Inner[0].Inner[0] * a20) + (b.Inner[1].Inner[0] * a21) + (b.Inner[2].Inner[0] * a22) + (b.Inner[3].Inner[0] * a23);
        result.Inner[2].Inner[1] = (b.Inner[0].Inner[1] * a20) + (b.Inner[1].Inner[1] * a21) + (b.Inner[2].Inner[1] * a22) + (b.Inner[3].Inner[1] * a23);
        result.Inner[2].Inner[2] = (b.Inner[0].Inner[2] * a20) + (b.Inner[1].Inner[2] * a21) + (b.Inner[2].Inner[2] * a22) + (b.Inner[3].Inner[2] * a23);
        result.Inner[2].Inner[3] = (b.Inner[0].Inner[3] * a20) + (b.Inner[1].Inner[3] * a21) + (b.Inner[2].Inner[3] * a22) + (b.Inner[3].Inner[3] * a23);

        float const a30 = a.Inner[3].Inner[0];
        float const a31 = a.Inner[3].Inner[1];
        float const a32 = a.Inner[3].Inner[2];
        float const a33 = a.Inner[3].Inner[3];

        result.Inner[3].Inner[0] = (b.Inner[0].Inner[0] * a30) + (b.Inner[1].Inner[0] * a31) + (b.Inner[2].Inner[0] * a32) + (b.Inner[3].Inner[0] * a33);
        result.Inner[3].Inner[1] = (b.Inner[0].Inner[1] * a30) + (b.Inner[1].Inner[1] * a31) + (b.Inner[2].Inner[1] * a32) + (b.Inner[3].Inner[1] * a33);
        result.Inner[3].Inner[2] = (b.Inner[0].Inner[2] * a30) + (b.Inner[1].Inner[2] * a31) + (b.Inner[2].Inner[2] * a32) + (b.Inner[3].Inner[2] * a33);
        result.Inner[3].Inner[3] = (b.Inner[0].Inner[3] * a30) + (b.Inner[1].Inner[3] * a31) + (b.Inner[2].Inner[3] * a32) + (b.Inner[3].Inner[3] * a33);

        return result;
#elif ANEMONE_FEATURE_AVX2
        // = [a.row[0], a.row[1]]
        __m256 const a01 = _mm256_insertf128_ps(_mm256_castps128_ps256(a.Inner[0]), a.Inner[1], 1);
        // = [a.row[2], a.row[3]]
        __m256 const a23 = _mm256_insertf128_ps(_mm256_castps128_ps256(a.Inner[2]), a.Inner[3], 1);

        // = [b.row[0], b.row[1]]
        __m256 const b01 = _mm256_insertf128_ps(_mm256_castps128_ps256(b.Inner[0]), b.Inner[1], 1);
        // = [b.row[2], b.row[3]]
        __m256 const b23 = _mm256_insertf128_ps(_mm256_castps128_ps256(b.Inner[2]), b.Inner[3], 1);

        // = [[a.m11, a.m11, a.m11, a.m11], [a.m21, a.m21, a.m21, a.m21]]
        __m256 const a0_0 = _mm256_shuffle_ps(a01, a01, _MM_SHUFFLE(0, 0, 0, 0));
        // = [[a.m31, a.m31, a.m31, a.m31], [a.m41, a.m41, a.m41, a.m41]]
        __m256 const a1_0 = _mm256_shuffle_ps(a23, a23, _MM_SHUFFLE(0, 0, 0, 0));
        // = [b.row[0], b.row[0]]
        __m256 const b0_0 = _mm256_permute2f128_ps(b01, b01, 0x00);
        // = [a.m11 * b.row[0], a.m21 * b.row[0]]
        __m256 const c01_00 = _mm256_mul_ps(a0_0, b0_0);
        // = [a.m31 * b.row[0], a.m41 * b.row[0]]
        __m256 const c23_00 = _mm256_mul_ps(a1_0, b0_0);

        // = [[a.m12, a.m12, a.m12, a.m12], [a.m22, a.m22, a.m22, a.m22]]
        __m256 const a0_1 = _mm256_shuffle_ps(a01, a01, _MM_SHUFFLE(1, 1, 1, 1));
        // = [[a.m32, a.m32, a.m32, a.m32], [a.m42, a.m42, a.m42, a.m42]]
        __m256 const a1_1 = _mm256_shuffle_ps(a23, a23, _MM_SHUFFLE(1, 1, 1, 1));
        // = [b.row[1], b.row[1]]
        __m256 const b0_1 = _mm256_permute2f128_ps(b01, b01, 0x11);
        // = [a.m12 * b.row[1] + c01_0, a.m22 * b.row[1] + c01_0]
        __m256 const c01_01 = _mm256_fmadd_ps(a0_1, b0_1, c01_00);
        // = [a.m32 * b.row[1] + c23_0, a.m42 * b.row[2] + c23_0]
        __m256 const c23_01 = _mm256_fmadd_ps(a1_1, b0_1, c23_00);

        // = [[a.m13, a.m13, a.m13, a.m13], [a.m23, a.m23, a.m23, a.m23]]
        __m256 const a0_2 = _mm256_shuffle_ps(a01, a01, _MM_SHUFFLE(2, 2, 2, 2));
        // = [[a.m33, a.m33, a.m33, a.m33], [a.m43, a.m43, a.m43, a.m43]]
        __m256 const a1_2 = _mm256_shuffle_ps(a23, a23, _MM_SHUFFLE(2, 2, 2, 2));
        // = [b.row[2], b.row[2]]
        __m256 const b0_2 = _mm256_permute2f128_ps(b23, b23, 0x00);
        // = [a.m13 * b.row[2], a.m23 * b.row[2]]
        __m256 const c01_10 = _mm256_mul_ps(a0_2, b0_2);
        // = [a.m33 * b.row[2], a.m43 * b.row[2]]
        __m256 const c23_10 = _mm256_mul_ps(a1_2, b0_2);

        // = [[a.m14, a.m14, a.m14, a.m14], [a.m24, a.m24, a.m24, a.m24]]
        __m256 const a0_3 = _mm256_shuffle_ps(a01, a01, _MM_SHUFFLE(3, 3, 3, 3));
        // = [[a.m34, a.m34, a.m34, a.m34], [a.m44, a.m44, a.m44, a.m44]]
        __m256 const a1_3 = _mm256_shuffle_ps(a23, a23, _MM_SHUFFLE(3, 3, 3, 3));
        // = [b.row[3], b.row[3]]
        __m256 const b0_3 = _mm256_permute2f128_ps(b23, b23, 0x11);
        // = [a.m14 * b.row[3] + c01_01, a.m24 * b.row[3] + c01_01]
        __m256 const c01_11 = _mm256_fmadd_ps(a0_3, b0_3, c01_10);
        // = [a.m34 * b.row[3] + c23_01, a.m44 * b.row[3] + c23_01]
        __m256 const c23_11 = _mm256_fmadd_ps(a1_3, b0_3, c23_10);

        // Sum parts
        __m256 const r01 = _mm256_add_ps(c01_01, c01_11);
        __m256 const r23 = _mm256_add_ps(c23_01, c23_11);

        return SimdMatrix4x4F{
            _mm256_castps256_ps128(r01),
            _mm256_extractf128_ps(r01, 1),
            _mm256_castps256_ps128(r23),
            _mm256_extractf128_ps(r23, 1),
        };
#elif ANEMONE_FEATURE_AVX
        // Row 0
        __m128 const m00 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[0]) + 0);
        __m128 const m01 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[0]) + 1);
        __m128 const m02 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[0]) + 2);
        __m128 const m03 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[0]) + 3);

        __m128 const t00 = _mm_mul_ps(m00, b.Inner[0]);
        __m128 const t01 = Vector4F_MultiplyAdd(m01, b.Inner[1], t00);
        __m128 const t02 = Vector4F_MultiplyAdd(m02, b.Inner[2], t01);
        __m128 const r0 = Vector4F_MultiplyAdd(m03, b.Inner[3], t02);

        // Row 1
        __m128 const m10 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[1]) + 0);
        __m128 const m11 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[1]) + 1);
        __m128 const m12 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[1]) + 2);
        __m128 const m13 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[1]) + 3);

        __m128 const t10 = _mm_mul_ps(m10, b.Inner[0]);
        __m128 const t11 = Vector4F_MultiplyAdd(m11, b.Inner[1], t10);
        __m128 const t12 = Vector4F_MultiplyAdd(m12, b.Inner[2], t11);
        __m128 const r1 = Vector4F_MultiplyAdd(m13, b.Inner[3], t12);

        // Row 2
        __m128 const m20 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[2]) + 0);
        __m128 const m21 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[2]) + 1);
        __m128 const m22 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[2]) + 2);
        __m128 const m23 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[2]) + 3);

        __m128 const t20 = _mm_mul_ps(m20, b.Inner[0]);
        __m128 const t21 = Vector4F_MultiplyAdd(m21, b.Inner[1], t20);
        __m128 const t22 = Vector4F_MultiplyAdd(m22, b.Inner[2], t21);
        __m128 const r2 = Vector4F_MultiplyAdd(m23, b.Inner[3], t22);

        // Row 3
        __m128 const m30 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[3]) + 0);
        __m128 const m31 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[3]) + 1);
        __m128 const m32 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[3]) + 2);
        __m128 const m33 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[3]) + 3);


        __m128 const t30 = _mm_mul_ps(m30, b.Inner[0]);
        __m128 const t31 = Vector4F_MultiplyAdd(m31, b.Inner[1], t30);
        __m128 const t32 = Vector4F_MultiplyAdd(m32, b.Inner[2], t31);
        __m128 const r3 = Vector4F_MultiplyAdd(m33, b.Inner[3], t32);

        return SimdMatrix4x4F{
            r0,
            r1,
            r2,
            r3,
        };
#elif ANEMONE_FEATURE_NEON
        SimdMatrix4x4F result;

        float32x4_t const c00 = vmulq_laneq_f32(b.val[0], a.val[0], 0);
        float32x4_t const c01 = vmulq_laneq_f32(b.val[1], a.val[0], 1);
        float32x4_t const c02 = vmlaq_laneq_f32(c00, b.val[2], a.val[0], 2);
        float32x4_t const c03 = vmlaq_laneq_f32(c01, b.val[3], a.val[0], 3);
        result.val[0] = vaddq_f32(c02, c03);

        float32x4_t const c10 = vmulq_laneq_f32(b.val[0], a.val[1], 0);
        float32x4_t const c11 = vmulq_laneq_f32(b.val[1], a.val[1], 1);
        float32x4_t const c12 = vmlaq_laneq_f32(c10, b.val[2], a.val[1], 2);
        float32x4_t const c13 = vmlaq_laneq_f32(c11, b.val[3], a.val[1], 3);
        result.val[1] = vaddq_f32(c12, c13);

        float32x4_t const c20 = vmulq_laneq_f32(b.val[0], a.val[2], 0);
        float32x4_t const c21 = vmulq_laneq_f32(b.val[1], a.val[2], 1);
        float32x4_t const c22 = vmlaq_laneq_f32(c20, b.val[2], a.val[2], 2);
        float32x4_t const c23 = vmlaq_laneq_f32(c21, b.val[3], a.val[2], 3);
        result.val[2] = vaddq_f32(c22, c23);

        float32x4_t const c30 = vmulq_laneq_f32(b.val[0], a.val[3], 0);
        float32x4_t const c31 = vmulq_laneq_f32(b.val[1], a.val[3], 1);
        float32x4_t const c32 = vmlaq_laneq_f32(c30, b.val[2], a.val[3], 2);
        float32x4_t const c33 = vmlaq_laneq_f32(c31, b.val[3], a.val[3], 3);
        result.val[3] = vaddq_f32(c32, c33);

        return result;
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_MultiplyTranspose(SimdMatrix4x4F a, SimdMatrix4x4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        SimdMatrix4x4F result;

        float const b00 = b.Inner[0].Inner[0];
        float const b10 = b.Inner[1].Inner[0];
        float const b20 = b.Inner[2].Inner[0];
        float const b30 = b.Inner[3].Inner[0];

        result.Inner[0].Inner[0] = (a.Inner[0].Inner[0] * b00) + (a.Inner[0].Inner[1] * b10) + (a.Inner[0].Inner[2] * b20) + (a.Inner[0].Inner[3] * b30);
        result.Inner[0].Inner[1] = (a.Inner[1].Inner[0] * b00) + (a.Inner[1].Inner[1] * b10) + (a.Inner[1].Inner[2] * b20) + (a.Inner[1].Inner[3] * b30);
        result.Inner[0].Inner[2] = (a.Inner[2].Inner[0] * b00) + (a.Inner[2].Inner[1] * b10) + (a.Inner[2].Inner[2] * b20) + (a.Inner[2].Inner[3] * b30);
        result.Inner[0].Inner[3] = (a.Inner[3].Inner[0] * b00) + (a.Inner[3].Inner[1] * b10) + (a.Inner[3].Inner[2] * b20) + (a.Inner[3].Inner[3] * b30);

        float const b01 = b.Inner[0].Inner[1];
        float const b11 = b.Inner[1].Inner[1];
        float const b21 = b.Inner[2].Inner[1];
        float const b31 = b.Inner[3].Inner[1];

        result.Inner[1].Inner[0] = (a.Inner[0].Inner[0] * b01) + (a.Inner[0].Inner[1] * b11) + (a.Inner[0].Inner[2] * b21) + (a.Inner[0].Inner[3] * b31);
        result.Inner[1].Inner[1] = (a.Inner[1].Inner[0] * b01) + (a.Inner[1].Inner[1] * b11) + (a.Inner[1].Inner[2] * b21) + (a.Inner[1].Inner[3] * b31);
        result.Inner[1].Inner[2] = (a.Inner[2].Inner[0] * b01) + (a.Inner[2].Inner[1] * b11) + (a.Inner[2].Inner[2] * b21) + (a.Inner[2].Inner[3] * b31);
        result.Inner[1].Inner[3] = (a.Inner[3].Inner[0] * b01) + (a.Inner[3].Inner[1] * b11) + (a.Inner[3].Inner[2] * b21) + (a.Inner[3].Inner[3] * b31);

        float const b02 = b.Inner[0].Inner[2];
        float const b12 = b.Inner[1].Inner[2];
        float const b22 = b.Inner[2].Inner[2];
        float const b32 = b.Inner[3].Inner[2];

        result.Inner[2].Inner[0] = (a.Inner[0].Inner[0] * b02) + (a.Inner[0].Inner[1] * b12) + (a.Inner[0].Inner[2] * b22) + (a.Inner[0].Inner[3] * b32);
        result.Inner[2].Inner[1] = (a.Inner[1].Inner[0] * b02) + (a.Inner[1].Inner[1] * b12) + (a.Inner[1].Inner[2] * b22) + (a.Inner[1].Inner[3] * b32);
        result.Inner[2].Inner[2] = (a.Inner[2].Inner[0] * b02) + (a.Inner[2].Inner[1] * b12) + (a.Inner[2].Inner[2] * b22) + (a.Inner[2].Inner[3] * b32);
        result.Inner[2].Inner[3] = (a.Inner[3].Inner[0] * b02) + (a.Inner[3].Inner[1] * b12) + (a.Inner[3].Inner[2] * b22) + (a.Inner[3].Inner[3] * b32);

        float const b03 = b.Inner[0].Inner[3];
        float const b13 = b.Inner[1].Inner[3];
        float const b23 = b.Inner[2].Inner[3];
        float const b33 = b.Inner[3].Inner[3];

        result.Inner[3].Inner[0] = (a.Inner[0].Inner[0] * b03) + (a.Inner[0].Inner[1] * b13) + (a.Inner[0].Inner[2] * b23) + (a.Inner[0].Inner[3] * b33);
        result.Inner[3].Inner[1] = (a.Inner[1].Inner[0] * b03) + (a.Inner[1].Inner[1] * b13) + (a.Inner[1].Inner[2] * b23) + (a.Inner[1].Inner[3] * b33);
        result.Inner[3].Inner[2] = (a.Inner[2].Inner[0] * b03) + (a.Inner[2].Inner[1] * b13) + (a.Inner[2].Inner[2] * b23) + (a.Inner[2].Inner[3] * b33);
        result.Inner[3].Inner[3] = (a.Inner[3].Inner[0] * b03) + (a.Inner[3].Inner[1] * b13) + (a.Inner[3].Inner[2] * b23) + (a.Inner[3].Inner[3] * b33);

        return result;
#elif ANEMONE_FEATURE_AVX2
        // = [a.row[0], a.row[1]]
        __m256 const a01 = _mm256_insertf128_ps(_mm256_castps128_ps256(a.Inner[0]), a.Inner[1], 1);
        // = [a.row[2], a.row[3]]
        __m256 const a23 = _mm256_insertf128_ps(_mm256_castps128_ps256(a.Inner[2]), a.Inner[3], 1);

        // = [b.row[0], b.row[1]]
        __m256 const b01 = _mm256_insertf128_ps(_mm256_castps128_ps256(b.Inner[0]), b.Inner[1], 1);
        // = [b.row[2], b.row[3]]
        __m256 const b23 = _mm256_insertf128_ps(_mm256_castps128_ps256(b.Inner[2]), b.Inner[3], 1);

        // = [[a.m11, a.m11, a.m11, a.m11], [a.m21, a.m21, a.m21, a.m21]]
        __m256 const a0_0 = _mm256_shuffle_ps(a01, a01, _MM_SHUFFLE(0, 0, 0, 0));
        // = [[a.m31, a.m31, a.m31, a.m31], [a.m41, a.m41, a.m41, a.m41]]
        __m256 const a1_0 = _mm256_shuffle_ps(a23, a23, _MM_SHUFFLE(0, 0, 0, 0));
        // = [b.row[0], b.row[0]]
        __m256 const b0_0 = _mm256_permute2f128_ps(b01, b01, 0x00);
        // = [a.m11 * b.row[0], a.m21 * b.row[0]]
        __m256 const c01_00 = _mm256_mul_ps(a0_0, b0_0);
        // = [a.m31 * b.row[0], a.m41 * b.row[0]]
        __m256 const c23_00 = _mm256_mul_ps(a1_0, b0_0);

        // = [[a.m12, a.m12, a.m12, a.m12], [a.m22, a.m22, a.m22, a.m22]]
        __m256 const a0_1 = _mm256_shuffle_ps(a01, a01, _MM_SHUFFLE(1, 1, 1, 1));
        // = [[a.m32, a.m32, a.m32, a.m32], [a.m42, a.m42, a.m42, a.m42]]
        __m256 const a1_1 = _mm256_shuffle_ps(a23, a23, _MM_SHUFFLE(1, 1, 1, 1));
        // = [b.row[1], b.row[1]]
        __m256 const b0_1 = _mm256_permute2f128_ps(b01, b01, 0x11);
        // = [a.m12 * b.row[1] + c01_0, a.m22 * b.row[1] + c01_0]
        __m256 const c01_01 = _mm256_fmadd_ps(a0_1, b0_1, c01_00);
        // = [a.m32 * b.row[1] + c23_0, a.m42 * b.row[2] + c23_0]
        __m256 const c23_01 = _mm256_fmadd_ps(a1_1, b0_1, c23_00);

        // = [[a.m13, a.m13, a.m13, a.m13], [a.m23, a.m23, a.m23, a.m23]]
        __m256 const a0_2 = _mm256_shuffle_ps(a01, a01, _MM_SHUFFLE(2, 2, 2, 2));
        // = [[a.m33, a.m33, a.m33, a.m33], [a.m43, a.m43, a.m43, a.m43]]
        __m256 const a1_2 = _mm256_shuffle_ps(a23, a23, _MM_SHUFFLE(2, 2, 2, 2));
        // = [b.row[2], b.row[2]]
        __m256 const b0_2 = _mm256_permute2f128_ps(b23, b23, 0x00);
        // = [a.m13 * b.row[2], a.m23 * b.row[2]]
        __m256 const c01_10 = _mm256_mul_ps(a0_2, b0_2);
        // = [a.m33 * b.row[2], a.m43 * b.row[2]]
        __m256 const c23_10 = _mm256_mul_ps(a1_2, b0_2);

        // = [[a.m14, a.m14, a.m14, a.m14], [a.m24, a.m24, a.m24, a.m24]]
        __m256 const a0_3 = _mm256_shuffle_ps(a01, a01, _MM_SHUFFLE(3, 3, 3, 3));
        // = [[a.m34, a.m34, a.m34, a.m34], [a.m44, a.m44, a.m44, a.m44]]
        __m256 const a1_3 = _mm256_shuffle_ps(a23, a23, _MM_SHUFFLE(3, 3, 3, 3));
        // = [b.row[3], b.row[3]]
        __m256 const b0_3 = _mm256_permute2f128_ps(b23, b23, 0x11);
        // = [a.m14 * b.row[3] + c01_01, a.m24 * b.row[3] + c01_01]
        __m256 const c01_11 = _mm256_fmadd_ps(a0_3, b0_3, c01_10);
        // = [a.m34 * b.row[3] + c23_01, a.m44 * b.row[3] + c23_01]
        __m256 const c23_11 = _mm256_fmadd_ps(a1_3, b0_3, c23_10);

        // Sum parts and transpose result

        // = [[m11, m12, m13, m14], [m21, m22, m23, m24]]
        __m256 const m11m12m13m14_m21m22m23m24 = _mm256_add_ps(c01_01, c01_11);
        // = [[m31, m32, m33, m34], [m41, m42, m43, m44]]
        __m256 const m31m32m33m34_m41m42m43m44 = _mm256_add_ps(c23_01, c23_11);

        // = [[m11, m31, m12, m32], [m21, m41, m22, m42]
        __m256 const m11m31m12m32_m21m41m22m42 = _mm256_unpacklo_ps(m11m12m13m14_m21m22m23m24, m31m32m33m34_m41m42m43m44);
        // = [[m13, m33, m14, m44], [m23, m43, m24, m44]]
        __m256 const m13m33m14m44_m23m43m24m44 = _mm256_unpackhi_ps(m11m12m13m14_m21m22m23m24, m31m32m33m34_m41m42m43m44);

        // = [[m11, m31, m12, m32], [m13, m33, m14, m44]]
        __m256 const m11m31m12m32_m13m33m14m44 = _mm256_permute2f128_ps(m11m31m12m32_m21m41m22m42, m13m33m14m44_m23m43m24m44, 0x20);
        // = [[m21, m41, m22, m42], [m32, m43, m24, m44]]
        __m256 const m21m41m22m42_m23m43m24m44 = _mm256_permute2f128_ps(m11m31m12m32_m21m41m22m42, m13m33m14m44_m23m43m24m44, 0x31);

        // = [[m11, m21, m31, m41], [m13, m23, m33, m43]]
        __m256 const m11m21m31m41_m13m23m33m43 = _mm256_unpacklo_ps(m11m31m12m32_m13m33m14m44, m21m41m22m42_m23m43m24m44);
        // = [[m12, m22, m32, m42], [m14, m24, m34, m44]
        __m256 const m12m22m32m42_m14m24m34m44 = _mm256_unpackhi_ps(m11m31m12m32_m13m33m14m44, m21m41m22m42_m23m43m24m44);

        // = [[m11, m21, m31, m41], [m12, m22, m32, m42]]
        __m256 const m11m21m31m41_m12m22m32m42 = _mm256_permute2f128_ps(m11m21m31m41_m13m23m33m43, m12m22m32m42_m14m24m34m44, 0x20);
        // = [[m13, m23, m33, m43], [m14, m24, m34, m44]]
        __m256 const m13m23m33m43_m14m24m34m44 = _mm256_permute2f128_ps(m11m21m31m41_m13m23m33m43, m12m22m32m42_m14m24m34m44, 0x31);

        return SimdMatrix4x4F{
            _mm256_castps256_ps128(m11m21m31m41_m12m22m32m42),
            _mm256_extractf128_ps(m11m21m31m41_m12m22m32m42, 1),
            _mm256_castps256_ps128(m13m23m33m43_m14m24m34m44),
            _mm256_extractf128_ps(m13m23m33m43_m14m24m34m44, 1),
        };
#elif ANEMONE_FEATURE_AVX
        // Row 0
        __m128 const m00 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[0]) + 0);
        __m128 const m01 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[0]) + 1);
        __m128 const m02 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[0]) + 2);
        __m128 const m03 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[0]) + 3);

        __m128 const t00 = _mm_mul_ps(m00, b.Inner[0]);
        __m128 const t01 = Vector4F_MultiplyAdd(m01, b.Inner[1], t00);
        __m128 const t02 = Vector4F_MultiplyAdd(m02, b.Inner[2], t01);
        __m128 const r0 = Vector4F_MultiplyAdd(m03, b.Inner[3], t02);

        // Row 1
        __m128 const m10 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[1]) + 0);
        __m128 const m11 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[1]) + 1);
        __m128 const m12 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[1]) + 2);
        __m128 const m13 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[1]) + 3);

        __m128 const t10 = _mm_mul_ps(m10, b.Inner[0]);
        __m128 const t11 = Vector4F_MultiplyAdd(m11, b.Inner[1], t10);
        __m128 const t12 = Vector4F_MultiplyAdd(m12, b.Inner[2], t11);
        __m128 const r1 = Vector4F_MultiplyAdd(m13, b.Inner[3], t12);

        // Row 2
        __m128 const m20 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[2]) + 0);
        __m128 const m21 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[2]) + 1);
        __m128 const m22 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[2]) + 2);
        __m128 const m23 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[2]) + 3);

        __m128 const t20 = _mm_mul_ps(m20, b.Inner[0]);
        __m128 const t21 = Vector4F_MultiplyAdd(m21, b.Inner[1], t20);
        __m128 const t22 = Vector4F_MultiplyAdd(m22, b.Inner[2], t21);
        __m128 const r2 = Vector4F_MultiplyAdd(m23, b.Inner[3], t22);

        // Row 3
        __m128 const m30 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[3]) + 0);
        __m128 const m31 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[3]) + 1);
        __m128 const m32 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[3]) + 2);
        __m128 const m33 = _mm_broadcast_ss(reinterpret_cast<float*>(&a.Inner[3]) + 3);


        __m128 const t30 = _mm_mul_ps(m30, b.Inner[0]);
        __m128 const t31 = Vector4F_MultiplyAdd(m31, b.Inner[1], t30);
        __m128 const t32 = Vector4F_MultiplyAdd(m32, b.Inner[2], t31);
        __m128 const r3 = Vector4F_MultiplyAdd(m33, b.Inner[3], t32);

        // Transpose result
        __m128 const m11_m12_m21_m22 = _mm_shuffle_ps(r0, r1, _MM_SHUFFLE(1, 0, 1, 0));
        __m128 const m13_m14_m23_m24 = _mm_shuffle_ps(r0, r1, _MM_SHUFFLE(3, 2, 3, 2));

        __m128 const m31_m32_m41_m42 = _mm_shuffle_ps(r2, r3, _MM_SHUFFLE(1, 0, 1, 0));
        __m128 const m33_m34_m43_m44 = _mm_shuffle_ps(r2, r3, _MM_SHUFFLE(3, 2, 3, 2));

        __m128 const m11_m21_m31_m41 = _mm_shuffle_ps(m11_m12_m21_m22, m31_m32_m41_m42, _MM_SHUFFLE(2, 0, 2, 0));
        __m128 const m12_m22_m32_m42 = _mm_shuffle_ps(m11_m12_m21_m22, m31_m32_m41_m42, _MM_SHUFFLE(3, 1, 3, 1));

        __m128 const m13_m23_m33_m43 = _mm_shuffle_ps(m13_m14_m23_m24, m33_m34_m43_m44, _MM_SHUFFLE(2, 0, 2, 0));
        __m128 const m14_m24_m34_m44 = _mm_shuffle_ps(m13_m14_m23_m24, m33_m34_m43_m44, _MM_SHUFFLE(3, 1, 3, 1));

        return SimdMatrix4x4F{
            m11_m21_m31_m41,
            m12_m22_m32_m42,
            m13_m23_m33_m43,
            m14_m24_m34_m44,
        };
#elif ANEMONE_FEATURE_NEON
        // Multiply matrices.
        float32x4_t const c00 = vmulq_laneq_f32(b.val[0], a.val[0], 0);
        float32x4_t const c01 = vmulq_laneq_f32(b.val[1], a.val[0], 1);
        float32x4_t const c02 = vmlaq_laneq_f32(c00, b.val[2], a.val[0], 2);
        float32x4_t const c03 = vmlaq_laneq_f32(c01, b.val[3], a.val[0], 3);
        float32x4_t const r0 = vaddq_f32(c02, c03);

        float32x4_t const c10 = vmulq_laneq_f32(b.val[0], a.val[1], 0);
        float32x4_t const c11 = vmulq_laneq_f32(b.val[1], a.val[1], 1);
        float32x4_t const c12 = vmlaq_laneq_f32(c10, b.val[2], a.val[1], 2);
        float32x4_t const c13 = vmlaq_laneq_f32(c11, b.val[3], a.val[1], 3);
        float32x4_t const r1 = vaddq_f32(c12, c13);

        float32x4_t const c20 = vmulq_laneq_f32(b.val[0], a.val[2], 0);
        float32x4_t const c21 = vmulq_laneq_f32(b.val[1], a.val[2], 1);
        float32x4_t const c22 = vmlaq_laneq_f32(c20, b.val[2], a.val[2], 2);
        float32x4_t const c23 = vmlaq_laneq_f32(c21, b.val[3], a.val[2], 3);
        float32x4_t const r2 = vaddq_f32(c22, c23);

        float32x4_t const c30 = vmulq_laneq_f32(b.val[0], a.val[3], 0);
        float32x4_t const c31 = vmulq_laneq_f32(b.val[1], a.val[3], 1);
        float32x4_t const c32 = vmlaq_laneq_f32(c30, b.val[2], a.val[3], 2);
        float32x4_t const c33 = vmlaq_laneq_f32(c31, b.val[3], a.val[3], 3);
        float32x4_t const r3 = vaddq_f32(c32, c33);

        // Transpose result
        float32x4x2_t const s02 = vzipq_f32(r0, r2);
        float32x4x2_t const s13 = vzipq_f32(r1, r3);
        float32x4x2_t const t01 = vzipq_f32(s02.val[0], s13.val[0]);
        float32x4x2_t const t23 = vzipq_f32(s02.val[1], s13.val[1]);

        SimdMatrix4x4F result;

        result.val[0] = t01.val[0];
        result.val[1] = t01.val[1];
        result.val[2] = t23.val[0];
        result.val[3] = t23.val[1];

        return result;
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Transpose(SimdMatrix4x4F m)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        SimdMatrix4x4F result;

        result.Inner[0].Inner[0] = m.Inner[0].Inner[0];
        result.Inner[0].Inner[1] = m.Inner[1].Inner[0];
        result.Inner[0].Inner[2] = m.Inner[2].Inner[0];
        result.Inner[0].Inner[3] = m.Inner[3].Inner[0];

        result.Inner[1].Inner[0] = m.Inner[0].Inner[1];
        result.Inner[1].Inner[1] = m.Inner[1].Inner[1];
        result.Inner[1].Inner[2] = m.Inner[2].Inner[1];
        result.Inner[1].Inner[3] = m.Inner[3].Inner[1];

        result.Inner[2].Inner[0] = m.Inner[0].Inner[2];
        result.Inner[2].Inner[1] = m.Inner[1].Inner[2];
        result.Inner[2].Inner[2] = m.Inner[2].Inner[2];
        result.Inner[2].Inner[3] = m.Inner[3].Inner[2];

        result.Inner[3].Inner[0] = m.Inner[0].Inner[3];
        result.Inner[3].Inner[1] = m.Inner[1].Inner[3];
        result.Inner[3].Inner[2] = m.Inner[2].Inner[3];
        result.Inner[3].Inner[3] = m.Inner[3].Inner[3];

        return result;
#elif ANEMONE_FEATURE_AVX2
        // = [[m11, m12, m13, m14], [m21, m22, m23, m24]]
        __m256 const m11m12m13m14_m21m22m23m24 = _mm256_insertf128_ps(_mm256_castps128_ps256(m.Inner[0]), m.Inner[1], 1);
        // = [[m31, m32, m33, m34], [m41, m42, m43, m44]]
        __m256 const m31m32m33m34_m41m42m43m44 = _mm256_insertf128_ps(_mm256_castps128_ps256(m.Inner[2]), m.Inner[3], 1);

        // = [[m11, m31, m12, m32], [m21, m41, m22, m42]
        __m256 const m11m31m12m32_m21m41m22m42 = _mm256_unpacklo_ps(m11m12m13m14_m21m22m23m24, m31m32m33m34_m41m42m43m44);
        // = [[m13, m33, m14, m44], [m23, m43, m24, m44]]
        __m256 const m13m33m14m44_m23m43m24m44 = _mm256_unpackhi_ps(m11m12m13m14_m21m22m23m24, m31m32m33m34_m41m42m43m44);

        // = [[m11, m31, m12, m32], [m13, m33, m14, m44]]
        __m256 const m11m31m12m32_m13m33m14m44 = _mm256_permute2f128_ps(m11m31m12m32_m21m41m22m42, m13m33m14m44_m23m43m24m44, 0x20);
        // = [[m21, m41, m22, m42], [m32, m43, m24, m44]]
        __m256 const m21m41m22m42_m23m43m24m44 = _mm256_permute2f128_ps(m11m31m12m32_m21m41m22m42, m13m33m14m44_m23m43m24m44, 0x31);

        // = [[m11, m21, m31, m41], [m13, m23, m33, m43]]
        __m256 const m11m21m31m41_m13m23m33m43 = _mm256_unpacklo_ps(m11m31m12m32_m13m33m14m44, m21m41m22m42_m23m43m24m44);
        // = [[m12, m22, m32, m42], [m14, m24, m34, m44]
        __m256 const m12m22m32m42_m14m24m34m44 = _mm256_unpackhi_ps(m11m31m12m32_m13m33m14m44, m21m41m22m42_m23m43m24m44);

        // = [[m11, m21, m31, m41], [m12, m22, m32, m42]]
        __m256 const m11m21m31m41_m12m22m32m42 = _mm256_permute2f128_ps(m11m21m31m41_m13m23m33m43, m12m22m32m42_m14m24m34m44, 0x20);
        // = [[m13, m23, m33, m43], [m14, m24, m34, m44]]
        __m256 const m13m23m33m43_m14m24m34m44 = _mm256_permute2f128_ps(m11m21m31m41_m13m23m33m43, m12m22m32m42_m14m24m34m44, 0x31);

        return SimdMatrix4x4F{
            _mm256_castps256_ps128(m11m21m31m41_m12m22m32m42),
            _mm256_extractf128_ps(m11m21m31m41_m12m22m32m42, 1),
            _mm256_castps256_ps128(m13m23m33m43_m14m24m34m44),
            _mm256_extractf128_ps(m13m23m33m43_m14m24m34m44, 1),
        };
#elif ANEMONE_FEATURE_AVX
        __m128 const m00_m01_m10_m11 = _mm_shuffle_ps(m.Inner[0], m.Inner[1], _MM_SHUFFLE(1, 0, 1, 0));
        __m128 const m02_m03_m12_m13 = _mm_shuffle_ps(m.Inner[0], m.Inner[1], _MM_SHUFFLE(3, 2, 3, 2));

        __m128 const m20_m21_m30_m31 = _mm_shuffle_ps(m.Inner[2], m.Inner[3], _MM_SHUFFLE(1, 0, 1, 0));
        __m128 const m22_m23_m32_m33 = _mm_shuffle_ps(m.Inner[2], m.Inner[3], _MM_SHUFFLE(3, 2, 3, 2));

        __m128 const m00_m10_m20_m30 = _mm_shuffle_ps(m00_m01_m10_m11, m20_m21_m30_m31, _MM_SHUFFLE(2, 0, 2, 0));
        __m128 const m01_m11_m21_m31 = _mm_shuffle_ps(m00_m01_m10_m11, m20_m21_m30_m31, _MM_SHUFFLE(3, 1, 3, 1));

        __m128 const m02_m12_m22_m32 = _mm_shuffle_ps(m02_m03_m12_m13, m22_m23_m32_m33, _MM_SHUFFLE(2, 0, 2, 0));
        __m128 const m03_m13_m23_m33 = _mm_shuffle_ps(m02_m03_m12_m13, m22_m23_m32_m33, _MM_SHUFFLE(3, 1, 3, 1));

        return SimdMatrix4x4F{
            m00_m10_m20_m30,
            m01_m11_m21_m31,
            m02_m12_m22_m32,
            m03_m13_m23_m33,
        };
#elif ANEMONE_FEATURE_NEON
        float32x4x2_t const m11m31m12m32_m13m33m14m34 = vzipq_f32(m.val[0], m.val[2]);
        float32x4x2_t const m21m41m22m42_m23m43m24m44 = vzipq_f32(m.val[1], m.val[3]);
        float32x4x2_t const m11m12m13m14_m21m22m23m24 = vzipq_f32(m11m31m12m32_m13m33m14m34.val[0], m21m41m22m42_m23m43m24m44.val[0]);
        float32x4x2_t const m31m32m33m34_m41m42m43m44 = vzipq_f32(m11m31m12m32_m13m33m14m34.val[1], m21m41m22m42_m23m43m24m44.val[1]);

        SimdMatrix4x4F result;
        result.val[0] = m11m12m13m14_m21m22m23m24.val[0];
        result.val[1] = m11m12m13m14_m21m22m23m24.val[1];
        result.val[2] = m31m32m33m34_m41m42m43m44.val[0];
        result.val[3] = m31m32m33m34_m41m42m43m44.val[1];
        return result;
#endif
    }

    inline SimdVector4F anemone_vectorcall Matrix4x4F_Determinant(SimdMatrix4x4F matrix)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        //
        // Determinant:
        //
        // | a b c d |     | f g h |     | e g h |     | e f h |    | e f g |
        // | e f g h | = a | j k l | - b | i k l | + c | i j l | -d | i j k |
        // | i j k l |     | n o p |     | m o p |     | m n p |    | m n o |
        // | m n o p |
        //

        // Where:
        //
        //   | f g h |
        // a | j k l | = a ( f ( kp - lo ) - g ( jp - ln ) + h ( jo - kn ) )
        //   | n o p |
        //
        //   | e g h |
        // b | i k l | = b ( e ( kp - lo ) - g ( ip - lm ) + h ( io - km ) )
        //   | m o p |
        //
        //   | e f h |
        // c | i j l | = c ( e ( jp - ln ) - f ( ip - lm ) + h ( in - jm ) )
        //   | m n p |
        //
        //   | e f g |
        // d | i j k | = d ( e ( jo - kn ) - f ( io - km ) + g ( in - jm ) )
        //   | m n o |
        //

        float const a = matrix.Inner[0].Inner[0];
        float const b = matrix.Inner[0].Inner[1];
        float const c = matrix.Inner[0].Inner[2];
        float const d = matrix.Inner[0].Inner[3];

        float const e = matrix.Inner[1].Inner[0];
        float const f = matrix.Inner[1].Inner[1];
        float const g = matrix.Inner[1].Inner[2];
        float const h = matrix.Inner[1].Inner[3];

        float const i = matrix.Inner[2].Inner[0];
        float const j = matrix.Inner[2].Inner[1];
        float const k = matrix.Inner[2].Inner[2];
        float const l = matrix.Inner[2].Inner[3];

        float const m = matrix.Inner[3].Inner[0];
        float const n = matrix.Inner[3].Inner[1];
        float const o = matrix.Inner[3].Inner[2];
        float const p = matrix.Inner[3].Inner[3];

        float const kp_lo = k * p - l * o;
        float const jp_ln = j * p - l * n;
        float const jo_kn = j * o - k * n;
        float const ip_lm = i * p - l * m;
        float const io_km = i * o - k * m;
        float const in_jm = i * n - j * m;

        float const det = a * (f * kp_lo - g * jp_ln + h * jo_kn) -
            b * (e * kp_lo - g * ip_lm + h * io_km) +
            c * (e * jp_ln - f * ip_lm + h * in_jm) -
            d * (e * jo_kn - f * io_km + g * in_jm);

        return SimdVector4F{det, det, det, det};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // = [m21, m20, m20, m20]
        __m128 const t0 = _mm_permute_ps(matrix.Inner[2], _MM_SHUFFLE(0, 0, 0, 1));
        // = [m32, m32, m31, m31]
        __m128 const t1 = _mm_permute_ps(matrix.Inner[3], _MM_SHUFFLE(1, 1, 2, 2));
        // = [m33, m33, m33, m32]
        __m128 const t2 = _mm_permute_ps(matrix.Inner[3], _MM_SHUFFLE(2, 3, 3, 3));
        // = [m22, m22, m21, m21]
        __m128 const t3 = _mm_permute_ps(matrix.Inner[2], _MM_SHUFFLE(1, 1, 2, 2));

        // = [m21, m20, m20, m20] * [m32, m32, m31, m31]
        __m128 const t01 = _mm_mul_ps(t0, t1);
        // = [m21, m20, m20, m20] * [m33, m33, m33, m32]
        __m128 const t02 = _mm_mul_ps(t0, t2);
        // = [m33, m33, m33, m32] * [m22, m22, m21, m21]
        __m128 const t23 = _mm_mul_ps(t2, t3);

        // = [m31, m30, m30, m30]
        __m128 const t4 = _mm_permute_ps(matrix.Inner[3], _MM_SHUFFLE(0, 0, 0, 1));
        // = [m23, m23, m23, m22]
        __m128 const t5 = _mm_permute_ps(matrix.Inner[2], _MM_SHUFFLE(2, 3, 3, 3));

        // = [m21, m20, m20, m20] * [m32, m32, m31, m31] - [m22, m22, m21, m21] * [m31, m30, m30, m30]
        __m128 const s01 = Vector4F_NegateMultiplyAdd(t3, t4, t01);
        // = [m21, m20, m20, m20] * [m33, m33, m33, m32] - [m23, m23, m23, m22] * [m31, m30, m30, m30]
        __m128 const s23 = Vector4F_NegateMultiplyAdd(t5, t4, t02);
        // = [m33, m33, m33, m32] * [m22, m22, m21, m21] - [m23, m23, m23, m22] * [m32, m32, m31, m31]
        __m128 const s45 = Vector4F_NegateMultiplyAdd(t5, t1, t23);

        // = [m13, m13, m13, m12]
        __m128 const v0 = _mm_permute_ps(matrix.Inner[1], _MM_SHUFFLE(2, 3, 3, 3));
        // = [m12, m12, m11, m11]
        __m128 const v1 = _mm_permute_ps(matrix.Inner[1], _MM_SHUFFLE(1, 1, 2, 2));
        // = [m11, m10, m10, m10];
        __m128 const v2 = _mm_permute_ps(matrix.Inner[1], _MM_SHUFFLE(0, 0, 0, 1));

        __m128 const negate_yw = _mm_setr_ps(1.0f, -1.0f, 1.0f, -1.0f);

        // = [m00, -m01, m02, -m03]
        __m128 const r0 = _mm_mul_ps(matrix.Inner[0], negate_yw);
        __m128 const r1 = _mm_mul_ps(v0, s01);
        __m128 const r2 = Vector4F_NegateMultiplyAdd(v1, s23, r1);
        __m128 const r3 = Vector4F_MultiplyAdd(v2, s45, r2);

        return Vector4F_Dot4(r0, r3);
#elif ANEMONE_FEATURE_NEON
        // Note: implementation is based on AVX version, however to avoid unnecessary shuffling,
        // we changed the order of columns.

        float32x2_t const m20_m21 = vget_low_f32(matrix.val[2]);
        float32x2_t const m22_m23 = vget_high_f32(matrix.val[2]);
        float32x2_t const m30_m31 = vget_low_f32(matrix.val[3]);
        float32x2_t const m32_m33 = vget_high_f32(matrix.val[3]);

        float32x2_t const m20_m20 = vdup_lane_f32(m20_m21, 0);
        float32x2_t const m23_m23 = vdup_lane_f32(m22_m23, 1);
        float32x2_t const m22_m22 = vdup_lane_f32(m22_m23, 0);
        float32x2_t const m21_m21 = vdup_lane_f32(m20_m21, 1);

        float32x2_t const m31_m31 = vdup_lane_f32(m30_m31, 1);
        float32x2_t const m30_m30 = vdup_lane_f32(m30_m31, 0);
        float32x2_t const m32_m32 = vdup_lane_f32(m32_m33, 0);
        float32x2_t const m33_m33 = vdup_lane_f32(m32_m33, 1);

        // = [m21, m20, m20, m20]
        float32x4_t const t0 = vcombine_f32(m20_m21, m20_m20);
        // = [m32, m32, m31, m31]
        float32x4_t const t1 = vcombine_f32(m32_m32, m31_m31);
        // = [m33, m33, m33, m32]
        float32x4_t const t2 = vcombine_f32(m33_m33, m32_m33);
        // = [m22, m22, m21, m21]
        float32x4_t const t3 = vcombine_f32(m22_m22, m21_m21);

        // = [m21, m20, m20, m20] * [m32, m32, m31, m31]
        float32x4_t const t01 = vmulq_f32(t0, t1);
        // = [m21, m20, m20, m20] * [m33, m33, m33, m32]
        float32x4_t const t02 = vmulq_f32(t0, t2);
        // = [m33, m33, m33, m32] * [m22, m22, m21, m21]
        float32x4_t const t23 = vmulq_f32(t2, t3);

        // = [m31, m30, m30, m30]
        float32x4_t const t4 = vcombine_f32(m30_m31, m30_m30);
        // = [m23, m23, m23, m22]
        float32x4_t const t5 = vcombine_f32(m23_m23, m22_m23);

        // = [m21, m20, m20, m20] * [m32, m32, m31, m31] - [m22, m22, m21, m21] * [m31, m30, m30, m30]
        float32x4_t const s01 = vmlsq_f32(t01, t3, t4);
        // = [m21, m20, m20, m20] * [m33, m33, m33, m32] - [m23, m23, m23, m22] * [m31, m30, m30, m30]
        float32x4_t const s23 = vmlsq_f32(t02, t5, t4);
        // = [m33, m33, m33, m32] * [m22, m22, m21, m21] - [m23, m23, m23, m22] * [m32, m32, m31, m31]
        float32x4_t const s45 = vmlsq_f32(t23, t5, t1);

        float32x2_t const m10_m11 = vget_low_f32(matrix.val[1]);
        float32x2_t const m12_m13 = vget_high_f32(matrix.val[1]);
        float32x2_t const m10_m10 = vdup_lane_f32(m10_m11, 0);
        float32x2_t const m11_m11 = vdup_lane_f32(m10_m11, 1);
        float32x2_t const m12_m12 = vdup_lane_f32(m12_m13, 0);
        float32x2_t const m13_m13 = vdup_lane_f32(m12_m13, 1);

        // = [m13, m13, m13, m12]
        float32x4_t const v0 = vcombine_f32(m13_m13, m12_m13);
        // = [m12, m12, m11, m11]
        float32x4_t const v1 = vcombine_f32(m12_m12, m11_m11);
        // = [m11, m10, m10, m10];
        float32x4_t const v2 = vcombine_f32(m10_m11, m10_m10);

        static constexpr float fnegate_yw[4]{-1.0f, 1.0f, -1.0f, 1.0f};
        float32x4_t const negate_yw = vld1q_f32(fnegate_yw);

        float32x2_t const m00_m01 = vget_low_f32(matrix.val[0]);
        float32x2_t const m02_m03 = vget_high_f32(matrix.val[0]);

        float32x2_t const m01_m00 = vrev64_f32(m00_m01);
        float32x2_t const m03_m02 = vrev64_f32(m02_m03);

        // = [m00, -m01, m02, -m03]
        float32x4_t const r0 = vmulq_f32(vcombine_f32(m01_m00, m03_m02), negate_yw);
        float32x4_t const r1 = vmulq_f32(v0, s01);
        float32x4_t const r2 = vmlsq_f32(r1, v1, s23);
        float32x4_t const r3 = vmlaq_f32(r2, v2, s45);

        return Vector4F_Dot4(r0, r3);
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Inverse(SimdMatrix4x4F matrix, SimdVector4F& determinant)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        // Use the Cramer's rule to compute the inverse of a 4x4 matrix.
        //
        // M^-1 = 1 / det(M) * adj(M)
        //
        // adj(M) = C^T
        //
        // Cij = (-1)^(i+j) * det(Mij)

        float const a = matrix.Inner[0].Inner[0];
        float const b = matrix.Inner[0].Inner[1];
        float const c = matrix.Inner[0].Inner[2];
        float const d = matrix.Inner[0].Inner[3];

        float const e = matrix.Inner[1].Inner[0];
        float const f = matrix.Inner[1].Inner[1];
        float const g = matrix.Inner[1].Inner[2];
        float const h = matrix.Inner[1].Inner[3];

        float const i = matrix.Inner[2].Inner[0];
        float const j = matrix.Inner[2].Inner[1];
        float const k = matrix.Inner[2].Inner[2];
        float const l = matrix.Inner[2].Inner[3];

        float const m = matrix.Inner[3].Inner[0];
        float const n = matrix.Inner[3].Inner[1];
        float const o = matrix.Inner[3].Inner[2];
        float const p = matrix.Inner[3].Inner[3];

        float const kp_lo = (k * p) - (l * o);
        float const jp_ln = (j * p) - (l * n);
        float const jo_kn = (j * o) - (k * n);
        float const ip_lm = (i * p) - (l * m);
        float const io_km = (i * o) - (k * m);
        float const in_jm = (i * n) - (j * m);

        float const a11 = +((f * kp_lo) - (g * jp_ln) + (h * jo_kn));
        float const a12 = -((e * kp_lo) - (g * ip_lm) + (h * io_km));
        float const a13 = +((e * jp_ln) - (f * ip_lm) + (h * in_jm));
        float const a14 = -((e * jo_kn) - (f * io_km) + (g * in_jm));

        float const det = (a * a11) + (b * a12) + (c * a13) + (d * a14);
        determinant = SimdVector4F{det, det, det, det};

        if (IsNearZero<float>(det))
        {
            return Matrix4x4F_NaN();
        }

        SimdMatrix4x4F result;
        float const invDet = 1.0f / det;

        result.Inner[0].Inner[0] = a11 * invDet;
        result.Inner[1].Inner[0] = a12 * invDet;
        result.Inner[2].Inner[0] = a13 * invDet;
        result.Inner[3].Inner[0] = a14 * invDet;

        result.Inner[0].Inner[1] = -((b * kp_lo) - (c * jp_ln) + (d * jo_kn)) * invDet;
        result.Inner[1].Inner[1] = +((a * kp_lo) - (c * ip_lm) + (d * io_km)) * invDet;
        result.Inner[2].Inner[1] = -((a * jp_ln) - (b * ip_lm) + (d * in_jm)) * invDet;
        result.Inner[3].Inner[1] = +((a * jo_kn) - (b * io_km) + (c * in_jm)) * invDet;

        float const gp_ho = (g * p) - (h * o);
        float const fp_hn = (f * p) - (h * n);
        float const fo_gn = (f * o) - (g * n);
        float const ep_hm = (e * p) - (h * m);
        float const eo_gm = (e * o) - (g * m);
        float const en_fm = (e * n) - (f * m);

        result.Inner[0].Inner[2] = +((b * gp_ho) - (c * fp_hn) + (d * fo_gn)) * invDet;
        result.Inner[1].Inner[2] = -((a * gp_ho) - (c * ep_hm) + (d * eo_gm)) * invDet;
        result.Inner[2].Inner[2] = +((a * fp_hn) - (b * ep_hm) + (d * en_fm)) * invDet;
        result.Inner[3].Inner[2] = -((a * fo_gn) - (b * eo_gm) + (c * en_fm)) * invDet;

        float const gl_hk = (g * l) - (h * k);
        float const fl_hj = (f * l) - (h * j);
        float const fk_gj = (f * k) - (g * j);
        float const el_hi = (e * l) - (h * i);
        float const ek_gi = (e * k) - (g * i);
        float const ej_fi = (e * j) - (f * i);

        result.Inner[0].Inner[3] = -((b * gl_hk) - (c * fl_hj) + (d * fk_gj)) * invDet;
        result.Inner[1].Inner[3] = +((a * gl_hk) - (c * el_hi) + (d * ek_gi)) * invDet;
        result.Inner[2].Inner[3] = -((a * fl_hj) - (b * el_hi) + (d * ej_fi)) * invDet;
        result.Inner[3].Inner[3] = +((a * fk_gj) - (b * ek_gi) + (c * ej_fi)) * invDet;

        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // Transpose matrix
        __m128 const m00_m10_m01_m11 = _mm_shuffle_ps(matrix.Inner[0], matrix.Inner[1], _MM_SHUFFLE(1, 0, 1, 0));
        __m128 const m20_m30_m21_m31 = _mm_shuffle_ps(matrix.Inner[0], matrix.Inner[1], _MM_SHUFFLE(3, 2, 3, 2));
        __m128 const m02_m12_m03_m13 = _mm_shuffle_ps(matrix.Inner[2], matrix.Inner[3], _MM_SHUFFLE(1, 0, 1, 0));
        __m128 const m22_m32_m23_m33 = _mm_shuffle_ps(matrix.Inner[2], matrix.Inner[3], _MM_SHUFFLE(3, 2, 3, 2));

        __m128 const m00_m01_m02_m03 = _mm_shuffle_ps(m00_m10_m01_m11, m02_m12_m03_m13, _MM_SHUFFLE(2, 0, 2, 0));
        __m128 const m10_m11_m12_m13 = _mm_shuffle_ps(m00_m10_m01_m11, m02_m12_m03_m13, _MM_SHUFFLE(3, 1, 3, 1));
        __m128 const m20_m21_m22_m23 = _mm_shuffle_ps(m20_m30_m21_m31, m22_m32_m23_m33, _MM_SHUFFLE(2, 0, 2, 0));
        __m128 const m30_m31_m32_m33 = _mm_shuffle_ps(m20_m30_m21_m31, m22_m32_m23_m33, _MM_SHUFFLE(3, 1, 3, 1));

        // Calculate inverse using Cramer's rule
        __m128 const m20_m20_m21_m21 = _mm_permute_ps(m20_m21_m22_m23, _MM_SHUFFLE(1, 1, 0, 0));
        __m128 const m32_m33_m32_m33 = _mm_permute_ps(m30_m31_m32_m33, _MM_SHUFFLE(3, 2, 3, 2));
        __m128 const m00_m00_m01_m01 = _mm_permute_ps(m00_m01_m02_m03, _MM_SHUFFLE(1, 1, 0, 0));
        __m128 const m12_m13_m12_m13 = _mm_permute_ps(m10_m11_m12_m13, _MM_SHUFFLE(3, 2, 3, 2));
        __m128 const m20_m22_m00_m02 = _mm_shuffle_ps(m20_m21_m22_m23, m00_m01_m02_m03, _MM_SHUFFLE(2, 0, 2, 0));
        __m128 const m31_m33_m11_m13 = _mm_shuffle_ps(m30_m31_m32_m33, m10_m11_m12_m13, _MM_SHUFFLE(3, 1, 3, 1));

        __m128 d0x_d0y_d0z_d0w = _mm_mul_ps(m20_m20_m21_m21, m32_m33_m32_m33);
        __m128 d1x_d1y_d1z_d1w = _mm_mul_ps(m00_m00_m01_m01, m12_m13_m12_m13);
        __m128 d2x_d2y_d2z_d2w = _mm_mul_ps(m20_m22_m00_m02, m31_m33_m11_m13);

        __m128 const m22_m23_m22_m23 = _mm_permute_ps(m20_m21_m22_m23, _MM_SHUFFLE(3, 2, 3, 2));
        __m128 const m30_m30_m31_m31 = _mm_permute_ps(m30_m31_m32_m33, _MM_SHUFFLE(1, 1, 0, 0));
        __m128 const m02_m03_m02_m03 = _mm_permute_ps(m00_m01_m02_m03, _MM_SHUFFLE(3, 2, 3, 2));
        __m128 const m10_m10_m11_m11 = _mm_permute_ps(m10_m11_m12_m13, _MM_SHUFFLE(1, 1, 0, 0));
        __m128 const m21_m23_m01_m03 = _mm_shuffle_ps(m20_m21_m22_m23, m00_m01_m02_m03, _MM_SHUFFLE(3, 1, 3, 1));
        __m128 const m30_m32_m10_m12 = _mm_shuffle_ps(m30_m31_m32_m33, m10_m11_m12_m13, _MM_SHUFFLE(2, 0, 2, 0));

        d0x_d0y_d0z_d0w = Vector4F_NegateMultiplyAdd(m22_m23_m22_m23, m30_m30_m31_m31, d0x_d0y_d0z_d0w);
        d1x_d1y_d1z_d1w = Vector4F_NegateMultiplyAdd(m02_m03_m02_m03, m10_m10_m11_m11, d1x_d1y_d1z_d1w);
        d2x_d2y_d2z_d2w = Vector4F_NegateMultiplyAdd(m21_m23_m01_m03, m30_m32_m10_m12, d2x_d2y_d2z_d2w);

        __m128 const d0y_d0w_d2y_d2y = _mm_shuffle_ps(d0x_d0y_d0z_d0w, d2x_d2y_d2z_d2w, _MM_SHUFFLE(1, 1, 3, 1));
        __m128 const m11_m12_m10_m11 = _mm_permute_ps(m10_m11_m12_m13, _MM_SHUFFLE(1, 0, 2, 1));
        __m128 const d2y_d0y_d0w_d0x = _mm_shuffle_ps(d0y_d0w_d2y_d2y, d0x_d0y_d0z_d0w, _MM_SHUFFLE(0, 3, 0, 2));
        __m128 const m02_m00_m01_m00 = _mm_permute_ps(m00_m01_m02_m03, _MM_SHUFFLE(0, 1, 0, 2));
        __m128 const d0w_d2y_d0y_d0z = _mm_shuffle_ps(d0y_d0w_d2y_d2y, d0x_d0y_d0z_d0w, _MM_SHUFFLE(2, 1, 2, 1));

        __m128 const d1y_d1w_d2w_d2w = _mm_shuffle_ps(d1x_d1y_d1z_d1w, d2x_d2y_d2z_d2w, _MM_SHUFFLE(3, 3, 3, 1));
        __m128 const m31_m32_m30_m31 = _mm_permute_ps(m30_m31_m32_m33, _MM_SHUFFLE(1, 0, 2, 1));
        __m128 const d2w_d1y_d1w_d1x = _mm_shuffle_ps(d1y_d1w_d2w_d2w, d1x_d1y_d1z_d1w, _MM_SHUFFLE(0, 3, 0, 2));
        __m128 const m22_m20_m21_m20 = _mm_permute_ps(m20_m21_m22_m23, _MM_SHUFFLE(0, 1, 0, 2));
        __m128 const d1w_d2w_d1y_d1z = _mm_shuffle_ps(d1y_d1w_d2w_d2w, d1x_d1y_d1z_d1w, _MM_SHUFFLE(2, 1, 2, 1));

        __m128 c0x_c0y_c0z_c0w = _mm_mul_ps(m11_m12_m10_m11, d2y_d0y_d0w_d0x);
        __m128 c2x_c2y_c2z_c2w = _mm_mul_ps(m02_m00_m01_m00, d0w_d2y_d0y_d0z);
        __m128 c4x_c4y_c4z_c4w = _mm_mul_ps(m31_m32_m30_m31, d2w_d1y_d1w_d1x);
        __m128 c6x_c6y_c6z_c6w = _mm_mul_ps(m22_m20_m21_m20, d1w_d2w_d1y_d1z);

        __m128 const d0x_d0y_d2x_d2x = _mm_shuffle_ps(d0x_d0y_d0z_d0w, d2x_d2y_d2z_d2w, _MM_SHUFFLE(0, 0, 1, 0));
        __m128 const m12_m13_m11_m12 = _mm_permute_ps(m10_m11_m12_m13, _MM_SHUFFLE(2, 1, 3, 2));
        __m128 const d0w_d0x_d0y_d2x = _mm_shuffle_ps(d0x_d0y_d0z_d0w, d0x_d0y_d2x_d2x, _MM_SHUFFLE(2, 1, 0, 3));
        __m128 const m03_m02_m03_m01 = _mm_permute_ps(m00_m01_m02_m03, _MM_SHUFFLE(1, 3, 2, 3));
        __m128 const d0z_d0y_d2x_d0x = _mm_shuffle_ps(d0x_d0y_d0z_d0w, d0x_d0y_d2x_d2x, _MM_SHUFFLE(0, 2, 1, 2));

        __m128 const d1x_d1y_d2z_d2z = _mm_shuffle_ps(d1x_d1y_d1z_d1w, d2x_d2y_d2z_d2w, _MM_SHUFFLE(2, 2, 1, 0));
        __m128 const m32_m33_m31_m32 = _mm_permute_ps(m30_m31_m32_m33, _MM_SHUFFLE(2, 1, 3, 2));
        __m128 const d1w_d1x_d1y_d2z = _mm_shuffle_ps(d1x_d1y_d1z_d1w, d1x_d1y_d2z_d2z, _MM_SHUFFLE(2, 1, 0, 3));
        __m128 const m23_m22_m23_m21 = _mm_permute_ps(m20_m21_m22_m23, _MM_SHUFFLE(1, 3, 2, 3));
        __m128 const d1z_d1y_d2z_d1x = _mm_shuffle_ps(d1x_d1y_d1z_d1w, d1x_d1y_d2z_d2z, _MM_SHUFFLE(0, 2, 1, 2));

        c0x_c0y_c0z_c0w = Vector4F_NegateMultiplyAdd(m12_m13_m11_m12, d0w_d0x_d0y_d2x, c0x_c0y_c0z_c0w);
        c2x_c2y_c2z_c2w = Vector4F_NegateMultiplyAdd(m03_m02_m03_m01, d0z_d0y_d2x_d0x, c2x_c2y_c2z_c2w);
        c4x_c4y_c4z_c4w = Vector4F_NegateMultiplyAdd(m32_m33_m31_m32, d1w_d1x_d1y_d2z, c4x_c4y_c4z_c4w);
        c6x_c6y_c6z_c6w = Vector4F_NegateMultiplyAdd(m23_m22_m23_m21, d1z_d1y_d2z_d1x, c6x_c6y_c6z_c6w);

        __m128 const m13_m10_m13_m10 = _mm_permute_ps(m10_m11_m12_m13, _MM_SHUFFLE(0, 3, 0, 3));
        __m128 const d0z_d0z_d2x_d2y = _mm_shuffle_ps(d0x_d0y_d0z_d0w, d2x_d2y_d2z_d2w, _MM_SHUFFLE(1, 0, 2, 2));
        __m128 const d0z_d2y_d2x_d0z = _mm_permute_ps(d0z_d0z_d2x_d2y, _MM_SHUFFLE(0, 2, 3, 0));
        __m128 const m01_m03_m00_m02 = _mm_permute_ps(m00_m01_m02_m03, _MM_SHUFFLE(2, 0, 3, 1));
        __m128 const d0x_d0w_d2x_d2y = _mm_shuffle_ps(d0x_d0y_d0z_d0w, d2x_d2y_d2z_d2w, _MM_SHUFFLE(1, 0, 3, 0));
        __m128 const d2y_d0x_d0w_d2x = _mm_permute_ps(d0x_d0w_d2x_d2y, _MM_SHUFFLE(2, 1, 0, 3));
        __m128 const m33_m30_m33_m30 = _mm_permute_ps(m30_m31_m32_m33, _MM_SHUFFLE(0, 3, 0, 3));
        __m128 const d1z_d1z_d2z_d2w = _mm_shuffle_ps(d1x_d1y_d1z_d1w, d2x_d2y_d2z_d2w, _MM_SHUFFLE(3, 2, 2, 2));
        __m128 const d1z_d2w_d2z_d1z = _mm_permute_ps(d1z_d1z_d2z_d2w, _MM_SHUFFLE(0, 2, 3, 0));
        __m128 const m21_m23_m20_m22 = _mm_permute_ps(m20_m21_m22_m23, _MM_SHUFFLE(2, 0, 3, 1));
        __m128 const d1x_d1w_d2z_d2w = _mm_shuffle_ps(d1x_d1y_d1z_d1w, d2x_d2y_d2z_d2w, _MM_SHUFFLE(3, 2, 3, 0));
        __m128 const d2w_d1x_d1w_d2z = _mm_permute_ps(d1x_d1w_d2z_d2w, _MM_SHUFFLE(2, 1, 0, 3));

        __m128 const t0 = _mm_mul_ps(m13_m10_m13_m10, d0z_d2y_d2x_d0z);
        __m128 const t1 = _mm_mul_ps(m01_m03_m00_m02, d2y_d0x_d0w_d2x);
        __m128 const t2 = _mm_mul_ps(m33_m30_m33_m30, d1z_d2w_d2z_d1z);
        __m128 const t3 = _mm_mul_ps(m21_m23_m20_m22, d2w_d1x_d1w_d2z);

        __m128 c1x_c1y_c1z_c1w = _mm_sub_ps(c0x_c0y_c0z_c0w, t0);
        c0x_c0y_c0z_c0w = _mm_add_ps(c0x_c0y_c0z_c0w, t0);
        __m128 c3x_c3y_c3z_c3w = _mm_add_ps(c2x_c2y_c2z_c2w, t1);
        c2x_c2y_c2z_c2w = _mm_sub_ps(c2x_c2y_c2z_c2w, t1);
        __m128 c5x_c5y_c5z_c5w = _mm_sub_ps(c4x_c4y_c4z_c4w, t2);
        c4x_c4y_c4z_c4w = _mm_add_ps(c4x_c4y_c4z_c4w, t2);
        __m128 c7x_c7y_c7z_c7w = _mm_add_ps(c6x_c6y_c6z_c6w, t3);
        c6x_c6y_c6z_c6w = _mm_sub_ps(c6x_c6y_c6z_c6w, t3);

        __m128 const c0x_c0z_c1y_c1w = _mm_shuffle_ps(c0x_c0y_c0z_c0w, c1x_c1y_c1z_c1w, _MM_SHUFFLE(3, 1, 2, 0));
        __m128 const c2x_c2z_c3y_c3w = _mm_shuffle_ps(c2x_c2y_c2z_c2w, c3x_c3y_c3z_c3w, _MM_SHUFFLE(3, 1, 2, 0));
        __m128 const c4x_c4z_c5y_c5w = _mm_shuffle_ps(c4x_c4y_c4z_c4w, c5x_c5y_c5z_c5w, _MM_SHUFFLE(3, 1, 2, 0));
        __m128 const c6x_c6z_c7y_c7w = _mm_shuffle_ps(c6x_c6y_c6z_c6w, c7x_c7y_c7z_c7w, _MM_SHUFFLE(3, 1, 2, 0));
        __m128 const c0x_c1y_c0z_c1w = _mm_permute_ps(c0x_c0z_c1y_c1w, _MM_SHUFFLE(3, 1, 2, 0));
        __m128 const c2x_c3y_c2z_c3w = _mm_permute_ps(c2x_c2z_c3y_c3w, _MM_SHUFFLE(3, 1, 2, 0));
        __m128 const c4x_c5y_c4z_c5w = _mm_permute_ps(c4x_c4z_c5y_c5w, _MM_SHUFFLE(3, 1, 2, 0));
        __m128 const c6x_c7y_c6z_c7w = _mm_permute_ps(c6x_c6z_c7y_c7w, _MM_SHUFFLE(3, 1, 2, 0));

        __m128 const det = Vector4F_Dot4(c0x_c1y_c0z_c1w, m00_m01_m02_m03);
        determinant = det;

        __m128 const reciprocal = _mm_div_ps(_mm_load_ps(F32x4_PositiveOne.As<float>()), det);

        SimdMatrix4x4F result;
        result.Inner[0] = _mm_mul_ps(c0x_c1y_c0z_c1w, reciprocal);
        result.Inner[1] = _mm_mul_ps(c2x_c3y_c2z_c3w, reciprocal);
        result.Inner[2] = _mm_mul_ps(c4x_c5y_c4z_c5w, reciprocal);
        result.Inner[3] = _mm_mul_ps(c6x_c7y_c6z_c7w, reciprocal);
        return result;
#elif ANEMONE_FEATURE_NEON
        SimdMatrix4x4F const transposed = Matrix4x4F_Transpose(matrix);
        SimdVector4F const r0 = transposed.val[0];
        SimdVector4F const r1 = transposed.val[1];
        SimdVector4F const r2 = transposed.val[2];
        SimdVector4F const r3 = transposed.val[3];

        SimdVector4F const m20_m20_m21_m21 = Vector4F_Permute<0, 0, 1, 1>(r2);
        SimdVector4F const m32_m33_m32_m33 = Vector4F_Permute<2, 3, 2, 3>(r3);
        SimdVector4F const m00_m00_m01_m01 = Vector4F_Permute<0, 0, 1, 1>(r0);
        SimdVector4F const m12_m13_m12_m13 = Vector4F_Permute<2, 3, 2, 3>(r1);
        SimdVector4F const m20_m22_m00_m02 = Vector4F_Permute<0, 2, 4, 6>(r2, r0);
        SimdVector4F const m31_m33_m11_m13 = Vector4F_Permute<1, 3, 5, 7>(r3, r1);

        SimdVector4F d0x_d0y_d0z_d0w = Vector4F_Multiply(m20_m20_m21_m21, m32_m33_m32_m33);
        SimdVector4F d1x_d1y_d1z_d1w = Vector4F_Multiply(m00_m00_m01_m01, m12_m13_m12_m13);
        SimdVector4F d2x_d2y_d2z_d2w = Vector4F_Multiply(m20_m22_m00_m02, m31_m33_m11_m13);

        SimdVector4F const m22_m23_m22_m23 = Vector4F_Permute<2, 3, 2, 3>(r2);
        SimdVector4F const m30_m30_m31_m31 = Vector4F_Permute<0, 0, 1, 1>(r3);
        SimdVector4F const m02_m03_m02_m03 = Vector4F_Permute<2, 3, 2, 3>(r0);
        SimdVector4F const m10_m10_m11_m11 = Vector4F_Permute<0, 0, 1, 1>(r1);
        SimdVector4F const m21_m23_m01_m03 = Vector4F_Permute<1, 3, 5, 7>(r2, r0);
        SimdVector4F const m30_m32_m10_m12 = Vector4F_Permute<0, 2, 4, 6>(r3, r1);

        d0x_d0y_d0z_d0w = Vector4F_NegateMultiplyAdd(m22_m23_m22_m23, m30_m30_m31_m31, d0x_d0y_d0z_d0w);
        d1x_d1y_d1z_d1w = Vector4F_NegateMultiplyAdd(m02_m03_m02_m03, m10_m10_m11_m11, d1x_d1y_d1z_d1w);
        d2x_d2y_d2z_d2w = Vector4F_NegateMultiplyAdd(m21_m23_m01_m03, m30_m32_m10_m12, d2x_d2y_d2z_d2w);

        SimdVector4F const d0y_d0w_d2y_d2y = Vector4F_Permute<1, 3, 5, 5>(d0x_d0y_d0z_d0w, d2x_d2y_d2z_d2w);
        SimdVector4F const m11_m12_m10_m11 = Vector4F_Permute<1, 2, 0, 1>(r1);
        SimdVector4F const d2y_d0y_d0w_d0x = Vector4F_Permute<2, 0, 7, 4>(d0y_d0w_d2y_d2y, d0x_d0y_d0z_d0w);
        SimdVector4F const m02_m00_m01_m00 = Vector4F_Permute<2, 0, 1, 0>(r0);
        SimdVector4F const d0w_d2y_d0y_d0z = Vector4F_Permute<1, 2, 5, 6>(d0y_d0w_d2y_d2y, d0x_d0y_d0z_d0w);

        SimdVector4F const d1y_d1w_d2w_d2w = Vector4F_Permute<1, 3, 7, 7>(d1x_d1y_d1z_d1w, d2x_d2y_d2z_d2w);
        SimdVector4F const m31_m32_m30_m31 = Vector4F_Permute<1, 2, 0, 1>(r3);
        SimdVector4F const d2w_d1y_d1w_d1x = Vector4F_Permute<2, 0, 7, 4>(d1y_d1w_d2w_d2w, d1x_d1y_d1z_d1w);
        SimdVector4F const m22_m20_m21_m20 = Vector4F_Permute<2, 0, 1, 0>(r2);
        SimdVector4F const d1w_d2w_d1y_d1z = Vector4F_Permute<1, 2, 5, 6>(d1y_d1w_d2w_d2w, d1x_d1y_d1z_d1w);

        SimdVector4F c0x_c0y_c0z_c0w = Vector4F_Multiply(m11_m12_m10_m11, d2y_d0y_d0w_d0x);
        SimdVector4F c2x_c2y_c2z_c2w = Vector4F_Multiply(m02_m00_m01_m00, d0w_d2y_d0y_d0z);
        SimdVector4F c4x_c4y_c4z_c4w = Vector4F_Multiply(m31_m32_m30_m31, d2w_d1y_d1w_d1x);
        SimdVector4F c6x_c6y_c6z_c6w = Vector4F_Multiply(m22_m20_m21_m20, d1w_d2w_d1y_d1z);

        SimdVector4F const d0x_d0y_d2x_d2x = Vector4F_Permute<0, 1, 4, 4>(d0x_d0y_d0z_d0w, d2x_d2y_d2z_d2w);
        SimdVector4F const m12_m13_m11_m12 = Vector4F_Permute<2, 3, 1, 2>(r1);
        SimdVector4F const d0w_d0x_d0y_d2x = Vector4F_Permute<3, 0, 5, 6>(d0x_d0y_d0z_d0w, d0x_d0y_d2x_d2x);
        SimdVector4F const m03_m02_m03_m01 = Vector4F_Permute<3, 2, 3, 1>(r0);
        SimdVector4F const d0z_d0y_d2x_d0x = Vector4F_Permute<2, 1, 6, 4>(d0x_d0y_d0z_d0w, d0x_d0y_d2x_d2x);

        SimdVector4F const d1x_d1y_d2z_d2z = Vector4F_Permute<0, 1, 6, 6>(d1x_d1y_d1z_d1w, d2x_d2y_d2z_d2w);
        SimdVector4F const m32_m33_m31_m32 = Vector4F_Permute<2, 3, 1, 2>(r3);
        SimdVector4F const d1w_d1x_d1y_d2z = Vector4F_Permute<3, 0, 5, 6>(d1x_d1y_d1z_d1w, d1x_d1y_d2z_d2z);
        SimdVector4F const m23_m22_m23_m21 = Vector4F_Permute<3, 2, 3, 1>(r2);
        SimdVector4F const d1z_d1y_d2z_d1x = Vector4F_Permute<2, 1, 6, 4>(d1x_d1y_d1z_d1w, d1x_d1y_d2z_d2z);

        c0x_c0y_c0z_c0w = Vector4F_NegateMultiplyAdd(m12_m13_m11_m12, d0w_d0x_d0y_d2x, c0x_c0y_c0z_c0w);
        c2x_c2y_c2z_c2w = Vector4F_NegateMultiplyAdd(m03_m02_m03_m01, d0z_d0y_d2x_d0x, c2x_c2y_c2z_c2w);
        c4x_c4y_c4z_c4w = Vector4F_NegateMultiplyAdd(m32_m33_m31_m32, d1w_d1x_d1y_d2z, c4x_c4y_c4z_c4w);
        c6x_c6y_c6z_c6w = Vector4F_NegateMultiplyAdd(m23_m22_m23_m21, d1z_d1y_d2z_d1x, c6x_c6y_c6z_c6w);

        SimdVector4F const m13_m10_m13_m10 = Vector4F_Permute<3, 0, 3, 0>(r1);
        SimdVector4F const d0z_d0z_d2x_d2y = Vector4F_Permute<2, 2, 4, 5>(d0x_d0y_d0z_d0w, d2x_d2y_d2z_d2w);
        SimdVector4F const d0z_d2y_d2x_d0z = Vector4F_Permute<0, 3, 2, 0>(d0z_d0z_d2x_d2y);
        SimdVector4F const m01_m03_m00_m02 = Vector4F_Permute<1, 3, 0, 2>(r0);
        SimdVector4F const d0x_d0w_d2x_d2y = Vector4F_Permute<0, 3, 4, 5>(d0x_d0y_d0z_d0w, d2x_d2y_d2z_d2w);
        SimdVector4F const d2y_d0x_d0w_d2x = Vector4F_Permute<3, 0, 1, 2>(d0x_d0w_d2x_d2y);
        SimdVector4F const m33_m30_m33_m30 = Vector4F_Permute<3, 0, 3, 0>(r3);
        SimdVector4F const d1z_d1z_d2z_d2w = Vector4F_Permute<2, 2, 6, 7>(d1x_d1y_d1z_d1w, d2x_d2y_d2z_d2w);
        SimdVector4F const d1z_d2w_d2z_d1z = Vector4F_Permute<0, 3, 2, 0>(d1z_d1z_d2z_d2w);
        SimdVector4F const m21_m23_m20_m22 = Vector4F_Permute<1, 3, 0, 2>(r2);
        SimdVector4F const d1x_d1w_d2z_d2w = Vector4F_Permute<0, 3, 6, 7>(d1x_d1y_d1z_d1w, d2x_d2y_d2z_d2w);
        SimdVector4F const d2w_d1x_d1w_d2z = Vector4F_Permute<3, 0, 1, 2>(d1x_d1w_d2z_d2w);

        SimdVector4F const t0 = Vector4F_Multiply(m13_m10_m13_m10, d0z_d2y_d2x_d0z);
        SimdVector4F const t1 = Vector4F_Multiply(m01_m03_m00_m02, d2y_d0x_d0w_d2x);
        SimdVector4F const t2 = Vector4F_Multiply(m33_m30_m33_m30, d1z_d2w_d2z_d1z);
        SimdVector4F const t3 = Vector4F_Multiply(m21_m23_m20_m22, d2w_d1x_d1w_d2z);

        SimdVector4F c1x_c1y_c1z_c1w = Vector4F_Subtract(c0x_c0y_c0z_c0w, t0);
        c0x_c0y_c0z_c0w = Vector4F_Add(c0x_c0y_c0z_c0w, t0);
        SimdVector4F c3x_c3y_c3z_c3w = Vector4F_Add(c2x_c2y_c2z_c2w, t1);
        c2x_c2y_c2z_c2w = Vector4F_Subtract(c2x_c2y_c2z_c2w, t1);
        SimdVector4F c5x_c5y_c5z_c5w = Vector4F_Subtract(c4x_c4y_c4z_c4w, t2);
        c4x_c4y_c4z_c4w = Vector4F_Add(c4x_c4y_c4z_c4w, t2);
        SimdVector4F c7x_c7y_c7z_c7w = Vector4F_Add(c6x_c6y_c6z_c6w, t3);
        c6x_c6y_c6z_c6w = Vector4F_Subtract(c6x_c6y_c6z_c6w, t3);

        SimdVector4F const c0x_c0z_c1y_c1w = Vector4F_Permute<0, 2, 5, 7>(c0x_c0y_c0z_c0w, c1x_c1y_c1z_c1w);
        SimdVector4F const c2x_c2z_c3y_c3w = Vector4F_Permute<0, 2, 5, 7>(c2x_c2y_c2z_c2w, c3x_c3y_c3z_c3w);
        SimdVector4F const c4x_c4z_c5y_c5w = Vector4F_Permute<0, 2, 5, 7>(c4x_c4y_c4z_c4w, c5x_c5y_c5z_c5w);
        SimdVector4F const c6x_c6z_c7y_c7w = Vector4F_Permute<0, 2, 5, 7>(c6x_c6y_c6z_c6w, c7x_c7y_c7z_c7w);
        SimdVector4F const c0x_c1y_c0z_c1w = Vector4F_Permute<0, 2, 1, 3>(c0x_c0z_c1y_c1w);
        SimdVector4F const c2x_c3y_c2z_c3w = Vector4F_Permute<0, 2, 1, 3>(c2x_c2z_c3y_c3w);
        SimdVector4F const c4x_c5y_c4z_c5w = Vector4F_Permute<0, 2, 1, 3>(c4x_c4z_c5y_c5w);
        SimdVector4F const c6x_c7y_c6z_c7w = Vector4F_Permute<0, 2, 1, 3>(c6x_c6z_c7y_c7w);

        SimdVector4F const det = Vector4F_Dot4(c0x_c1y_c0z_c1w, r0);
        determinant = det;

        SimdVector4F const reciprocal = Vector4F_Reciprocal(det);

        SimdMatrix4x4F result;
        result.val[0] = Vector4F_Multiply(c0x_c1y_c0z_c1w, reciprocal);
        result.val[1] = Vector4F_Multiply(c2x_c3y_c2z_c3w, reciprocal);
        result.val[2] = Vector4F_Multiply(c4x_c5y_c4z_c5w, reciprocal);
        result.val[3] = Vector4F_Multiply(c6x_c7y_c6z_c7w, reciprocal);
        return result;
#endif
    }

    inline SimdVector4F anemone_vectorcall Matrix4x4F_Diagonal(SimdMatrix4x4F m)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4F{
            m.Inner[0].Inner[0],
            m.Inner[1].Inner[1],
            m.Inner[2].Inner[2],
            m.Inner[3].Inner[3],
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const m11_m22 = _mm_insert_ps(m.Inner[0], m.Inner[1], 0b01'01'0000);
        __m128 const m11_m22_m33 = _mm_insert_ps(m11_m22, m.Inner[2], 0b10'10'0000);
        return _mm_insert_ps(m11_m22_m33, m.Inner[3], 0b11'11'0000);
#elif ANEMONE_FEATURE_NEON
        // = [m11, ..., ..., ...]
        float32x4_t r = m.val[0];
        // = [m11, m22, ..., ...]
        r = vcopyq_laneq_f32(r, 1, m.val[1], 1);
        // = [m11, m22, m33, ...]
        r = vcopyq_laneq_f32(r, 2, m.val[2], 2);
        // = [m11, m22, m33, m44]
        r = vcopyq_laneq_f32(r, 3, m.val[3], 3);
        return r;
#endif
    }

    inline SimdVector4F anemone_vectorcall Matrix4x4F_Trace(SimdMatrix4x4F m)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        float const r = m.Inner[0].Inner[0] + m.Inner[1].Inner[1] + m.Inner[2].Inner[2] + m.Inner[3].Inner[3];
        return SimdVector4F{
            r,
            r,
            r,
            r,
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const m00 = _mm_permute_ps(m.Inner[0], _MM_SHUFFLE(0, 0, 0, 0));
        __m128 const m11 = _mm_permute_ps(m.Inner[1], _MM_SHUFFLE(1, 1, 1, 1));
        __m128 const m00m11 = _mm_add_ps(m00, m11);
        __m128 const m22 = _mm_permute_ps(m.Inner[2], _MM_SHUFFLE(2, 2, 2, 2));
        __m128 const m33 = _mm_permute_ps(m.Inner[3], _MM_SHUFFLE(3, 3, 3, 3));
        __m128 const m22m33 = _mm_add_ps(m22, m33);
        return _mm_add_ps(m00m11, m22m33);
#elif ANEMONE_FEATURE_NEON
        // = [m11, ...]
        float32x2_t const m11_m12 = vget_low_f32(m.val[0]);
        // = [..., m22]
        float32x2_t const m21_m22 = vget_low_f32(m.val[1]);
        // = [m33, ...]
        float32x2_t const m33_m34 = vget_high_f32(m.val[2]);
        // = [..., m44]
        float32x2_t const m43_m44 = vget_high_f32(m.val[3]);

        // = [m22, m11]
        float32x2_t const m22_m11 = vext_f32(m21_m22, m11_m12, 1);
        // = [m44, m33]
        float32x2_t const m44_m33 = vext_f32(m43_m44, m33_m34, 1);

        // = [m22+m44, m11+m33]
        float32x2_t const t0 = vadd_f32(m22_m11, m44_m33);
        // = [m11+m22+m33+m44]
        float32x2_t const t1 = vpadd_f32(t0, t0);
        return vcombine_f32(t1, t1);
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_TensorProduct(SimdVector4F a, SimdVector4F b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        SimdMatrix4x4F result;

        result.Inner[0].Inner[0] = a.Inner[0] * b.Inner[0];
        result.Inner[0].Inner[1] = a.Inner[0] * b.Inner[1];
        result.Inner[0].Inner[2] = a.Inner[0] * b.Inner[2];
        result.Inner[0].Inner[3] = a.Inner[0] * b.Inner[3];

        result.Inner[1].Inner[0] = a.Inner[1] * b.Inner[0];
        result.Inner[1].Inner[1] = a.Inner[1] * b.Inner[1];
        result.Inner[1].Inner[2] = a.Inner[1] * b.Inner[2];
        result.Inner[1].Inner[3] = a.Inner[1] * b.Inner[3];

        result.Inner[2].Inner[0] = a.Inner[2] * b.Inner[0];
        result.Inner[2].Inner[1] = a.Inner[2] * b.Inner[1];
        result.Inner[2].Inner[2] = a.Inner[2] * b.Inner[2];
        result.Inner[2].Inner[3] = a.Inner[2] * b.Inner[3];

        result.Inner[3].Inner[0] = a.Inner[3] * b.Inner[0];
        result.Inner[3].Inner[1] = a.Inner[3] * b.Inner[1];
        result.Inner[3].Inner[2] = a.Inner[3] * b.Inner[2];
        result.Inner[3].Inner[3] = a.Inner[3] * b.Inner[3];

        return result;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdMatrix4x4F{
            _mm_mul_ps(_mm_permute_ps(a, _MM_SHUFFLE(0, 0, 0, 0)), b),
            _mm_mul_ps(_mm_permute_ps(a, _MM_SHUFFLE(1, 1, 1, 1)), b),
            _mm_mul_ps(_mm_permute_ps(a, _MM_SHUFFLE(2, 2, 2, 2)), b),
            _mm_mul_ps(_mm_permute_ps(a, _MM_SHUFFLE(3, 3, 3, 3)), b),
        };
#elif ANEMONE_FEATURE_NEON
        return SimdMatrix4x4F{
            vmulq_laneq_f32(a, b, 0),
            vmulq_laneq_f32(a, b, 1),
            vmulq_laneq_f32(a, b, 2),
            vmulq_laneq_f32(a, b, 3),
        };
#endif
    }

    inline bool anemone_vectorcall Matrix4x4F_IsIdentity(SimdMatrix4x4F m)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        // Diagonal must be 1, off-diagonal must be 0. Combine bits of all values together.
        uint32_t mask_ones = std::bit_cast<uint32_t>(m.Inner[0].Inner[0]) ^ Float32::PositiveOneBits;
        uint32_t mask_zeros = std::bit_cast<uint32_t>(m.Inner[0].Inner[1]);
        mask_zeros |= std::bit_cast<uint32_t>(m.Inner[0].Inner[2]);
        mask_zeros |= std::bit_cast<uint32_t>(m.Inner[0].Inner[3]);
        mask_zeros |= std::bit_cast<uint32_t>(m.Inner[1].Inner[0]);
        mask_ones |= std::bit_cast<uint32_t>(m.Inner[1].Inner[1]) ^ Float32::PositiveOneBits;
        mask_zeros |= std::bit_cast<uint32_t>(m.Inner[1].Inner[2]);
        mask_zeros |= std::bit_cast<uint32_t>(m.Inner[1].Inner[3]);
        mask_zeros |= std::bit_cast<uint32_t>(m.Inner[2].Inner[0]);
        mask_zeros |= std::bit_cast<uint32_t>(m.Inner[2].Inner[1]);
        mask_ones |= std::bit_cast<uint32_t>(m.Inner[2].Inner[2]) ^ Float32::PositiveOneBits;
        mask_zeros |= std::bit_cast<uint32_t>(m.Inner[2].Inner[3]);
        mask_zeros |= std::bit_cast<uint32_t>(m.Inner[3].Inner[0]);
        mask_zeros |= std::bit_cast<uint32_t>(m.Inner[3].Inner[1]);
        mask_zeros |= std::bit_cast<uint32_t>(m.Inner[3].Inner[2]);
        mask_ones |= std::bit_cast<uint32_t>(m.Inner[3].Inner[3]) ^ Float32::PositiveOneBits;

        // Handle -0 here.
        mask_ones |= Float32::BitAbs(mask_zeros);

        return mask_ones == 0;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const t0 = _mm_cmpeq_ps(m.Inner[0], _mm_load_ps(F32x4_PositiveUnitX.As<float>()));
        __m128 const t1 = _mm_cmpeq_ps(m.Inner[1], _mm_load_ps(F32x4_PositiveUnitY.As<float>()));
        __m128 const t2 = _mm_cmpeq_ps(m.Inner[2], _mm_load_ps(F32x4_PositiveUnitZ.As<float>()));
        __m128 const t3 = _mm_cmpeq_ps(m.Inner[3], _mm_load_ps(F32x4_PositiveUnitW.As<float>()));

        __m128 const t01 = _mm_and_ps(t0, t1);
        __m128 const t23 = _mm_and_ps(t2, t3);
        return _mm_movemask_ps(_mm_and_ps(t01, t23)) == 0b1111;
#elif ANEMONE_FEATURE_NEON
        uint32x4_t const t0 = vceqq_f32(m.val[0], vld1q_f32(F32x4_PositiveUnitX.As<float>()));
        uint32x4_t const t1 = vceqq_f32(m.val[1], vld1q_f32(F32x4_PositiveUnitY.As<float>()));
        uint32x4_t const t2 = vceqq_f32(m.val[2], vld1q_f32(F32x4_PositiveUnitZ.As<float>()));
        uint32x4_t const t3 = vceqq_f32(m.val[3], vld1q_f32(F32x4_PositiveUnitW.As<float>()));

        uint32x4_t const t01 = vandq_u32(t0, t1);
        uint32x4_t const t23 = vandq_u32(t2, t3);
        uint32x4_t const t0123 = vandq_u32(t01, t23);

        return NeonExtractMask4(t0123) == 0b1111;
#endif
    }

    inline bool anemone_vectorcall Matrix4x4F_IsNaN(SimdMatrix4x4F m)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        bool r0 = Float32::IsNaN(m.Inner[0].Inner[0]);
        r0 |= Float32::IsNaN(m.Inner[0].Inner[1]);
        r0 |= Float32::IsNaN(m.Inner[0].Inner[2]);
        r0 |= Float32::IsNaN(m.Inner[0].Inner[3]);

        bool r1 = Float32::IsNaN(m.Inner[1].Inner[0]);
        r1 |= Float32::IsNaN(m.Inner[1].Inner[1]);
        r1 |= Float32::IsNaN(m.Inner[1].Inner[2]);
        r1 |= Float32::IsNaN(m.Inner[1].Inner[3]);

        bool r2 = Float32::IsNaN(m.Inner[2].Inner[0]);
        r2 |= Float32::IsNaN(m.Inner[2].Inner[1]);
        r2 |= Float32::IsNaN(m.Inner[2].Inner[2]);
        r2 |= Float32::IsNaN(m.Inner[2].Inner[3]);

        bool r3 = Float32::IsNaN(m.Inner[3].Inner[0]);
        r3 |= Float32::IsNaN(m.Inner[3].Inner[1]);
        r3 |= Float32::IsNaN(m.Inner[3].Inner[2]);
        r3 |= Float32::IsNaN(m.Inner[3].Inner[3]);

        return r0 || r1 || r2 || r3;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const t01 = _mm_cmpunord_ps(m.Inner[0], m.Inner[1]);
        __m128 const t23 = _mm_cmpunord_ps(m.Inner[2], m.Inner[3]);
        __m128 const t0123 = _mm_or_ps(t01, t23);
        return _mm_movemask_ps(t0123) != 0;
#elif ANEMONE_FEATURE_NEON
        uint32x4_t const t0 = vmvnq_u32(vceqq_f32(m.val[0], m.val[0]));
        uint32x4_t const t1 = vmvnq_u32(vceqq_f32(m.val[1], m.val[1]));
        uint32x4_t const t2 = vmvnq_u32(vceqq_f32(m.val[2], m.val[2]));
        uint32x4_t const t3 = vmvnq_u32(vceqq_f32(m.val[3], m.val[3]));

        uint32x4_t const t01 = vorrq_u32(t0, t1);
        uint32x4_t const t23 = vorrq_u32(t2, t3);
        uint32x4_t const t0123 = vorrq_u32(t01, t23);

        return NeonExtractMask4(t0123) != 0;
#endif
    }

    inline bool anemone_vectorcall Matrix4x4F_IsInfinite(SimdMatrix4x4F m)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        bool r0 = Float32::IsInfinity(m.Inner[0].Inner[0]);
        r0 |= Float32::IsInfinity(m.Inner[0].Inner[1]);
        r0 |= Float32::IsInfinity(m.Inner[0].Inner[2]);
        r0 |= Float32::IsInfinity(m.Inner[0].Inner[3]);

        bool r1 = Float32::IsInfinity(m.Inner[1].Inner[0]);
        r1 |= Float32::IsInfinity(m.Inner[1].Inner[1]);
        r1 |= Float32::IsInfinity(m.Inner[1].Inner[2]);
        r1 |= Float32::IsInfinity(m.Inner[1].Inner[3]);

        bool r2 = Float32::IsInfinity(m.Inner[2].Inner[0]);
        r2 |= Float32::IsInfinity(m.Inner[2].Inner[1]);
        r2 |= Float32::IsInfinity(m.Inner[2].Inner[2]);
        r2 |= Float32::IsInfinity(m.Inner[2].Inner[3]);

        bool r3 = Float32::IsInfinity(m.Inner[3].Inner[0]);
        r3 |= Float32::IsInfinity(m.Inner[3].Inner[1]);
        r3 |= Float32::IsInfinity(m.Inner[3].Inner[2]);
        r3 |= Float32::IsInfinity(m.Inner[3].Inner[3]);

        return r0 || r1 || r2 || r3;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128 const maskAbs = _mm_load_ps(F32x4_AbsMask_XXXX.As<float>());
        __m128 const valueInfinity = _mm_load_ps(F32x4_PositiveInfinity_XXXX.As<float>());

        __m128 const t0 = _mm_cmpeq_ps(_mm_and_ps(m.Inner[0], maskAbs), valueInfinity);
        __m128 const t1 = _mm_cmpeq_ps(_mm_and_ps(m.Inner[1], maskAbs), valueInfinity);
        __m128 const t2 = _mm_cmpeq_ps(_mm_and_ps(m.Inner[2], maskAbs), valueInfinity);
        __m128 const t3 = _mm_cmpeq_ps(_mm_and_ps(m.Inner[3], maskAbs), valueInfinity);

        __m128 const t01 = _mm_or_ps(t0, t1);
        __m128 const t23 = _mm_or_ps(t2, t3);
        __m128 const t0123 = _mm_or_ps(t01, t23);
        return _mm_movemask_ps(t0123) != 0;
#elif ANEMONE_FEATURE_NEON
        float32x4_t const valueInfinity = vld1q_f32(F32x4_PositiveInfinity_XXXX.As<float>());
        float32x4_t const r0 = vabsq_f32(m.val[0]);
        float32x4_t const r1 = vabsq_f32(m.val[1]);
        float32x4_t const r2 = vabsq_f32(m.val[2]);
        float32x4_t const r3 = vabsq_f32(m.val[3]);

        uint32x4_t const t0 = vceqq_f32(r0, valueInfinity);
        uint32x4_t const t1 = vceqq_f32(r1, valueInfinity);
        uint32x4_t const t2 = vceqq_f32(r2, valueInfinity);
        uint32x4_t const t3 = vceqq_f32(r3, valueInfinity);

        uint32x4_t const t01 = vorrq_u32(t0, t1);
        uint32x4_t const t23 = vorrq_u32(t2, t3);
        uint32x4_t const t0123 = vorrq_u32(t01, t23);

        return NeonExtractMask4(t0123) != 0;
#endif
    }
}

// PlaneF
namespace Anemone::Math::Detail
{
    inline SimdVector4F anemone_vectorcall PlaneF_Create(float a, float b, float c, float d)
    {
        return Vector4F_Create(a, b, c, d);
    }

    inline SimdVector4F anemone_vectorcall PlaneF_CreateFromPointNormal(SimdVector4F point, SimdVector4F normal)
    {
        SimdVector4F const d = Vector4F_Negate(Vector4F_Dot3(point, normal));
        return Vector4F_Select<false, false, false, true>(normal, d);
    }

    inline SimdVector4F anemone_vectorcall PlaneF_CreateFromPoints(SimdVector4F point1, SimdVector4F point2, SimdVector4F point3)
    {
        SimdVector4F const p21 = Vector4F_Subtract(point1, point2);
        SimdVector4F const p31 = Vector4F_Subtract(point1, point3);
        SimdVector4F const normal = Vector4F_Normalize3(Vector4F_Cross3(p21, p31));

        SimdVector4F const d = Vector4F_Negate(Vector4F_Dot3(normal, point1));
        return Vector4F_Select<false, false, false, true>(normal, d);
    }

    inline SimdVector4F anemone_vectorcall PlaneF_Dot(SimdVector4F plane, SimdVector4F point)
    {
        return Vector4F_Dot4(plane, point);
    }

    inline SimdVector4F anemone_vectorcall PlaneF_DotCoord(SimdVector4F plane, SimdVector4F point)
    {
        // = [point.xyz, 1]
        SimdVector4F const p = Vector4F_Insert<3>(point, 1.0f);
        return Vector4F_Dot4(plane, p);
    }

    inline SimdVector4F anemone_vectorcall PlaneF_DotNormal(SimdVector4F plane, SimdVector4F point)
    {
        return Vector4F_Dot3(plane, point);
    }

    inline SimdVector4F anemone_vectorcall PlaneF_Normalize(SimdVector4F plane)
    {
        SimdVector4F const length = Vector4F_ReciprocalLength3(plane);
        return Vector4F_Multiply(plane, length);
    }

    inline SimdVector4F anemone_vectorcall PlaneF_IntersectLine(SimdVector4F plane, SimdVector4F linePoint1, SimdVector4F linePoint2)
    {
        SimdVector4F const v1 = Vector4F_Dot3(plane, linePoint1);
        SimdVector4F const v2 = Vector4F_Dot3(plane, linePoint2);
        SimdVector4F const d = Vector4F_Subtract(v1, v2);
        SimdVector4F const t = Vector4F_Divide(PlaneF_DotCoord(plane, linePoint1), d);

        SimdVector4F point = Vector4F_Subtract(linePoint2, linePoint1);
        point = Vector4F_MultiplyAdd(point, t, linePoint1);

        SimdVector4F const zero = Vector4F_Zero();
        SimdMask4F const mask = Vector4F_CompareNearEqual(d, zero);
        return Vector4F_Select(mask, point, Vector4F_NaN());
    }

    inline void anemone_vectorcall PlaneF_IntersectPlane(SimdVector4F& linePoint1, SimdVector4F& linePoint2, SimdVector4F plane1, SimdVector4F plane2)
    {
        SimdVector4F const v1 = Vector4F_Cross3(plane2, plane1);
        SimdVector4F const lengthSquared = Vector4F_LengthSquared3(v1);
        SimdVector4F const v2 = Vector4F_Cross3(plane2, v1);
        SimdVector4F const plane1_wwww = Vector4F_Broadcast<3>(plane1);

        SimdVector4F point = Vector4F_Multiply(v2, plane1_wwww);

        SimdVector4F const v3 = Vector4F_Cross3(v1, plane1);
        SimdVector4F const plane2_wwww = Vector4F_Broadcast<3>(plane2);
        point = Vector4F_MultiplyAdd(v3, plane2_wwww, point);

        SimdVector4F const lp1 = Vector4F_Divide(point, lengthSquared);
        SimdVector4F const lp2 = Vector4F_Add(lp1, v1);

        SimdMask4F const mask = Vector4F_CompareLessEqual(lengthSquared, Vector4F_Epsilon());
        SimdVector4F const nan = Vector4F_NaN();

        linePoint1 = Vector4F_Select(mask, lp1, nan);
        linePoint2 = Vector4F_Select(mask, lp2, nan);
    }

    inline SimdVector4F anemone_vectorcall PlaneF_Transform(SimdVector4F plane, SimdMatrix4x4F matrix)
    {
        return Vector4F_Transform4(plane, matrix);
    }

    inline SimdMask4F anemone_vectorcall PlaneF_CompareEqual(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_CompareEqual(a, b);
    }

    inline bool anemone_vectorcall PlaneF_IsEqual(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_IsEqual4(a, b);
    }

    inline SimdMask4F anemone_vectorcall PlaneF_CompareNearEqual(SimdVector4F a, SimdVector4F b, SimdVector4F tolerance)
    {
        return Vector4F_CompareNearEqual(a, b, tolerance);
    }

    inline SimdMask4F anemone_vectorcall PlaneF_CompareNotEqual(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_CompareNotEqual(a, b);
    }

    inline bool anemone_vectorcall PlaneF_IsNotEqual(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_IsNotEqual4(a, b);
    }

    inline SimdMask4F anemone_vectorcall PlaneF_CompareNaN(SimdVector4F q)
    {
        return Vector4F_CompareNaN(q);
    }

    inline bool anemone_vectorcall PlaneF_IsNaN(SimdVector4F q)
    {
        return Vector4F_IsNaN4(q);
    }

    inline SimdMask4F anemone_vectorcall PlaneF_CompareInfinite(SimdVector4F q)
    {
        return Vector4F_CompareInfinite(q);
    }

    inline bool anemone_vectorcall PlaneF_IsInfinite(SimdVector4F q)
    {
        return Vector4F_IsInfinite4(q);
    }
}
