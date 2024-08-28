#pragma once
#include "AnemoneRuntime/Numerics/Simd.hxx"

namespace Anemone::Numerics::Private
{
    anemone_forceinline __m256d anemone_vectorcall AvxMultiplyAdd(__m256d a, __m256d b, __m256d c)
    {
#if ANEMONE_FEATURE_AVX2
        return _mm256_fmadd_pd(a, b, c);
#else
        // = (a * b) + c
        return _mm256_add_pd(_mm256_mul_pd(a, b), c);
#endif
    }

    anemone_forceinline __m256d anemone_vectorcall AvxMultiplySubtract(__m256d a, __m256d b, __m256d c)
    {
#if ANEMONE_FEATURE_AVX2
        return _mm256_fmsub_pd(a, b, c);
#else
        // = (a * b) - c
        return _mm256_sub_pd(_mm256_mul_pd(a, b), c);
#endif
    }

    anemone_forceinline __m256d anemone_vectorcall AvxNegateMultiplyAdd(__m256d a, __m256d b, __m256d c)
    {
#if ANEMONE_FEATURE_AVX2
        return _mm256_fnmadd_pd(a, b, c);
#else
        // = -(a * b) + c
        return _mm256_sub_pd(c, _mm256_mul_pd(a, b));
#endif
    }

    anemone_forceinline __m256d anemone_vectorcall AvxNegateMultiplySubtract(__m256d a, __m256d b, __m256d c)
    {
#if ANEMONE_FEATURE_AVX2
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
// Mask type
namespace Anemone::Numerics::Simd
{
    template <>
    inline Mask<double>::Mask(bool x, bool y, bool z, bool w)
        : Private::Mask<double>{
              _mm256_castsi256_pd(_mm256_setr_epi64x(
                  x ? Private::AvxMaskComponentInt64 : 0,
                  y ? Private::AvxMaskComponentInt64 : 0,
                  z ? Private::AvxMaskComponentInt64 : 0,
                  w ? Private::AvxMaskComponentInt64 : 0))}
    {
    }

    template <>
    inline Mask<double>::Mask(bool value)
        : Private::Mask<double>{
              _mm256_castsi256_pd(_mm256_set1_epi64x(value ? Private::AvxMaskComponentInt64 : 0))}
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
        : Private::Matrix<double>{
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
}

#endif
