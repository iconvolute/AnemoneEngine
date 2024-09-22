// ReSharper disable CppClangTidyClangDiagnosticFloatEqual
// ReSharper disable CppPassValueParameterByConstReference
#pragma once
#include "AnemoneRuntime/Numerics/Simd.hxx"

// Implementation details
namespace Anemone::Numerics::Private
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

// Vector functions
namespace Anemone::Numerics::Private
{
    inline SimdVector4F anemone_vectorcall Vector4F_LoadAlignedFloat4(float const* source)
    {
        return _mm_load_ps(source);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LoadAlignedFloat3(float const* source)
    {
        return _mm_load_ps(source);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LoadAlignedFloat2(float const* source)
    {
        return _mm_load_ps(source);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LoadAlignedFloat1(float const* source)
    {
        return _mm_load_ps(source);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LoadUnalignedFloat4(float const* source)
    {
        return _mm_loadu_ps(source);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LoadUnalignedFloat3(float const* source)
    {
        // = [x, y, 0, 0]
        __m128 const v_xy00 = _mm_castsi128_ps(_mm_loadl_epi64(reinterpret_cast<__m128i const*>(source)));
        // = [z, 0, 0, 0]
        __m128 const v_z000 = _mm_load_ss(source + 2);
        return _mm_movelh_ps(v_xy00, v_z000);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LoadUnalignedFloat2(float const* source)
    {
        // = [x, y, 0, 0]
        return _mm_castsi128_ps(_mm_loadl_epi64(reinterpret_cast<__m128i const*>(source)));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LoadUnalignedFloat1(float const* source)
    {
        return _mm_load_ss(source);
    }

    inline void anemone_vectorcall Vector4F_StoreAlignedFloat4(float* destination, SimdVector4F source)
    {
        _mm_store_ps(destination, source);
    }

    inline void anemone_vectorcall Vector4F_StoreAlignedFloat3(float* destination, SimdVector4F source)
    {
        _mm_store_ps(destination, source);
    }

    inline void anemone_vectorcall Vector4F_StoreAlignedFloat2(float* destination, SimdVector4F source)
    {
        _mm_store_ps(destination, source);
    }

    inline void anemone_vectorcall Vector4F_StoreAlignedFloat1(float* destination, SimdVector4F source)
    {
        _mm_store_ps(destination, source);
    }

    inline void anemone_vectorcall Vector4F_StoreUnalignedFloat4(float* destination, SimdVector4F source)
    {
        _mm_storeu_ps(destination, source);
    }

    inline void anemone_vectorcall Vector4F_StoreUnalignedFloat3(float* destination, SimdVector4F source)
    {
        // = [x, y, ...]
        _mm_storel_epi64(reinterpret_cast<__m128i*>(destination), _mm_castps_si128(source));
        // = [z, ...]
        _mm_store_ss(destination + 2, _mm_movehl_ps(source, source));
    }

    inline void anemone_vectorcall Vector4F_StoreUnalignedFloat2(float* destination, SimdVector4F source)
    {
        // target[0] = source.0
        // target[1] = source.1
        _mm_storel_epi64(reinterpret_cast<__m128i*>(destination), _mm_castps_si128(source));
    }
    inline void anemone_vectorcall Vector4F_StoreUnalignedFloat1(float* destination, SimdVector4F source)
    {
        _mm_store_ss(destination, source);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Create(float x, float y, float z, float w)
    {
        return _mm_setr_ps(x, y, z, w);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Replicate(float f)
    {
        return _mm_set1_ps(f);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Zero()
    {
        return _mm_setzero_ps();
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PositiveUnitX()
    {
        return _mm_load_ps(F32x4_PositiveUnitX.As<float>());
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PositiveUnitY()
    {
        return _mm_load_ps(F32x4_PositiveUnitY.As<float>());
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PositiveUnitZ()
    {
        return _mm_load_ps(F32x4_PositiveUnitZ.As<float>());
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PositiveUnitW()
    {
        return _mm_load_ps(F32x4_PositiveUnitW.As<float>());
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegativeUnitX()
    {
        return _mm_load_ps(F32x4_NegativeUnitX.As<float>());
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegativeUnitY()
    {
        return _mm_load_ps(F32x4_NegativeUnitY.As<float>());
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegativeUnitZ()
    {
        return _mm_load_ps(F32x4_NegativeUnitZ.As<float>());
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegativeUnitW()
    {
        return _mm_load_ps(F32x4_NegativeUnitW.As<float>());
    }

    template <size_t N>
    inline float anemone_vectorcall Vector4F_Extract(SimdVector4F v)
        requires(N < 4)
    {
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
    }

    template <size_t N>
    inline SimdVector4F anemone_vectorcall Vector4F_Insert(SimdVector4F v, float f)
        requires(N < 4)
    {
        if constexpr (N == 0)
        {
            return _mm_move_ss(v, _mm_set_ss(f));
        }
        else
        {
            return _mm_insert_ps(v, _mm_set_ss(f), N << 4);
        }
    }

    template <size_t N>
    inline SimdVector4F anemone_vectorcall Vector4F_Broadcast(SimdVector4F v)
        requires(N < 4)
    {
        return _mm_shuffle_ps(v, v, _MM_SHUFFLE(N, N, N, N));
    }

    template <size_t X, size_t Y, size_t Z, size_t W>
    inline SimdVector4F anemone_vectorcall Vector4F_Permute(SimdVector4F v)
        requires((X < 4) and (Y < 4) and (Z < 4) and (W < 4))
    {
        return _mm_shuffle_ps(v, v, _MM_SHUFFLE(W, Z, Y, X));
    }

    template <size_t X, size_t Y, size_t Z, size_t W>
    inline SimdVector4F anemone_vectorcall Vector4F_Permute(SimdVector4F a, SimdVector4F b)
        requires((X < 8) and (Y < 8) and (Z < 8) and (W < 8))
    {
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
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Select(SimdMask4F mask, SimdVector4F whenFalse, SimdVector4F whenTrue)
    {
        return _mm_blendv_ps(whenFalse, whenTrue, mask);
    }

    template <bool X, bool Y, bool Z, bool W>
    inline SimdVector4F anemone_vectorcall Vector4F_Select(SimdVector4F whenFalse, SimdVector4F whenTrue)
    {
        return _mm_blend_ps(whenFalse, whenTrue, (X ? 0b0001 : 0) | (Y ? 0b0010 : 0) | (Z ? 0b0100 : 0) | (W ? 0b1000 : 0));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_MultiplyAdd(SimdVector4F a, SimdVector4F b, SimdVector4F c)
    {
#if ANEMONE_FEATURE_AVX2
        return _mm_fmadd_ps(a, b, c);
#else
        // = (a * b) + c
        return _mm_add_ps(_mm_mul_ps(a, b), c);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_MultiplySubtract(SimdVector4F a, SimdVector4F b, SimdVector4F c)
    {
#if ANEMONE_FEATURE_AVX2
        return _mm_fmsub_ps(a, b, c);
#else
        // (a * b) - c
        return _mm_sub_ps(_mm_mul_ps(a, b), c);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegateMultiplyAdd(SimdVector4F a, SimdVector4F b, SimdVector4F c)
    {
#if ANEMONE_FEATURE_AVX2
        return _mm_fnmadd_ps(a, b, c);
#else
        // -(a * b) + c
        return _mm_sub_ps(c, _mm_mul_ps(a, b));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegateMultiplySubtract(SimdVector4F a, SimdVector4F b, SimdVector4F c)
    {
#if ANEMONE_FEATURE_AVX2
        return _mm_fnmsub_ps(a, b, c);
#else
        // -(a * b) - c
        return _mm_sub_ps(Vector4F_Negate(_mm_mul_ps(a, b)), c);
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Add(SimdVector4F a, SimdVector4F b)
    {
        return _mm_add_ps(a, b);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Subtract(SimdVector4F a, SimdVector4F b)
    {
        return _mm_sub_ps(a, b);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Multiply(SimdVector4F a, SimdVector4F b)
    {
        return _mm_mul_ps(a, b);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Divide(SimdVector4F a, SimdVector4F b)
    {
        return _mm_div_ps(a, b);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Negate(SimdVector4F v)
    {
        __m128 const mask = _mm_load_ps(F32x4_SignMask_XXXX.As<float>());
        return _mm_xor_ps(v, mask);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Reciprocal(SimdVector4F v)
    {
        return _mm_div_ps(_mm_load_ps(F32x4_PositiveOne.As<float>()), v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_ReciprocalEst(SimdVector4F v)
    {
        return _mm_rcp_ps(v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Min(SimdVector4F a, SimdVector4F b)
    {
        return _mm_min_ps(a, b);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Max(SimdVector4F a, SimdVector4F b)
    {
        return _mm_max_ps(a, b);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Abs(SimdVector4F v)
    {
        __m128 const mask = _mm_load_ps(F32x4_AbsMask_XXXX.As<float>());
        return _mm_and_ps(v, mask);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Square(SimdVector4F v)
    {
        return _mm_mul_ps(v, v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_SignedSquare(SimdVector4F v)
    {
        return _mm_mul_ps(v, Vector4F_Abs(v));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Clamp(SimdVector4F v, SimdVector4F lower, SimdVector4F upper)
    {
        return _mm_min_ps(upper, _mm_max_ps(lower, v));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Saturate(SimdVector4F v)
    {
        __m128 const zero = _mm_setzero_ps();
        __m128 const one = _mm_load_ps(F32x4_PositiveOne.As<float>());
        return _mm_min_ps(one, _mm_max_ps(zero, v));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_SquareRoot(SimdVector4F v)
    {
        return _mm_sqrt_ps(v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_SquareRootEst(SimdVector4F v)
    {
        return _mm_rcp_ps(_mm_rsqrt_ps(v));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_ReciprocalSquareRoot(SimdVector4F v)
    {
        return _mm_div_ps(_mm_load_ps(F32x4_PositiveOne.As<float>()), _mm_sqrt_ps(v));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_ReciprocalSquareRootEst(SimdVector4F v)
    {
        return _mm_rsqrt_ps(v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Ceil(SimdVector4F v)
    {
        return _mm_round_ps(v, _MM_FROUND_TO_POS_INF | _MM_FROUND_NO_EXC);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Floor(SimdVector4F v)
    {
        return _mm_round_ps(v, _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Truncate(SimdVector4F v)
    {
        return _mm_round_ps(v, _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Round(SimdVector4F v)
    {
        return _mm_round_ps(v, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Fraction(SimdVector4F v)
    {
        return _mm_sub_ps(v, _mm_round_ps(v, _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Remainder(SimdVector4F x, SimdVector4F y)
    {
#if ANEMONE_FEATURE_SVML
        return _mm_fmod_ps(x, y);
#else
        // = [x / y]
        __m128 const r0 = _mm_div_ps(x, y);
        // = [trunc(x / y)]
        __m128 const r1 = _mm_round_ps(r0, _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC);
        // = [x - (trunc(x / y) * y)]
        return _mm_sub_ps(x, _mm_mul_ps(r1, y));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Repeat(SimdVector4F v, SimdVector4F length)
    {
        __m128 const scaled = Vector4F_Fraction(_mm_div_ps(v, length));
        return _mm_mul_ps(scaled, length);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Wrap(SimdVector4F v, SimdVector4F lower, SimdVector4F upper)
    {
        __m128 const range = _mm_sub_ps(upper, lower);
        __m128 const wrapped = _mm_sub_ps(v, lower);
        __m128 const scaled = Vector4F_Fraction(_mm_div_ps(wrapped, range));
        return Vector4F_MultiplyAdd(scaled, range, lower);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Power(SimdVector4F x, SimdVector4F y)
    {
#if ANEMONE_FEATURE_SVML
        return _mm_pow_ps(x, y);
#else
        alignas(16) float xs[4];
        alignas(16) float ys[4];

        _mm_store_ps(xs, x);
        _mm_store_ps(ys, y);

        return _mm_setr_ps(
            Power<float>(xs[0], ys[0]),
            Power<float>(xs[1], ys[1]),
            Power<float>(xs[2], ys[2]),
            Power<float>(xs[3], ys[3]));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Exp(SimdVector4F v)
    {
#if ANEMONE_FEATURE_SVML
        return _mm_exp_ps(v);
#else
        alignas(16) float xs[4];
        _mm_store_ps(xs, v);

        return _mm_setr_ps(
            Exp<float>(xs[0]),
            Exp<float>(xs[1]),
            Exp<float>(xs[2]),
            Exp<float>(xs[3]));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Exp2(SimdVector4F v)
    {
#if ANEMONE_FEATURE_SVML
        return _mm_exp2_ps(v);
#else
        alignas(16) float xs[4];
        _mm_store_ps(xs, v);

        return _mm_setr_ps(
            Exp2<float>(xs[0]),
            Exp2<float>(xs[1]),
            Exp2<float>(xs[2]),
            Exp2<float>(xs[3]));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Exp10(SimdVector4F v)
    {
#if ANEMONE_FEATURE_SVML
        return _mm_exp10_ps(v);
#else
        alignas(16) float xs[4];
        _mm_store_ps(xs, v);

        return _mm_setr_ps(
            Exp10<float>(xs[0]),
            Exp10<float>(xs[1]),
            Exp10<float>(xs[2]),
            Exp10<float>(xs[3]));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Log(SimdVector4F v)
    {
#if ANEMONE_FEATURE_SVML
        return _mm_log_ps(v);
#else
        alignas(16) float xs[4];
        _mm_store_ps(xs, v);

        return _mm_setr_ps(
            Log<float>(xs[0]),
            Log<float>(xs[1]),
            Log<float>(xs[2]),
            Log<float>(xs[3]));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Log(SimdVector4F x, SimdVector4F base)
    {
#if ANEMONE_FEATURE_SVML
        return _mm_div_ps(_mm_log_ps(x), _mm_log_ps(base));
#else
        alignas(16) float xs[4];
        alignas(16) float ys[4];
        _mm_store_ps(xs, x);
        _mm_store_ps(ys, base);

        return _mm_setr_ps(
            Log<float>(xs[0], ys[0]),
            Log<float>(xs[1], ys[1]),
            Log<float>(xs[2], ys[2]),
            Log<float>(xs[3], ys[3]));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Log2(SimdVector4F v)
    {
#if ANEMONE_FEATURE_SVML
        return _mm_log2_ps(v);
#else
        alignas(16) float xs[4];
        _mm_store_ps(xs, v);

        return _mm_setr_ps(
            Log2<float>(xs[0]),
            Log2<float>(xs[1]),
            Log2<float>(xs[2]),
            Log2<float>(xs[3]));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Log10(SimdVector4F v)
    {
#if ANEMONE_FEATURE_SVML
        return _mm_log10_ps(v);
#else
        alignas(16) float xs[4];
        _mm_store_ps(xs, v);

        return _mm_setr_ps(
            Log10<float>(xs[0]),
            Log10<float>(xs[1]),
            Log10<float>(xs[2]),
            Log10<float>(xs[3]));
#endif
    }

    inline void anemone_vectorcall Vector4F_SinCos(SimdVector4F& sin, SimdVector4F& cos, SimdVector4F v)
    {
#if ANEMONE_FEATURE_SVML
        sin = _mm_sincos_ps(&cos, v);
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
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Sin(SimdVector4F v)
    {
#if ANEMONE_FEATURE_SVML
        return _mm_sin_ps(v);
#else
        alignas(16) float xs[4];
        _mm_store_ps(xs, v);
        return _mm_setr_ps(
            Sin<float>(xs[0]),
            Sin<float>(xs[1]),
            Sin<float>(xs[2]),
            Sin<float>(xs[3]));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Cos(SimdVector4F v)
    {
#if ANEMONE_FEATURE_SVML
        return _mm_cos_ps(v);
#else
        alignas(16) float xs[4];
        _mm_store_ps(xs, v);
        return _mm_setr_ps(
            Cos<float>(xs[0]),
            Cos<float>(xs[1]),
            Cos<float>(xs[2]),
            Cos<float>(xs[3]));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Tan(SimdVector4F v)
    {
#if ANEMONE_FEATURE_SVML
        return _mm_tan_ps(v);
#else
        alignas(16) float xs[4];
        _mm_store_ps(xs, v);
        return _mm_setr_ps(
            Tan<float>(xs[0]),
            Tan<float>(xs[1]),
            Tan<float>(xs[2]),
            Tan<float>(xs[3]));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Asin(SimdVector4F v)
    {
#if ANEMONE_FEATURE_SVML
        return _mm_asin_ps(v);
#else
        alignas(16) float xs[4];
        _mm_store_ps(xs, v);
        return _mm_setr_ps(
            Asin<float>(xs[0]),
            Asin<float>(xs[1]),
            Asin<float>(xs[2]),
            Asin<float>(xs[3]));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Acos(SimdVector4F v)
    {
#if ANEMONE_FEATURE_SVML
        return _mm_acos_ps(v);
#else
        alignas(16) float xs[4];
        _mm_store_ps(xs, v);
        return _mm_setr_ps(
            Acos<float>(xs[0]),
            Acos<float>(xs[1]),
            Acos<float>(xs[2]),
            Acos<float>(xs[3]));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Atan(SimdVector4F v)
    {
#if ANEMONE_FEATURE_SVML
        return _mm_atan_ps(v);
#else
        alignas(16) float xs[4];
        _mm_store_ps(xs, v);
        return _mm_setr_ps(
            Atan<float>(xs[0]),
            Atan<float>(xs[1]),
            Atan<float>(xs[2]),
            Atan<float>(xs[3]));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Atan2(SimdVector4F y, SimdVector4F x)
    {
#if ANEMONE_FEATURE_SVML
        return _mm_atan2_ps(y, x);
#else
        alignas(16) float ys[4];
        alignas(16) float xs[4];
        _mm_store_ps(ys, y);
        _mm_store_ps(xs, x);
        return _mm_setr_ps(
            Atan2<float>(ys[0], xs[0]),
            Atan2<float>(ys[1], xs[1]),
            Atan2<float>(ys[2], xs[2]),
            Atan2<float>(ys[3], xs[3]));
#endif
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PreciseLerp(SimdVector4F a, SimdVector4F b, SimdVector4F t)
    {
        return Vector4F_MultiplyAdd(a, _mm_sub_ps(_mm_load_ps(F32x4_PositiveOne.As<float>()), t), _mm_mul_ps(b, t));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PreciseLerp(SimdVector4F a, SimdVector4F b, float t)
    {
        return Vector4F_MultiplyAdd(a, _mm_set1_ps(1.0f - t), _mm_mul_ps(b, _mm_set1_ps(t)));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Lerp(SimdVector4F a, SimdVector4F b, SimdVector4F t)
    {
        __m128 const ba = _mm_sub_ps(b, a);
        return Vector4F_MultiplyAdd(ba, t, a);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Lerp(SimdVector4F a, SimdVector4F b, float t)
    {
        __m128 const ba = _mm_sub_ps(b, a);
        return Vector4F_MultiplyAdd(ba, _mm_set1_ps(t), a);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PreciseBilinearLerp(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F d, SimdVector4F u, SimdVector4F v)
    {
        __m128 const ab = Vector4F_PreciseLerp(a, b, u);
        __m128 const cd = Vector4F_PreciseLerp(c, d, u);
        return Vector4F_PreciseLerp(ab, cd, v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PreciseBilinearLerp(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F d, float u, float v)
    {
        __m128 const ab = Vector4F_PreciseLerp(a, b, u);
        __m128 const cd = Vector4F_PreciseLerp(c, d, u);
        return Vector4F_PreciseLerp(ab, cd, v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BilinearLerp(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F d, SimdVector4F u, SimdVector4F v)
    {
        __m128 const x1 = Vector4F_Lerp(a, b, u);
        __m128 const x2 = Vector4F_Lerp(c, d, u);
        return Vector4F_Lerp(x1, x2, v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BilinearLerp(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F d, float u, float v)
    {
        __m128 const x1 = Vector4F_Lerp(a, b, u);
        __m128 const x2 = Vector4F_Lerp(c, d, u);
        return Vector4F_Lerp(x1, x2, v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Barycentric(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F u, SimdVector4F v)
    {
        __m128 const ba = _mm_sub_ps(b, a);
        __m128 const ca = _mm_sub_ps(c, a);

        __m128 const r = Vector4F_MultiplyAdd(ba, u, a);
        return Vector4F_MultiplyAdd(ca, v, r);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Barycentric(SimdVector4F a, SimdVector4F b, SimdVector4F c, float u, float v)
    {
        __m128 const ba = _mm_sub_ps(b, a);
        __m128 const ca = _mm_sub_ps(c, a);

        __m128 const r = Vector4F_MultiplyAdd(ba, _mm_set1_ps(u), a);
        return Vector4F_MultiplyAdd(ca, _mm_set1_ps(v), r);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Unlerp(SimdVector4F value, SimdVector4F lower, SimdVector4F upper)
    {
        __m128 const progress = _mm_sub_ps(value, lower);
        __m128 const range = _mm_sub_ps(upper, lower);
        return _mm_div_ps(progress, range);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Map(SimdVector4F value, SimdVector4F sourceLower, SimdVector4F sourceUpper, SimdVector4F targetLower, SimdVector4F targetUpper)
    {
        __m128 const progress = Vector4F_Unlerp(value, sourceLower, sourceUpper);
        return Vector4F_Lerp(targetLower, targetUpper, progress);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PreciseMap(SimdVector4F value, SimdVector4F sourceLower, SimdVector4F sourceUpper, SimdVector4F targetLower, SimdVector4F targetUpper)
    {
        __m128 const progress = Vector4F_Unlerp(value, sourceLower, sourceUpper);
        return Vector4F_PreciseLerp(targetLower, targetUpper, progress);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierPosition(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F t)
    {
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
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierPosition(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, float t)
    {
        return Vector4F_BezierPosition(p0, p1, p2, _mm_set1_ps(t));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierTangent(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F t)
    {
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
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierTangent(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, float t)
    {
        return Vector4F_BezierTangent(p0, p1, p2, _mm_set1_ps(t));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierAcceleration(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F t)
    {
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
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierAcceleration(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, float t)
    {
        return Vector4F_BezierAcceleration(p0, p1, p2, _mm_set1_ps(t));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierPosition(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, SimdVector4F t)
    {
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
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierPosition(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, float t)
    {
        return Vector4F_BezierPosition(p0, p1, p2, p3, _mm_set1_ps(t));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierTangent(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, SimdVector4F t)
    {
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
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierTangent(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, float t)
    {
        return Vector4F_BezierTangent(p0, p1, p2, p3, _mm_set1_ps(t));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierAcceleration(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, SimdVector4F t)
    {
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
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierAcceleration(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, float t)
    {
        return Vector4F_BezierAcceleration(p0, p1, p2, p3, _mm_set1_ps(t));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Hermite(SimdVector4F p0, SimdVector4F m0, SimdVector4F p1, SimdVector4F m1, SimdVector4F t)
    {
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
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Hermite(SimdVector4F p0, SimdVector4F m0, SimdVector4F p1, SimdVector4F m1, float t)
    {
        return Vector4F_Hermite(p0, m0, p1, m1, _mm_set1_ps(t));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_CatmullRom(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, SimdVector4F t)
    {
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
    }

    inline SimdVector4F anemone_vectorcall Vector4F_CatmullRom(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, float t)
    {
        return Vector4F_CatmullRom(p0, p1, p2, p3, _mm_set1_ps(t));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_DeCasteljau(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, SimdVector4F t)
    {
        // https://en.wikipedia.org/wiki/De_Casteljau%27s_algorithm

        __m128 const a0 = Vector4F_Lerp(p0, p1, t);
        __m128 const a1 = Vector4F_Lerp(p1, p2, t);
        __m128 const a2 = Vector4F_Lerp(p2, p3, t);

        __m128 const b0 = Vector4F_Lerp(a0, a1, t);
        __m128 const b1 = Vector4F_Lerp(a1, a2, t);

        return Vector4F_Lerp(b0, b1, t);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_UnwindRadians(SimdVector4F v)
    {
        __m128 const vToRevolutions = _mm_load_ps(F32x4_InvPi2.As<float>());
        __m128 const vFromRevolutions = _mm_load_ps(F32x4_Pi2.As<float>());

        // Convert radians to full revolutions
        __m128 r = _mm_mul_ps(v, vToRevolutions);
        r = Vector4F_Round(r);
        // Convert full revolutions back to radians and subtract from original
        return Vector4F_NegateMultiplyAdd(r, vFromRevolutions, v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_UnwindDegrees(SimdVector4F v)
    {
        __m128 const vToRevolutions = _mm_set1_ps(Private::Factor_DegreesToRevolutions);
        __m128 const vFromRevolutions = _mm_set1_ps(Private::Factor_RevolutionsToDegrees);

        // Convert degrees to full revolutions
        __m128 r = _mm_mul_ps(v, vToRevolutions);
        r = Vector4F_Round(r);
        // Convert full revolutions back to degrees and subtract from original
        return Vector4F_NegateMultiplyAdd(r, vFromRevolutions, v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_AngleDifferenceRadians(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_UnwindRadians(_mm_sub_ps(a, b));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_AngleDifferenceDegrees(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_UnwindDegrees(_mm_sub_ps(a, b));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_RevolutionsToDegrees(SimdVector4F v)
    {
        return _mm_mul_ps(v, _mm_set1_ps(Private::Factor_RevolutionsToDegrees));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_RevolutionsToRadians(SimdVector4F v)
    {
        return _mm_mul_ps(v, _mm_set1_ps(Private::Factor_RevolutionsToRadians));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_DegreesToRevolutions(SimdVector4F v)
    {
        return _mm_mul_ps(v, _mm_set1_ps(Private::Factor_DegreesToRevolutions));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_RadiansToRevolutions(SimdVector4F v)
    {
        return _mm_mul_ps(v, _mm_set1_ps(Private::Factor_RadiansToRevolutions));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_RadiansToDegrees(SimdVector4F v)
    {
        return _mm_mul_ps(v, _mm_set1_ps(Private::Factor_RadiansToDegrees));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_DegreesToRadians(SimdVector4F v)
    {
        return _mm_mul_ps(v, _mm_set1_ps(Private::Factor_DegreesToRadians));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Dot4(SimdVector4F a, SimdVector4F b)
    {
        return _mm_dp_ps(a, b, 0b1111'1111);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Dot3(SimdVector4F a, SimdVector4F b)
    {
        return _mm_dp_ps(a, b, 0b0111'1111);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Dot2(SimdVector4F a, SimdVector4F b)
    {
        return _mm_dp_ps(a, b, 0b0011'1111);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Cross4(SimdVector4F a, SimdVector4F b, SimdVector4F c)
    {
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
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Cross3(SimdVector4F a, SimdVector4F b)
    {
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
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Cross2(SimdVector4F a, SimdVector4F b)
    {
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
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Wedge3(SimdVector4F a, SimdVector4F b)
    {
        // = [x,x,y,y]
        __m128 const a_xxyy = _mm_permute_ps(a, _MM_SHUFFLE(1, 1, 0, 0));
        __m128 const b_xxyy = _mm_permute_ps(b, _MM_SHUFFLE(1, 1, 0, 0));
        // = [y,z,z,z]
        __m128 const a_yzzz = _mm_permute_ps(a, _MM_SHUFFLE(2, 2, 2, 1));
        __m128 const b_yzzz = _mm_permute_ps(b, _MM_SHUFFLE(2, 2, 2, 1));

        return Vector4F_MultiplySubtract(a_xxyy, b_yzzz, _mm_mul_ps(a_yzzz, b_xxyy));
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
        __m128 const dot = Vector4F_Dot4(v, v);
        return _mm_sqrt_ps(dot);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Length3(SimdVector4F v)
    {
        __m128 const dot = Vector4F_Dot3(v, v);
        return _mm_sqrt_ps(dot);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Length2(SimdVector4F v)
    {
        __m128 const dot = Vector4F_Dot2(v, v);
        return _mm_sqrt_ps(dot);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Normalize4(SimdVector4F v)
    {
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
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Normalize3(SimdVector4F v)
    {
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
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Normalize2(SimdVector4F v)
    {
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
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Reflect4(SimdVector4F incident, SimdVector4F normal)
    {
        // = 2 * dot(incident, normal)
        __m128 const d = Vector4F_Dot4(incident, normal);
        __m128 const r = _mm_add_ps(d, d);

        // = incident - (r * normal)
        return Vector4F_NegateMultiplyAdd(r, normal, incident);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Reflect3(SimdVector4F incident, SimdVector4F normal)
    {
        // = 2 * dot(incident, normal)
        __m128 const d = Vector4F_Dot3(incident, normal);
        __m128 const r = _mm_add_ps(d, d);

        // = incident - (r * normal)
        return Vector4F_NegateMultiplyAdd(r, normal, incident);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Reflect2(SimdVector4F incident, SimdVector4F normal)
    {
        // = 2 * dot(incident, normal)
        __m128 const d = Vector4F_Dot3(incident, normal);
        __m128 const r = _mm_add_ps(d, d);

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
        __m128 const i_dot_n = Vector4F_Dot4(incident, normal);
        __m128 const one = _mm_load_ps(F32x4_PositiveOne.As<float>());

        // = 1 - dot(I, N)^2
        __m128 const t0 = Vector4F_NegateMultiplyAdd(i_dot_n, i_dot_n, one);

        // = index * (1 - dot(I, N)^2)
        __m128 const t1 = _mm_mul_ps(t0, index);

        // = 1 - (index^2 * (1 - dot(I, N)^2))
        __m128 const k = Vector4F_NegateMultiplyAdd(t1, index, one);

        __m128 const zero = _mm_setzero_ps();
        if (Vector4F_IsLessEqual4(k, zero))
        {
            return zero;
        }

        // = sqrt(t3)
        __m128 const r0 = _mm_sqrt_ps(k);

        // = (index * dot(I, N)) + sqrt(k)
        __m128 const r1 = Vector4F_MultiplyAdd(index, i_dot_n, r0);

        // = index * I
        __m128 const r2 = _mm_mul_ps(index, incident);

        // = (index * I) - (N * (index * dot(I, N)) + sqrt(k))
        return Vector4F_NegateMultiplyAdd(normal, r1, r2);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Refract4(SimdVector4F incident, SimdVector4F normal, float index)
    {
        return Vector4F_Refract4(incident, normal, _mm_set1_ps(index));
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
        __m128 const i_dot_n = Vector4F_Dot3(incident, normal);
        __m128 const one = _mm_load_ps(F32x4_PositiveOne.As<float>());

        // = 1 - dot(I, N)^2
        __m128 const t0 = Vector4F_NegateMultiplyAdd(i_dot_n, i_dot_n, one);

        // = index * (1 - dot(I, N)^2)
        __m128 const t1 = _mm_mul_ps(t0, index);

        // = 1 - (index^2 * (1 - dot(I, N)^2))
        __m128 const k = Vector4F_NegateMultiplyAdd(t1, index, one);

        __m128 const zero = _mm_setzero_ps();
        if (Vector4F_IsLessEqual3(k, zero))
        {
            return zero;
        }

        // = sqrt(t3)
        __m128 const r0 = _mm_sqrt_ps(k);

        // = (index * dot(I, N)) + sqrt(k)
        __m128 const r1 = Vector4F_MultiplyAdd(index, i_dot_n, r0);

        // = index * I
        __m128 const r2 = _mm_mul_ps(index, incident);

        // = (index * I) - (N * (index * dot(I, N)) + sqrt(k))
        return Vector4F_NegateMultiplyAdd(normal, r1, r2);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Refract3(SimdVector4F incident, SimdVector4F normal, float index)
    {
        return Vector4F_Refract3(incident, normal, _mm_set1_ps(index));
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
        __m128 const i_dot_n = Vector4F_Dot2(incident, normal);
        __m128 const one = _mm_load_ps(F32x4_PositiveOne.As<float>());

        // = 1 - dot(I, N)^2
        __m128 const t0 = Vector4F_NegateMultiplyAdd(i_dot_n, i_dot_n, one);

        // = index * (1 - dot(I, N)^2)
        __m128 const t1 = _mm_mul_ps(t0, index);

        // = 1 - (index^2 * (1 - dot(I, N)^2))
        __m128 const k = Vector4F_NegateMultiplyAdd(t1, index, one);

        __m128 const zero = _mm_setzero_ps();
        if (Vector4F_IsLessEqual2(k, zero))
        {
            return zero;
        }

        // = sqrt(t3)
        __m128 const r0 = _mm_sqrt_ps(k);

        // = (index * dot(I, N)) + sqrt(k)
        __m128 const r1 = Vector4F_MultiplyAdd(index, i_dot_n, r0);

        // = index * I
        __m128 const r2 = _mm_mul_ps(index, incident);

        // = (index * I) - (N * (index * dot(I, N)) + sqrt(k))
        return Vector4F_NegateMultiplyAdd(normal, r1, r2);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Refract2(SimdVector4F incident, SimdVector4F normal, float index)
    {
        return Vector4F_Refract2(incident, normal, _mm_set1_ps(index));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Transform3(SimdVector4F v, SimdMatrix4x4F m)
    {
        __m128 const v_xxxx = _mm_permute_ps(v, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 const v_yyyy = _mm_permute_ps(v, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 const v_zzzz = _mm_permute_ps(v, _MM_SHUFFLE(2, 2, 2, 2));

        __m128 r = Vector4F_MultiplyAdd(v_zzzz, m.Inner[2], m.Inner[3]);
        r = Vector4F_MultiplyAdd(v_yyyy, m.Inner[1], r);
        r = Vector4F_MultiplyAdd(v_xxxx, m.Inner[0], r);
        return r;
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareEqual(SimdVector4F a, SimdVector4F b)
    {
        return _mm_cmpeq_ps(a, b);
    }

    inline bool anemone_vectorcall Vector4F_IsEqual4(SimdVector4F a, SimdVector4F b)
    {
        return _mm_movemask_ps(_mm_cmpeq_ps(a, b)) == 0b1111;
    }

    inline bool anemone_vectorcall Vector4F_IsEqual3(SimdVector4F a, SimdVector4F b)
    {
        return (_mm_movemask_ps(_mm_cmpeq_ps(a, b)) & 0b0111) == 0b0111;
    }

    inline bool anemone_vectorcall Vector4F_IsEqual2(SimdVector4F a, SimdVector4F b)
    {
        return (_mm_movemask_ps(_mm_cmpeq_ps(a, b)) & 0b0011) == 0b0011;
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNotEqual(SimdVector4F a, SimdVector4F b)
    {
        return _mm_cmpneq_ps(a, b);
    }

    inline bool anemone_vectorcall Vector4F_IsNotEqual4(SimdVector4F a, SimdVector4F b)
    {
        return _mm_movemask_ps(_mm_cmpneq_ps(a, b)) == 0b1111;
    }

    inline bool anemone_vectorcall Vector4F_IsNotEqual3(SimdVector4F a, SimdVector4F b)
    {
        return (_mm_movemask_ps(_mm_cmpneq_ps(a, b)) & 0b0111) == 0b0111;
    }

    inline bool anemone_vectorcall Vector4F_IsNotEqual2(SimdVector4F a, SimdVector4F b)
    {
        return (_mm_movemask_ps(_mm_cmpneq_ps(a, b)) & 0b0011) == 0b0011;
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareLessThan(SimdVector4F a, SimdVector4F b)
    {
        return _mm_cmplt_ps(a, b);
    }

    inline bool anemone_vectorcall Vector4F_IsLessThan4(SimdVector4F a, SimdVector4F b)
    {
        return _mm_movemask_ps(_mm_cmplt_ps(a, b)) == 0b1111;
    }

    inline bool anemone_vectorcall Vector4F_IsLessThan3(SimdVector4F a, SimdVector4F b)
    {
        return (_mm_movemask_ps(_mm_cmplt_ps(a, b)) & 0b0111) == 0b0111;
    }

    inline bool anemone_vectorcall Vector4F_IsLessThan2(SimdVector4F a, SimdVector4F b)
    {
        return (_mm_movemask_ps(_mm_cmplt_ps(a, b)) & 0b0011) == 0b0011;
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareLessEqual(SimdVector4F a, SimdVector4F b)
    {
        return _mm_cmple_ps(a, b);
    }

    inline bool anemone_vectorcall Vector4F_IsLessEqual4(SimdVector4F a, SimdVector4F b)
    {
        return _mm_movemask_ps(_mm_cmple_ps(a, b)) == 0b1111;
    }

    inline bool anemone_vectorcall Vector4F_IsLessEqual3(SimdVector4F a, SimdVector4F b)
    {
        return (_mm_movemask_ps(_mm_cmple_ps(a, b)) & 0b0111) == 0b0111;
    }

    inline bool anemone_vectorcall Vector4F_IsLessEqual2(SimdVector4F a, SimdVector4F b)
    {
        return (_mm_movemask_ps(_mm_cmple_ps(a, b)) & 0b0011) == 0b0011;
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareGreaterThan(SimdVector4F a, SimdVector4F b)
    {
        return _mm_cmpgt_ps(a, b);
    }

    inline bool anemone_vectorcall Vector4F_IsGreaterThan4(SimdVector4F a, SimdVector4F b)
    {
        return _mm_movemask_ps(_mm_cmpgt_ps(a, b)) == 0b1111;
    }

    inline bool anemone_vectorcall Vector4F_IsGreaterThan3(SimdVector4F a, SimdVector4F b)
    {
        return (_mm_movemask_ps(_mm_cmpgt_ps(a, b)) & 0b0111) == 0b0111;
    }

    inline bool anemone_vectorcall Vector4F_IsGreaterThan2(SimdVector4F a, SimdVector4F b)
    {
        return (_mm_movemask_ps(_mm_cmpgt_ps(a, b)) & 0b0011) == 0b0011;
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareGreaterEqual(SimdVector4F a, SimdVector4F b)
    {
        return _mm_cmpge_ps(a, b);
    }

    inline bool anemone_vectorcall Vector4F_IsGreaterEqual4(SimdVector4F a, SimdVector4F b)
    {
        return _mm_movemask_ps(_mm_cmpge_ps(a, b)) == 0b1111;
    }

    inline bool anemone_vectorcall Vector4F_IsGreaterEqual3(SimdVector4F a, SimdVector4F b)
    {
        return (_mm_movemask_ps(_mm_cmpge_ps(a, b)) & 0b0111) == 0b0111;
    }

    inline bool anemone_vectorcall Vector4F_IsGreaterEqual2(SimdVector4F a, SimdVector4F b)
    {
        return (_mm_movemask_ps(_mm_cmpge_ps(a, b)) & 0b0011) == 0b0011;
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNaN(SimdVector4F v)
    {
        return _mm_cmpunord_ps(v, v);
    }

    inline bool anemone_vectorcall Vector4F_IsNaN4(SimdVector4F v)
    {
        return _mm_movemask_ps(_mm_cmpunord_ps(v, v)) == 0b1111;
    }

    inline bool anemone_vectorcall Vector4F_IsNaN3(SimdVector4F v)
    {
        return (_mm_movemask_ps(_mm_cmpunord_ps(v, v)) & 0b0111) == 0b0111;
    }

    inline bool anemone_vectorcall Vector4F_IsNaN2(SimdVector4F v)
    {
        return (_mm_movemask_ps(_mm_cmpunord_ps(v, v)) & 0b0011) == 0b0011;
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareInfinite(SimdVector4F v)
    {
        __m128 const mask = _mm_load_ps(F32x4_AbsMask_XXXX.As<float>());
        __m128 const infinity = _mm_load_ps(F32x4_PositiveInfinity_XXXX.As<float>());

        __m128 const abs_value = _mm_and_ps(v, mask);
        return _mm_cmpeq_ps(abs_value, infinity);
    }

    inline bool anemone_vectorcall Vector4F_IsInfinite4(SimdVector4F v)
    {
        return _mm_movemask_ps(Vector4F_CompareInfinite(v)) == 0b1111;
    }

    inline bool anemone_vectorcall Vector4F_IsInfinite3(SimdVector4F v)
    {
        return (_mm_movemask_ps(Vector4F_CompareInfinite(v)) & 0b0111) == 0b0111;
    }

    inline bool anemone_vectorcall Vector4F_IsInfinite2(SimdVector4F v)
    {
        return (_mm_movemask_ps(Vector4F_CompareInfinite(v)) & 0b0011) == 0b0011;
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareInRange(SimdVector4F v, SimdVector4F lower, SimdVector4F upper)
    {
        __m128 const mask_lower = _mm_cmple_ps(lower, v);
        __m128 const mask_upper = _mm_cmple_ps(v, upper);
        return _mm_and_ps(mask_lower, mask_upper);
    }

    inline bool anemone_vectorcall Vector4F_InRange4(SimdVector4F v, SimdVector4F lower, SimdVector4F upper)
    {
        return _mm_movemask_ps(Vector4F_CompareInRange(v, lower, upper)) == 0b1111;
    }

    inline bool anemone_vectorcall Vector4F_InRange3(SimdVector4F v, SimdVector4F lower, SimdVector4F upper)
    {
        return (_mm_movemask_ps(Vector4F_CompareInRange(v, lower, upper)) & 0b0111) == 0b0111;
    }

    inline bool anemone_vectorcall Vector4F_InRange2(SimdVector4F v, SimdVector4F lower, SimdVector4F upper)
    {
        return (_mm_movemask_ps(Vector4F_CompareInRange(v, lower, upper)) & 0b0011) == 0b0011;
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearZero(SimdVector4F v, SimdVector4F tolerance)
    {
        __m128 const abs = Vector4F_Abs(v);
        return _mm_cmple_ps(abs, tolerance);
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearZero(SimdVector4F v, float tolerance)
    {
        return Vector4F_CompareNearZero(v, _mm_set1_ps(tolerance));
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearZero(SimdVector4F v)
    {
        return Vector4F_CompareNearZero(v, _mm_set1_ps(std::numeric_limits<float>::epsilon()));
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearZeroPrecise(SimdVector4F v)
    {
        return _mm_castsi128_ps(AvxCompareNearZeroPreciseI32(_mm_castps_si128(v)));
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearEqual(SimdVector4F a, SimdVector4F b, SimdVector4F tolerance)
    {
        // Compute difference between two values
        __m128 const diff = _mm_sub_ps(a, b);
        // Compute absolute value
        __m128 const abs_mask = _mm_load_ps(F32x4_AbsMask_XXXX.As<float>());
        __m128 const abs_diff = _mm_and_ps(diff, abs_mask);
        return _mm_cmple_ps(abs_diff, tolerance);
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearEqual(SimdVector4F a, SimdVector4F b, float tolerance)
    {
        return Vector4F_CompareNearEqual(a, b, _mm_set1_ps(tolerance));
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearEqual(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_CompareNearEqual(a, b, _mm_set1_ps(std::numeric_limits<float>::epsilon()));
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearEqualPrecise(SimdVector4F a, SimdVector4F b)
    {
        // = abs(a - b) < tolerance
        __m128 const mask_within_tolerance = Vector4F_CompareNearEqual(a, b);

        __m128i const a_ulp = _mm_castps_si128(a);
        __m128i const b_ulp = _mm_castps_si128(b);

        // = (sign(ulp(a)) == sign(ulp(b)))
        __m128 const mask_same_sign_ulp = _mm_castsi128_ps(AvxCompareSignI32(a_ulp, b_ulp));

        // = ulp(a - b) < ulp_tolerance
        __m128 const mask_within_ulp = _mm_castsi128_ps(AvxCompareNearZeroPreciseI32(_mm_sub_epi32(a_ulp, b_ulp)));

        return _mm_or_ps(mask_within_tolerance, _mm_and_ps(mask_same_sign_ulp, mask_within_ulp));
    }

}

// Mask functions
namespace Anemone::Numerics::Private
{
    inline SimdMask4F anemone_vectorcall Mask4F_Create(bool x, bool y, bool z, bool w)
    {
        return _mm_castsi128_ps(
            _mm_setr_epi32(
                x ? AvxMaskComponentInt32 : 0,
                y ? AvxMaskComponentInt32 : 0,
                z ? AvxMaskComponentInt32 : 0,
                w ? AvxMaskComponentInt32 : 0));
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Create(bool x, bool y, bool z)
    {
        return _mm_castsi128_ps(
            _mm_setr_epi32(
                x ? AvxMaskComponentInt32 : 0,
                y ? AvxMaskComponentInt32 : 0,
                z ? AvxMaskComponentInt32 : 0,
                0));
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Create(bool x, bool y)
    {
        return _mm_castsi128_ps(
            _mm_setr_epi32(
                x ? AvxMaskComponentInt32 : 0,
                y ? AvxMaskComponentInt32 : 0,
                0,
                0));
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Replicate(bool value)
    {
        return _mm_castsi128_ps(_mm_set1_epi32(value ? AvxMaskComponentInt32 : 0));
    }

    template <size_t N>
    inline uint32_t anemone_vectorcall Mask4F_Extract(SimdMask4F v)
        requires(N < 4)
    {
        return static_cast<uint32_t>(_mm_extract_epi32(_mm_castps_si128(v), N));
    }

    template <size_t N>
    inline SimdMask4F anemone_vectorcall Mask4F_Insert(SimdMask4F v, uint32_t b)
        requires(N < 4)
    {
        return _mm_castsi128_ps(_mm_insert_epi32(_mm_castps_si128(v), static_cast<int>(b), N));
    }

    inline SimdMask4F anemone_vectorcall Mask4F_True()
    {
        return _mm_castsi128_ps(_mm_set1_epi32(AvxMaskComponentInt32));
    }

    inline SimdMask4F anemone_vectorcall Mask4F_False()
    {
        return _mm_setzero_ps();
    }

    inline SimdMask4F anemone_vectorcall Mask4F_CompareEqual(SimdMask4F a, SimdMask4F b)
    {
        return _mm_castsi128_ps(_mm_cmpeq_epi32(_mm_castps_si128(a), _mm_castps_si128(b)));
    }

    inline SimdMask4F anemone_vectorcall Mask4F_CompareNotEqual(SimdMask4F a, SimdMask4F b)
    {
        __m128 const mask = _mm_castsi128_ps(_mm_cmpeq_epi32(_mm_castps_si128(a), _mm_castps_si128(b)));
        return _mm_xor_ps(mask, mask);
    }

    inline SimdMask4F anemone_vectorcall Mask4F_And(SimdMask4F a, SimdMask4F b)
    {
        return _mm_and_ps(a, b);
    }

    inline SimdMask4F anemone_vectorcall Mask4F_AndNot(SimdMask4F a, SimdMask4F b)
    {
        return _mm_andnot_ps(a, b);
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Or(SimdMask4F a, SimdMask4F b)
    {
        return _mm_or_ps(a, b);
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Xor(SimdMask4F a, SimdMask4F b)
    {
        return _mm_xor_ps(a, b);
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Not(SimdMask4F mask)
    {
        return _mm_xor_ps(mask, mask);
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Select(SimdMask4F mask, SimdMask4F whenFalse, SimdMask4F whenTrue)
    {
        return _mm_blendv_ps(whenFalse, whenTrue, mask);
    }

    template <bool X, bool Y, bool Z, bool W>
    inline SimdMask4F anemone_vectorcall Mask4F_Select(SimdMask4F whenFalse, SimdMask4F whenTrue)
    {
        return _mm_blend_ps(whenFalse, whenTrue, (X ? 0b0001 : 0) | (Y ? 0b0010 : 0) | (Z ? 0b0100 : 0) | (W ? 0b1000 : 0));
    }

    inline bool anemone_vectorcall Mask4F_All4(SimdMask4F mask)
    {
        return _mm_movemask_ps(mask) == 0b1111;
    }

    inline bool anemone_vectorcall Mask4F_All3(SimdMask4F mask)
    {
        return (_mm_movemask_ps(mask) & 0b0111) == 0b0111;
    }

    inline bool anemone_vectorcall Mask4F_All2(SimdMask4F mask)
    {
        return (_mm_movemask_ps(mask) & 0b0011) == 0b0011;
    }

    inline bool anemone_vectorcall Mask4F_Any4(SimdMask4F mask)
    {
        return _mm_movemask_ps(mask) != 0;
    }

    inline bool anemone_vectorcall Mask4F_Any3(SimdMask4F mask)
    {
        return (_mm_movemask_ps(mask) & 0b0111) != 0;
    }

    inline bool anemone_vectorcall Mask4F_Any2(SimdMask4F mask)
    {
        return (_mm_movemask_ps(mask) & 0b0011) != 0;
    }

    inline bool anemone_vectorcall Mask4F_None4(SimdMask4F mask)
    {
        return _mm_movemask_ps(mask) == 0;
    }

    inline bool anemone_vectorcall Mask4F_None3(SimdMask4F mask)
    {
        return (_mm_movemask_ps(mask) & 0b0111) == 0;
    }

    inline bool anemone_vectorcall Mask4F_None2(SimdMask4F mask)
    {
        return (_mm_movemask_ps(mask) & 0b0011) == 0;
    }
}

// Quaternion functions
namespace Anemone::Numerics::Private
{
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
        return Vector4F_CompareEqual(q, _mm_load_ps(F32x4_PositiveUnitW.As<float>()));
    }

    inline bool anemone_vectorcall QuaternionF_IsIdentity(SimdVector4F q)
    {
        return Vector4F_IsEqual4(q, _mm_load_ps(F32x4_PositiveUnitW.As<float>()));
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Identity()
    {
        return _mm_load_ps(F32x4_PositiveUnitW.As<float>());
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_FromNormalAngle(SimdVector4F normal, float angle)
    {
        auto [fSin, fCos] = SinCos<float>(0.5f * angle);

        // = [fSin, fSin, fSin, ...]
        __m128 const vSin = _mm_set1_ps(fSin);
        // = [fSin * n.x, fSin * n.y, fSin * n.z, ...]
        __m128 r = _mm_mul_ps(normal, vSin);
        // = [fSin * n.x, fSin * n.y, fSin * n.z, fCos]
        return _mm_insert_ps(r, _mm_set_ss(fCos), 3u << 4);
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_FromAxisAngle(SimdVector4F axis, float angle)
    {
        // assert (axis is not zero)
        // assert (axis is not infinite)

        SimdVector4F const normal = Vector4F_Normalize3(axis);
        return QuaternionF_FromNormalAngle(normal, angle);
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_FromEulerAngles(SimdVector4F pitchYawRoll)
    {
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
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_FromEulerAngles(float pitch, float yaw, float roll)
    {
        return QuaternionF_FromEulerAngles(_mm_setr_ps(pitch, yaw, roll, 0.0f));
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Rotate3(SimdVector4F q, SimdVector4F v)
    {
        // = [v.xyz, 0]
        __m128 const v_xyz = _mm_and_ps(v, _mm_load_ps(F32x4_SelectMask_XXXn.As<float>()));
        __m128 const q_conj = QuaternionF_Conjugate(q);

        // = q' * v.xyz * q
        return QuaternionF_Multiply(QuaternionF_Multiply(q_conj, v_xyz), q);
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_InverseRotate3(SimdVector4F q, SimdVector4F v)
    {
        // = [v.xyz, 0]
        __m128 const v_xyz = _mm_and_ps(v, _mm_load_ps(F32x4_SelectMask_XXXn.As<float>()));
        __m128 const q_conj = QuaternionF_Conjugate(q);

        // = q * v.xyz * q'
        return QuaternionF_Multiply(QuaternionF_Multiply(q, v_xyz), q_conj);
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
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Conjugate(SimdVector4F q)
    {
        __m128 const negate_xyz = _mm_setr_ps(-1.0f, -1.0f, -1.0f, 1.0f);
        return _mm_mul_ps(q, negate_xyz);
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Inverse(SimdVector4F q)
    {
        __m128 const lengthSquared = Vector4F_LengthSquared4(q);
        __m128 const conj = QuaternionF_Conjugate(q);
        __m128 const mask = _mm_cmple_ps(lengthSquared, _mm_load_ps(F32x4_Epsilon_XXXX.As<float>()));
        __m128 const result = _mm_div_ps(conj, lengthSquared);
        return _mm_and_ps(result, mask);
    }

    inline void anemone_vectorcall QuaternionF_ToAxisAngle(SimdVector4F& axis, float& angle, SimdVector4F q)
    {
        // Axis part
        axis = q;

        // Angle part
        angle = 2.0f * Acos<float>(Vector4F_Extract<3>(q));
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Nlerp(SimdVector4F from, SimdVector4F to, SimdVector4F t)
    {
        SimdVector4F const r = Vector4F_PreciseLerp(from, to, t);
        return Vector4F_Normalize4(r);
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Nlerp(SimdVector4F from, SimdVector4F to, float t)
    {
        return QuaternionF_Nlerp(from, to, _mm_set1_ps(t));
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Slerp(SimdVector4F from, SimdVector4F to, SimdVector4F t)
    {
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
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Slerp(SimdVector4F from, SimdVector4F to, float t)
    {
        return QuaternionF_Slerp(from, to, _mm_set1_ps(t));
    }
}

// Matrix functions
namespace Anemone::Numerics::Private
{
    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Identity()
    {
        return SimdMatrix4x4F{
            _mm_load_ps(F32x4_PositiveUnitX.As<float>()),
            _mm_load_ps(F32x4_PositiveUnitY.As<float>()),
            _mm_load_ps(F32x4_PositiveUnitZ.As<float>()),
            _mm_load_ps(F32x4_PositiveUnitW.As<float>()),
        };
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_NaN()
    {
        __m128 const lane = _mm_load_ps(F32x4_PositiveQNaN_XXXX.As<float>());
        return SimdMatrix4x4F{lane, lane, lane, lane};
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Infinity()
    {
        __m128 const lane = _mm_load_ps(F32x4_PositiveInfinity_XXXX.As<float>());
        return SimdMatrix4x4F{lane, lane, lane, lane};
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateTranslation(float x, float y, float z)
    {
        return SimdMatrix4x4F{
            _mm_load_ps(F32x4_PositiveUnitX.As<float>()),
            _mm_load_ps(F32x4_PositiveUnitY.As<float>()),
            _mm_load_ps(F32x4_PositiveUnitZ.As<float>()),
            _mm_setr_ps(x, y, z, 1.0f),
        };
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateTranslation(SimdVector4F translation)
    {
        return SimdMatrix4x4F{
            _mm_load_ps(F32x4_PositiveUnitX.As<float>()),
            _mm_load_ps(F32x4_PositiveUnitY.As<float>()),
            _mm_load_ps(F32x4_PositiveUnitZ.As<float>()),
            _mm_blend_ps(translation, _mm_load_ps(F32x4_PositiveOne.As<float>()), 0b1000),
        };
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateScale(float x, float y, float z)
    {
        return SimdMatrix4x4F{
            _mm_setr_ps(x, 0.0f, 0.0f, 0.0f),
            _mm_setr_ps(0.0f, y, 0.0f, 0.0f),
            _mm_setr_ps(0.0f, 0.0f, z, 0.0f),
            _mm_load_ps(F32x4_PositiveUnitW.As<float>()),
        };
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateScale(SimdVector4F scale)
    {
        __m128 const zero = _mm_setzero_ps();
        return SimdMatrix4x4F{
            _mm_blend_ps(zero, scale, 0b0001),
            _mm_blend_ps(zero, scale, 0b0010),
            _mm_blend_ps(zero, scale, 0b0100),
            _mm_load_ps(F32x4_PositiveUnitW.As<float>()),
        };
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateRotationX(float angle)
    {
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
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateRotationY(float angle)
    {
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
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateRotationZ(float angle)
    {
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
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateFromQuaternion(SimdVector4F q)
    {
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
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateFromPitchYawRoll(float pitch, float yaw, float roll)
    {
        return Matrix4x4F_CreateFromPitchYawRoll(_mm_setr_ps(pitch, yaw, roll, 0.0f));
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateFromPitchYawRoll(SimdVector4F pitchYawRoll)
    {
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
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateAffineTransform2(
        SimdVector4F scaling,
        SimdVector4F rotationOrigin,
        float rotation,
        SimdVector4F translation)
    {
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
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateAffineTransform3(
        SimdVector4F scaling,
        SimdVector4F rotationOrigin,
        SimdVector4F rotationQuaternion,
        SimdVector4F translation)
    {
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
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadFloat4x4(float const* source)
    {
        return SimdMatrix4x4F{
            _mm_load_ps(source + 0),
            _mm_load_ps(source + 4),
            _mm_load_ps(source + 8),
            _mm_load_ps(source + 12),
        };
    }

    inline void anemone_vectorcall Matrix4x4F_StoreFloat4x4(float* destination, SimdMatrix4x4F source)
    {
        _mm_store_ps(destination + 0, source.Inner[0]);
        _mm_store_ps(destination + 4, source.Inner[1]);
        _mm_store_ps(destination + 8, source.Inner[2]);
        _mm_store_ps(destination + 12, source.Inner[3]);
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadFloat4x3(float const* source)
    {
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
    }

    inline void anemone_vectorcall Matrix4x4F_StoreFloat4x3(float* destination, SimdMatrix4x4F source)
    {
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
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadFloat3x4(float const* source)
    {
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
    }

    inline void anemone_vectorcall Matrix4x4F_StoreFloat3x4(float* destination, SimdMatrix4x4F source)
    {
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
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadFloat3x3(float const* source)
    {
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
    }

    inline void anemone_vectorcall Matrix4x4F_StoreFloat3x3(float* destination, SimdMatrix4x4F source)
    {
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
    }

    template <size_t N>
    inline SimdVector4F anemone_vectorcall Matrix4x4F_Extract(SimdMatrix4x4F m)
        requires(N < 4)
    {
        return m.Inner[N];
    }

    template <size_t N>
    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Insert(SimdMatrix4x4F m, SimdVector4F v)
        requires(N < 4)
    {
        m.Inner[N] = v;
        return m;
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Multiply(SimdMatrix4x4F a, SimdMatrix4x4F b)
    {
#if ANEMONE_FEATURE_AVX2
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
#else

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
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_MultiplyTranspose(SimdMatrix4x4F a, SimdMatrix4x4F b)
    {
#if ANEMONE_FEATURE_AVX2
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
#else

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
#endif
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Transpose(SimdMatrix4x4F m)
    {
#if ANEMONE_FEATURE_AVX2
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
#else
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
#endif
    }

    inline SimdVector4F anemone_vectorcall Matrix4x4F_Determinant(SimdMatrix4x4F m)
    {
        // = [m21, m20, m20, m20]
        __m128 const t0 = _mm_permute_ps(m.Inner[2], _MM_SHUFFLE(0, 0, 0, 1));
        // = [m32, m32, m31, m31]
        __m128 const t1 = _mm_permute_ps(m.Inner[3], _MM_SHUFFLE(1, 1, 2, 2));
        // = [m33, m33, m33, m32]
        __m128 const t2 = _mm_permute_ps(m.Inner[3], _MM_SHUFFLE(2, 3, 3, 3));
        // = [m22, m22, m21, m21]
        __m128 const t3 = _mm_permute_ps(m.Inner[2], _MM_SHUFFLE(1, 1, 2, 2));

        // = [m21, m20, m20, m20] * [m32, m32, m31, m31]
        __m128 const t01 = _mm_mul_ps(t0, t1);
        // = [m21, m20, m20, m20] * [m33, m33, m33, m32]
        __m128 const t02 = _mm_mul_ps(t0, t2);
        // = [m33, m33, m33, m32] * [m22, m22, m21, m21]
        __m128 const t23 = _mm_mul_ps(t2, t3);

        // = [m31, m30, m30, m30]
        __m128 const t4 = _mm_permute_ps(m.Inner[3], _MM_SHUFFLE(0, 0, 0, 1));
        // = [m23, m23, m23, m22]
        __m128 const t5 = _mm_permute_ps(m.Inner[2], _MM_SHUFFLE(2, 3, 3, 3));

        // = [m21, m20, m20, m20] * [m32, m32, m31, m31] - [m22, m22, m21, m21] * [m31, m30, m30, m30]
        __m128 const s01 = Vector4F_NegateMultiplyAdd(t3, t4, t01);
        // = [m21, m20, m20, m20] * [m33, m33, m33, m32] - [m23, m23, m23, m22] * [m31, m30, m30, m30]
        __m128 const s23 = Vector4F_NegateMultiplyAdd(t5, t4, t02);
        // = [m33, m33, m33, m32] * [m22, m22, m21, m21] - [m23, m23, m23, m22] * [m32, m32, m31, m31]
        __m128 const s45 = Vector4F_NegateMultiplyAdd(t5, t1, t23);

        // = [m13, m13, m13, m12]
        __m128 const v0 = _mm_permute_ps(m.Inner[1], _MM_SHUFFLE(2, 3, 3, 3));
        // = [m12, m12, m11, m11]
        __m128 const v1 = _mm_permute_ps(m.Inner[1], _MM_SHUFFLE(1, 1, 2, 2));
        // = [m11, m10, m10, m10];
        __m128 const v2 = _mm_permute_ps(m.Inner[1], _MM_SHUFFLE(0, 0, 0, 1));

        __m128 const negate_yw = _mm_setr_ps(1.0f, -1.0f, 1.0f, -1.0f);

        // = [m00, -m01, m02, -m03]
        __m128 const r0 = _mm_mul_ps(m.Inner[0], negate_yw);
        __m128 const r1 = _mm_mul_ps(v0, s01);
        __m128 const r2 = Vector4F_NegateMultiplyAdd(v1, s23, r1);
        __m128 const r3 = Vector4F_MultiplyAdd(v2, s45, r2);

        return Vector4F_Dot4(r0, r3);
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Inverse(SimdMatrix4x4F m, SimdVector4F& determinant)
    {
        // Transpose matrix
        __m128 const m00_m10_m01_m11 = _mm_shuffle_ps(m.Inner[0], m.Inner[1], _MM_SHUFFLE(1, 0, 1, 0));
        __m128 const m20_m30_m21_m31 = _mm_shuffle_ps(m.Inner[0], m.Inner[1], _MM_SHUFFLE(3, 2, 3, 2));
        __m128 const m02_m12_m03_m13 = _mm_shuffle_ps(m.Inner[2], m.Inner[3], _MM_SHUFFLE(1, 0, 1, 0));
        __m128 const m22_m32_m23_m33 = _mm_shuffle_ps(m.Inner[2], m.Inner[3], _MM_SHUFFLE(3, 2, 3, 2));

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
    }

    inline SimdVector4F anemone_vectorcall Matrix4x4F_Diagonal(SimdMatrix4x4F m)
    {
        __m128 const m11_m22 = _mm_insert_ps(m.Inner[0], m.Inner[1], 0b01'01'0000);
        __m128 const m11_m22_m33 = _mm_insert_ps(m11_m22, m.Inner[2], 0b10'10'0000);
        return _mm_insert_ps(m11_m22_m33, m.Inner[3], 0b11'11'0000);
    }

    inline SimdVector4F anemone_vectorcall Matrix4x4F_Trace(SimdMatrix4x4F m)
    {
        __m128 const m00 = _mm_permute_ps(m.Inner[0], _MM_SHUFFLE(0, 0, 0, 0));
        __m128 const m11 = _mm_permute_ps(m.Inner[1], _MM_SHUFFLE(1, 1, 1, 1));
        __m128 const m00m11 = _mm_add_ps(m00, m11);
        __m128 const m22 = _mm_permute_ps(m.Inner[2], _MM_SHUFFLE(2, 2, 2, 2));
        __m128 const m33 = _mm_permute_ps(m.Inner[3], _MM_SHUFFLE(3, 3, 3, 3));
        __m128 const m22m33 = _mm_add_ps(m22, m33);
        return _mm_add_ps(m00m11, m22m33);
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_TensorProduct(SimdVector4F a, SimdVector4F b)
    {
        return SimdMatrix4x4F{
            _mm_mul_ps(_mm_permute_ps(a, _MM_SHUFFLE(0, 0, 0, 0)), b),
            _mm_mul_ps(_mm_permute_ps(a, _MM_SHUFFLE(1, 1, 1, 1)), b),
            _mm_mul_ps(_mm_permute_ps(a, _MM_SHUFFLE(2, 2, 2, 2)), b),
            _mm_mul_ps(_mm_permute_ps(a, _MM_SHUFFLE(3, 3, 3, 3)), b),
        };
    }

    inline bool anemone_vectorcall Matrix4x4F_IsIdentity(SimdMatrix4x4F m)
    {
        __m128 const t0 = _mm_cmpeq_ps(m.Inner[0], _mm_load_ps(F32x4_PositiveUnitX.As<float>()));
        __m128 const t1 = _mm_cmpeq_ps(m.Inner[1], _mm_load_ps(F32x4_PositiveUnitY.As<float>()));
        __m128 const t2 = _mm_cmpeq_ps(m.Inner[2], _mm_load_ps(F32x4_PositiveUnitZ.As<float>()));
        __m128 const t3 = _mm_cmpeq_ps(m.Inner[3], _mm_load_ps(F32x4_PositiveUnitW.As<float>()));

        __m128 const t01 = _mm_and_ps(t0, t1);
        __m128 const t23 = _mm_and_ps(t2, t3);
        return _mm_movemask_ps(_mm_and_ps(t01, t23)) == 0b1111;
    }

    inline bool anemone_vectorcall Matrix4x4F_IsNaN(SimdMatrix4x4F m)
    {
        __m128 const t01 = _mm_cmpunord_ps(m.Inner[0], m.Inner[1]);
        __m128 const t23 = _mm_cmpunord_ps(m.Inner[2], m.Inner[3]);
        __m128 const t0123 = _mm_or_ps(t01, t23);
        return _mm_movemask_ps(t0123) != 0;
    }

    inline bool anemone_vectorcall Matrix4x4F_IsInfinite(SimdMatrix4x4F m)
    {
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
    }
}

namespace Anemone::Numerics::Private
{
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
        return Vector4F_CompareEqual(q, _mm_load_ps(F32x4_PositiveUnitW.As<float>()));
    }

    inline bool anemone_vectorcall RotorF_IsIdentity(SimdVector4F q)
    {
        return Vector4F_IsEqual4(q, _mm_load_ps(F32x4_PositiveUnitW.As<float>()));
    }

    inline SimdVector4F anemone_vectorcall RotorF_Create(float xy, float xz, float yz, float scalar)
    {
        return Vector4F_Create(xy, xz, yz, scalar);
    }

    inline SimdVector4F anemone_vectorcall RotorF_Create(SimdVector4F bv, float scalar)
    {
        return Vector4F_Insert<3>(bv, scalar);
    }

    inline SimdVector4F anemone_vectorcall RotorF_Identity()
    {
        return _mm_load_ps(F32x4_PositiveUnitW.As<float>());
    }

    inline SimdVector4F anemone_vectorcall RotorF_FromAxisAngle(SimdVector4F axis, float angle)
    {
        SimdVector4F const normal = Vector4F_Normalize3(axis);
        return RotorF_FromNormalAngle(normal, angle);
    }

    inline SimdVector4F anemone_vectorcall RotorF_FromNormalAngle(SimdVector4F normal, float angle)
    {
        auto [fSin, fCos] = SinCos<float>(angle * 0.5f);

        // = [-sin, -sin, -sin, -sin]
        __m128 const vSin = _mm_set1_ps(-fSin);
        // = sin.xyz * normal.xyz
        __m128 const r = _mm_mul_ps(vSin, normal);
        return _mm_insert_ps(r, _mm_set_ss(fCos), 3 << 4);
    }

    inline SimdVector4F anemone_vectorcall RotorF_RotationBetween(SimdVector4F from, SimdVector4F to)
    {
        // = 1 + dot3(to, from)
        __m128 const t0 = Vector4F_Dot3(to, from);
        __m128 const t1 = _mm_add_ps(_mm_load_ps(F32x4_PositiveOne.As<float>()), t0);

        // = wedge(to, from)
        __m128 const r0 = Vector4F_Wedge3(to, from);
        // = [r0.xyz, t1.w]
        __m128 const r1 = _mm_blend_ps(r0, t1, 0b1000);
        return RotorF_Normalize(r1);
    }

    inline SimdVector4F anemone_vectorcall RotorF_Rotate3(SimdVector4F rotor, SimdVector4F v)
    {
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
    }

    inline SimdVector4F anemone_vectorcall RotorF_InverseRotate3(SimdVector4F rotor, SimdVector4F v)
    {
        return RotorF_Rotate3(RotorF_Reverse(rotor), v);
    }

    inline SimdVector4F anemone_vectorcall RotorF_Multiply(SimdVector4F a, SimdVector4F b)
    {
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
    }

    inline SimdVector4F anemone_vectorcall RotorF_RotateRotor(SimdVector4F rotor, SimdVector4F rotation)
    {
        SimdVector4F const t = RotorF_Multiply(rotor, rotation);
        return RotorF_Multiply(t, RotorF_Reverse(rotor));
    }

    inline SimdVector4F anemone_vectorcall RotorF_Reverse(SimdVector4F rotor)
    {
        __m128 const negate_xyz = _mm_setr_ps(-1.0f, -1.0f, -1.0f, 1.0f);
        return _mm_mul_ps(rotor, negate_xyz);
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
    }

    inline SimdVector4F anemone_vectorcall RotorF_Nlerp(SimdVector4F from, SimdVector4F to, SimdVector4F t)
    {
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
    }

    inline SimdVector4F anemone_vectorcall RotorF_Nlerp(SimdVector4F from, SimdVector4F to, float t)
    {
        return RotorF_Nlerp(from, to, _mm_set1_ps(t));
    }

    inline SimdVector4F anemone_vectorcall RotorF_Slerp(SimdVector4F from, SimdVector4F to, SimdVector4F t)
    {
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
    }

    inline SimdVector4F anemone_vectorcall RotorF_Slerp(SimdVector4F from, SimdVector4F to, float t)
    {
        return RotorF_Slerp(from, to, _mm_set1_ps(t));
    }
}
