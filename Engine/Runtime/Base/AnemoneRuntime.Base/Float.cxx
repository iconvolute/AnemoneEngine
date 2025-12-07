#include "AnemoneRuntime.Base/Float.hxx"

#if ANEMONE_BUILD_DISABLE_SIMD
#elif ANEMONE_ARCHITECTURE_X64
#include <immintrin.h>
#elif ANEMONE_ARCHITECTURE_ARM64
#if ANEMONE_PLATFORM_WINDOWS
#include <arm64_neon.h>
#else
#include <arm_neon.h>
#endif
#else
#error "Not implemented"
#endif

#define ANEMONE_FORCE_DISABLE_FP16_INTRINSICS (ANEMONE_PLATFORM_WINDOWS && ANEMONE_ARCHITECTURE_ARM64) || (!ANEMONE_FEATURE_F16C)

namespace Anemone
{
    Half ToHalf(float value)
    {
#if ANEMONE_BUILD_DISABLE_SIMD || ANEMONE_FORCE_DISABLE_FP16_INTRINSICS
        return ToHalfBitwise(value);
#elif ANEMONE_ARCHITECTURE_X64
        // use f16c to convert float to half
        __m128 const v = _mm_set_ss(value);
        __m128i const vi = _mm_cvtps_ph(v, _MM_FROUND_TO_NEAREST_INT);
        return std::bit_cast<Half>(static_cast<uint16_t>(_mm_cvtsi128_si32(vi)));
#elif ANEMONE_ARCHITECTURE_ARM64
        float32x4_t const vf32 = vdupq_n_f32(value);
        float16x4_t const vf16 = vcvt_f16_f32(vf32);

        float16_t rf16[4];
        vst1_f16(rf16, vf16);
        return std::bit_cast<Half>(rf16[0]);
#else
#error "Not implemented"
#endif
    }

    float FromHalf(Half value)
    {
#if ANEMONE_BUILD_DISABLE_SIMD || ANEMONE_FORCE_DISABLE_FP16_INTRINSICS
        return FromHalfBitwise(value);
#elif ANEMONE_ARCHITECTURE_X64
        // use f16c to convert half to float
        __m128i const vi = _mm_cvtsi32_si128(std::bit_cast<uint16_t>(value));
        __m128 const v = _mm_cvtph_ps(vi);
        return _mm_cvtss_f32(v);
#elif ANEMONE_ARCHITECTURE_ARM64
        float16_t const rf16[4] = {std::bit_cast<float16_t>(value)};
        float16x4_t const vf16 = vld1_f16(rf16);
        float32x4_t const vf32 = vcvt_f32_f16(vf16);
        return vgetq_lane_f32(vf32, 0);
#else
#error "Not implemented"
#endif
    }

    std::array<Half, 4> ToHalf(std::array<float, 4> const& values)
    {
#if ANEMONE_BUILD_DISABLE_SIMD || ANEMONE_FORCE_DISABLE_FP16_INTRINSICS
        return {
            ToHalf(values[0]),
            ToHalf(values[1]),
            ToHalf(values[2]),
            ToHalf(values[3]),
        };
#elif ANEMONE_ARCHITECTURE_X64
        __m128 const v = _mm_load_ps(values.data());
        __m128i const vr = _mm_cvtps_ph(v, _MM_FROUND_TO_NEAREST_INT);
        int64_t const r = _mm_cvtsi128_si64(vr);
        return std::bit_cast<std::array<Half, 4>>(r);
#elif ANEMONE_ARCHITECTURE_ARM64
        float32x4_t const vf32 = vld1q_f32(values.data());
        float16x4_t const vf16 = vcvt_f16_f32(vf32);
        std::array<Half, 4> r;
        vst1_f16(reinterpret_cast<float16_t*>(r.data()), vf16);
        return r;
#else
#error "Not implemented"
#endif
    }

    std::array<float, 4> FromHalf(std::array<Half, 4> const& values)
    {
#if ANEMONE_BUILD_DISABLE_SIMD || ANEMONE_FORCE_DISABLE_FP16_INTRINSICS
        return {
            FromHalf(values[0]),
            FromHalf(values[1]),
            FromHalf(values[2]),
            FromHalf(values[3]),
        };
#elif ANEMONE_ARCHITECTURE_X64
        __m128i const vr = _mm_cvtsi64_si128(std::bit_cast<int64_t>(values));
        __m128 const v = _mm_cvtph_ps(vr);
        std::array<float, 4> r;
        _mm_store_ps(r.data(), v);
        return r;
#elif ANEMONE_ARCHITECTURE_ARM64
        float16x4_t const vf16 = vld1_f16(reinterpret_cast<float16_t const*>(values.data()));
        float32x4_t const vf32 = vcvt_f32_f16(vf16);
        std::array<float, 4> r;
        vst1q_f32(r.data(), vf32);
        return r;
#else
#error "Not implemented"
#endif
    }
}
