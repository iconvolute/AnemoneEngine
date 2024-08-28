// ReSharper disable CppClangTidyClangDiagnosticFloatEqual
#pragma once
#include "AnemoneRuntime/Numerics/FVector3.hxx"
#include "AnemoneRuntime/Numerics/FMask3.hxx"
#include "AnemoneRuntime/Numerics/FQuaternion.hxx"
#include "AnemoneRuntime/Numerics/FMatrix4x4.hxx"
#include "AnemoneRuntime/Numerics/Functions.hxx"
#include "AnemoneRuntime/Numerics/Simd.hxx"

// Type
namespace Anemone::Numerics
{
    inline FVector3A::FVector3A(float x, float y, float z)
        : Inner{Private::Vector4F_Create(x, y, z, 0.0f)}
    {
    }

    inline FVector3A::FVector3A(float value)
        : Inner{Private::Vector4F_Replicate(value)}
    {
    }

    inline FVector3A anemone_vectorcall FVector3A::Create(float x, float y, float z)
    {
        return FVector3A{Private::Vector4F_Create(x, y, z, 0.0f)};
    }

    inline FVector3A anemone_vectorcall FVector3A::Splat(float value)
    {
        return FVector3A{Private::Vector4F_Replicate(value)};
    }

    inline FVector3A anemone_vectorcall FVector3A::Zero()
    {
        return FVector3A{Private::Vector4F_Zero()};
    }

    inline FVector3A anemone_vectorcall FVector3A::One()
    {
        return FVector3A{Private::Vector4F_Replicate(1.0f)};
    }

    inline FVector3A anemone_vectorcall FVector3A::UnitX()
    {
        return FVector3A{Private::Vector4F_PositiveUnitX()};
    }

    inline FVector3A anemone_vectorcall FVector3A::UnitY()
    {
        return FVector3A{Private::Vector4F_PositiveUnitY()};
    }

    inline FVector3A anemone_vectorcall FVector3A::UnitZ()
    {
        return FVector3A{Private::Vector4F_PositiveUnitZ()};
    }

    inline FVector3A anemone_vectorcall FVector3A::NegativeUnitX()
    {
        return FVector3A{Private::Vector4F_NegativeUnitX()};
    }

    inline FVector3A anemone_vectorcall FVector3A::NegativeUnitY()
    {
        return FVector3A{Private::Vector4F_NegativeUnitY()};
    }

    inline FVector3A anemone_vectorcall FVector3A::NegativeUnitZ()
    {
        return FVector3A{Private::Vector4F_NegativeUnitZ()};
    }

    inline FVector3A anemone_vectorcall FVector3A::Load(FVector3 const& source)
    {
        return FVector3A{Private::Vector4F_LoadFloat3(reinterpret_cast<float const*>(&source))};
    }

    inline void anemone_vectorcall FVector3A::Store(FVector3& destination, FVector3A source)
    {
        Private::Vector4F_StoreFloat3(reinterpret_cast<float*>(&destination), source.Inner);
    }

    inline FVector3A anemone_vectorcall FromVector3(FVector3 const& source)
    {
        return FVector3A::Load(source);
    }

    inline FVector3 anemone_vectorcall ToVector3(FVector3A source)
    {
        FVector3 result;
        FVector3A::Store(result, source);
        return result;
    }
}

// Component-wise operations
namespace Anemone::Numerics
{
    template <size_t N>
    float anemone_vectorcall Extract(FVector3A a)
        requires(N < 3)
    {
        return Private::Vector4F_Extract<N>(a.Inner);
    }

    template <size_t N>
    FVector3A anemone_vectorcall Insert(FVector3A a, float b)
        requires(N < 3)
    {
        return FVector3A{Private::Vector4F_Insert<N>(a.Inner, b)};
    }

    template <size_t N>
    FVector3A anemone_vectorcall Broadcast(FVector3A a)
        requires(N < 3)
    {
        return FVector3A{Private::Vector4F_Broadcast<N>(a.Inner)};
    }

    template <size_t X, size_t Y, size_t Z>
    FVector3A anemone_vectorcall Permute(FVector3A a)
        requires((X < 3) and (Y < 3) and (Z < 3))
    {
        return FVector3A{Private::Vector4F_Permute<X, Y, Z, Y>(a.Inner)};
    }

    inline FVector3A anemone_vectorcall Select(FMask3A mask, FVector3A whenFalse, FVector3A whenTrue)
    {
        return FVector3A{Private::Vector4F_Select(mask.Inner, whenFalse.Inner, whenTrue.Inner)};
    }

    template <bool X, bool Y, bool Z>
    FVector3A anemone_vectorcall Select(FVector3A whenFalse, FVector3A whenTrue)
    {
        return FVector3A{Private::Vector4F_Select<X, Y, Z, Y>(whenFalse.Inner, whenTrue.Inner)};
    }
}

// Specific operations
namespace Anemone::Numerics
{
    inline FVector3A anemone_vectorcall Dot(FVector3A a, FVector3A b)
    {
        return FVector3A{Private::Vector4F_Dot3(a.Inner, b.Inner)};
    }

    inline FVector3A anemone_vectorcall Cross(FVector3A a, FVector3A b)
    {
        return FVector3A{Private::Vector4F_Cross3(a.Inner, b.Inner)};
    }

    inline FVector3A anemone_vectorcall Length(FVector3A v)
    {
        return FVector3A{Private::Vector4F_Length3(v.Inner)};
    }

    inline FVector3A anemone_vectorcall LengthSquared(FVector3A v)
    {
        return FVector3A{Private::Vector4F_LengthSquared3(v.Inner)};
    }

    inline FVector3A anemone_vectorcall Normalize(FVector3A v)
    {
        return FVector3A{Private::Vector4F_Normalize3(v.Inner)};
    }

    anemone_noinline inline FVector3A anemone_vectorcall Rotate(FQuaternionA q, FVector3A v)
    {
        return FVector3A{Private::QuaternionF_Rotate3(q.Inner, v.Inner)};
    }

    inline FVector3A anemone_vectorcall InverseRotate(FQuaternionA q, FVector3A v)
    {
        return FVector3A{Private::QuaternionF_InverseRotate3(q.Inner,v.Inner)};
    }

    inline FVector3A anemone_vectorcall Transform(FVector3A v, FMatrix4x4A m)
    {
        return FVector3A{Private::Vector4F_Transform3(v.Inner, m.Inner)};
    }
}

// Arithmetic operations
namespace Anemone::Numerics
{
    inline FVector3A anemone_vectorcall Add(FVector3A a, FVector3A b)
    {
        return FVector3A{Private::Vector4F_Add(a.Inner, b.Inner)};
    }

    inline FVector3A anemone_vectorcall Subtract(FVector3A a, FVector3A b)
    {
        return FVector3A{Private::Vector4F_Subtract(a.Inner, b.Inner)};
    }

    inline FVector3A anemone_vectorcall Multiply(FVector3A a, FVector3A b)
    {
        return FVector3A{Private::Vector4F_Multiply(a.Inner, b.Inner)};
    }

    inline FVector3A anemone_vectorcall Divide(FVector3A a, FVector3A b)
    {
        return FVector3A{Private::Vector4F_Divide(a.Inner, b.Inner)};
    }

    inline FVector3A anemone_vectorcall Negate(FVector3A v)
    {
        return FVector3A{Private::Vector4F_Negate(v.Inner)};
    }

    inline FVector3A anemone_vectorcall Reciprocal(FVector3A v)
    {
        return FVector3A{Private::Vector4F_Reciprocal(v.Inner)};
    }

    inline FVector3A anemone_vectorcall ReciprocalEst(FVector3A v)
    {
        return FVector3A{Private::Vector4F_ReciprocalEst(v.Inner)};
    }

    inline FVector3A anemone_vectorcall MultiplyAdd(FVector3A a, FVector3A b, FVector3A c)
    {
        return FVector3A{Private::Vector4F_MultiplyAdd(a.Inner, b.Inner, c.Inner)};
    }

    inline FVector3A anemone_vectorcall MultiplySubtract(FVector3A a, FVector3A b, FVector3A c)
    {
        return FVector3A{Private::Vector4F_MultiplySubtract(a.Inner, b.Inner, c.Inner)};
    }

    inline FVector3A anemone_vectorcall NegateMultiplyAdd(FVector3A a, FVector3A b, FVector3A c)
    {
        return FVector3A{Private::Vector4F_NegateMultiplyAdd(a.Inner, b.Inner, c.Inner)};
    }

    inline FVector3A anemone_vectorcall NegateMultiplySubtract(FVector3A a, FVector3A b, FVector3A c)
    {
        return FVector3A{Private::Vector4F_NegateMultiplySubtract(a.Inner, b.Inner, c.Inner)};
    }

    inline FVector3A anemone_vectorcall Min(FVector3A a, FVector3A b)
    {
        return FVector3A{Private::Vector4F_Min(a.Inner, b.Inner)};
    }

    inline FVector3A anemone_vectorcall Max(FVector3A a, FVector3A b)
    {
        return FVector3A{Private::Vector4F_Max(a.Inner, b.Inner)};
    }

    inline FVector3A anemone_vectorcall Abs(FVector3A v)
    {
        return FVector3A{Private::Vector4F_Abs(v.Inner)};
    }

    inline FVector3A anemone_vectorcall Square(FVector3A v)
    {
        return FVector3A{Private::Vector4F_Square(v.Inner)};
    }

    inline FVector3A anemone_vectorcall SignedSquare(FVector3A v)
    {
        return FVector3A{Private::Vector4F_SignedSquare(v.Inner)};
    }

    inline FVector3A anemone_vectorcall Clamp(FVector3A v, FVector3A lower, FVector3A upper)
    {
        return FVector3A{Private::Vector4F_Clamp(v.Inner, lower.Inner, upper.Inner)};
    }

    inline FVector3A anemone_vectorcall Saturate(FVector3A v)
    {
        return FVector3A{Private::Vector4F_Saturate(v.Inner)};
    }

    inline FVector3A anemone_vectorcall Sqrt(FVector3A v)
    {
        return FVector3A{Private::Vector4F_SquareRoot(v.Inner)};
    }

    inline FVector3A anemone_vectorcall SqrtEst(FVector3A v)
    {
        return FVector3A{Private::Vector4F_SquareRootEst(v.Inner)};
    }

    inline FVector3A anemone_vectorcall ReciprocalSqrt(FVector3A v)
    {
        return FVector3A{Private::Vector4F_ReciprocalSquareRoot(v.Inner)};
    }

    inline FVector3A anemone_vectorcall ReciprocalSqrtEst(FVector3A v)
    {
        return FVector3A{Private::Vector4F_ReciprocalSquareRootEst(v.Inner)};
    }

    inline FVector3A anemone_vectorcall Ceil(FVector3A v)
    {
        return FVector3A{Private::Vector4F_Ceil(v.Inner)};
    }

    inline FVector3A anemone_vectorcall Floor(FVector3A v)
    {
        return FVector3A{Private::Vector4F_Floor(v.Inner)};
    }

    inline FVector3A anemone_vectorcall Truncate(FVector3A v)
    {
        return FVector3A{Private::Vector4F_Truncate(v.Inner)};
    }

    inline FVector3A anemone_vectorcall Round(FVector3A v)
    {
        return FVector3A{Private::Vector4F_Round(v.Inner)};
    }

    inline FVector3A anemone_vectorcall Fraction(FVector3A v)
    {
        return FVector3A{Private::Vector4F_Fraction(v.Inner)};
    }

    inline FVector3A anemone_vectorcall Remainder(FVector3A x, FVector3A y)
    {
        return FVector3A{Private::Vector4F_Remainder(x.Inner, y.Inner)};
    }

    inline FVector3A anemone_vectorcall Repeat(FVector3A v, FVector3A length)
    {
        return FVector3A{Private::Vector4F_Repeat(v.Inner, length.Inner)};
    }

    inline FVector3A anemone_vectorcall Wrap(FVector3A v, FVector3A lower, FVector3A upper)
    {
        return FVector3A{Private::Vector4F_Wrap(v.Inner, lower.Inner, upper.Inner)};
    }

    inline FVector3A anemone_vectorcall Power(FVector3A x, FVector3A y)
    {
        return FVector3A{Private::Vector4F_Power(x.Inner, y.Inner)};
    }

    inline FVector3A anemone_vectorcall Exp(FVector3A v)
    {
        return FVector3A{Private::Vector4F_Exp(v.Inner)};
    }

    inline FVector3A anemone_vectorcall Exp2(FVector3A v)
    {
        return FVector3A{Private::Vector4F_Exp2(v.Inner)};
    }

    inline FVector3A anemone_vectorcall Exp10(FVector3A v)
    {
        return FVector3A{Private::Vector4F_Exp10(v.Inner)};
    }

    inline FVector3A anemone_vectorcall Log(FVector3A v)
    {
        return FVector3A{Private::Vector4F_Log(v.Inner)};
    }

    inline FVector3A anemone_vectorcall Log(FVector3A x, FVector3A base)
    {
        return FVector3A{Private::Vector4F_Log(x.Inner, base.Inner)};
    }

    inline FVector3A anemone_vectorcall Log2(FVector3A v)
    {
        return FVector3A{Private::Vector4F_Log2(v.Inner)};
    }

    inline FVector3A anemone_vectorcall Log10(FVector3A v)
    {
        return FVector3A{Private::Vector4F_Log10(v.Inner)};
    }

    inline SinCosResult<FVector3A> anemone_vectorcall SinCos(FVector3A v)
    {
        SinCosResult<FVector3A> result;
        Private::Vector4F_SinCos(result.Sin.Inner, result.Cos.Inner, v.Inner);
        return result;
    }

    inline FVector3A anemone_vectorcall Sin(FVector3A v)
    {
        return FVector3A{Private::Vector4F_Sin(v.Inner)};
    }

    inline FVector3A anemone_vectorcall Cos(FVector3A v)
    {
        return FVector3A{Private::Vector4F_Cos(v.Inner)};
    }

    inline FVector3A anemone_vectorcall Tan(FVector3A v)
    {
        return FVector3A{Private::Vector4F_Tan(v.Inner)};
    }

    inline FVector3A anemone_vectorcall Asin(FVector3A v)
    {
        return FVector3A{Private::Vector4F_Asin(v.Inner)};
    }

    inline FVector3A anemone_vectorcall Acos(FVector3A v)
    {
        return FVector3A{Private::Vector4F_Acos(v.Inner)};
    }

    inline FVector3A anemone_vectorcall Atan(FVector3A v)
    {
        return FVector3A{Private::Vector4F_Atan(v.Inner)};
    }

    inline FVector3A anemone_vectorcall Atan2(FVector3A y, FVector3A x)
    {
                return FVector3A{Private::Vector4F_Atan2(y.Inner, x.Inner)};
    }
}

// Masked operations
namespace Anemone::Numerics
{
    inline FMask3A anemone_vectorcall CompareEqual(FVector3A a, FVector3A b)
    {
        return FMask3A{Private::Vector4F_CompareEqual(a.Inner, b.Inner)};
    }

    inline FMask3A anemone_vectorcall CompareNotEqual(FVector3A a, FVector3A b)
    {
        return FMask3A{Private::Vector4F_CompareNotEqual(a.Inner, b.Inner)};
    }

    inline FMask3A anemone_vectorcall CompareLessThan(FVector3A a, FVector3A b)
    {
        return FMask3A{Private::Vector4F_CompareLessThan(a.Inner, b.Inner)};
    }

    inline FMask3A anemone_vectorcall CompareLessEqual(FVector3A a, FVector3A b)
    {
        return FMask3A{Private::Vector4F_CompareLessEqual(a.Inner, b.Inner)};
    }

    inline FMask3A anemone_vectorcall CompareGreaterThan(FVector3A a, FVector3A b)
    {
        return FMask3A{Private::Vector4F_CompareGreaterThan(a.Inner, b.Inner)};
    }

    inline FMask3A anemone_vectorcall CompareGreaterEqual(FVector3A a, FVector3A b)
    {
        return FMask3A{Private::Vector4F_CompareGreaterEqual(a.Inner, b.Inner)};
    }

    inline FMask3A anemone_vectorcall CompareNearEqual(FVector3A a, FVector3A b, FVector3A tolerance)
    {
        return FMask3A{Private::Vector4F_CompareNearEqual(a.Inner, b.Inner, tolerance.Inner)};
    }

    inline FMask3A anemone_vectorcall CompareNearEqual(FVector3A a, FVector3A b, float tolerance)
    {
        return FMask3A{Private::Vector4F_CompareNearEqual(a.Inner, b.Inner, tolerance)};
    }

    inline FMask3A anemone_vectorcall CompareNearEqual(FVector3A a, FVector3A b)
    {
        return FMask3A{Private::Vector4F_CompareNearEqual(a.Inner, b.Inner)};
    }

    inline FMask3A anemone_vectorcall CompareNearEqualPrecise(FVector3A a, FVector3A b)
    {
        return FMask3A{Private::Vector4F_CompareNearEqualPrecise(a.Inner, b.Inner)};
    }

    inline FMask3A anemone_vectorcall CompareNearZero(FVector3A v, FVector3A tolerance)
    {
        return FMask3A{Private::Vector4F_CompareNearZero(v.Inner, tolerance.Inner)};
    }

    inline FMask3A anemone_vectorcall CompareNearZero(FVector3A v, float tolerance)
    {
        return FMask3A{Private::Vector4F_CompareNearZero(v.Inner, tolerance)};
    }

    inline FMask3A anemone_vectorcall CompareNearZero(FVector3A v)
    {
        return FMask3A{Private::Vector4F_CompareNearZero(v.Inner)};
    }

    anemone_noinline inline FMask3A anemone_vectorcall CompareNearZeroPrecise(FVector3A v)
    {
        return FMask3A{Private::Vector4F_CompareNearZeroPrecise(v.Inner)};
    }
}
