#pragma once
#include <cstdint>

namespace Anemone::Math
{
    struct fixed32p16 final
    {
        static constexpr size_t fraction_bits = 16;
        static constexpr int32_t value_unit = int32_t{1} << fraction_bits;
        static constexpr size_t bits_size = 32;

        static constexpr uint32_t fraction_mask = (uint32_t{1} << fraction_bits) - 1;

        using overflow_type = int64_t;

        int32_t value;
    };

    constexpr fixed32p16 __vectorcall fixed32p16_create(float v)
    {
        return {static_cast<int32_t>(v * static_cast<float>(1 << fixed32p16::fraction_bits) + (v >= 0.0f ? 0.5f : -0.5f))};
    }

    constexpr float __vectorcall fixed32p16_to_float(fixed32p16 a)
    {
        return static_cast<float>(a.value) / static_cast<float>(1 << fixed32p16::fraction_bits);
    }

    constexpr int32_t __vectorcall fixed32p16_to_int(fixed32p16 a)
    {
        return static_cast<int32_t>(a.value >> fixed32p16::fraction_bits);
    }

    constexpr fixed32p16 __vectorcall fixed32p16_add(fixed32p16 a, fixed32p16 b)
    {
        return {a.value + b.value};
    }

    constexpr fixed32p16 __vectorcall fixed32p16_subtract(fixed32p16 a, fixed32p16 b)
    {
        return {a.value - b.value};
    }

    constexpr fixed32p16 __vectorcall fixed32p16_multiply(fixed32p16 a, fixed32p16 b)
    {
        int64_t const sext = static_cast<int64_t>(a.value) * b.value;
        return {static_cast<int32_t>(sext >> fixed32p16::fraction_bits)};
    }

    constexpr fixed32p16 __vectorcall fixed32p16_divide(fixed32p16 a, fixed32p16 b)
    {
        int64_t const high = static_cast<int64_t>(a.value) << fixed32p16::fraction_bits;
        return {static_cast<int32_t>(high / b.value)};
    }

    constexpr fixed32p16 __vectorcall fixed32p16_negate(fixed32p16 v)
    {
        return {-v.value};
    }

    constexpr fixed32p16 __vectorcall fixed32p16_fraction(int32_t num, int32_t den)
    {
        return {static_cast<int32_t>(((static_cast<int64_t>(num) << 32) / (static_cast<int64_t>(den) << fixed32p16::fraction_bits)) >> fixed32p16::fraction_bits)};
    }

    constexpr fixed32p16 __vectorcall fixed32p16_frac(fixed32p16 a)
    {
        return {static_cast<int32_t>(a.value & fixed32p16::fraction_mask)};
    }

    constexpr fixed32p16 __vectorcall fixed32p16_floor(fixed32p16 a)
    {
        uint32_t const bits = static_cast<uint32_t>(a.value);
        constexpr uint32_t mask = (uint32_t{1} << fixed32p16::fraction_bits) - 1;
        uint32_t const result = bits & ~mask;
        return {static_cast<int32_t>(result)};
    }

    constexpr fixed32p16 __vectorcall fixed32p16_ceil(fixed32p16 a)
    {
        uint32_t const bits = static_cast<uint32_t>(a.value);
        constexpr uint32_t unit = uint32_t{1} << fixed32p16::fraction_bits;
        constexpr uint32_t mask = unit - 1;
        uint32_t const result = (bits + unit) & ~mask;
        return {static_cast<int32_t>(result)};
    }

    constexpr fixed32p16 __vectorcall fixed32p16_round(fixed32p16 a)
    {
        uint32_t const bits = static_cast<uint32_t>(a.value);
        uint32_t const result = (bits + (fixed32p16::fraction_mask >> 1)) & ~fixed32p16::fraction_mask;
        return {static_cast<int32_t>(result)};
    }

    constexpr fixed32p16 __vectorcall fixed32p16_abs(fixed32p16 a)
    {
        // Is this right?
        return {(a.value > 0) ? a.value : -a.value};
    }

    constexpr fixed32p16 __vectorcall fixed32p16_fmod(fixed32p16 x, fixed32p16 y)
    {
        return {x.value % y.value};
    }

    constexpr fixed32p16 __vectorcall fixed32p16_sqrt(fixed32p16 x)
    {
        fixed32p16::overflow_type root = 0;
        fixed32p16::overflow_type remhi = 0;
        fixed32p16::overflow_type remlo = static_cast<fixed32p16::overflow_type>(x.value) << (fixed32p16::fraction_bits & 1);
        fixed32p16::overflow_type count = static_cast<fixed32p16::overflow_type>(fixed32p16::bits_size >> 1) + static_cast<fixed32p16::overflow_type>(fixed32p16::fraction_bits >> 1) - (static_cast<fixed32p16::overflow_type>(fixed32p16::fraction_bits + 1) & 1);

        do
        {
            remhi = (remhi << 2) | (remlo >> (fixed32p16::bits_size - 2));
            remlo <<= 2;
            remlo &= (fixed32p16::overflow_type{1} << fixed32p16::bits_size) - 1;
            root <<= 1;
            root &= (fixed32p16::overflow_type{1} << fixed32p16::bits_size) - 1;

            fixed32p16::overflow_type div = (root << 1) + 1;

            if (remhi >= div)
            {
                remhi -= div;
                remhi &= (fixed32p16::overflow_type{1} << fixed32p16::bits_size) - 1;
                root += 1;
            }
        } while (count--);

        return {static_cast<int32_t>(root >> (fixed32p16::fraction_bits & 1))};
    }

    struct fixed32p16x4native final
    {
        fixed32p16 x;
        fixed32p16 y;
        fixed32p16 z;
        fixed32p16 w;
    };

    __forceinline fixed32p16x4native __vectorcall fixed32p16x4native_create(fixed32p16 x, fixed32p16 y, fixed32p16 z, fixed32p16 w)
    {
        return {x, y, z, w};
    }

    __forceinline fixed32p16x4native __vectorcall fixed32p16x4native_add(fixed32p16x4native a, fixed32p16x4native b)
    {
        return {
            fixed32p16_add(a.x, b.x),
            fixed32p16_add(a.y, b.y),
            fixed32p16_add(a.z, b.z),
            fixed32p16_add(a.w, b.w),
        };
    }

    __forceinline fixed32p16x4native __vectorcall fixed32p16x4native_subtract(fixed32p16x4native a, fixed32p16x4native b)
    {
        return {
            fixed32p16_subtract(a.x, b.x),
            fixed32p16_subtract(a.y, b.y),
            fixed32p16_subtract(a.z, b.z),
            fixed32p16_subtract(a.w, b.w),
        };
    }

    __forceinline fixed32p16x4native __vectorcall fixed32p16x4native_multiply(fixed32p16x4native a, fixed32p16x4native b)
    {
        return {
            fixed32p16_multiply(a.x, b.x),
            fixed32p16_multiply(a.y, b.y),
            fixed32p16_multiply(a.z, b.z),
            fixed32p16_multiply(a.w, b.w),
        };
    }

    __forceinline fixed32p16x4native __vectorcall fixed32p16x4native_divide(fixed32p16x4native a, fixed32p16x4native b)
    {
        return {
            fixed32p16_divide(a.x, b.x),
            fixed32p16_divide(a.y, b.y),
            fixed32p16_divide(a.z, b.z),
            fixed32p16_divide(a.w, b.w),
        };
    }

    struct fixed32p16x4sse final
    {
        __m128i inner;
    };

    __forceinline fixed32p16x4sse __vectorcall fixed32p16x4sse_create(fixed32p16 x, fixed32p16 y, fixed32p16 z, fixed32p16 w)
    {
        return {_mm_set_epi32(w.value, z.value, y.value, x.value)};
    }

    __forceinline fixed32p16x4sse __vectorcall fixed32p16x4sse_add(fixed32p16x4sse a, fixed32p16x4sse b)
    {
        return {_mm_add_epi32(a.inner, b.inner)};
    }

    __forceinline fixed32p16x4sse __vectorcall fixed32p16x4sse_subtract(fixed32p16x4sse a, fixed32p16x4sse b)
    {
        return {_mm_sub_epi32(a.inner, b.inner)};
    }

    __forceinline fixed32p16x4sse __vectorcall fixed32p16x4sse_multiply(fixed32p16x4sse a, fixed32p16x4sse b)
    {
        __m128i const va_xz = a.inner;
        __m128i const vb_xz = b.inner;
        __m128i const va_yw = _mm_shuffle_epi32(a.inner, _MM_SHUFFLE(2, 3, 0, 1));
        __m128i const vb_yw = _mm_shuffle_epi32(b.inner, _MM_SHUFFLE(2, 3, 0, 1));
        __m128i const vavb_xz = _mm_srli_epi64(_mm_mul_epi32(va_xz, vb_xz), 16);
        __m128i const vavb_yw = _mm_srli_epi64(_mm_mul_epi32(va_yw, vb_yw), 16);

        __m128i const vr = _mm_or_epi32(
            //_mm_shuffle_epi32(vavb_xz, _MM_SHUFFLE(3, 2, 1, 0)),
            vavb_xz,
            _mm_shuffle_epi32(vavb_yw, _MM_SHUFFLE(2, 3, 0, 1)));

        return {vr};
    }

    __forceinline fixed32p16x4sse __vectorcall fixed32p16x4sse_divide(fixed32p16x4sse a, fixed32p16x4sse b)
    {
        // = [x as i32, y as i32, ...]
        __m128i const va_x32_y32 = a.inner;
        // = [z as i32, w as i32, ...]
        __m128i const va_z32_w32 = _mm_shuffle_epi32(a.inner, _MM_SHUFFLE(2, 3, 0, 1));

        // = [x as i32, y as i32, ...]
        __m128i const vb_x32_y32 = b.inner;
        // = [z as i32, w as i32, ...]
        __m128i const vb_z32_w32 = _mm_shuffle_epi32(b.inner, _MM_SHUFFLE(2, 3, 0, 1));

        // = [(x as i64) << 16, (y as i64) << 16, ...]
        __m128i const va_x64_y64 = _mm_slli_epi64(_mm_cvtepi32_epi64(va_x32_y32), 16);
        // = [(z as i64) << 16, (w as i64) << 16, ...]
        __m128i const va_z64_w64 = _mm_slli_epi64(_mm_cvtepi32_epi64(va_z32_w32), 16);

        // = [x as i64, y as i64, ...]
        __m128i const vb_x64_y64 = _mm_cvtepi32_epi64(vb_x32_y32);
        // = [z as i64, w as i64, ...]
        __m128i const vb_z64_w64 = _mm_cvtepi32_epi64(vb_z32_w32);

        // = [((x1 as i64) << 16) / (x2 as i64), ((y1 as i64) << 16) / (y2 as i64), ...]
        __m128i const vr_x64_y64 = _mm_div_epi64(va_x64_y64, vb_x64_y64);
        // = [((z1 as i64) << 16) / (z2 as i64), ((w1 as i64) << 16) / (w2 as i64), ...]
        __m128i const vr_z64_w64 = _mm_div_epi64(va_z64_w64, vb_z64_w64);

        // = [x as i32, y as i32, z as i32, w as i32]
        __m128i const vr = _mm_or_epi32(
            _mm_shuffle_epi32(vr_x64_y64, _MM_SHUFFLE(3, 1, 2, 0)),
            _mm_shuffle_epi32(vr_z64_w64, _MM_SHUFFLE(2, 0, 3, 1)));

        return {vr};
    }

    struct fixed32p16x4avx2 final
    {
        __m128i inner;
    };

    __forceinline fixed32p16x4avx2 __vectorcall fixed32p16x4avx2_create(fixed32p16 x, fixed32p16 y, fixed32p16 z, fixed32p16 w)
    {
        return {_mm_set_epi32(w.value, z.value, y.value, x.value)};
    }

    __forceinline fixed32p16x4avx2 __vectorcall fixed32p16x4avx2_add(fixed32p16x4avx2 a, fixed32p16x4avx2 b)
    {
        return {_mm_add_epi32(a.inner, b.inner)};
    }

    __forceinline fixed32p16x4avx2 __vectorcall fixed32p16x4avx2_subtract(fixed32p16x4avx2 a, fixed32p16x4avx2 b)
    {
        return {_mm_sub_epi32(a.inner, b.inner)};
    }

    __forceinline fixed32p16x4avx2 __vectorcall fixed32p16x4avx2_multiply(fixed32p16x4avx2 a, fixed32p16x4avx2 b)
    {
        // = [x as i64, y as i64, z as i64, w as i64]
        __m256i va_xyzw = _mm256_cvtepi32_epi64(a.inner);
        __m256i vb_xyzw = _mm256_cvtepi32_epi64(b.inner);

        // = [(x1 * x2) >> 16, ...]
        __m256i vr_xyzw = _mm256_srli_epi64(_mm256_mul_epi32(va_xyzw, vb_xyzw), 16);

        // = [x as i32, y as i32, z as i32, w as i32]
        __m256i const indices = _mm256_set_epi32(0, 0, 0, 0, 6, 4, 2, 0); // indices for lower parts of 64 bit integers
        __m256i const vr = _mm256_permutevar8x32_epi32(vr_xyzw, indices);
        return {_mm256_castsi256_si128(vr)};
    }

    __forceinline fixed32p16x4avx2 __vectorcall fixed32p16x4avx2_divide(fixed32p16x4avx2 a, fixed32p16x4avx2 b)
    {
        // = [
        //      ((x1 as i64) >> 16) / (x2 as i64),
        //      ((y1 as i64) >> 16) / (y2 as i64),
        //      ((z1 as i64) >> 16) / (z2 as i64),
        //      ((w1 as i64) >> 16) / (w2 as i64)
        // ]
        __m256i va_i64 = _mm256_slli_epi64(_mm256_cvtepi32_epi64(a.inner), 16);
        __m256i vb_i64 = _mm256_cvtepi32_epi64(b.inner);
        __m256i vr_i64 = _mm256_div_epi64(va_i64, vb_i64);

        // = [x as i32, y as i32, z as i32, w as i32]
        __m256i const indices = _mm256_set_epi32(0, 0, 0, 0, 6, 4, 2, 0); // indices for lower parts of 64 bit integers
        __m256i const vr_i32 = _mm256_permutevar8x32_epi32(vr_i64, indices);
        return {_mm256_castsi256_si128(vr_i32)};
    }

}
