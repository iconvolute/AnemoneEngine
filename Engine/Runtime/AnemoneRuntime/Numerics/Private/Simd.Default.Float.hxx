#pragma once
#include "AnemoneRuntime/Numerics/Simd.hxx"
#include "AnemoneRuntime/Diagnostic/Assert.hxx"

namespace Anemone::Numerics::Private
{
    inline constexpr uint32_t ScalarMaskComponentUInt32{0xFFFF'FFFF};
}

// Vector functions
namespace Anemone::Numerics::Private
{
    inline SimdVector4F anemone_vectorcall Vector4F_LoadFloat4(float const* source)
    {
        return SimdVector4F{
            source[0],
            source[1],
            source[2],
            source[3],
        };
    }

    inline void anemone_vectorcall Vector4F_StoreFloat4(float* destination, SimdVector4F source)
    {
        destination[0] = source.Inner[0];
        destination[1] = source.Inner[1];
        destination[2] = source.Inner[2];
        destination[3] = source.Inner[3];
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LoadFloat3(float const* source)
    {
        return SimdVector4F{
            source[0],
            source[1],
            source[2],
            0.0f,
        };
    }

    inline void anemone_vectorcall Vector4F_StoreFloat3(float* destination, SimdVector4F source)
    {
        destination[0] = source.Inner[0];
        destination[1] = source.Inner[1];
        destination[2] = source.Inner[2];
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LoadFloat2(float const* source)
    {
        return SimdVector4F{
            source[0],
            source[1],
            0.0f,
            0.0f,
        };
    }

    inline void anemone_vectorcall Vector4F_StoreFloat2(float* destination, SimdVector4F source)
    {
        destination[0] = source.Inner[0];
        destination[1] = source.Inner[1];
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LoadFloat1(float const* source)
    {
        return SimdVector4F{
            source[0],
            0.0f,
            0.0f,
            0.0f,
        };
    }

    inline void anemone_vectorcall Vector4F_StoreFloat1(float* destination, SimdVector4F source)
    {
        destination[0] = source.Inner[0];
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Create(float x, float y, float z, float w)
    {
        return SimdVector4F{
            x,
            y,
            z,
            w,
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Replicate(float f)
    {
        return SimdVector4F{
            f,
            f,
            f,
            f,
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Zero()
    {
        return SimdVector4F{
            0.0f,
            0.0f,
            0.0f,
            0.0f,
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PositiveUnitX()
    {
        return SimdVector4F{
            F32x4_PositiveUnitX.Elements[0],
            F32x4_PositiveUnitX.Elements[1],
            F32x4_PositiveUnitX.Elements[2],
            F32x4_PositiveUnitX.Elements[3],
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PositiveUnitY()
    {
        return SimdVector4F{
            F32x4_PositiveUnitY.Elements[0],
            F32x4_PositiveUnitY.Elements[1],
            F32x4_PositiveUnitY.Elements[2],
            F32x4_PositiveUnitY.Elements[3],
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PositiveUnitZ()
    {
        return SimdVector4F{
            F32x4_PositiveUnitZ.Elements[0],
            F32x4_PositiveUnitZ.Elements[1],
            F32x4_PositiveUnitZ.Elements[2],
            F32x4_PositiveUnitZ.Elements[3],
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PositiveUnitW()
    {
        return SimdVector4F{
            F32x4_PositiveUnitW.Elements[0],
            F32x4_PositiveUnitW.Elements[1],
            F32x4_PositiveUnitW.Elements[2],
            F32x4_PositiveUnitW.Elements[3],
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegativeUnitX()
    {
        return SimdVector4F{
            F32x4_NegativeUnitX.Elements[0],
            F32x4_NegativeUnitX.Elements[1],
            F32x4_NegativeUnitX.Elements[2],
            F32x4_NegativeUnitX.Elements[3],
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegativeUnitY()
    {
        return SimdVector4F{
            F32x4_NegativeUnitY.Elements[0],
            F32x4_NegativeUnitY.Elements[1],
            F32x4_NegativeUnitY.Elements[2],
            F32x4_NegativeUnitY.Elements[3],
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegativeUnitZ()
    {
        return SimdVector4F{
            F32x4_NegativeUnitZ.Elements[0],
            F32x4_NegativeUnitZ.Elements[1],
            F32x4_NegativeUnitZ.Elements[2],
            F32x4_NegativeUnitZ.Elements[3],
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegativeUnitW()
    {
        return SimdVector4F{
            F32x4_NegativeUnitW.Elements[0],
            F32x4_NegativeUnitW.Elements[1],
            F32x4_NegativeUnitW.Elements[2],
            F32x4_NegativeUnitW.Elements[3],
        };
    }

    template <size_t N>
    inline float anemone_vectorcall Vector4F_Extract(SimdVector4F v)
        requires(N < 4)
    {
        return v.Inner[N];
    }

    template <size_t N>
    inline SimdVector4F anemone_vectorcall Vector4F_Insert(SimdVector4F v, float f)
        requires(N < 4)
    {
        v.Inner[N] = f;
        return v;
    }

    template <size_t N>
    inline SimdVector4F anemone_vectorcall Vector4F_Broadcast(SimdVector4F v)
        requires(N < 4)
    {
        return SimdVector4F{
            v.Inner[N],
            v.Inner[N],
            v.Inner[N],
            v.Inner[N],
        };
    }

    template <size_t X, size_t Y, size_t Z, size_t W>
    inline SimdVector4F anemone_vectorcall Vector4F_Permute(SimdVector4F v)
        requires((X < 4) and (Y < 4) and (Z < 4) and (W < 4))
    {
        return SimdVector4F{
            v.Inner[X],
            v.Inner[Y],
            v.Inner[Z],
            v.Inner[W],
        };
    }

    template <size_t X, size_t Y, size_t Z, size_t W>
    inline SimdVector4F anemone_vectorcall Vector4F_Permute(SimdVector4F a, SimdVector4F b)
        requires((X < 8) and (Y < 8) and (Z < 8) and (W < 8))
    {
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
    }

    inline float ScalarHelper_SelectBranchless(uint32_t mask, float whenFalse, float whenTrue)
    {
        uint32_t const bitsWhenFalse = std::bit_cast<uint32_t>(whenFalse);
        uint32_t const bitsWhenTrue = std::bit_cast<uint32_t>(whenTrue);
        uint32_t const bitsResult = (mask & bitsWhenTrue) | (~mask & bitsWhenFalse);
        return std::bit_cast<float>(bitsResult);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Select(SimdMask4F mask, SimdVector4F whenFalse, SimdVector4F whenTrue)
    {
        return SimdVector4F{
            ScalarHelper_SelectBranchless(mask.Inner[0], whenFalse.Inner[0], whenTrue.Inner[0]),
            ScalarHelper_SelectBranchless(mask.Inner[1], whenFalse.Inner[1], whenTrue.Inner[1]),
            ScalarHelper_SelectBranchless(mask.Inner[2], whenFalse.Inner[2], whenTrue.Inner[2]),
            ScalarHelper_SelectBranchless(mask.Inner[3], whenFalse.Inner[3], whenTrue.Inner[3]),
        };
    }

    template <bool X, bool Y, bool Z, bool W>
    inline SimdVector4F anemone_vectorcall Vector4F_Select(SimdVector4F whenFalse, SimdVector4F whenTrue)
    {
        return SimdVector4F{
            X ? whenTrue.Inner[0] : whenFalse.Inner[0],
            Y ? whenTrue.Inner[1] : whenFalse.Inner[1],
            Z ? whenTrue.Inner[2] : whenFalse.Inner[2],
            W ? whenTrue.Inner[3] : whenFalse.Inner[3],
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_MultiplyAdd(SimdVector4F a, SimdVector4F b, SimdVector4F c)
    {
        return SimdVector4F{
            (a.Inner[0] * b.Inner[0]) + c.Inner[0],
            (a.Inner[1] * b.Inner[1]) + c.Inner[1],
            (a.Inner[2] * b.Inner[2]) + c.Inner[2],
            (a.Inner[3] * b.Inner[3]) + c.Inner[3],
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_MultiplySubtract(SimdVector4F a, SimdVector4F b, SimdVector4F c)
    {
        return SimdVector4F{
            (a.Inner[0] * b.Inner[0]) - c.Inner[0],
            (a.Inner[1] * b.Inner[1]) - c.Inner[1],
            (a.Inner[2] * b.Inner[2]) - c.Inner[2],
            (a.Inner[3] * b.Inner[3]) - c.Inner[3],
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegateMultiplyAdd(SimdVector4F a, SimdVector4F b, SimdVector4F c)
    {
        return SimdVector4F{
            -(a.Inner[0] * b.Inner[0]) + c.Inner[0],
            -(a.Inner[1] * b.Inner[1]) + c.Inner[1],
            -(a.Inner[2] * b.Inner[2]) + c.Inner[2],
            -(a.Inner[3] * b.Inner[3]) + c.Inner[3],
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegateMultiplySubtract(SimdVector4F a, SimdVector4F b, SimdVector4F c)
    {
        return SimdVector4F{
            -(a.Inner[0] * b.Inner[0]) - c.Inner[0],
            -(a.Inner[1] * b.Inner[1]) - c.Inner[1],
            -(a.Inner[2] * b.Inner[2]) - c.Inner[2],
            -(a.Inner[3] * b.Inner[3]) - c.Inner[3],
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Add(SimdVector4F a, SimdVector4F b)
    {
        return SimdVector4F{
            a.Inner[0] + b.Inner[0],
            a.Inner[1] + b.Inner[1],
            a.Inner[2] + b.Inner[2],
            a.Inner[3] + b.Inner[3],
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Subtract(SimdVector4F a, SimdVector4F b)
    {
        return SimdVector4F{
            a.Inner[0] - b.Inner[0],
            a.Inner[1] - b.Inner[1],
            a.Inner[2] - b.Inner[2],
            a.Inner[3] - b.Inner[3],
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Multiply(SimdVector4F a, SimdVector4F b)
    {
        return SimdVector4F{
            a.Inner[0] * b.Inner[0],
            a.Inner[1] * b.Inner[1],
            a.Inner[2] * b.Inner[2],
            a.Inner[3] * b.Inner[3],
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Divide(SimdVector4F a, SimdVector4F b)
    {
        return SimdVector4F{
            a.Inner[0] / b.Inner[0],
            a.Inner[1] / b.Inner[1],
            a.Inner[2] / b.Inner[2],
            a.Inner[3] / b.Inner[3],
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Negate(SimdVector4F v)
    {
        return SimdVector4F{
            -v.Inner[0],
            -v.Inner[1],
            -v.Inner[2],
            -v.Inner[3],
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Reciprocal(SimdVector4F v)
    {
        return SimdVector4F{
            Reciprocal<float>(v.Inner[0]),
            Reciprocal<float>(v.Inner[1]),
            Reciprocal<float>(v.Inner[2]),
            Reciprocal<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_ReciprocalEst(SimdVector4F v)
    {
        return SimdVector4F{
            Reciprocal<float>(v.Inner[0]),
            Reciprocal<float>(v.Inner[1]),
            Reciprocal<float>(v.Inner[2]),
            Reciprocal<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Min(SimdVector4F a, SimdVector4F b)
    {
        return SimdVector4F{
            Min<float>(a.Inner[0], b.Inner[0]),
            Min<float>(a.Inner[1], b.Inner[1]),
            Min<float>(a.Inner[2], b.Inner[2]),
            Min<float>(a.Inner[3], b.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Max(SimdVector4F a, SimdVector4F b)
    {
        return SimdVector4F{
            Max<float>(a.Inner[0], b.Inner[0]),
            Max<float>(a.Inner[1], b.Inner[1]),
            Max<float>(a.Inner[2], b.Inner[2]),
            Max<float>(a.Inner[3], b.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Abs(SimdVector4F v)
    {
        return SimdVector4F{
            Abs<float>(v.Inner[0]),
            Abs<float>(v.Inner[1]),
            Abs<float>(v.Inner[2]),
            Abs<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Square(SimdVector4F v)
    {
        return SimdVector4F{
            Square<float>(v.Inner[0]),
            Square<float>(v.Inner[1]),
            Square<float>(v.Inner[2]),
            Square<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_SignedSquare(SimdVector4F v)
    {
        return SimdVector4F{
            SignedSquare<float>(v.Inner[0]),
            SignedSquare<float>(v.Inner[1]),
            SignedSquare<float>(v.Inner[2]),
            SignedSquare<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Clamp(SimdVector4F v, SimdVector4F lower, SimdVector4F upper)
    {
        return SimdVector4F{
            Clamp<float>(v.Inner[0], lower.Inner[0], upper.Inner[0]),
            Clamp<float>(v.Inner[1], lower.Inner[1], upper.Inner[1]),
            Clamp<float>(v.Inner[2], lower.Inner[2], upper.Inner[2]),
            Clamp<float>(v.Inner[3], lower.Inner[3], upper.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Saturate(SimdVector4F v)
    {
        return SimdVector4F{
            Saturate<float>(v.Inner[0]),
            Saturate<float>(v.Inner[1]),
            Saturate<float>(v.Inner[2]),
            Saturate<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_SquareRoot(SimdVector4F v)
    {
        return SimdVector4F{
            Sqrt<float>(v.Inner[0]),
            Sqrt<float>(v.Inner[1]),
            Sqrt<float>(v.Inner[2]),
            Sqrt<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_SquareRootEst(SimdVector4F v)
    {
        return SimdVector4F{
            SqrtEst<float>(v.Inner[0]),
            SqrtEst<float>(v.Inner[1]),
            SqrtEst<float>(v.Inner[2]),
            SqrtEst<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_ReciprocalSquareRoot(SimdVector4F v)
    {
        return SimdVector4F{
            ReciprocalSqrt<float>(v.Inner[0]),
            ReciprocalSqrt<float>(v.Inner[1]),
            ReciprocalSqrt<float>(v.Inner[2]),
            ReciprocalSqrt<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_ReciprocalSquareRootEst(SimdVector4F v)
    {
        return SimdVector4F{
            ReciprocalSqrtEst<float>(v.Inner[0]),
            ReciprocalSqrtEst<float>(v.Inner[1]),
            ReciprocalSqrtEst<float>(v.Inner[2]),
            ReciprocalSqrtEst<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Ceil(SimdVector4F v)
    {
        return SimdVector4F{
            Ceil<float>(v.Inner[0]),
            Ceil<float>(v.Inner[1]),
            Ceil<float>(v.Inner[2]),
            Ceil<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Floor(SimdVector4F v)
    {
        return SimdVector4F{
            Floor<float>(v.Inner[0]),
            Floor<float>(v.Inner[1]),
            Floor<float>(v.Inner[2]),
            Floor<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Truncate(SimdVector4F v)
    {
        return SimdVector4F{
            Truncate<float>(v.Inner[0]),
            Truncate<float>(v.Inner[1]),
            Truncate<float>(v.Inner[2]),
            Truncate<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Round(SimdVector4F v)
    {
        return SimdVector4F{
            Round<float>(v.Inner[0]),
            Round<float>(v.Inner[1]),
            Round<float>(v.Inner[2]),
            Round<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Fraction(SimdVector4F v)
    {
        return SimdVector4F{
            Fraction<float>(v.Inner[0]),
            Fraction<float>(v.Inner[1]),
            Fraction<float>(v.Inner[2]),
            Fraction<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Remainder(SimdVector4F x, SimdVector4F y)
    {
        return SimdVector4F{
            Remainder<float>(x.Inner[0], y.Inner[0]),
            Remainder<float>(x.Inner[1], y.Inner[1]),
            Remainder<float>(x.Inner[2], y.Inner[2]),
            Remainder<float>(x.Inner[3], y.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Repeat(SimdVector4F v, SimdVector4F length)
    {
        return SimdVector4F{
            Repeat<float>(v.Inner[0], length.Inner[0]),
            Repeat<float>(v.Inner[1], length.Inner[1]),
            Repeat<float>(v.Inner[2], length.Inner[2]),
            Repeat<float>(v.Inner[3], length.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Wrap(SimdVector4F v, SimdVector4F lower, SimdVector4F upper)
    {
        return SimdVector4F{
            Wrap<float>(v.Inner[0], lower.Inner[0], upper.Inner[0]),
            Wrap<float>(v.Inner[1], lower.Inner[1], upper.Inner[1]),
            Wrap<float>(v.Inner[2], lower.Inner[2], upper.Inner[2]),
            Wrap<float>(v.Inner[3], lower.Inner[3], upper.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Power(SimdVector4F x, SimdVector4F y)
    {
        return SimdVector4F{
            Power<float>(x.Inner[0], y.Inner[0]),
            Power<float>(x.Inner[1], y.Inner[1]),
            Power<float>(x.Inner[2], y.Inner[2]),
            Power<float>(x.Inner[3], y.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Exp(SimdVector4F v)
    {
        return SimdVector4F{
            Exp<float>(v.Inner[0]),
            Exp<float>(v.Inner[1]),
            Exp<float>(v.Inner[2]),
            Exp<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Exp2(SimdVector4F v)
    {
        return SimdVector4F{
            Exp2<float>(v.Inner[0]),
            Exp2<float>(v.Inner[1]),
            Exp2<float>(v.Inner[2]),
            Exp2<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Exp10(SimdVector4F v)
    {
        return SimdVector4F{
            Exp10<float>(v.Inner[0]),
            Exp10<float>(v.Inner[1]),
            Exp10<float>(v.Inner[2]),
            Exp10<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Log(SimdVector4F v)
    {
        return SimdVector4F{
            Log<float>(v.Inner[0]),
            Log<float>(v.Inner[1]),
            Log<float>(v.Inner[2]),
            Log<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Log(SimdVector4F x, SimdVector4F base)
    {
        return SimdVector4F{
            Log<float>(x.Inner[0], base.Inner[0]),
            Log<float>(x.Inner[1], base.Inner[1]),
            Log<float>(x.Inner[2], base.Inner[2]),
            Log<float>(x.Inner[3], base.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Log2(SimdVector4F v)
    {
        return SimdVector4F{
            Log2<float>(v.Inner[0]),
            Log2<float>(v.Inner[1]),
            Log2<float>(v.Inner[2]),
            Log2<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Log10(SimdVector4F v)
    {
        return SimdVector4F{
            Log10<float>(v.Inner[0]),
            Log10<float>(v.Inner[1]),
            Log10<float>(v.Inner[2]),
            Log10<float>(v.Inner[3]),
        };
    }

    inline void anemone_vectorcall Vector4F_SinCos(SimdVector4F& sin, SimdVector4F& cos, SimdVector4F v)
    {
        sin = Vector4F_Sin(v);
        cos = Vector4F_Cos(v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Sin(SimdVector4F v)
    {
        return SimdVector4F{
            Sin<float>(v.Inner[0]),
            Sin<float>(v.Inner[1]),
            Sin<float>(v.Inner[2]),
            Sin<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Cos(SimdVector4F v)
    {
        return SimdVector4F{
            Cos<float>(v.Inner[0]),
            Cos<float>(v.Inner[1]),
            Cos<float>(v.Inner[2]),
            Cos<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Tan(SimdVector4F v)
    {
        return SimdVector4F{
            Tan<float>(v.Inner[0]),
            Tan<float>(v.Inner[1]),
            Tan<float>(v.Inner[2]),
            Tan<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Asin(SimdVector4F v)
    {
        return SimdVector4F{
            Asin<float>(v.Inner[0]),
            Asin<float>(v.Inner[1]),
            Asin<float>(v.Inner[2]),
            Asin<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Acos(SimdVector4F v)
    {
        return SimdVector4F{
            Acos<float>(v.Inner[0]),
            Acos<float>(v.Inner[1]),
            Acos<float>(v.Inner[2]),
            Acos<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Atan(SimdVector4F v)
    {
        return SimdVector4F{
            Atan<float>(v.Inner[0]),
            Atan<float>(v.Inner[1]),
            Atan<float>(v.Inner[2]),
            Atan<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Atan2(SimdVector4F y, SimdVector4F x)
    {
        return SimdVector4F{
            Atan2<float>(y.Inner[0], x.Inner[0]),
            Atan2<float>(y.Inner[1], x.Inner[1]),
            Atan2<float>(y.Inner[2], x.Inner[2]),
            Atan2<float>(y.Inner[3], x.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PreciseLerp(SimdVector4F a, SimdVector4F b, SimdVector4F t)
    {
        return SimdVector4F{
            PreciseLerp<float>(a.Inner[0], b.Inner[0], t.Inner[0]),
            PreciseLerp<float>(a.Inner[1], b.Inner[1], t.Inner[1]),
            PreciseLerp<float>(a.Inner[2], b.Inner[2], t.Inner[2]),
            PreciseLerp<float>(a.Inner[3], b.Inner[3], t.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PreciseLerp(SimdVector4F a, SimdVector4F b, float t)
    {
        return SimdVector4F{
            PreciseLerp<float>(a.Inner[0], b.Inner[0], t),
            PreciseLerp<float>(a.Inner[1], b.Inner[1], t),
            PreciseLerp<float>(a.Inner[2], b.Inner[2], t),
            PreciseLerp<float>(a.Inner[3], b.Inner[3], t),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Lerp(SimdVector4F a, SimdVector4F b, SimdVector4F t)
    {
        return SimdVector4F{
            Lerp<float>(a.Inner[0], b.Inner[0], t.Inner[0]),
            Lerp<float>(a.Inner[1], b.Inner[1], t.Inner[1]),
            Lerp<float>(a.Inner[2], b.Inner[2], t.Inner[2]),
            Lerp<float>(a.Inner[3], b.Inner[3], t.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Lerp(SimdVector4F a, SimdVector4F b, float t)
    {
        return SimdVector4F{
            Lerp<float>(a.Inner[0], b.Inner[0], t),
            Lerp<float>(a.Inner[1], b.Inner[1], t),
            Lerp<float>(a.Inner[2], b.Inner[2], t),
            Lerp<float>(a.Inner[3], b.Inner[3], t),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PreciseBilinearLerp(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F d, SimdVector4F u, SimdVector4F v)
    {
        return SimdVector4F{
            PreciseBilinearLerp<float>(a.Inner[0], b.Inner[0], c.Inner[0], d.Inner[0], u.Inner[0], v.Inner[0]),
            PreciseBilinearLerp<float>(a.Inner[1], b.Inner[1], c.Inner[1], d.Inner[1], u.Inner[1], v.Inner[1]),
            PreciseBilinearLerp<float>(a.Inner[2], b.Inner[2], c.Inner[2], d.Inner[2], u.Inner[2], v.Inner[2]),
            PreciseBilinearLerp<float>(a.Inner[3], b.Inner[3], c.Inner[3], d.Inner[3], u.Inner[3], v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PreciseBilinearLerp(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F d, float u, float v)
    {
        return SimdVector4F{
            PreciseBilinearLerp<float>(a.Inner[0], b.Inner[0], c.Inner[0], d.Inner[0], u, v),
            PreciseBilinearLerp<float>(a.Inner[1], b.Inner[1], c.Inner[1], d.Inner[1], u, v),
            PreciseBilinearLerp<float>(a.Inner[2], b.Inner[2], c.Inner[2], d.Inner[2], u, v),
            PreciseBilinearLerp<float>(a.Inner[3], b.Inner[3], c.Inner[3], d.Inner[3], u, v),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BilinearLerp(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F d, SimdVector4F u, SimdVector4F v)
    {
        return SimdVector4F{
            BilinearLerp<float>(a.Inner[0], b.Inner[0], c.Inner[0], d.Inner[0], u.Inner[0], v.Inner[0]),
            BilinearLerp<float>(a.Inner[1], b.Inner[1], c.Inner[1], d.Inner[1], u.Inner[1], v.Inner[1]),
            BilinearLerp<float>(a.Inner[2], b.Inner[2], c.Inner[2], d.Inner[2], u.Inner[2], v.Inner[2]),
            BilinearLerp<float>(a.Inner[3], b.Inner[3], c.Inner[3], d.Inner[3], u.Inner[3], v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BilinearLerp(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F d, float u, float v)
    {
        return SimdVector4F{
            BilinearLerp<float>(a.Inner[0], b.Inner[0], c.Inner[0], d.Inner[0], u, v),
            BilinearLerp<float>(a.Inner[1], b.Inner[1], c.Inner[1], d.Inner[1], u, v),
            BilinearLerp<float>(a.Inner[2], b.Inner[2], c.Inner[2], d.Inner[2], u, v),
            BilinearLerp<float>(a.Inner[3], b.Inner[3], c.Inner[3], d.Inner[3], u, v),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Barycentric(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F u, SimdVector4F v)
    {
        return SimdVector4F{
            Barycentric<float>(a.Inner[0], b.Inner[0], c.Inner[0], u.Inner[0], v.Inner[0]),
            Barycentric<float>(a.Inner[1], b.Inner[1], c.Inner[1], u.Inner[1], v.Inner[1]),
            Barycentric<float>(a.Inner[2], b.Inner[2], c.Inner[2], u.Inner[2], v.Inner[2]),
            Barycentric<float>(a.Inner[3], b.Inner[3], c.Inner[3], u.Inner[3], v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Barycentric(SimdVector4F a, SimdVector4F b, SimdVector4F c, float u, float v)
    {
        return SimdVector4F{
            Barycentric<float>(a.Inner[0], b.Inner[0], c.Inner[0], u, v),
            Barycentric<float>(a.Inner[1], b.Inner[1], c.Inner[1], u, v),
            Barycentric<float>(a.Inner[2], b.Inner[2], c.Inner[2], u, v),
            Barycentric<float>(a.Inner[3], b.Inner[3], c.Inner[3], u, v),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Unlerp(SimdVector4F value, SimdVector4F lower, SimdVector4F upper)
    {
        return SimdVector4F{
            Unlerp<float>(value.Inner[0], lower.Inner[0], upper.Inner[0]),
            Unlerp<float>(value.Inner[1], lower.Inner[1], upper.Inner[1]),
            Unlerp<float>(value.Inner[2], lower.Inner[2], upper.Inner[2]),
            Unlerp<float>(value.Inner[3], lower.Inner[3], upper.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Map(SimdVector4F value, SimdVector4F sourceLower, SimdVector4F sourceUpper, SimdVector4F targetLower, SimdVector4F targetUpper)
    {
        return SimdVector4F{
            Map<float>(value.Inner[0], sourceLower.Inner[0], sourceUpper.Inner[0], targetLower.Inner[0], targetUpper.Inner[0]),
            Map<float>(value.Inner[1], sourceLower.Inner[1], sourceUpper.Inner[1], targetLower.Inner[1], targetUpper.Inner[1]),
            Map<float>(value.Inner[2], sourceLower.Inner[2], sourceUpper.Inner[2], targetLower.Inner[2], targetUpper.Inner[2]),
            Map<float>(value.Inner[3], sourceLower.Inner[3], sourceUpper.Inner[3], targetLower.Inner[3], targetUpper.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PreciseMap(SimdVector4F value, SimdVector4F sourceLower, SimdVector4F sourceUpper, SimdVector4F targetLower, SimdVector4F targetUpper)
    {
        return SimdVector4F{
            PreciseMap<float>(value.Inner[0], sourceLower.Inner[0], sourceUpper.Inner[0], targetLower.Inner[0], targetUpper.Inner[0]),
            PreciseMap<float>(value.Inner[1], sourceLower.Inner[1], sourceUpper.Inner[1], targetLower.Inner[1], targetUpper.Inner[1]),
            PreciseMap<float>(value.Inner[2], sourceLower.Inner[2], sourceUpper.Inner[2], targetLower.Inner[2], targetUpper.Inner[2]),
            PreciseMap<float>(value.Inner[3], sourceLower.Inner[3], sourceUpper.Inner[3], targetLower.Inner[3], targetUpper.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierPosition(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F t)
    {
        return SimdVector4F{
            BezierPosition<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], t.Inner[0]),
            BezierPosition<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], t.Inner[1]),
            BezierPosition<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], t.Inner[2]),
            BezierPosition<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], t.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierPosition(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, float t)
    {
        return SimdVector4F{
            BezierPosition<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], t),
            BezierPosition<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], t),
            BezierPosition<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], t),
            BezierPosition<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], t),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierTangent(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F t)
    {
        return SimdVector4F{
            BezierTangent<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], t.Inner[0]),
            BezierTangent<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], t.Inner[1]),
            BezierTangent<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], t.Inner[2]),
            BezierTangent<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], t.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierTangent(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, float t)
    {
        return SimdVector4F{
            BezierTangent<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], t),
            BezierTangent<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], t),
            BezierTangent<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], t),
            BezierTangent<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], t),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierAcceleration(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F t)
    {
        return SimdVector4F{
            BezierAcceleration<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], t.Inner[0]),
            BezierAcceleration<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], t.Inner[1]),
            BezierAcceleration<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], t.Inner[2]),
            BezierAcceleration<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], t.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierAcceleration(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, float t)
    {
        return SimdVector4F{
            BezierAcceleration<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], t),
            BezierAcceleration<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], t),
            BezierAcceleration<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], t),
            BezierAcceleration<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], t),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierPosition(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, SimdVector4F t)
    {
        return SimdVector4F{
            BezierPosition<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], p3.Inner[0], t.Inner[0]),
            BezierPosition<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], p3.Inner[1], t.Inner[1]),
            BezierPosition<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], p3.Inner[2], t.Inner[2]),
            BezierPosition<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], p3.Inner[3], t.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierPosition(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, float t)
    {
        return SimdVector4F{
            BezierPosition<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], p3.Inner[0], t),
            BezierPosition<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], p3.Inner[1], t),
            BezierPosition<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], p3.Inner[2], t),
            BezierPosition<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], p3.Inner[3], t),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierTangent(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, SimdVector4F t)
    {
        return SimdVector4F{
            BezierTangent<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], p3.Inner[0], t.Inner[0]),
            BezierTangent<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], p3.Inner[1], t.Inner[1]),
            BezierTangent<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], p3.Inner[2], t.Inner[2]),
            BezierTangent<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], p3.Inner[3], t.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierTangent(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, float t)
    {
        return SimdVector4F{
            BezierTangent<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], p3.Inner[0], t),
            BezierTangent<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], p3.Inner[1], t),
            BezierTangent<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], p3.Inner[2], t),
            BezierTangent<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], p3.Inner[3], t),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierAcceleration(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, SimdVector4F t)
    {
        return SimdVector4F{
            BezierAcceleration<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], p3.Inner[0], t.Inner[0]),
            BezierAcceleration<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], p3.Inner[1], t.Inner[1]),
            BezierAcceleration<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], p3.Inner[2], t.Inner[2]),
            BezierAcceleration<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], p3.Inner[3], t.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierAcceleration(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, float t)
    {
        return SimdVector4F{
            BezierAcceleration<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], p3.Inner[0], t),
            BezierAcceleration<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], p3.Inner[1], t),
            BezierAcceleration<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], p3.Inner[2], t),
            BezierAcceleration<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], p3.Inner[3], t),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Hermite(SimdVector4F p0, SimdVector4F m0, SimdVector4F p1, SimdVector4F m1, SimdVector4F t)
    {
        return SimdVector4F{
            Hermite<float>(p0.Inner[0], m0.Inner[0], p1.Inner[0], m1.Inner[0], t.Inner[0]),
            Hermite<float>(p0.Inner[1], m0.Inner[1], p1.Inner[1], m1.Inner[1], t.Inner[1]),
            Hermite<float>(p0.Inner[2], m0.Inner[2], p1.Inner[2], m1.Inner[2], t.Inner[2]),
            Hermite<float>(p0.Inner[3], m0.Inner[3], p1.Inner[3], m1.Inner[3], t.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Hermite(SimdVector4F p0, SimdVector4F m0, SimdVector4F p1, SimdVector4F m1, float t)
    {
        return SimdVector4F{
            Hermite<float>(p0.Inner[0], m0.Inner[0], p1.Inner[0], m1.Inner[0], t),
            Hermite<float>(p0.Inner[1], m0.Inner[1], p1.Inner[1], m1.Inner[1], t),
            Hermite<float>(p0.Inner[2], m0.Inner[2], p1.Inner[2], m1.Inner[2], t),
            Hermite<float>(p0.Inner[3], m0.Inner[3], p1.Inner[3], m1.Inner[3], t),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_CatmullRom(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, SimdVector4F t)
    {
        return SimdVector4F{
            CatmullRom<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], p3.Inner[0], t.Inner[0]),
            CatmullRom<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], p3.Inner[1], t.Inner[1]),
            CatmullRom<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], p3.Inner[2], t.Inner[2]),
            CatmullRom<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], p3.Inner[3], t.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_CatmullRom(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, float t)
    {
        return SimdVector4F{
            CatmullRom<float>(p0.Inner[0], p1.Inner[0], p2.Inner[0], p3.Inner[0], t),
            CatmullRom<float>(p0.Inner[1], p1.Inner[1], p2.Inner[1], p3.Inner[1], t),
            CatmullRom<float>(p0.Inner[2], p1.Inner[2], p2.Inner[2], p3.Inner[2], t),
            CatmullRom<float>(p0.Inner[3], p1.Inner[3], p2.Inner[3], p3.Inner[3], t),
        };
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
        return SimdVector4F{
            UnwindRadians<float>(v.Inner[0]),
            UnwindRadians<float>(v.Inner[1]),
            UnwindRadians<float>(v.Inner[2]),
            UnwindRadians<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_UnwindDegrees(SimdVector4F v)
    {
        return SimdVector4F{
            UnwindDegrees<float>(v.Inner[0]),
            UnwindDegrees<float>(v.Inner[1]),
            UnwindDegrees<float>(v.Inner[2]),
            UnwindDegrees<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_AngleDifferenceRadians(SimdVector4F a, SimdVector4F b)
    {
        return SimdVector4F{
            AngleDifferenceRadians<float>(a.Inner[0], b.Inner[0]),
            AngleDifferenceRadians<float>(a.Inner[1], b.Inner[1]),
            AngleDifferenceRadians<float>(a.Inner[2], b.Inner[2]),
            AngleDifferenceRadians<float>(a.Inner[3], b.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_AngleDifferenceDegrees(SimdVector4F a, SimdVector4F b)
    {
        return SimdVector4F{
            AngleDifferenceDegrees<float>(a.Inner[0], b.Inner[0]),
            AngleDifferenceDegrees<float>(a.Inner[1], b.Inner[1]),
            AngleDifferenceDegrees<float>(a.Inner[2], b.Inner[2]),
            AngleDifferenceDegrees<float>(a.Inner[3], b.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_RevolutionsToDegrees(SimdVector4F v)
    {
        return SimdVector4F{
            RevolutionsToDegrees<float>(v.Inner[0]),
            RevolutionsToDegrees<float>(v.Inner[1]),
            RevolutionsToDegrees<float>(v.Inner[2]),
            RevolutionsToDegrees<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_RevolutionsToRadians(SimdVector4F v)
    {
        return SimdVector4F{
            RevolutionsToRadians<float>(v.Inner[0]),
            RevolutionsToRadians<float>(v.Inner[1]),
            RevolutionsToRadians<float>(v.Inner[2]),
            RevolutionsToRadians<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_DegreesToRevolutions(SimdVector4F v)
    {
        return SimdVector4F{
            DegreesToRevolutions<float>(v.Inner[0]),
            DegreesToRevolutions<float>(v.Inner[1]),
            DegreesToRevolutions<float>(v.Inner[2]),
            DegreesToRevolutions<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_RadiansToRevolutions(SimdVector4F v)
    {
        return SimdVector4F{
            RadiansToRevolutions<float>(v.Inner[0]),
            RadiansToRevolutions<float>(v.Inner[1]),
            RadiansToRevolutions<float>(v.Inner[2]),
            RadiansToRevolutions<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_RadiansToDegrees(SimdVector4F v)
    {
        return SimdVector4F{
            RadiansToDegrees<float>(v.Inner[0]),
            RadiansToDegrees<float>(v.Inner[1]),
            RadiansToDegrees<float>(v.Inner[2]),
            RadiansToDegrees<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_DegreesToRadians(SimdVector4F v)
    {
        return SimdVector4F{
            DegreesToRadians<float>(v.Inner[0]),
            DegreesToRadians<float>(v.Inner[1]),
            DegreesToRadians<float>(v.Inner[2]),
            DegreesToRadians<float>(v.Inner[3]),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Dot4(SimdVector4F a, SimdVector4F b)
    {
        float const r = (a.Inner[0] * b.Inner[0]) + (a.Inner[1] * b.Inner[1]) + (a.Inner[2] * b.Inner[2]) + (a.Inner[3] * b.Inner[3]);
        return SimdVector4F{r, r, r, r};
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Dot3(SimdVector4F a, SimdVector4F b)
    {
        float const r = (a.Inner[0] * b.Inner[0]) + (a.Inner[1] * b.Inner[1]) + (a.Inner[2] * b.Inner[2]);
        return SimdVector4F{r, r, r, r};
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Dot2(SimdVector4F a, SimdVector4F b)
    {
        float const r = (a.Inner[0] * b.Inner[0]) + (a.Inner[1] * b.Inner[1]);
        return SimdVector4F{r, r, r, r};
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Cross4(SimdVector4F a, SimdVector4F b, SimdVector4F c)
    {
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
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Cross3(SimdVector4F a, SimdVector4F b)
    {
        float const rx = (a.Inner[1] * b.Inner[2]) - (a.Inner[2] * b.Inner[1]);
        float const ry = (a.Inner[2] * b.Inner[0]) - (a.Inner[0] * b.Inner[2]);
        float const rz = (a.Inner[0] * b.Inner[1]) - (a.Inner[1] * b.Inner[0]);

        return SimdVector4F{
            rx,
            ry,
            rz,
            0.0f,
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Cross2(SimdVector4F a, SimdVector4F b)
    {
        float const r = (a.Inner[0] * b.Inner[1]) - (a.Inner[1] * b.Inner[0]);

        return SimdVector4F{
            r,
            r,
            0.0f,
            0.0f,
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Wedge3(SimdVector4F a, SimdVector4F b)
    {
        return SimdVector4F{
            a.Inner[0] * b.Inner[1] - a.Inner[1] * b.Inner[0], // XY
            a.Inner[0] * b.Inner[2] - a.Inner[2] * b.Inner[0], // XZ
            a.Inner[1] * b.Inner[2] - a.Inner[2] * b.Inner[1], // YZ
        };
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
        float const lengthSquared = (v.Inner[0] * v.Inner[0]) + (v.Inner[1] * v.Inner[1]) + (v.Inner[2] * v.Inner[2]) + (v.Inner[3] * v.Inner[3]);
        float const length = Sqrt<float>(lengthSquared);

        return SimdVector4F{
            length,
            length,
            length,
            length,
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Length3(SimdVector4F v)
    {
        float const lengthSquared = (v.Inner[0] * v.Inner[0]) + (v.Inner[1] * v.Inner[1]) + (v.Inner[2] * v.Inner[2]);
        float const length = Sqrt<float>(lengthSquared);

        return SimdVector4F{
            length,
            length,
            length,
            length,
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Length2(SimdVector4F v)
    {
        float const lengthSquared = (v.Inner[0] * v.Inner[0]) + (v.Inner[1] * v.Inner[1]);
        float const length = Sqrt<float>(lengthSquared);

        return SimdVector4F{
            length,
            length,
            length,
            length,
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Normalize4(SimdVector4F v)
    {
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
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Normalize3(SimdVector4F v)
    {
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
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Normalize2(SimdVector4F v)
    {
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
        return Vector4F_Refract4(incident, normal, SimdVector4F{index, index, index, index});
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Refract3(SimdVector4F incident, SimdVector4F normal, float index)
    {
        return Vector4F_Refract3(incident, normal, SimdVector4F{index, index, index, index});
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Refract2(SimdVector4F incident, SimdVector4F normal, float index)
    {
        return Vector4F_Refract2(incident, normal, SimdVector4F{index, index, index, index});
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Transform3(SimdVector4F v, SimdMatrix4x4F m)
    {
        return SimdVector4F{
            (v.Inner[0] * m.Inner[0].Inner[0]) + (v.Inner[1] * m.Inner[1].Inner[0]) + (v.Inner[2] * m.Inner[2].Inner[0] + m.Inner[3].Inner[0]),
            (v.Inner[0] * m.Inner[0].Inner[1]) + (v.Inner[1] * m.Inner[1].Inner[1]) + (v.Inner[2] * m.Inner[2].Inner[1] + m.Inner[3].Inner[1]),
            (v.Inner[0] * m.Inner[0].Inner[2]) + (v.Inner[1] * m.Inner[1].Inner[2]) + (v.Inner[2] * m.Inner[2].Inner[2] + m.Inner[3].Inner[2]),
            (v.Inner[0] * m.Inner[0].Inner[3]) + (v.Inner[1] * m.Inner[1].Inner[3]) + (v.Inner[2] * m.Inner[2].Inner[3] + m.Inner[3].Inner[3]),
        };
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareEqual(SimdVector4F a, SimdVector4F b)
    {
        return SimdMask4F{
            (a.Inner[0] == b.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[1] == b.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[2] == b.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[3] == b.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
    }

    inline bool anemone_vectorcall Vector4F_IsEqual4(SimdVector4F a, SimdVector4F b)
    {
        return (a.Inner[0] == b.Inner[0]) && (a.Inner[1] == b.Inner[1]) && (a.Inner[2] == b.Inner[2]) && (a.Inner[3] == b.Inner[3]);
    }

    inline bool anemone_vectorcall Vector4F_IsEqual3(SimdVector4F a, SimdVector4F b)
    {
        return (a.Inner[0] == b.Inner[0]) && (a.Inner[1] == b.Inner[1]) && (a.Inner[2] == b.Inner[2]);
    }

    inline bool anemone_vectorcall Vector4F_IsEqual2(SimdVector4F a, SimdVector4F b)
    {
        return (a.Inner[0] == b.Inner[0]) && (a.Inner[1] == b.Inner[1]);
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNotEqual(SimdVector4F a, SimdVector4F b)
    {
        return SimdMask4F{
            (a.Inner[0] != b.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[1] != b.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[2] != b.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[3] != b.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
    }

    inline bool anemone_vectorcall Vector4F_IsNotEqual4(SimdVector4F a, SimdVector4F b)
    {
        return (a.Inner[0] != b.Inner[0]) || (a.Inner[1] != b.Inner[1]) || (a.Inner[2] != b.Inner[2]) || (a.Inner[3] != b.Inner[3]);
    }

    inline bool anemone_vectorcall Vector4F_IsNotEqual3(SimdVector4F a, SimdVector4F b)
    {
        return (a.Inner[0] != b.Inner[0]) || (a.Inner[1] != b.Inner[1]) || (a.Inner[2] != b.Inner[2]);
    }

    inline bool anemone_vectorcall Vector4F_IsNotEqual2(SimdVector4F a, SimdVector4F b)
    {
        return (a.Inner[0] != b.Inner[0]) || (a.Inner[1] != b.Inner[1]);
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareLessThan(SimdVector4F a, SimdVector4F b)
    {
        return SimdMask4F{
            (a.Inner[0] < b.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[1] < b.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[2] < b.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[3] < b.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
    }

    inline bool anemone_vectorcall Vector4F_IsLessThan4(SimdVector4F a, SimdVector4F b)
    {
        return (a.Inner[0] < b.Inner[0]) && (a.Inner[1] < b.Inner[1]) && (a.Inner[2] < b.Inner[2]) && (a.Inner[3] < b.Inner[3]);
    }

    inline bool anemone_vectorcall Vector4F_IsLessThan3(SimdVector4F a, SimdVector4F b)
    {
        return (a.Inner[0] < b.Inner[0]) && (a.Inner[1] < b.Inner[1]) && (a.Inner[2] < b.Inner[2]);
    }

    inline bool anemone_vectorcall Vector4F_IsLessThan2(SimdVector4F a, SimdVector4F b)
    {
        return (a.Inner[0] < b.Inner[0]) && (a.Inner[1] < b.Inner[1]);
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareLessEqual(SimdVector4F a, SimdVector4F b)
    {
        return SimdMask4F{
            (a.Inner[0] <= b.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[1] <= b.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[2] <= b.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[3] <= b.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
    }

    inline bool anemone_vectorcall Vector4F_IsLessEqual4(SimdVector4F a, SimdVector4F b)
    {
        return (a.Inner[0] <= b.Inner[0]) && (a.Inner[1] <= b.Inner[1]) && (a.Inner[2] <= b.Inner[2]) && (a.Inner[3] <= b.Inner[3]);
    }

    inline bool anemone_vectorcall Vector4F_IsLessEqual3(SimdVector4F a, SimdVector4F b)
    {
        return (a.Inner[0] <= b.Inner[0]) && (a.Inner[1] <= b.Inner[1]) && (a.Inner[2] <= b.Inner[2]);
    }

    inline bool anemone_vectorcall Vector4F_IsLessEqual2(SimdVector4F a, SimdVector4F b)
    {
        return (a.Inner[0] <= b.Inner[0]) && (a.Inner[1] <= b.Inner[1]);
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareGreaterThan(SimdVector4F a, SimdVector4F b)
    {
        return SimdMask4F{
            (a.Inner[0] > b.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[1] > b.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[2] > b.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[3] > b.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
    }

    inline bool anemone_vectorcall Vector4F_IsGreaterThan4(SimdVector4F a, SimdVector4F b)
    {
        return (a.Inner[0] > b.Inner[0]) && (a.Inner[1] > b.Inner[1]) && (a.Inner[2] > b.Inner[2]) && (a.Inner[3] > b.Inner[3]);
    }

    inline bool anemone_vectorcall Vector4F_IsGreaterThan3(SimdVector4F a, SimdVector4F b)
    {
        return (a.Inner[0] > b.Inner[0]) && (a.Inner[1] > b.Inner[1]) && (a.Inner[2] > b.Inner[2]);
    }

    inline bool anemone_vectorcall Vector4F_IsGreaterThan2(SimdVector4F a, SimdVector4F b)
    {
        return (a.Inner[0] > b.Inner[0]) && (a.Inner[1] > b.Inner[1]);
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareGreaterEqual(SimdVector4F a, SimdVector4F b)
    {
        return SimdMask4F{
            (a.Inner[0] >= b.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[1] >= b.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[2] >= b.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            (a.Inner[3] >= b.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
    }

    inline bool anemone_vectorcall Vector4F_IsGreaterEqual4(SimdVector4F a, SimdVector4F b)
    {
        return (a.Inner[0] >= b.Inner[0]) && (a.Inner[1] >= b.Inner[1]) && (a.Inner[2] >= b.Inner[2]) && (a.Inner[3] >= b.Inner[3]);
    }

    inline bool anemone_vectorcall Vector4F_IsGreaterEqual3(SimdVector4F a, SimdVector4F b)
    {
        return (a.Inner[0] >= b.Inner[0]) && (a.Inner[1] >= b.Inner[1]) && (a.Inner[2] >= b.Inner[2]);
    }

    inline bool anemone_vectorcall Vector4F_IsGreaterEqual2(SimdVector4F a, SimdVector4F b)
    {
        return (a.Inner[0] >= b.Inner[0]) && (a.Inner[1] >= b.Inner[1]);
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNaN(SimdVector4F v)
    {
        return SimdMask4F{
            IsNaN<float>(v.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            IsNaN<float>(v.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            IsNaN<float>(v.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            IsNaN<float>(v.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
    }

    inline bool anemone_vectorcall Vector4F_IsNaN4(SimdVector4F v)
    {
        return IsNaN<float>(v.Inner[0]) && IsNaN<float>(v.Inner[1]) && IsNaN<float>(v.Inner[2]) && IsNaN<float>(v.Inner[3]);
    }

    inline bool anemone_vectorcall Vector4F_IsNaN3(SimdVector4F v)
    {
        return IsNaN<float>(v.Inner[0]) && IsNaN<float>(v.Inner[1]) && IsNaN<float>(v.Inner[2]);
    }

    inline bool anemone_vectorcall Vector4F_IsNaN2(SimdVector4F v)
    {
        return IsNaN<float>(v.Inner[0]) && IsNaN<float>(v.Inner[1]);
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareInfinite(SimdVector4F v)
    {
        return SimdMask4F{
            IsInfinite<float>(v.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            IsInfinite<float>(v.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            IsInfinite<float>(v.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            IsInfinite<float>(v.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
    }

    inline bool anemone_vectorcall Vector4F_IsInfinite4(SimdVector4F v)
    {
        return IsInfinite<float>(v.Inner[0]) && IsInfinite<float>(v.Inner[1]) && IsInfinite<float>(v.Inner[2]) && IsInfinite<float>(v.Inner[3]);
    }

    inline bool anemone_vectorcall Vector4F_IsInfinite3(SimdVector4F v)
    {
        return IsInfinite<float>(v.Inner[0]) && IsInfinite<float>(v.Inner[1]) && IsInfinite<float>(v.Inner[2]);
    }

    inline bool anemone_vectorcall Vector4F_IsInfinite2(SimdVector4F v)
    {
        return IsInfinite<float>(v.Inner[0]) && IsInfinite<float>(v.Inner[1]);
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareInRange(SimdVector4F v, SimdVector4F lower, SimdVector4F upper)
    {
        return SimdMask4F{
            InRange<float>(v.Inner[0], lower.Inner[0], upper.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            InRange<float>(v.Inner[1], lower.Inner[1], upper.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            InRange<float>(v.Inner[2], lower.Inner[2], upper.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            InRange<float>(v.Inner[3], lower.Inner[3], upper.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
    }

    inline bool anemone_vectorcall Vector4F_InRange4(SimdVector4F v, SimdVector4F lower, SimdVector4F upper)
    {
        return InRange<float>(v.Inner[0], lower.Inner[0], upper.Inner[0]) &&
            InRange<float>(v.Inner[1], lower.Inner[1], upper.Inner[1]) &&
            InRange<float>(v.Inner[2], lower.Inner[2], upper.Inner[2]) &&
            InRange<float>(v.Inner[3], lower.Inner[3], upper.Inner[3]);
    }

    inline bool anemone_vectorcall Vector4F_InRange3(SimdVector4F v, SimdVector4F lower, SimdVector4F upper)
    {
        return InRange<float>(v.Inner[0], lower.Inner[0], upper.Inner[0]) &&
            InRange<float>(v.Inner[1], lower.Inner[1], upper.Inner[1]) &&
            InRange<float>(v.Inner[2], lower.Inner[2], upper.Inner[2]);
    }

    inline bool anemone_vectorcall Vector4F_InRange2(SimdVector4F v, SimdVector4F lower, SimdVector4F upper)
    {
        return InRange<float>(v.Inner[0], lower.Inner[0], upper.Inner[0]) &&
            InRange<float>(v.Inner[1], lower.Inner[1], upper.Inner[1]);
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearZero(SimdVector4F v, SimdVector4F tolerance)
    {
        return SimdMask4F{
            IsNearZero<float>(v.Inner[0], tolerance.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            IsNearZero<float>(v.Inner[1], tolerance.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            IsNearZero<float>(v.Inner[2], tolerance.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            IsNearZero<float>(v.Inner[3], tolerance.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearZero(SimdVector4F v, float tolerance)
    {
        return SimdMask4F{
            IsNearZero<float>(v.Inner[0], tolerance) ? ScalarMaskComponentUInt32 : 0,
            IsNearZero<float>(v.Inner[1], tolerance) ? ScalarMaskComponentUInt32 : 0,
            IsNearZero<float>(v.Inner[2], tolerance) ? ScalarMaskComponentUInt32 : 0,
            IsNearZero<float>(v.Inner[3], tolerance) ? ScalarMaskComponentUInt32 : 0,
        };
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearZero(SimdVector4F v)
    {
        return SimdMask4F{
            IsNearZero<float>(v.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            IsNearZero<float>(v.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            IsNearZero<float>(v.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            IsNearZero<float>(v.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearZeroPrecise(SimdVector4F v)
    {
        return SimdMask4F{
            IsNearZeroPrecise(v.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            IsNearZeroPrecise(v.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            IsNearZeroPrecise(v.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            IsNearZeroPrecise(v.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearEqual(SimdVector4F a, SimdVector4F b, SimdVector4F tolerance)
    {
        return SimdMask4F{
            IsNearEqual<float>(a.Inner[0], b.Inner[0], tolerance.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            IsNearEqual<float>(a.Inner[1], b.Inner[1], tolerance.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            IsNearEqual<float>(a.Inner[2], b.Inner[2], tolerance.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            IsNearEqual<float>(a.Inner[3], b.Inner[3], tolerance.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearEqual(SimdVector4F a, SimdVector4F b, float tolerance)
    {
        return SimdMask4F{
            IsNearEqual<float>(a.Inner[0], b.Inner[0], tolerance) ? ScalarMaskComponentUInt32 : 0,
            IsNearEqual<float>(a.Inner[1], b.Inner[1], tolerance) ? ScalarMaskComponentUInt32 : 0,
            IsNearEqual<float>(a.Inner[2], b.Inner[2], tolerance) ? ScalarMaskComponentUInt32 : 0,
            IsNearEqual<float>(a.Inner[3], b.Inner[3], tolerance) ? ScalarMaskComponentUInt32 : 0,
        };
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearEqual(SimdVector4F a, SimdVector4F b)
    {
        return SimdMask4F{
            IsNearEqual<float>(a.Inner[0], b.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            IsNearEqual<float>(a.Inner[1], b.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            IsNearEqual<float>(a.Inner[2], b.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            IsNearEqual<float>(a.Inner[3], b.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearEqualPrecise(SimdVector4F a, SimdVector4F b)
    {
        return SimdMask4F{
            IsNearEqualPrecise(a.Inner[0], b.Inner[0]) ? ScalarMaskComponentUInt32 : 0,
            IsNearEqualPrecise(a.Inner[1], b.Inner[1]) ? ScalarMaskComponentUInt32 : 0,
            IsNearEqualPrecise(a.Inner[2], b.Inner[2]) ? ScalarMaskComponentUInt32 : 0,
            IsNearEqualPrecise(a.Inner[3], b.Inner[3]) ? ScalarMaskComponentUInt32 : 0,
        };
    }
}

// Mask functions
namespace Anemone::Numerics::Private
{
    inline SimdMask4F anemone_vectorcall Mask4F_Create(bool x, bool y, bool z, bool w)
    {
        return SimdMask4F{
            x ? ScalarMaskComponentUInt32 : 0,
            y ? ScalarMaskComponentUInt32 : 0,
            z ? ScalarMaskComponentUInt32 : 0,
            w ? ScalarMaskComponentUInt32 : 0,
        };
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Create(bool x, bool y, bool z)
    {
        return SimdMask4F{
            x ? ScalarMaskComponentUInt32 : 0,
            y ? ScalarMaskComponentUInt32 : 0,
            z ? ScalarMaskComponentUInt32 : 0,
            0,
        };
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Create(bool x, bool y)
    {

        return SimdMask4F{
            x ? ScalarMaskComponentUInt32 : 0,
            y ? ScalarMaskComponentUInt32 : 0,
            0,
            0,
        };
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Replicate(bool value)
    {
        uint32_t const mask = value ? ScalarMaskComponentUInt32 : 0;
        return SimdMask4F{mask, mask, mask, mask};
    }

    template <size_t N>
    inline uint32_t anemone_vectorcall Mask4F_Extract(SimdMask4F v)
        requires(N < 4)
    {
        return v.Inner[N];
    }

    template <size_t N>
    SimdMask4F anemone_vectorcall Mask4F_Insert(SimdMask4F v, uint32_t b)
        requires(N < 4)
    {
        v.Inner[N] = b;
        return v;
    }

    inline SimdMask4F anemone_vectorcall Mask4F_True()
    {
        return SimdMask4F{
            ScalarMaskComponentUInt32,
            ScalarMaskComponentUInt32,
            ScalarMaskComponentUInt32,
            ScalarMaskComponentUInt32,
        };
    }

    inline SimdMask4F anemone_vectorcall Mask4F_False()
    {
        return SimdMask4F{
            0,
            0,
            0,
            0,
        };
    }

    inline SimdMask4F anemone_vectorcall Mask4F_CompareEqual(SimdMask4F a, SimdMask4F b)
    {
        return SimdMask4F{
            a.Inner[0] == b.Inner[0],
            a.Inner[1] == b.Inner[1],
            a.Inner[2] == b.Inner[2],
            a.Inner[3] == b.Inner[3],
        };
    }

    inline SimdMask4F anemone_vectorcall Mask4F_CompareNotEqual(SimdMask4F a, SimdMask4F b)
    {
        return SimdMask4F{
            a.Inner[0] != b.Inner[0],
            a.Inner[1] != b.Inner[1],
            a.Inner[2] != b.Inner[2],
            a.Inner[3] != b.Inner[3],
        };
    }

    inline SimdMask4F anemone_vectorcall Mask4F_And(SimdMask4F a, SimdMask4F b)
    {
        return SimdMask4F{
            a.Inner[0] & b.Inner[0],
            a.Inner[1] & b.Inner[1],
            a.Inner[2] & b.Inner[2],
            a.Inner[3] & b.Inner[3],
        };
    }

    inline SimdMask4F anemone_vectorcall Mask4F_AndNot(SimdMask4F a, SimdMask4F b)
    {
        return SimdMask4F{
            a.Inner[0] & ~b.Inner[0],
            a.Inner[1] & ~b.Inner[1],
            a.Inner[2] & ~b.Inner[2],
            a.Inner[3] & ~b.Inner[3],
        };
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Or(SimdMask4F a, SimdMask4F b)
    {
        return SimdMask4F{
            a.Inner[0] | b.Inner[0],
            a.Inner[1] | b.Inner[1],
            a.Inner[2] | b.Inner[2],
            a.Inner[3] | b.Inner[3],
        };
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Xor(SimdMask4F a, SimdMask4F b)
    {
        return SimdMask4F{
            a.Inner[0] ^ b.Inner[0],
            a.Inner[1] ^ b.Inner[1],
            a.Inner[2] ^ b.Inner[2],
            a.Inner[3] ^ b.Inner[3],
        };
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Not(SimdMask4F mask)
    {
        return SimdMask4F{
            ~mask.Inner[0],
            ~mask.Inner[1],
            ~mask.Inner[2],
            ~mask.Inner[3],
        };
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Select(SimdMask4F mask, SimdMask4F whenFalse, SimdMask4F whenTrue)
    {
        return SimdMask4F{
            (mask.Inner[0] & whenTrue.Inner[0]) | (~mask.Inner[0] & whenFalse.Inner[0]),
            (mask.Inner[1] & whenTrue.Inner[1]) | (~mask.Inner[1] & whenFalse.Inner[1]),
            (mask.Inner[2] & whenTrue.Inner[2]) | (~mask.Inner[2] & whenFalse.Inner[2]),
            (mask.Inner[3] & whenTrue.Inner[3]) | (~mask.Inner[3] & whenFalse.Inner[3]),
        };
    }

    template <bool X, bool Y, bool Z, bool W>
    inline SimdMask4F anemone_vectorcall Mask4F_Select(SimdMask4F whenFalse, SimdMask4F whenTrue)
    {
        return SimdMask4F{
            X ? whenTrue.Inner[0] : whenFalse.Inner[0],
            Y ? whenTrue.Inner[1] : whenFalse.Inner[1],
            Z ? whenTrue.Inner[2] : whenFalse.Inner[2],
            W ? whenTrue.Inner[3] : whenFalse.Inner[3],
        };
    }

    inline bool anemone_vectorcall Mask4F_All4(SimdMask4F mask)
    {
        uint32_t const reduced = mask.Inner[0] & mask.Inner[1] & mask.Inner[2] & mask.Inner[3];
        return reduced != 0;
    }

    inline bool anemone_vectorcall Mask4F_All3(SimdMask4F mask)
    {
        uint32_t const reduced = mask.Inner[0] & mask.Inner[1] & mask.Inner[2];
        return reduced != 0;
    }

    inline bool anemone_vectorcall Mask4F_All2(SimdMask4F mask)
    {
        uint32_t const reduced = mask.Inner[0] & mask.Inner[1];
        return reduced != 0;
    }

    inline bool anemone_vectorcall Mask4F_Any4(SimdMask4F mask)
    {
        uint32_t const reduced = mask.Inner[0] | mask.Inner[1] | mask.Inner[2] | mask.Inner[3];
        return reduced != 0;
    }

    inline bool anemone_vectorcall Mask4F_Any3(SimdMask4F mask)
    {
        uint32_t const reduced = mask.Inner[0] | mask.Inner[1] | mask.Inner[2];
        return reduced != 0;
    }

    inline bool anemone_vectorcall Mask4F_Any2(SimdMask4F mask)
    {
        uint32_t const reduced = mask.Inner[0] | mask.Inner[1];
        return reduced != 0;
    }

    inline bool anemone_vectorcall Mask4F_None4(SimdMask4F mask)
    {
        uint32_t const reduced = mask.Inner[0] | mask.Inner[1] | mask.Inner[2] | mask.Inner[3];
        return reduced == 0;
    }

    inline bool anemone_vectorcall Mask4F_None3(SimdMask4F mask)
    {
        uint32_t const reduced = mask.Inner[0] | mask.Inner[1] | mask.Inner[2];
        return reduced == 0;
    }

    inline bool anemone_vectorcall Mask4F_None2(SimdMask4F mask)
    {
        uint32_t const reduced = mask.Inner[0] | mask.Inner[1];
        return reduced == 0;
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
        return Vector4F_CompareEqual(q, QuaternionF_Identity());
    }

    inline bool anemone_vectorcall QuaternionF_IsIdentity(SimdVector4F q)
    {
        return Vector4F_IsEqual4(q, QuaternionF_Identity());
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Identity()
    {
        return SimdVector4F{
            F32x4_PositiveUnitW.Elements[0],
            F32x4_PositiveUnitW.Elements[1],
            F32x4_PositiveUnitW.Elements[2],
            F32x4_PositiveUnitW.Elements[3],
        };
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_FromNormalAngle(SimdVector4F normal, float angle)
    {
        auto [fSin, fCos] = SinCos<float>(0.5f * angle);

        return SimdVector4F{
            fSin * normal.Inner[0],
            fSin * normal.Inner[1],
            fSin * normal.Inner[2],
            fCos,
        };
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
        return QuaternionF_FromEulerAngles(pitchYawRoll.Inner[0], pitchYawRoll.Inner[1], pitchYawRoll.Inner[2]);
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_FromEulerAngles(float pitch, float yaw, float roll)
    {
        auto const [sp, cp] = SinCos<float>(pitch * 0.5f);
        auto const [sy, cy] = SinCos<float>(yaw * 0.5f);
        auto const [sr, cr] = SinCos<float>(roll * 0.5f);

        return SimdVector4F{
            cr * sp * cy + sr * cp * sy,
            cr * cp * sy - sr * sp * cy,
            sr * cp * cy - cr * sp * sy,
            cr * cp * cy + sr * sp * sy,
        };
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Rotate3(SimdVector4F q, SimdVector4F v)
    {
        // = [v.xyz, 0]
        SimdVector4F v_xyz = v;
        v_xyz.Inner[3] = 0.0f;

        // = q' * v.xyz * q
        SimdVector4F const q_conj = QuaternionF_Conjugate(q);
        return QuaternionF_Multiply(QuaternionF_Multiply(q_conj, v_xyz), q);
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_InverseRotate3(SimdVector4F q, SimdVector4F v)
    {
        // = [v.xyz, 0]
        SimdVector4F v_xyz = v;
        v_xyz.Inner[3] = 0.0f;

        // = q * v.xyz * q'
        SimdVector4F const q_conj = QuaternionF_Conjugate(q);
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
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Conjugate(SimdVector4F q)
    {
        return SimdVector4F{
            -q.Inner[0],
            -q.Inner[1],
            -q.Inner[2],
            q.Inner[3],
        };
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Inverse(SimdVector4F q)
    {
        SimdVector4F const lengthSquared = Vector4F_LengthSquared4(q);
        SimdVector4F const conj = QuaternionF_Conjugate(q);
        SimdMask4F const mask = Vector4F_CompareNearZero(lengthSquared);

        SimdVector4F result = Vector4F_Divide(conj, lengthSquared);
        result = Vector4F_Select(mask, result, Vector4F_Zero());
        return result;
    }

    inline void anemone_vectorcall QuaternionF_ToAxisAngle(SimdVector4F& axis, float& angle, SimdVector4F q)
    {
        // Axis
        axis.Inner[0] = q.Inner[0];
        axis.Inner[1] = q.Inner[1];
        axis.Inner[2] = q.Inner[2];
        axis.Inner[3] = 0.0f;

        // Angle
        angle = 2.0f * Acos<float>(q.Inner[3]);
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Nlerp(SimdVector4F from, SimdVector4F to, SimdVector4F t)
    {
        return QuaternionF_Nlerp(from, to, t.Inner[0]);
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Nlerp(SimdVector4F from, SimdVector4F to, float t)
    {
        SimdVector4F const r = Vector4F_PreciseLerp(from, to, t);
        return Vector4F_Normalize4(r);
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Slerp(SimdVector4F from, SimdVector4F to, SimdVector4F t)
    {
        return QuaternionF_Slerp(from, to, t.Inner[0]);
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Slerp(SimdVector4F from, SimdVector4F to, float t)
    {
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
    }
}

// Matrix functions
namespace Anemone::Numerics::Private
{
    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Identity()
    {
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
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_NaN()
    {
        SimdVector4F const lane = Vector4F_LoadFloat4(F32x4_PositiveQNaN_XXXX.As<float>());
        return SimdMatrix4x4F{lane, lane, lane, lane};
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Infinity()
    {
        SimdVector4F const lane = Vector4F_LoadFloat4(F32x4_PositiveInfinity_XXXX.As<float>());
        return SimdMatrix4x4F{lane, lane, lane, lane};
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateTranslation(float x, float y, float z)
    {
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
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateTranslation(SimdVector4F translation)
    {
        return Matrix4x4F_CreateTranslation(translation.Inner[0], translation.Inner[1], translation.Inner[2]);
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateScale(float x, float y, float z)
    {
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
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateScale(SimdVector4F scale)
    {
        return Matrix4x4F_CreateScale(scale.Inner[0], scale.Inner[1], scale.Inner[2]);
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateRotationX(float angle)
    {
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
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateRotationY(float angle)
    {
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
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateRotationZ(float angle)
    {
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
    }

    anemone_noinline inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateFromQuaternion(SimdVector4F q)
    {
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
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateFromPitchYawRoll(float pitch, float yaw, float roll)
    {
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
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateFromPitchYawRoll(SimdVector4F pitchYawRoll)
    {
        return Matrix4x4F_CreateFromPitchYawRoll(pitchYawRoll.Inner[0], pitchYawRoll.Inner[1], pitchYawRoll.Inner[2]);
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateAffineTransform2(
        SimdVector4F scaling,
        SimdVector4F rotationOrigin,
        float rotation,
        SimdVector4F translation)
    {
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
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateAffineTransform3(
        SimdVector4F scaling,
        SimdVector4F rotationOrigin,
        SimdVector4F rotationQuaternion,
        SimdVector4F translation)
    {
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
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadFloat4x4(float const* source)
    {
        /*
        result.Inner[0].Inner[0] = source.M11;
        result.Inner[0].Inner[1] = source.M12;
        result.Inner[0].Inner[2] = source.M13;
        result.Inner[0].Inner[3] = source.M14;
        result.Inner[1].Inner[0] = source.M21;
        result.Inner[1].Inner[1] = source.M22;
        result.Inner[1].Inner[2] = source.M23;
        result.Inner[1].Inner[3] = source.M24;
        result.Inner[2].Inner[0] = source.M31;
        result.Inner[2].Inner[1] = source.M32;
        result.Inner[2].Inner[2] = source.M33;
        result.Inner[2].Inner[3] = source.M34;
        result.Inner[3].Inner[0] = source.M41;
        result.Inner[3].Inner[1] = source.M42;
        result.Inner[3].Inner[2] = source.M43;
        result.Inner[3].Inner[3] = source.M44;
        */
        SimdMatrix4x4F result;

        float const* it = source;

        result.Inner[0].Inner[0] = (*it++);
        result.Inner[0].Inner[1] = (*it++);
        result.Inner[0].Inner[2] = (*it++);
        result.Inner[0].Inner[3] = (*it++);

        result.Inner[1].Inner[0] = (*it++);
        result.Inner[1].Inner[1] = (*it++);
        result.Inner[1].Inner[2] = (*it++);
        result.Inner[1].Inner[3] = (*it++);

        result.Inner[2].Inner[0] = (*it++);
        result.Inner[2].Inner[1] = (*it++);
        result.Inner[2].Inner[2] = (*it++);
        result.Inner[2].Inner[3] = (*it++);

        result.Inner[3].Inner[0] = (*it++);
        result.Inner[3].Inner[1] = (*it++);
        result.Inner[3].Inner[2] = (*it++);
        result.Inner[3].Inner[3] = (*it++);

        return result;
    }

    inline void anemone_vectorcall Matrix4x4F_StoreFloat4x4(float* destination, SimdMatrix4x4F source)
    {
        /*
        destination.M11 = source.Inner[0].Inner[0];
        destination.M12 = source.Inner[0].Inner[1];
        destination.M13 = source.Inner[0].Inner[2];
        destination.M14 = source.Inner[0].Inner[3];
        destination.M21 = source.Inner[1].Inner[0];
        destination.M22 = source.Inner[1].Inner[1];
        destination.M23 = source.Inner[1].Inner[2];
        destination.M24 = source.Inner[1].Inner[3];
        destination.M31 = source.Inner[2].Inner[0];
        destination.M32 = source.Inner[2].Inner[1];
        destination.M33 = source.Inner[2].Inner[2];
        destination.M34 = source.Inner[2].Inner[3];
        destination.M41 = source.Inner[3].Inner[0];
        destination.M42 = source.Inner[3].Inner[1];
        destination.M43 = source.Inner[3].Inner[2];
        destination.M44 = source.Inner[3].Inner[3];
        */

        float* it = destination;

        (*it++) = source.Inner[0].Inner[0];
        (*it++) = source.Inner[0].Inner[1];
        (*it++) = source.Inner[0].Inner[2];
        (*it++) = source.Inner[0].Inner[3];

        (*it++) = source.Inner[1].Inner[0];
        (*it++) = source.Inner[1].Inner[1];
        (*it++) = source.Inner[1].Inner[2];
        (*it++) = source.Inner[1].Inner[3];

        (*it++) = source.Inner[2].Inner[0];
        (*it++) = source.Inner[2].Inner[1];
        (*it++) = source.Inner[2].Inner[2];
        (*it++) = source.Inner[2].Inner[3];

        (*it++) = source.Inner[3].Inner[0];
        (*it++) = source.Inner[3].Inner[1];
        (*it++) = source.Inner[3].Inner[2];
        (*it++) = source.Inner[3].Inner[3];
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadFloat4x3(float const* source)
    {
        /*
        result.Inner[0].Inner[0] = source.M11;
        result.Inner[0].Inner[1] = source.M12;
        result.Inner[0].Inner[2] = source.M13;
        result.Inner[0].Inner[3] = 0.0f;

        result.Inner[1].Inner[0] = source.M21;
        result.Inner[1].Inner[1] = source.M22;
        result.Inner[1].Inner[2] = source.M23;
        result.Inner[1].Inner[3] = 0.0f;

        result.Inner[2].Inner[0] = source.M31;
        result.Inner[2].Inner[1] = source.M32;
        result.Inner[2].Inner[2] = source.M33;
        result.Inner[2].Inner[3] = 0.0f;

        result.Inner[3].Inner[0] = source.M41;
        result.Inner[3].Inner[1] = source.M42;
        result.Inner[3].Inner[2] = source.M43;
        result.Inner[3].Inner[3] = 1.0f;
        */
        SimdMatrix4x4F result;

        float const* it = source;

        result.Inner[0].Inner[0] = (*it++);
        result.Inner[0].Inner[1] = (*it++);
        result.Inner[0].Inner[2] = (*it++);
        result.Inner[0].Inner[3] = 0.0f;

        result.Inner[1].Inner[0] = (*it++);
        result.Inner[1].Inner[1] = (*it++);
        result.Inner[1].Inner[2] = (*it++);
        result.Inner[1].Inner[3] = 0.0f;

        result.Inner[2].Inner[0] = (*it++);
        result.Inner[2].Inner[1] = (*it++);
        result.Inner[2].Inner[2] = (*it++);
        result.Inner[2].Inner[3] = 0.0f;

        result.Inner[3].Inner[0] = (*it++);
        result.Inner[3].Inner[1] = (*it++);
        result.Inner[3].Inner[2] = (*it++);
        result.Inner[3].Inner[3] = 1.0f;

        return result;
    }

    inline void anemone_vectorcall Matrix4x4F_StoreFloat4x3(float* destination, SimdMatrix4x4F source)
    {
        /*
        destination.M11 = source.Inner[0].Inner[0];
        destination.M12 = source.Inner[0].Inner[1];
        destination.M13 = source.Inner[0].Inner[2];

        destination.M21 = source.Inner[1].Inner[0];
        destination.M22 = source.Inner[1].Inner[1];
        destination.M23 = source.Inner[1].Inner[2];

        destination.M31 = source.Inner[2].Inner[0];
        destination.M32 = source.Inner[2].Inner[1];
        destination.M33 = source.Inner[2].Inner[2];

        destination.M41 = source.Inner[3].Inner[0];
        destination.M42 = source.Inner[3].Inner[1];
        destination.M43 = source.Inner[3].Inner[2];
        */

        float* it = destination;

        (*it++) = source.Inner[0].Inner[0];
        (*it++) = source.Inner[0].Inner[1];
        (*it++) = source.Inner[0].Inner[2];

        (*it++) = source.Inner[1].Inner[0];
        (*it++) = source.Inner[1].Inner[1];
        (*it++) = source.Inner[1].Inner[2];

        (*it++) = source.Inner[2].Inner[0];
        (*it++) = source.Inner[2].Inner[1];
        (*it++) = source.Inner[2].Inner[2];

        (*it++) = source.Inner[3].Inner[0];
        (*it++) = source.Inner[3].Inner[1];
        (*it++) = source.Inner[3].Inner[2];
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadFloat3x4(float const* source)
    {
        /*
        result.Inner[0].Inner[0] = source.M11;
        result.Inner[0].Inner[1] = source.M21;
        result.Inner[0].Inner[2] = source.M31;
        result.Inner[0].Inner[3] = 0.0f;

        result.Inner[1].Inner[0] = source.M12;
        result.Inner[1].Inner[1] = source.M22;
        result.Inner[1].Inner[2] = source.M32;
        result.Inner[1].Inner[3] = 0.0f;

        result.Inner[2].Inner[0] = source.M13;
        result.Inner[2].Inner[1] = source.M23;
        result.Inner[2].Inner[2] = source.M33;
        result.Inner[2].Inner[3] = 0.0f;

        result.Inner[3].Inner[0] = source.M14;
        result.Inner[3].Inner[1] = source.M24;
        result.Inner[3].Inner[2] = source.M34;
        result.Inner[3].Inner[3] = 1.0f;
        */

        SimdMatrix4x4F result;

        // struct FMatrix3x4
        // {
        //     float M11, M12, M13, M14;
        //     float M21, M22, M23, M24;
        //     float M31, M32, M33, M34;
        // };

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

        result.Inner[0].Inner[3] = 0.0f;
        result.Inner[1].Inner[3] = 0.0f;
        result.Inner[2].Inner[3] = 0.0f;
        result.Inner[3].Inner[3] = 1.0f;

        return result;
    }

    inline void anemone_vectorcall Matrix4x4F_StoreFloat3x4(float* destination, SimdMatrix4x4F source)
    {
        /*
        destination.M11 = source.Inner[0].Inner[0];
        destination.M12 = source.Inner[1].Inner[0];
        destination.M13 = source.Inner[2].Inner[0];
        destination.M14 = source.Inner[3].Inner[0];

        destination.M21 = source.Inner[0].Inner[1];
        destination.M22 = source.Inner[1].Inner[1];
        destination.M23 = source.Inner[2].Inner[1];
        destination.M24 = source.Inner[3].Inner[1];

        destination.M31 = source.Inner[0].Inner[2];
        destination.M32 = source.Inner[1].Inner[2];
        destination.M33 = source.Inner[2].Inner[2];
        destination.M34 = source.Inner[3].Inner[2];
        */

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
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadFloat3x3(float const* source)
    {
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
    }

    inline void anemone_vectorcall Matrix4x4F_StoreFloat3x3(float* destination, SimdMatrix4x4F source)
    {
        destination[0 * 3 + 0] = source.Inner[0].Inner[0];
        destination[0 * 3 + 1] = source.Inner[0].Inner[1];
        destination[0 * 3 + 2] = source.Inner[0].Inner[2];

        destination[1 * 3 + 0] = source.Inner[1].Inner[0];
        destination[1 * 3 + 1] = source.Inner[1].Inner[1];
        destination[1 * 3 + 2] = source.Inner[1].Inner[2];

        destination[2 * 3 + 0] = source.Inner[2].Inner[0];
        destination[2 * 3 + 1] = source.Inner[2].Inner[1];
        destination[2 * 3 + 2] = source.Inner[2].Inner[2];
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
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_MultiplyTranspose(SimdMatrix4x4F a, SimdMatrix4x4F b)
    {
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
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Transpose(SimdMatrix4x4F m)
    {
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
    }

    inline SimdVector4F anemone_vectorcall Matrix4x4F_Determinant(SimdMatrix4x4F matrix)
    {
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
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Inverse(SimdMatrix4x4F matrix, SimdVector4F& determinant)
    {
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
    }

    inline SimdVector4F anemone_vectorcall Matrix4x4F_Diagonal(SimdMatrix4x4F m)
    {
        return SimdVector4F{
            m.Inner[0].Inner[0],
            m.Inner[1].Inner[1],
            m.Inner[2].Inner[2],
            m.Inner[3].Inner[3],
        };
    }

    inline SimdVector4F anemone_vectorcall Matrix4x4F_Trace(SimdMatrix4x4F m)
    {
        float const r = m.Inner[0].Inner[0] + m.Inner[1].Inner[1] + m.Inner[2].Inner[2] + m.Inner[3].Inner[3];
        return SimdVector4F{
            r,
            r,
            r,
            r,
        };
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_TensorProduct(SimdVector4F a, SimdVector4F b)
    {
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
    }

    inline bool anemone_vectorcall Matrix4x4F_IsIdentity(SimdMatrix4x4F m)
    {
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
    }

    inline bool anemone_vectorcall Matrix4x4F_IsNaN(SimdMatrix4x4F m)
    {
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
    }

    inline bool anemone_vectorcall Matrix4x4F_IsInfinite(SimdMatrix4x4F m)
    {
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
    }
}

namespace Anemone::Numerics::Private
{
    inline SimdVector4F anemone_vectorcall RotorF_Create(float xy, float xz, float yz, float scalar)
    {
        return SimdVector4F{
            xy,
            xz,
            yz,
            scalar,
        };
    }

    inline SimdVector4F anemone_vectorcall RotorF_Create(SimdVector4F bv, float scalar)
    {
        bv.Inner[3] = scalar;
        return bv;
    }

    inline SimdVector4F anemone_vectorcall RotorF_FromAxisAngle(SimdVector4F axis, float angle)
    {
        SimdVector4F const normal = Vector4F_Normalize3(axis);
        return RotorF_FromNormalAngle(normal, angle);
    }

    inline SimdVector4F anemone_vectorcall RotorF_FromNormalAngle(SimdVector4F normal, float angle)
    {
        auto [fSin, fCos] = SinCos<float>(angle * 0.5f);

        return SimdVector4F{
            -fSin * normal.Inner[0],
            -fSin * normal.Inner[1],
            -fSin * normal.Inner[2],
            fCos,
        };
    }

    inline SimdVector4F anemone_vectorcall RotorF_RotationBetween(SimdVector4F from, SimdVector4F to)
    {
        SimdVector4F const dot = Vector4F_Dot3(to, from);
        SimdVector4F result = Vector4F_Wedge3(to, from);

        result.Inner[3] = 1.0f + dot.Inner[3];
        return RotorF_Normalize(result);
    }

    inline SimdVector4F anemone_vectorcall RotorF_Dot(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_Dot4(a, b);
    }

    inline SimdVector4F anemone_vectorcall RotorF_Rotate3(SimdVector4F rotor, SimdVector4F v)
    {
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
    }

    inline SimdVector4F anemone_vectorcall RotorF_InverseRotate3(SimdVector4F rotor, SimdVector4F v)
    {
        float const vx = v.Inner[0];
        float const vy = v.Inner[1];
        float const vz = v.Inner[2];

        float const rx = -rotor.Inner[0];
        float const ry = -rotor.Inner[1];
        float const rz = -rotor.Inner[2];
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
    }

    inline SimdVector4F anemone_vectorcall RotorF_Multiply(SimdVector4F a, SimdVector4F b)
    {
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
    }

    inline SimdVector4F anemone_vectorcall RotorF_RotateRotor(SimdVector4F rotor, SimdVector4F rotation)
    {
        SimdVector4F const t = RotorF_Multiply(rotor, rotation);
        return RotorF_Multiply(t, RotorF_Reverse(rotor));
    }

    inline SimdVector4F anemone_vectorcall RotorF_Reverse(SimdVector4F rotor)
    {
        return SimdVector4F{
            -rotor.Inner[0],
            -rotor.Inner[1],
            -rotor.Inner[2],
            rotor.Inner[3],
        };
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
    }

    inline SimdVector4F anemone_vectorcall RotorF_Nlerp(SimdVector4F from, SimdVector4F to, SimdVector4F t)
    {
        return RotorF_Nlerp(from, to, t.Inner[0]);
    }

    inline SimdVector4F anemone_vectorcall RotorF_Nlerp(SimdVector4F from, SimdVector4F to, float t)
    {
        SimdVector4F const dot = Vector4F_Dot4(from, to);

        if (dot.Inner[0] < 0.0f)
        {
            to = Vector4F_Negate(to);
        }

        SimdVector4F const r = Vector4F_PreciseLerp(from, to, t);
        return Vector4F_Normalize4(r);
    }

    inline SimdVector4F anemone_vectorcall RotorF_Slerp(SimdVector4F from, SimdVector4F to, SimdVector4F t)
    {
        return RotorF_Slerp(from, to, t.Inner[0]);
    }

    inline SimdVector4F anemone_vectorcall RotorF_Slerp(SimdVector4F from, SimdVector4F to, float t)
    {
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
    }
}
