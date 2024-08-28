// ReSharper disable CppClangTidyClangDiagnosticFloatEqual
#pragma once
#include "AnemoneRuntime/Numerics/FVector2.hxx"
#include "AnemoneRuntime/Numerics/FMask2.hxx"
#include "AnemoneRuntime/Numerics/Functions.hxx"
#include "AnemoneRuntime/Numerics/Simd.hxx"

// Type
namespace Anemone::Numerics
{
    inline FVector2A::FVector2A(float x, float y)
        : Inner{Private::Vector4F_Create(x, y, 0.0f, 0.0f)}
    {
    }

    inline FVector2A::FVector2A(float value)
        : Inner{Private::Vector4F_Replicate(value)}
    {
    }

    inline FVector2A anemone_vectorcall FVector2A::Create(float x, float y)
    {
        return FVector2A{Private::Vector4F_Create(x, y, x, y)};
    }

    inline FVector2A anemone_vectorcall FVector2A::Splat(float value)
    {
        return FVector2A{Private::Vector4F_Replicate(value)};
    }

    inline FVector2A anemone_vectorcall FVector2A::Zero()
    {
        return FVector2A{Private::Vector4F_Zero()};
    }

    inline FVector2A anemone_vectorcall FVector2A::One()
    {
        return FVector2A{Private::Vector4F_Replicate(1.0f)};
    }

    inline FVector2A anemone_vectorcall FVector2A::UnitX()
    {
        return FVector2A{Private::Vector4F_PositiveUnitX()};
    }

    inline FVector2A anemone_vectorcall FVector2A::UnitY()
    {
        return FVector2A{Private::Vector4F_PositiveUnitY()};
    }

    inline FVector2A anemone_vectorcall FVector2A::NegativeUnitX()
    {
        return FVector2A{Private::Vector4F_NegativeUnitX()};
    }

    inline FVector2A anemone_vectorcall FVector2A::NegativeUnitY()
    {
        return FVector2A{Private::Vector4F_NegativeUnitY()};
    }

    inline FVector2A anemone_vectorcall FVector2A::Load(FVector2 const& source)
    {
        return FVector2A{Private::Vector4F_LoadFloat2(reinterpret_cast<float const*>(&source))};
    }

    inline void anemone_vectorcall FVector2A::Store(FVector2& destination, FVector2A source)
    {
        Private::Vector4F_StoreFloat2(reinterpret_cast<float*>(&destination), source.Inner);
    }

    inline FVector2A anemone_vectorcall FromVector2(FVector2 const& source)
    {
        return FVector2A::Load(source);
    }

    inline FVector2 anemone_vectorcall ToVector2(FVector2A source)
    {
        FVector2 result;
        FVector2A::Store(result, source);
        return result;
    }
}

// Component-wise operations
namespace Anemone::Numerics
{
    template <size_t N>
    float anemone_vectorcall Extract(FVector2A a)
        requires(N < 2)
    {
        return Private::Vector4F_Extract<N>(a.Inner);
    }

    template <size_t N>
    FVector2A anemone_vectorcall Insert(FVector2A a, float b)
        requires(N < 2)
    {
        return FVector2A{Private::Vector4F_Insert<N>(a.Inner, b)};
    }

    template <size_t N>
    FVector2A anemone_vectorcall Broadcast(FVector2A a)
        requires(N < 2)
    {
        return FVector2A{Private::Vector4F_Broadcast<N>(a.Inner)};
    }

    template <size_t X, size_t Y>
    FVector2A anemone_vectorcall Permute(FVector2A a)
        requires((X < 2) and (Y < 2))
    {
        return FVector2A{Private::Vector4F_Permute<X, Y, X, Y>(a.Inner)};
    }

    inline FVector2A anemone_vectorcall Select(FMask2A mask, FVector2A whenFalse, FVector2A whenTrue)
    {
        return FVector2A{Private::Vector4F_Select(mask.Inner, whenFalse.Inner, whenTrue.Inner)};
    }

    template <bool X, bool Y>
    FVector2A anemone_vectorcall Select(FVector2A whenFalse, FVector2A whenTrue)
    {
        return FVector2A{Private::Vector4F_Select<X, Y, X, Y>(whenFalse.Inner, whenTrue.Inner)};
    }
}

// Specific operations
namespace Anemone::Numerics
{
    inline FVector2A anemone_vectorcall Dot(FVector2A a, FVector2A b)
    {
        return FVector2A{Private::Vector4F_Dot2(a.Inner, b.Inner)};
    }

    inline FVector2A anemone_vectorcall Cross(FVector2A a, FVector2A b)
    {
        return FVector2A{Private::Vector4F_Cross2(a.Inner, b.Inner)};
    }

    inline FVector2A anemone_vectorcall Length(FVector2A v)
    {
        return FVector2A{Private::Vector4F_Length2(v.Inner)};
    }

    inline FVector2A anemone_vectorcall LengthSquared(FVector2A v)
    {
        return FVector2A{Private::Vector4F_LengthSquared2(v.Inner)};
    }

    inline FVector2A anemone_vectorcall Normalize(FVector2A v)
    {
        return FVector2A{Private::Vector4F_Normalize2(v.Inner)};
    }
}

// Arithmetic operations
namespace Anemone::Numerics
{
    inline FVector2A anemone_vectorcall Add(FVector2A a, FVector2A b)
    {
        return FVector2A{Private::Vector4F_Add(a.Inner, b.Inner)};
    }

    inline FVector2A anemone_vectorcall Subtract(FVector2A a, FVector2A b)
    {
        return FVector2A{Private::Vector4F_Subtract(a.Inner, b.Inner)};
    }

    inline FVector2A anemone_vectorcall Multiply(FVector2A a, FVector2A b)
    {
        return FVector2A{Private::Vector4F_Multiply(a.Inner, b.Inner)};
    }

    inline FVector2A anemone_vectorcall Divide(FVector2A a, FVector2A b)
    {
        return FVector2A{Private::Vector4F_Divide(a.Inner, b.Inner)};
    }

    inline FVector2A anemone_vectorcall Negate(FVector2A v)
    {
        return FVector2A{Private::Vector4F_Negate(v.Inner)};
    }

    inline FVector2A anemone_vectorcall Reciprocal(FVector2A v)
    {
        return FVector2A{Private::Vector4F_Reciprocal(v.Inner)};
    }

    inline FVector2A anemone_vectorcall ReciprocalEst(FVector2A v)
    {
        return FVector2A{Private::Vector4F_ReciprocalEst(v.Inner)};
    }

    inline FVector2A anemone_vectorcall MultiplyAdd(FVector2A a, FVector2A b, FVector2A c)
    {
        return FVector2A{Private::Vector4F_MultiplyAdd(a.Inner, b.Inner, c.Inner)};
    }

    inline FVector2A anemone_vectorcall MultiplySubtract(FVector2A a, FVector2A b, FVector2A c)
    {
        return FVector2A{Private::Vector4F_MultiplySubtract(a.Inner, b.Inner, c.Inner)};
    }

    inline FVector2A anemone_vectorcall NegateMultiplyAdd(FVector2A a, FVector2A b, FVector2A c)
    {
        return FVector2A{Private::Vector4F_NegateMultiplyAdd(a.Inner, b.Inner, c.Inner)};
    }

    inline FVector2A anemone_vectorcall NegateMultiplySubtract(FVector2A a, FVector2A b, FVector2A c)
    {
        return FVector2A{Private::Vector4F_NegateMultiplySubtract(a.Inner, b.Inner, c.Inner)};
    }

    inline FVector2A anemone_vectorcall Min(FVector2A a, FVector2A b)
    {
        return FVector2A{Private::Vector4F_Min(a.Inner, b.Inner)};
    }

    inline FVector2A anemone_vectorcall Max(FVector2A a, FVector2A b)
    {
        return FVector2A{Private::Vector4F_Max(a.Inner, b.Inner)};
    }

    inline FVector2A anemone_vectorcall Abs(FVector2A v)
    {
        return FVector2A{Private::Vector4F_Abs(v.Inner)};
    }

    inline FVector2A anemone_vectorcall Square(FVector2A v)
    {
        return FVector2A{Private::Vector4F_Square(v.Inner)};
    }

    inline FVector2A anemone_vectorcall SignedSquare(FVector2A v)
    {
        return FVector2A{Private::Vector4F_SignedSquare(v.Inner)};
    }

    inline FVector2A anemone_vectorcall Clamp(FVector2A v, FVector2A lower, FVector2A upper)
    {
        return FVector2A{Private::Vector4F_Clamp(v.Inner, lower.Inner, upper.Inner)};
    }

    inline FVector2A anemone_vectorcall Saturate(FVector2A v)
    {
        return FVector2A{Private::Vector4F_Saturate(v.Inner)};
    }

    inline FVector2A anemone_vectorcall Sqrt(FVector2A v)
    {
        return FVector2A{Private::Vector4F_SquareRoot(v.Inner)};
    }

    inline FVector2A anemone_vectorcall SqrtEst(FVector2A v)
    {
        return FVector2A{Private::Vector4F_SquareRootEst(v.Inner)};
    }

    inline FVector2A anemone_vectorcall ReciprocalSqrt(FVector2A v)
    {
        return FVector2A{Private::Vector4F_ReciprocalSquareRoot(v.Inner)};
    }

    inline FVector2A anemone_vectorcall ReciprocalSqrtEst(FVector2A v)
    {
        return FVector2A{Private::Vector4F_ReciprocalSquareRootEst(v.Inner)};
    }

    inline FVector2A anemone_vectorcall Ceil(FVector2A v)
    {
        return FVector2A{Private::Vector4F_Ceil(v.Inner)};
    }

    inline FVector2A anemone_vectorcall Floor(FVector2A v)
    {
        return FVector2A{Private::Vector4F_Floor(v.Inner)};
    }

    inline FVector2A anemone_vectorcall Truncate(FVector2A v)
    {
        return FVector2A{Private::Vector4F_Truncate(v.Inner)};
    }

    inline FVector2A anemone_vectorcall Round(FVector2A v)
    {
        return FVector2A{Private::Vector4F_Round(v.Inner)};
    }

    inline FVector2A anemone_vectorcall Fraction(FVector2A v)
    {
        return FVector2A{Private::Vector4F_Fraction(v.Inner)};
    }

    inline FVector2A anemone_vectorcall Remainder(FVector2A x, FVector2A y)
    {
        return FVector2A{Private::Vector4F_Remainder(x.Inner, y.Inner)};
    }

    inline FVector2A anemone_vectorcall Repeat(FVector2A v, FVector2A length)
    {
        return FVector2A{Private::Vector4F_Repeat(v.Inner, length.Inner)};
    }

    inline FVector2A anemone_vectorcall Wrap(FVector2A v, FVector2A lower, FVector2A upper)
    {
        return FVector2A{Private::Vector4F_Wrap(v.Inner, lower.Inner, upper.Inner)};
    }

    inline FVector2A anemone_vectorcall Power(FVector2A x, FVector2A y)
    {
        return FVector2A{Private::Vector4F_Power(x.Inner, y.Inner)};
    }

    inline FVector2A anemone_vectorcall Exp(FVector2A v)
    {
        return FVector2A{Private::Vector4F_Exp(v.Inner)};
    }

    inline FVector2A anemone_vectorcall Exp2(FVector2A v)
    {
        return FVector2A{Private::Vector4F_Exp2(v.Inner)};
    }

    inline FVector2A anemone_vectorcall Exp10(FVector2A v)
    {
        return FVector2A{Private::Vector4F_Exp10(v.Inner)};
    }

    inline FVector2A anemone_vectorcall Log(FVector2A v)
    {
        return FVector2A{Private::Vector4F_Log(v.Inner)};
    }

    inline FVector2A anemone_vectorcall Log(FVector2A x, FVector2A base)
    {
        return FVector2A{Private::Vector4F_Log(x.Inner, base.Inner)};
    }

    inline FVector2A anemone_vectorcall Log2(FVector2A v)
    {
        return FVector2A{Private::Vector4F_Log2(v.Inner)};
    }

    inline FVector2A anemone_vectorcall Log10(FVector2A v)
    {
        return FVector2A{Private::Vector4F_Log10(v.Inner)};
    }

    inline SinCosResult<FVector2A> anemone_vectorcall SinCos(FVector2A v)
    {
        SinCosResult<FVector2A> result;
        Private::Vector4F_SinCos(result.Sin.Inner, result.Cos.Inner, v.Inner);
        return result;
    }

    inline FVector2A anemone_vectorcall Sin(FVector2A v)
    {
        return FVector2A{Private::Vector4F_Sin(v.Inner)};
    }

    inline FVector2A anemone_vectorcall Cos(FVector2A v)
    {
        return FVector2A{Private::Vector4F_Cos(v.Inner)};
    }

    inline FVector2A anemone_vectorcall Tan(FVector2A v)
    {
        return FVector2A{Private::Vector4F_Tan(v.Inner)};
    }

    inline FVector2A anemone_vectorcall Asin(FVector2A v)
    {
        return FVector2A{Private::Vector4F_Asin(v.Inner)};
    }

    inline FVector2A anemone_vectorcall Acos(FVector2A v)
    {
                return FVector2A{Private::Vector4F_Acos(v.Inner)};
    }

    inline FVector2A anemone_vectorcall Atan(FVector2A v)
    {
                return FVector2A{Private::Vector4F_Atan(v.Inner)};
    }

    inline FVector2A anemone_vectorcall Atan2(FVector2A y, FVector2A x)
    {
                        return FVector2A{Private::Vector4F_Atan2(y.Inner, x.Inner)};
    }
}

// Masked operations
namespace Anemone::Numerics
{
    inline FMask2A anemone_vectorcall CompareEqual(FVector2A a, FVector2A b)
    {
        return FMask2A{Private::Vector4F_CompareEqual(a.Inner, b.Inner)};
    }

    inline FMask2A anemone_vectorcall CompareNotEqual(FVector2A a, FVector2A b)
    {
        return FMask2A{Private::Vector4F_CompareNotEqual(a.Inner, b.Inner)};
    }

    inline FMask2A anemone_vectorcall CompareLessThan(FVector2A a, FVector2A b)
    {
        return FMask2A{Private::Vector4F_CompareLessThan(a.Inner, b.Inner)};
    }

    inline FMask2A anemone_vectorcall CompareLessEqual(FVector2A a, FVector2A b)
    {
        return FMask2A{Private::Vector4F_CompareLessEqual(a.Inner, b.Inner)};
    }

    inline FMask2A anemone_vectorcall CompareGreaterThan(FVector2A a, FVector2A b)
    {
        return FMask2A{Private::Vector4F_CompareGreaterThan(a.Inner, b.Inner)};
    }

    inline FMask2A anemone_vectorcall CompareGreaterEqual(FVector2A a, FVector2A b)
    {
        return FMask2A{Private::Vector4F_CompareGreaterEqual(a.Inner, b.Inner)};
    }

    inline FMask2A anemone_vectorcall CompareNearEqual(FVector2A a, FVector2A b, FVector2A tolerance)
    {
        return FMask2A{Private::Vector4F_CompareNearEqual(a.Inner, b.Inner, tolerance.Inner)};
    }

    inline FMask2A anemone_vectorcall CompareNearEqual(FVector2A a, FVector2A b, float tolerance)
    {
        return FMask2A{Private::Vector4F_CompareNearEqual(a.Inner, b.Inner, tolerance)};
    }

    inline FMask2A anemone_vectorcall CompareNearEqual(FVector2A a, FVector2A b)
    {
        return FMask2A{Private::Vector4F_CompareNearEqual(a.Inner, b.Inner)};
    }

    inline FMask2A anemone_vectorcall CompareNearEqualPrecise(FVector2A a, FVector2A b)
    {
        return FMask2A{Private::Vector4F_CompareNearEqualPrecise(a.Inner, b.Inner)};
    }

    inline FMask2A anemone_vectorcall CompareNearZero(FVector2A v, FVector2A tolerance)
    {
        return FMask2A{Private::Vector4F_CompareNearZero(v.Inner, tolerance.Inner)};
    }

    inline FMask2A anemone_vectorcall CompareNearZero(FVector2A v, float tolerance)
    {
        return FMask2A{Private::Vector4F_CompareNearZero(v.Inner, tolerance)};
    }

    inline FMask2A anemone_vectorcall CompareNearZero(FVector2A v)
    {
        return FMask2A{Private::Vector4F_CompareNearZero(v.Inner)};
    }

    anemone_noinline inline FMask2A anemone_vectorcall CompareNearZeroPrecise(FVector2A v)
    {
        return FMask2A{Private::Vector4F_CompareNearZeroPrecise(v.Inner)};
    }
}
