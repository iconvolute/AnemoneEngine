// ReSharper disable CppClangTidyClangDiagnosticFloatEqual
// ReSharper disable CppPassValueParameterByConstReference
#pragma once
#include "AnemoneMath/Detail/SimdDouble.hxx"

#if ANEMONE_BUILD_DISABLE_SIMD

namespace Anemone::Internal
{
    inline constexpr uint64_t ScalarMaskComponentUInt64{0xFFFF'FFFF'FFFF'FFFF};

    inline double ScalarHelper_SelectBranchless(uint64_t mask, double whenFalse, double whenTrue)
    {
        uint64_t const bitsWhenFalse = std::bit_cast<uint64_t>(whenFalse);
        uint64_t const bitsWhenTrue = std::bit_cast<uint64_t>(whenTrue);
        uint64_t const bitsResult = (mask & bitsWhenTrue) | (~mask & bitsWhenFalse);
        return std::bit_cast<double>(bitsResult);
    }
}

#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2

namespace Anemone::Internal
{
    anemone_forceinline __m256d anemone_vectorcall AvxMultiplyAdd(__m256d a, __m256d b, __m256d c)
    {
#if ANEMONE_FEATURE_FMA4
        return _mm256_macc_pd(a, b, c);
#elif ANEMONE_FEATURE_AVX2
        return _mm256_fmadd_pd(a, b, c);
#else
        // = (a * b) + c
        return _mm256_add_pd(_mm256_mul_pd(a, b), c);
#endif
    }

    anemone_forceinline __m256d anemone_vectorcall AvxMultiplySubtract(__m256d a, __m256d b, __m256d c)
    {
#if ANEMONE_FEATURE_FMA4
        return _mm256_msub_pd(a, b, c);
#elif ANEMONE_FEATURE_AVX2
        return _mm256_fmsub_pd(a, b, c);
#else
        // = (a * b) - c
        return _mm256_sub_pd(_mm256_mul_pd(a, b), c);
#endif
    }

    anemone_forceinline __m256d anemone_vectorcall AvxNegateMultiplyAdd(__m256d a, __m256d b, __m256d c)
    {
#if ANEMONE_FEATURE_FMA4
        return _mm256_nmacc_pd(a, b, c);
#elif ANEMONE_FEATURE_AVX2
        return _mm256_fnmadd_pd(a, b, c);
#else
        // = -(a * b) + c
        return _mm256_sub_pd(c, _mm256_mul_pd(a, b));
#endif
    }

    anemone_forceinline __m256d anemone_vectorcall AvxNegateMultiplySubtract(__m256d a, __m256d b, __m256d c)
    {
#if ANEMONE_FEATURE_FMA4
        return _mm256_nmsub_pd(a, b, c);
#elif ANEMONE_FEATURE_AVX2
        return _mm256_fnmsub_pd(a, b, c);
#else
        // = -(a * b) - c
        return _mm256_sub_pd(_mm256_sub_pd(_mm256_setzero_pd(), _mm256_mul_pd(a, b)), c);
#endif
    }

    anemone_forceinline __m256d anemone_vectorcall AvxLoadDouble4(double const* source)
    {
        return _mm256_load_pd(source);
    }

    anemone_forceinline void anemone_vectorcall AvxStoreDouble4(double* target, __m256d source)
    {
        _mm256_store_pd(target, source);
    }

    anemone_forceinline __m256d anemone_vectorcall AvxLoadDouble3(double const* source)
    {
        // = [x, y]
        __m128d const v_xy = _mm_load_pd(source);
        // = [z, 0]
        __m128d const v_z0 = _mm_load_sd(source + 2);
        // = [x, y, z, 0]
        return _mm256_insertf128_pd(_mm256_castpd128_pd256(v_xy), v_z0, 1);
    }

    anemone_forceinline void anemone_vectorcall AvxStoreDouble3(double* target, __m256d source)
    {
        // = [x, y]
        __m128d const v_xy = _mm256_castpd256_pd128(source);
        _mm_store_pd(target, v_xy);

        // = [z, ...]
        __m128d const v_z = _mm256_extractf128_pd(source, 1);
        _mm_store_sd(target + 2, v_z);
    }

    anemone_forceinline __m256d anemone_vectorcall AvxLoadDouble2(double const* source)
    {
        // = [x, y]
        __m128d const v_xy = _mm_load_pd(source);
        // = [x, y, 0, 0]
        return _mm256_insertf128_pd(_mm256_setzero_pd(), v_xy, 0);
    }

    anemone_forceinline void anemone_vectorcall AvxStoreDouble2(double* target, __m256d source)
    {
        // = [x, y]
        __m128d const v_xy = _mm256_castpd256_pd128(source);
        _mm_store_pd(target, v_xy);
    }

    anemone_forceinline __m128i AvxCompareLessEqualI64(__m128i a, __m128i b)
    {
        // = [a < b]
        __m128i const m0 = _mm_cmpgt_epi64(b, a);
        // = [a == b]
        __m128i const m1 = _mm_cmpeq_epi64(b, a);
        return _mm_xor_si128(m0, m1);
    }

    anemone_forceinline __m256i AvxCompareLessEqualI64(__m256i a, __m256i b)
    {
        // = [a < b]
        __m256i const m0 = _mm256_cmpgt_epi64(b, a);
        // = [a == b]
        __m256i const m1 = _mm256_cmpeq_epi64(b, a);
        return _mm256_xor_si256(m0, m1);
    }

    anemone_forceinline __m128i AvxCompareZeroPreciseI64(__m128i v)
    {
        __m128i const ulpTolerance = _mm_set1_epi64x(4);
        __m128i const ulpValue = _mm_abs_epi64(v);
        return AvxCompareLessEqualI64(ulpValue, ulpTolerance);
    }

    anemone_forceinline __m256i AvxCompareNearZeroPreciseI64(__m256i v)
    {
        __m256i const ulpTolerance = _mm256_set1_epi32(4);
        __m256i const ulpValue = _mm256_abs_epi32(v);
        return AvxCompareLessEqualI64(ulpValue, ulpTolerance);
    }

    inline constexpr int64_t AvxMaskComponentInt64{-1};
}

#if false
    template <>
    inline Mask<double>::Mask(bool x, bool y, bool z, bool w)
        : Internal::Mask<double>{
              _mm256_castsi256_pd(_mm256_setr_epi64x(
                  x ? Internal::AvxMaskComponentInt64 : 0,
                  y ? Internal::AvxMaskComponentInt64 : 0,
                  z ? Internal::AvxMaskComponentInt64 : 0,
                  w ? Internal::AvxMaskComponentInt64 : 0))}
    {
    }

    template <>
    inline Mask<double>::Mask(bool value)
        : Internal::Mask<double>{
              _mm256_castsi256_pd(_mm256_set1_epi64x(value ? Internal::AvxMaskComponentInt64 : 0))}
    {
    }
}

// Matrix type
namespace Anemone::Numerics::Simd
{
    template <>
    inline Matrix<double>::Matrix(
        double m00, double m01, double m02, double m03,
        double m10, double m11, double m12, double m13,
        double m20, double m21, double m22, double m23,
        double m30, double m31, double m32, double m33)
        : Internal::Matrix<double>{
              _mm256_setr_pd(m00, m01, m02, m03),
              _mm256_setr_pd(m10, m11, m12, m13),
              _mm256_setr_pd(m20, m21, m22, m23),
              _mm256_setr_pd(m30, m31, m32, m33)}
    {
    }

    // Vector
    namespace Anemone::Numerics::Simd
    {
        template <>
        inline double anemone_vectorcall Extract<0, double>(Vector<double> value)
        {
            return _mm_cvtsd_f64(_mm256_castpd256_pd128(value.Inner));
        }

        template <>
        inline double anemone_vectorcall Extract<1, double>(Vector<double> value)
        {
            __m128d const v_xy = _mm256_castpd256_pd128(value.Inner);
            __m128d const v_yy = _mm_permute_pd(v_xy, _MM_SHUFFLE2(1, 1));
            return _mm_cvtsd_f64(v_yy);
        }

        template <>
        inline double anemone_vectorcall Extract<2, double>(Vector<double> value)
        {
            __m128d const v_zw = _mm256_extractf128_pd(value.Inner, 1);
            return _mm_cvtsd_f64(v_zw);
        }

        template <>
        inline double anemone_vectorcall Extract<3, double>(Vector<double> value)
        {
            __m128d const v_zw = _mm256_extractf128_pd(value.Inner, 1);
            __m128d const v_ww = _mm_permute_pd(v_zw, _MM_SHUFFLE2(1, 1));
            return _mm_cvtsd_f64(v_ww);
        }

        template <>
        inline Vector<double> anemone_vectorcall Insert<0, double>(Vector<double> value, double element)
        {
            // = [x, y]
            __m128d const v_xy = _mm256_castpd256_pd128(value.Inner);
            // = [e, y]
            __m128d const v_ey = _mm_move_sd(v_xy, _mm_set_sd(element));
            // = [e, y, z, w]
            return {{
                _mm256_insertf128_pd(value.Inner, v_ey, 0),
            }};
        }

        template <>
        inline Vector<double> anemone_vectorcall Insert<1, double>(Vector<double> value, double element)
        {
            // = [x, y]
            __m128d const v_xy = _mm256_castpd256_pd128(value.Inner);
            // = [y, x]
            __m128d const v_yx = _mm_permute_pd(v_xy, _MM_SHUFFLE2(0, 1));
            // = [e, x]
            __m128d const v_ex = _mm_move_sd(v_yx, _mm_set_sd(element));
            // = [x, e]
            __m128d const v_xe = _mm_permute_pd(v_ex, _MM_SHUFFLE2(0, 1));
            // = [x, e, z, w]
            return {{
                _mm256_insertf128_pd(value.Inner, v_xe, 0),
            }};
        }

        template <>
        inline Vector<double> anemone_vectorcall Insert<2, double>(Vector<double> value, double element)
        {
            // = [z, w]
            __m128d const v_zw = _mm256_extractf128_pd(value.Inner, 1);
            // = [e, w]
            __m128d const v_ew = _mm_move_sd(v_zw, _mm_set_sd(element));
            // = [x, y, e, w]
            return {{
                _mm256_insertf128_pd(value.Inner, v_ew, 1),
            }};
        }

        template <>
        inline Vector<double> anemone_vectorcall Insert<3, double>(Vector<double> value, double element)
        {
            // = [z, w]
            __m128d const v_zw = _mm256_extractf128_pd(value.Inner, 1);
            // = [w, z]
            __m128d const v_wz = _mm_permute_pd(v_zw, _MM_SHUFFLE2(0, 1));
            // = [e, z]
            __m128d const v_ez = _mm_move_sd(v_wz, _mm_set_sd(element));
            // = [z, e]
            __m128d const v_ze = _mm_permute_pd(v_ez, _MM_SHUFFLE2(0, 1));
            // = [x, y, z, e]
            return {{
                _mm256_insertf128_pd(value.Inner, v_ze, 1),
            }};
        }
#endif

#elif ANEMONE_FEATURE_NEON

namespace Anemone::Internal
{
    inline constexpr uint64_t NeonMaskComponentUInt64{0xFFFF'FFFF'FFFF'FFFF};

    anemone_forceinline uint64x2_t anemone_vectorcall NeonCompareInBounds(float64x2_t v, float64x2_t lower, float64x2_t upper)
    {
        uint64x2_t const maskLower = vcleq_f64(lower, v);
        uint64x2_t const maskUpper = vcleq_f64(v, upper);
        return vandq_u64(maskLower, maskUpper);
    }
}

#endif

namespace Anemone::Internal
{
    inline SimdVector4D anemone_vectorcall Vector4D_LoadAlignedDouble4(double const* source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4D{source[0], source[1], source[2], source[3]};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdVector4D{_mm256_load_pd(source)};
#elif ANEMONE_FEATURE_NEON
        return SimdVector4D{vld1q_f64_x2(source)};
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_LoadAlignedDouble3(double const* source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4D{source[0], source[1], source[2], 0.0};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdVector4D{_mm256_load_pd(source)};
#elif ANEMONE_FEATURE_NEON
        return SimdVector4D{vld1q_f64_x2(source)};
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_LoadAlignedDouble2(double const* source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4D{source[0], source[1], 0.0, 0.0};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdVector4D{_mm256_load_pd(source)};
#elif ANEMONE_FEATURE_NEON
        return SimdVector4D{vld1q_f64_x2(source)};
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_LoadAlignedDouble1(double const* source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4D{source[0], 0.0, 0.0, 0.0};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdVector4D{_mm256_load_pd(source)};
#elif ANEMONE_FEATURE_NEON
        return SimdVector4D{vld1q_f64_x2(source)};
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_LoadUnalignedDouble4(double const* source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4D{source[0], source[1], source[2], source[3]};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdVector4D{_mm256_loadu_pd(source)};
#elif ANEMONE_FEATURE_NEON
#error Not implemented
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_LoadUnalignedDouble3(double const* source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4D{source[0], source[1], source[2], 0.0};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128d const xy = _mm_loadu_pd(source);
        __m128d const z0 = _mm_load_sd(source + 2);
        return SimdVector4D{_mm256_insertf128_pd(_mm256_castpd128_pd256(xy), z0, 1)};
#elif ANEMONE_FEATURE_NEON
#error Not implemented
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_LoadUnalignedDouble2(double const* source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4D{source[0], source[1], 0.0, 0.0};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128d const xy = _mm_loadu_pd(source);
        // = [x, y, 0, 0]
        return SimdVector4D{_mm256_insertf128_pd(_mm256_setzero_pd(), xy, 0)};
#elif ANEMONE_FEATURE_NEON
#error Not implemented
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_LoadUnalignedDouble1(double const* source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4D{source[0], 0.0, 0.0, 0.0};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        __m128d const x0 = _mm_load_sd(source);
        // = [x, 0, 0, 0]
        return SimdVector4D{_mm256_insertf128_pd(_mm256_setzero_pd(), x0, 0)};
#elif ANEMONE_FEATURE_NEON
#error Not implemented
#endif
    }

    inline void anemone_vectorcall Vector4D_StoreAlignedDouble4(double* destination, SimdVector4D source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        destination[0] = source.Inner[0];
        destination[1] = source.Inner[1];
        destination[2] = source.Inner[2];
        destination[3] = source.Inner[3];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        _mm256_store_pd(destination, source);
#elif ANEMONE_FEATURE_NEON
        vst1q_f64_x2(destination, source.Inner);
#endif
    }

    inline void anemone_vectorcall Vector4D_StoreAlignedDouble3(double* destination, SimdVector4D source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        destination[0] = source.Inner[0];
        destination[1] = source.Inner[1];
        destination[2] = source.Inner[2];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        _mm256_store_pd(destination, source);
#elif ANEMONE_FEATURE_NEON
        vst1q_f64_x2(destination, source.Inner);
#endif
    }

    inline void anemone_vectorcall Vector4D_StoreAlignedDouble2(double* destination, SimdVector4D source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        destination[0] = source.Inner[0];
        destination[1] = source.Inner[1];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        _mm256_store_pd(destination, source);
#elif ANEMONE_FEATURE_NEON
        vst1q_f64_x2(destination, source.Inner);
#endif
    }

    inline void anemone_vectorcall Vector4D_StoreAlignedDouble1(double* destination, SimdVector4D source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        destination[0] = source.Inner[0];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        _mm256_store_pd(destination, source);
#elif ANEMONE_FEATURE_NEON
        vst1q_f64_x2(destination, source.Inner);
#endif
    }

    inline void anemone_vectorcall Vector4D_StoreUnalignedDouble4(double* destination, SimdVector4D source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        destination[0] = source.Inner[0];
        destination[1] = source.Inner[1];
        destination[2] = source.Inner[2];
        destination[3] = source.Inner[3];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        _mm256_storeu_pd(destination, source);
#elif ANEMONE_FEATURE_NEON
#error Not implemented
#endif
    }

    inline void anemone_vectorcall Vector4D_StoreUnalignedDouble3(double* destination, SimdVector4D source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        destination[0] = source.Inner[0];
        destination[1] = source.Inner[1];
        destination[2] = source.Inner[2];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        _mm_storeu_pd(destination, _mm256_castpd256_pd128(source));
        _mm_storeu_pd(destination + 2, _mm256_extractf128_pd(source, 1));
#elif ANEMONE_FEATURE_NEON
#error Not implemented
#endif
    }

    inline void anemone_vectorcall Vector4D_StoreUnalignedDouble2(double* destination, SimdVector4D source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        destination[0] = source.Inner[0];
        destination[1] = source.Inner[1];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        _mm_storeu_pd(destination, _mm256_castpd256_pd128(source));
#elif ANEMONE_FEATURE_NEON
#error Not implemented
#endif
    }

    inline void anemone_vectorcall Vector4D_StoreUnalignedDouble1(double* destination, SimdVector4D source)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        destination[0] = source.Inner[0];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        _mm_store_sd(destination, _mm256_castpd256_pd128(source));
#elif ANEMONE_FEATURE_NEON
#error Not implemented
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_Create(double x, double y, double z, double w)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4D{x, y, z, w};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdVector4D{_mm256_setr_pd(x, y, z, w)};
#elif ANEMONE_FEATURE_NEON
        double const r[4]{x, y, z, w};

        return SimdVector4D{vld1q_f64_x2(r)};
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_Replicate(double f)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4D{f, f, f, f};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdVector4D{_mm256_set1_pd(f)};
#elif ANEMONE_FEATURE_NEON
        float64x2_t const r = vdupq_n_f64(f);
        return SimdVector4D{
            float64x2x2_t{
                r,
                r,
            },
        };
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_Zero()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4D>(F64x4_PositiveZero_XXXX);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdVector4D{_mm256_setzero_pd()};
#elif ANEMONE_FEATURE_NEON
        return SimdVector4D{vld1q_f64_x2(F64x4_PositiveZero_XXXX.As<double>())};
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_NaN()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4D>(F64x4_PositiveQNaN_XXXX);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdVector4D{_mm256_load_pd(F64x4_PositiveQNaN_XXXX.As<double>())};
#elif ANEMONE_FEATURE_NEON
        return SimdVector4D{vld1q_f64_x2(F64x4_PositiveQNaN_XXXX.As<double>())};
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_PositiveInfinity()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4D>(F64x4_PositiveInfinity_XXXX);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdVector4D{_mm256_load_pd(F64x4_PositiveInfinity_XXXX.As<double>())};
#elif ANEMONE_FEATURE_NEON
        return SimdVector4D{vld1q_f64_x2(F64x4_PositiveInfinity_XXXX.As<double>())};
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_NegativeInfinity()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4D>(F64x4_NegativeInfinity_XXXX);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdVector4D{_mm256_load_pd(F64x4_NegativeInfinity_XXXX.As<double>())};
#elif ANEMONE_FEATURE_NEON
        return SimdVector4D{vld1q_f64_x2(F64x4_NegativeInfinity_XXXX.As<double>())};
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_Epsilon()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4D>(F64x4_Epsilon_XXXX);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdVector4D{_mm256_load_pd(F64x4_Epsilon_XXXX.As<double>())};
#elif ANEMONE_FEATURE_NEON
        return SimdVector4D{vld1q_f64_x2(F64x4_Epsilon_XXXX.As<double>())};
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_PositiveUnitX()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4D>(F64x4_PositiveUnitX);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdVector4D{_mm256_load_pd(F64x4_PositiveUnitX.As<double>())};
#elif ANEMONE_FEATURE_NEON
        return SimdVector4D{vld1q_f64_x2(F64x4_PositiveUnitX.As<double>())};
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_PositiveUnitY()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4D>(F64x4_PositiveUnitY);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdVector4D{_mm256_load_pd(F64x4_PositiveUnitY.As<double>())};
#elif ANEMONE_FEATURE_NEON
        return SimdVector4D{vld1q_f64_x2(F64x4_PositiveUnitY.As<double>())};
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_PositiveUnitZ()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4D>(F64x4_PositiveUnitZ);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdVector4D{_mm256_load_pd(F64x4_PositiveUnitZ.As<double>())};
#elif ANEMONE_FEATURE_NEON
        return SimdVector4D{vld1q_f64_x2(F64x4_PositiveUnitZ.As<double>())};
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_PositiveUnitW()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4D>(F64x4_PositiveUnitW);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdVector4D{_mm256_load_pd(F64x4_PositiveUnitW.As<double>())};
#elif ANEMONE_FEATURE_NEON
        return SimdVector4D{vld1q_f64_x2(F64x4_PositiveUnitW.As<double>())};
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_NegativeUnitX()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4D>(F64x4_NegativeUnitX);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdVector4D{_mm256_load_pd(F64x4_NegativeUnitX.As<double>())};
#elif ANEMONE_FEATURE_NEON
        return SimdVector4D{vld1q_f64_x2(F64x4_NegativeUnitX.As<double>())};
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_NegativeUnitY()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4D>(F64x4_NegativeUnitY);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdVector4D{_mm256_load_pd(F64x4_NegativeUnitY.As<double>())};
#elif ANEMONE_FEATURE_NEON
        return SimdVector4D{vld1q_f64_x2(F64x4_NegativeUnitY.As<double>())};
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_NegativeUnitZ()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4D>(F64x4_NegativeUnitZ);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdVector4D{_mm256_load_pd(F64x4_NegativeUnitZ.As<double>())};
#elif ANEMONE_FEATURE_NEON
        return SimdVector4D{vld1q_f64_x2(F64x4_NegativeUnitZ.As<double>())};
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_NegativeUnitW()
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return std::bit_cast<SimdVector4D>(F64x4_NegativeUnitW);
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return SimdVector4D{_mm256_load_pd(F64x4_NegativeUnitW.As<double>())};
#elif ANEMONE_FEATURE_NEON
        return SimdVector4D{vld1q_f64_x2(F64x4_NegativeUnitW.As<double>())};
#endif
    }

    template <size_t N>
    double anemone_vectorcall Vector4D_Extract(SimdVector4D v)
        requires(N < 4)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return v.Inner[N];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return std::bit_cast<double>(_mm256_extract_epi64(_mm256_castpd_si256(v), N));
#elif ANEMONE_FEATURE_NEON
        static constexpr size_t lane = N & 1u;
        static constexpr size_t index = (N >> 1u) & 1u;
        return vgetq_lane_f64(v.Inner.val[index], lane);
#endif
    }

    template <size_t N>
    void anemone_vectorcall Vector4D_Extract(SimdVector4D v, double* f)
        requires(N < 4)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        *f = v.Inner[N];
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        *f = std::bit_cast<double>(_mm256_extract_epi64(_mm256_castpd_si256(v), N));
#elif ANEMONE_FEATURE_NEON
        static constexpr size_t lane = N & 1u;
        static constexpr size_t index = (N >> 1u) & 1u;
        *f = vgetq_lane_f64(v.Inner.val[index], lane);
#endif
    }

    template <size_t N>
    SimdVector4D anemone_vectorcall Vector4D_Insert(SimdVector4D v, double f)
        requires(N < 4)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        v.Inner[N] = f;
        return v;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm256_castsi256_pd(_mm256_insert_epi64(
            _mm256_castpd_si256(v), std::bit_cast<int64_t>(f), N));
#elif ANEMONE_FEATURE_NEON
        if constexpr (N < 2)
        {
            return SimdVector4D{
                vsetq_lane_f64(f, v.Inner.val[0], N),
                v.Inner.val[1],
            };
        }
        else
        {
            return SimdVector4D{
                v.Inner.val[0],
                vsetq_lane_f64(f, v.Inner.val[1], N - 2),
            };
        }
#endif
    }

    template <size_t N>
    SimdVector4D anemone_vectorcall Vector4D_Insert(SimdVector4D v, double const* f)
        requires(N < 4)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        v.Inner[N] = *f;
        return v;
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm256_castsi256_pd(_mm256_insert_epi64(
            _mm256_castpd_si256(v), std::bit_cast<int64_t>(*f), N));
#elif ANEMONE_FEATURE_NEON
        if constexpr (N < 2)
        {
            return SimdVector4D{
                vsetq_lane_f64(*f, v.Inner.val[0], N),
                v.Inner.val[1],
            };
        }
        else
        {
            return SimdVector4D{
                v.Inner.val[0],
                vsetq_lane_f64(*f, v.Inner.val[1], N - 2),
            };
        }
#endif
    }

    template <size_t N>
    SimdVector4D anemone_vectorcall Vector4D_Broadcast(SimdVector4D v)
        requires(N < 4)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4D{v.Inner[N], v.Inner[N], v.Inner[N], v.Inner[N]};
#elif ANEMONE_FEATURE_AVX2
        if constexpr (N == 0)
        {
            return _mm256_broadcastsd_pd(_mm256_castpd256_pd128(v));
        }
        else
        {
            return _mm256_permute4x64_pd(v, _MM_SHUFFLE(N, N, N, N));
        }
#elif ANEMONE_FEATURE_AVX
        static constexpr int extractMask = (N < 2) ? 0 : 1;
        static constexpr int permuteMask = ((N & 1) != 0) ? _MM_SHUFFLE2(1, 1) : _MM_SHUFFLE2(0, 0);
        __m128d const r0 = _mm256_extractf128_pd(v, extractMask);
        __m128d const r1 = _mm_permute_pd(r0, permuteMask);
        return _mm256_set_m128d(r1, r1);
#elif ANEMONE_FEATURE_NEON
        static constexpr int lane = N & 1;
        static constexpr int index = (N >> 1) & 1;
        float64x2_t const r = vdupq_laneq_f64(v.Inner.val[index], lane);
        return SimdVector4D{
            float64x2x2_t{
                r,
                r,
            },
        };
#endif
    }

    template <size_t X, size_t Y, size_t Z, size_t W>
    SimdVector4D anemone_vectorcall Vector4D_Permute(SimdVector4D v)
        requires((X < 4) and (Y < 4) and (Z < 4) and (W < 4))
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4D{v.Inner[X], v.Inner[Y], v.Inner[Z], v.Inner[W]};
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        if constexpr (X == 0 and Y == 1 and Z == 2 and W == 3)
        {
            return v;
        }
        else
        {
#if ANEMONE_FEATURE_AVX2
            return _mm256_permute4x64_pd(v, _MM_SHUFFLE(W, Z, Y, X));
#elif ANEMONE_FEATURE_AVX
            // = [X, Y]
            __m128d const v_xy = _mm256_castpd256_pd128(v);
            // = [Z, W]
            __m128d const v_zw = _mm256_extractf128_pd(v, 1);

            double elements[4]{
                _mm_cvtsd_f64(v_xy),
                _mm_cvtsd_f64(_mm_unpackhi_pd(v_xy, v_xy)),
                _mm_cvtsd_f64(v_zw),
                _mm_cvtsd_f64(_mm_unpackhi_pd(v_zw, v_zw)),
            };

            // = [e.X, e.Y]
            __m128d const r_xy = _mm_setr_pd(elements[X], elements[Y]);
            // = [e.Z, e.W]
            __m128d const r_zw = _mm_setr_pd(elements[Z], elements[W]);

            // = [e.X, e.Y, e.Z, e.W]
            return _mm256_insertf128_pd(_mm256_castpd128_pd256(r_xy), r_zw, 1);
#else
            // = [Z, W, X, Y]
            __m256d const r0 = _mm256_permute2f128_pd(v, v, 0b0000'0001);
            constexpr int permuteMask = (X & 1) | ((Y & 1) << 1u) | ((Z & 1) << 2u) | ((W & 1) << 3u);
            __m256d const xyzw = _mm256_permute_pd(v, permuteMask);
            __m256d const zwxy = _mm256_permute_pd(r0, permuteMask);
            constexpr int blendMask = (static_cast<int>(X >= 2)) | (static_cast<int>(Y >= 2) << 1) | (static_cast<int>(Z < 2) << 2) | (static_cast<int>(W < 2) << 3);
            return _mm256_blend_pd(xyzw, zwxy, blendMask);
#endif
        }
#elif ANEMONE_FEATURE_NEON
#error Not implemented
#endif
    }

    template <size_t X, size_t Y, size_t Z, size_t W>
    SimdVector4D anemone_vectorcall Vector4D_Permute(SimdVector4D a, SimdVector4D b)
        requires((X < 8) and (Y < 8) and (Z < 8) and (W < 8))
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4D{
            ((X >= 4) ? b.Inner : a.Inner)[X & 3],
            ((Y >= 4) ? b.Inner : a.Inner)[Y & 3],
            ((Z >= 4) ? b.Inner : a.Inner)[Z & 3],
            ((W >= 4) ? b.Inner : a.Inner)[W & 3],
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2

        if constexpr (X == 0 and Y == 1 and Z == 2 and W == 3)
        {
            return a;
        }
        else if constexpr (X == 4 and Y == 5 and Z == 6 and W == 7)
        {
            return b;
        }
        else
        {
            constexpr int blendMask =
                ((X >= 4) ? 0b0001 : 0b0000) |
                ((Y >= 4) ? 0b0010 : 0b0000) |
                ((Z >= 4) ? 0b0100 : 0b0000) |
                ((W >= 4) ? 0b1000 : 0b0000);

            constexpr size_t maskX = X & 3;
            constexpr size_t maskY = Y & 3;
            constexpr size_t maskZ = Z & 3;
            constexpr size_t maskW = W & 3;

            __m256d const va = Vector4D_Permute<maskX, maskY, maskZ, maskW>(a);
            __m256d const vb = Vector4D_Permute<maskX, maskY, maskZ, maskW>(b);

            return _mm256_blend_pd(va, vb, blendMask);
        }

#elif ANEMONE_FEATURE_NEON
#error Not implemented
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_Select(SimdMask4D mask, SimdVector4D whenFalse, SimdVector4D whenTrue)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4D{
            ScalarHelper_SelectBranchless(mask.Inner[0], whenFalse.Inner[0], whenTrue.Inner[0]),
            ScalarHelper_SelectBranchless(mask.Inner[1], whenFalse.Inner[1], whenTrue.Inner[1]),
            ScalarHelper_SelectBranchless(mask.Inner[2], whenFalse.Inner[2], whenTrue.Inner[2]),
            ScalarHelper_SelectBranchless(mask.Inner[3], whenFalse.Inner[3], whenTrue.Inner[3]),
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        // Note: this approach has better performance on older CPUs.
        // return _mm256_blendv_pd(whenFalse, whenTrue, mask);
        return _mm256_or_pd(_mm256_andnot_pd(mask, whenFalse), _mm256_and_pd(mask, whenTrue));
#elif ANEMONE_FEATURE_NEON
        return SimdVector4D{
            float64x2x2_t{
                vbslq_f64(mask.Inner.val[0], whenTrue.Inner.val[0], whenFalse.Inner.val[0]),
                vbslq_f64(mask.Inner.val[1], whenTrue.Inner.val[1], whenFalse.Inner.val[1]),
            },
        };
#endif
    }

    template <bool X, bool Y, bool Z, bool W>
    SimdVector4D anemone_vectorcall Vector4D_Select(SimdVector4D whenFalse, SimdVector4D whenTrue)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4D{
            X ? whenTrue.Inner[0] : whenFalse.Inner[0],
            Y ? whenTrue.Inner[1] : whenFalse.Inner[1],
            Z ? whenTrue.Inner[2] : whenFalse.Inner[2],
            W ? whenTrue.Inner[3] : whenFalse.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        constexpr int blendMask =
            (X ? 0b0001 : 0b0000) |
            (Y ? 0b0010 : 0b0000) |
            (Z ? 0b0100 : 0b0000) |
            (W ? 0b1000 : 0b0000);
        return _mm256_blend_pd(whenFalse, whenTrue, blendMask);
#elif ANEMONE_FEATURE_NEON
        static constexpr uint64_t maskXY[2]{
            X ? NeonMaskComponentUInt64 : 0,
            Y ? NeonMaskComponentUInt64 : 0,
        };

        static constexpr uint64_t maskZW[2]{
            Z ? NeonMaskComponentUInt64 : 0,
            W ? NeonMaskComponentUInt64 : 0,
        };

        return SimdVector4D{
            float64x2x2_t{
                vbslq_f64(vld1q_u64(maskXY), whenTrue.Inner.val[0], whenFalse.Inner.val[0]),
                vbslq_f64(vld1q_u64(maskZW), whenTrue.Inner.val[1], whenFalse.Inner.val[1]),
            },
        };
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_MultiplyAdd(SimdVector4D a, SimdVector4D b, SimdVector4D c)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4D{
            (a.Inner[0] * b.Inner[0]) + c.Inner[0],
            (a.Inner[1] * b.Inner[1]) + c.Inner[1],
            (a.Inner[2] * b.Inner[2]) + c.Inner[2],
            (a.Inner[3] * b.Inner[3]) + c.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return AvxMultiplyAdd(a, b, c);
#elif ANEMONE_FEATURE_NEON
        return SimdVector4D{
            float64x2x2_t{
                vmlaq_f64(c.Inner.val[0], a.Inner.val[0], b.Inner.val[0]),
                vmlaq_f64(c.Inner.val[1], a.Inner.val[1], b.Inner.val[1]),
            },
        };
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_MultiplySubtract(SimdVector4D a, SimdVector4D b, SimdVector4D c)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4D{
            (a.Inner[0] * b.Inner[0]) - c.Inner[0],
            (a.Inner[1] * b.Inner[1]) - c.Inner[1],
            (a.Inner[2] * b.Inner[2]) - c.Inner[2],
            (a.Inner[3] * b.Inner[3]) - c.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return AvxMultiplySubtract(a, b, c);
#elif ANEMONE_FEATURE_NEON
        return SimdVector4D{
            float64x2x2_t{
                vmlaq_f64(vnegq_f64(c.Inner.val[0]), a.Inner.val[0], b.Inner.val[0]),
                vmlaq_f64(vnegq_f64(c.Inner.val[1]), a.Inner.val[1], b.Inner.val[1]),
            },
        };
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_NegateMultiplyAdd(SimdVector4D a, SimdVector4D b, SimdVector4D c)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4D{
            -(a.Inner[0] * b.Inner[0]) + c.Inner[0],
            -(a.Inner[1] * b.Inner[1]) + c.Inner[1],
            -(a.Inner[2] * b.Inner[2]) + c.Inner[2],
            -(a.Inner[3] * b.Inner[3]) + c.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return AvxNegateMultiplyAdd(a, b, c);
#elif ANEMONE_FEATURE_NEON
        return SimdVector4D{
            float64x2x2_t{
                vmlsq_f64(c.Inner.val[0], a.Inner.val[0], b.Inner.val[0]),
                vmlsq_f64(c.Inner.val[1], a.Inner.val[1], b.Inner.val[1]),
            },
        };
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_NegateMultiplySubtract(SimdVector4D a, SimdVector4D b, SimdVector4D c)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4D{
            -(a.Inner[0] * b.Inner[0]) - c.Inner[0],
            -(a.Inner[1] * b.Inner[1]) - c.Inner[1],
            -(a.Inner[2] * b.Inner[2]) - c.Inner[2],
            -(a.Inner[3] * b.Inner[3]) - c.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return AvxNegateMultiplySubtract(a, b, c);
#elif ANEMONE_FEATURE_NEON
        return SimdVector4D{
            float64x2x2_t{
                vmlsq_f64(vnegq_f64(c.Inner.val[0]), a.Inner.val[0], b.Inner.val[0]),
                vmlsq_f64(vnegq_f64(c.Inner.val[1]), a.Inner.val[1], b.Inner.val[1]),
            },
        };
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_Add(SimdVector4D a, SimdVector4D b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4D{
            a.Inner[0] + b.Inner[0],
            a.Inner[1] + b.Inner[1],
            a.Inner[2] + b.Inner[2],
            a.Inner[3] + b.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm256_add_pd(a, b);
#elif ANEMONE_FEATURE_NEON
        return SimdVector4D{
            float64x2x2_t{
                vaddq_f64(a.Inner.val[0], b.Inner.val[0]),
                vaddq_f64(a.Inner.val[1], b.Inner.val[1]),
            },
        };
#endif
    }

    inline SimdVector4D anemone_vectorcall Vector4D_Subtract(SimdVector4D a, SimdVector4D b)
    {
#if ANEMONE_BUILD_DISABLE_SIMD
        return SimdVector4D{
            a.Inner[0] - b.Inner[0],
            a.Inner[1] - b.Inner[1],
            a.Inner[2] - b.Inner[2],
            a.Inner[3] - b.Inner[3],
        };
#elif ANEMONE_FEATURE_AVX || ANEMONE_FEATURE_AVX2
        return _mm256_sub_pd(a, b);
#elif ANEMONE_FEATURE_NEON
        return SimdVector4D{
            float64x2x2_t{
                vsubq_f64(a.Inner.val[0], b.Inner.val[0]),
                vsubq_f64(a.Inner.val[1], b.Inner.val[1]),
            },
        };
#endif
    }

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
}
