// ReSharper disable CppClangTidyClangDiagnosticFloatEqual
#pragma once
#include "AnemoneRuntime/Numerics/FVector4.hxx"
#include "AnemoneRuntime/Numerics/FMask4.hxx"
#include "AnemoneRuntime/Numerics/Functions.hxx"
#include "AnemoneRuntime/Numerics/Simd.hxx"

// Type
namespace Anemone::Numerics
{
    inline FVector4A::FVector4A(float x, float y, float z, float w)
        : Inner{Private::Vector4F_Create(x, y, z, w)}
    {
    }

    inline FVector4A::FVector4A(float value)
        : Inner{Private::Vector4F_Replicate(value)}
    {
    }



    inline FVector4A anemone_vectorcall FVector4A::Create(float x, float y, float z, float w)
    {
        return FVector4A{Private::Vector4F_Create(x, y, z, w)};
    }

    inline FVector4A anemone_vectorcall FVector4A::Splat(float value)
    {
        return FVector4A{Private::Vector4F_Replicate(value)};
    }

    inline FVector4A anemone_vectorcall FVector4A::Zero()
    {
        return FVector4A{Private::Vector4F_Zero()};
    }

    inline FVector4A anemone_vectorcall FVector4A::One()
    {
        return FVector4A{Private::Vector4F_Replicate(1.0f)};
    }

    inline FVector4A anemone_vectorcall FVector4A::UnitX()
    {
        return FVector4A{Private::Vector4F_PositiveUnitX()};
    }

    inline FVector4A anemone_vectorcall FVector4A::UnitY()
    {
        return FVector4A{Private::Vector4F_PositiveUnitY()};
    }

    inline FVector4A anemone_vectorcall FVector4A::UnitZ()
    {
        return FVector4A{Private::Vector4F_PositiveUnitZ()};
    }

    inline FVector4A anemone_vectorcall FVector4A::UnitW()
    {
        return FVector4A{Private::Vector4F_PositiveUnitW()};
    }

    inline FVector4A anemone_vectorcall FVector4A::NegativeUnitX()
    {
        return FVector4A{Private::Vector4F_NegativeUnitX()};
    }

    inline FVector4A anemone_vectorcall FVector4A::NegativeUnitY()
    {
        return FVector4A{Private::Vector4F_NegativeUnitY()};
    }

    inline FVector4A anemone_vectorcall FVector4A::NegativeUnitZ()
    {
        return FVector4A{Private::Vector4F_NegativeUnitZ()};
    }

    inline FVector4A anemone_vectorcall FVector4A::NegativeUnitW()
    {
        return FVector4A{Private::Vector4F_NegativeUnitW()};
    }

    inline FVector4A anemone_vectorcall FVector4A::Load(FVector4 const& source)
    {
        return FVector4A{Private::Vector4F_LoadFloat4(reinterpret_cast<float const*>(&source))};
    }

    inline void anemone_vectorcall FVector4A::Store(FVector4& destination, FVector4A source)
    {
        Private::Vector4F_StoreFloat4(reinterpret_cast<float*>(&destination), source.Inner);
    }

    inline FVector4A anemone_vectorcall FromVector4(FVector4 const& source)
    {
        return FVector4A::Load(source);
    }

    inline FVector4 anemone_vectorcall ToVector4(FVector4A source)
    {
        FVector4 result;
        FVector4A::Store(result, source);
        return result;
    }
}

// Component-wise operations
namespace Anemone::Numerics
{
    template <size_t N>
    float anemone_vectorcall Extract(FVector4A a)
        requires(N < 4)
    {
        return Private::Vector4F_Extract<N>(a.Inner);
    }

    template <size_t N>
    FVector4A anemone_vectorcall Insert(FVector4A a, float b)
        requires(N < 4)
    {
        return FVector4A{Private::Vector4F_Insert<N>(a.Inner, b)};
    }

    template <size_t N>
    FVector4A anemone_vectorcall Broadcast(FVector4A a)
        requires(N < 4)
    {
        return FVector4A{Private::Vector4F_Broadcast<N>()};
    }

    template <size_t X, size_t Y, size_t Z, size_t W>
    FVector4A anemone_vectorcall Permute(FVector4A a)
        requires((X < 4) and (Y < 4) and (Z < 4) and (W < 4))
    {
        return FVector4A{Private::Vector4F_Permute<X, Y, Z, W>(a.Inner)};
    }

    inline FVector4A anemone_vectorcall Select(FMask4A mask, FVector4A whenFalse, FVector4A whenTrue)
    {
        return FVector4A{Private::Vector4F_Select(mask.Inner, whenFalse.Inner, whenTrue.Inner)};
    }

    template <bool X, bool Y, bool Z, bool W>
    FVector4A anemone_vectorcall Select(FVector4A whenFalse, FVector4A whenTrue)
    {
        return FVector4A{Private::Vector4F_Select<X, Y, Z, W>(whenFalse.Inner, whenTrue.Inner)};
    }
}

// Specific operations
namespace Anemone::Numerics
{
    inline FVector4A anemone_vectorcall Dot(FVector4A a, FVector4A b)
    {
        return FVector4A{Private::Vector4F_Dot4(a.Inner, b.Inner)};
    }

    inline FVector4A anemone_vectorcall Cross(FVector4A a, FVector4A b, FVector4A c)
    {
        return FVector4A{Private::Vector4F_Cross4(a.Inner, b.Inner, c.Inner)};
    }

    inline FVector4A anemone_vectorcall Length(FVector4A v)
    {
        return FVector4A{Private::Vector4F_Length4(v.Inner)};
    }

    inline FVector4A anemone_vectorcall LengthSquared(FVector4A v)
    {
        return FVector4A{Private::Vector4F_LengthSquared4(v.Inner)};
    }

    inline FVector4A anemone_vectorcall Normalize(FVector4A v)
    {
        return FVector4A{Private::Vector4F_Normalize4(v.Inner)};
    }
}

// Arithmetic operations
namespace Anemone::Numerics
{
    inline FVector4A anemone_vectorcall Add(FVector4A a, FVector4A b)
    {
        return FVector4A{Private::Vector4F_Add(a.Inner, b.Inner)};
    }

    inline FVector4A anemone_vectorcall Subtract(FVector4A a, FVector4A b)
    {
        return FVector4A{Private::Vector4F_Subtract(a.Inner, b.Inner)};
    }

    inline FVector4A anemone_vectorcall Multiply(FVector4A a, FVector4A b)
    {
        return FVector4A{Private::Vector4F_Multiply(a.Inner, b.Inner)};
    }

    inline FVector4A anemone_vectorcall Divide(FVector4A a, FVector4A b)
    {
        return FVector4A{Private::Vector4F_Divide(a.Inner, b.Inner)};
    }

    inline FVector4A anemone_vectorcall Negate(FVector4A v)
    {
        return FVector4A{Private::Vector4F_Negate(v.Inner)};
    }

    inline FVector4A anemone_vectorcall Reciprocal(FVector4A v)
    {
        return FVector4A{Private::Vector4F_Reciprocal(v.Inner)};
    }

    inline FVector4A anemone_vectorcall ReciprocalEst(FVector4A v)
    {
        return FVector4A{Private::Vector4F_ReciprocalEst(v.Inner)};
    }

    inline FVector4A anemone_vectorcall MultiplyAdd(FVector4A a, FVector4A b, FVector4A c)
    {
        return FVector4A{Private::Vector4F_MultiplyAdd(a.Inner, b.Inner, c.Inner)};
    }

    inline FVector4A anemone_vectorcall MultiplySubtract(FVector4A a, FVector4A b, FVector4A c)
    {
        return FVector4A{Private::Vector4F_MultiplySubtract(a.Inner, b.Inner, c.Inner)};
    }

    inline FVector4A anemone_vectorcall NegateMultiplyAdd(FVector4A a, FVector4A b, FVector4A c)
    {
        return FVector4A{Private::Vector4F_NegateMultiplyAdd(a.Inner, b.Inner, c.Inner)};
    }

    inline FVector4A anemone_vectorcall NegateMultiplySubtract(FVector4A a, FVector4A b, FVector4A c)
    {
        return FVector4A{Private::Vector4F_NegateMultiplySubtract(a.Inner, b.Inner, c.Inner)};
    }

    inline FVector4A anemone_vectorcall Min(FVector4A a, FVector4A b)
    {
        return FVector4A{Private::Vector4F_Min(a.Inner, b.Inner)};
    }

    inline FVector4A anemone_vectorcall Max(FVector4A a, FVector4A b)
    {
        return FVector4A{Private::Vector4F_Max(a.Inner, b.Inner)};
    }

    inline FVector4A anemone_vectorcall Abs(FVector4A v)
    {
        return FVector4A{Private::Vector4F_Abs(v.Inner)};
    }

    inline FVector4A anemone_vectorcall Square(FVector4A v)
    {
        return FVector4A{Private::Vector4F_Square(v.Inner)};
    }

    inline FVector4A anemone_vectorcall SignedSquare(FVector4A v)
    {
        return FVector4A{Private::Vector4F_SignedSquare(v.Inner)};
    }

    inline FVector4A anemone_vectorcall Clamp(FVector4A v, FVector4A lower, FVector4A upper)
    {
        return FVector4A{Private::Vector4F_Clamp(v.Inner, lower.Inner, upper.Inner)};
    }

    inline FVector4A anemone_vectorcall Saturate(FVector4A v)
    {
        return FVector4A{Private::Vector4F_Saturate(v.Inner)};
    }

    inline FVector4A anemone_vectorcall Sqrt(FVector4A v)
    {
        return FVector4A{Private::Vector4F_SquareRoot(v.Inner)};
    }

    inline FVector4A anemone_vectorcall SqrtEst(FVector4A v)
    {
        return FVector4A{Private::Vector4F_SquareRootEst(v.Inner)};
    }

    inline FVector4A anemone_vectorcall ReciprocalSqrt(FVector4A v)
    {
        return FVector4A{Private::Vector4F_ReciprocalSquareRoot(v.Inner)};
    }

    inline FVector4A anemone_vectorcall ReciprocalSqrtEst(FVector4A v)
    {
        return FVector4A{Private::Vector4F_ReciprocalSquareRootEst(v.Inner)};
    }

    inline FVector4A anemone_vectorcall Ceil(FVector4A v)
    {
        return FVector4A{Private::Vector4F_Ceil(v.Inner)};
    }

    inline FVector4A anemone_vectorcall Floor(FVector4A v)
    {
        return FVector4A{Private::Vector4F_Floor(v.Inner)};
    }

    inline FVector4A anemone_vectorcall Truncate(FVector4A v)
    {
        return FVector4A{Private::Vector4F_Truncate(v.Inner)};
    }

    inline FVector4A anemone_vectorcall Round(FVector4A v)
    {
        return FVector4A{Private::Vector4F_Round(v.Inner)};
    }

    inline FVector4A anemone_vectorcall Fraction(FVector4A v)
    {
        return FVector4A{Private::Vector4F_Fraction(v.Inner)};
    }

    inline FVector4A anemone_vectorcall Remainder(FVector4A x, FVector4A y)
    {
        return FVector4A{Private::Vector4F_Remainder(x.Inner, y.Inner)};
    }

    inline FVector4A anemone_vectorcall Repeat(FVector4A v, FVector4A length)
    {
        return FVector4A{Private::Vector4F_Repeat(v.Inner, length.Inner)};
    }

    inline FVector4A anemone_vectorcall Wrap(FVector4A v, FVector4A lower, FVector4A upper)
    {
        return FVector4A{Private::Vector4F_Wrap(v.Inner, lower.Inner, upper.Inner)};
    }

    inline FVector4A anemone_vectorcall Power(FVector4A x, FVector4A y)
    {
        return FVector4A{Private::Vector4F_Power(x.Inner, y.Inner)};
    }

    inline FVector4A anemone_vectorcall Exp(FVector4A v)
    {
        return FVector4A{Private::Vector4F_Exp(v.Inner)};
    }

    inline FVector4A anemone_vectorcall Exp2(FVector4A v)
    {
        return FVector4A{Private::Vector4F_Exp2(v.Inner)};
    }

    inline FVector4A anemone_vectorcall Exp10(FVector4A v)
    {
        return FVector4A{Private::Vector4F_Exp10(v.Inner)};
    }

    inline FVector4A anemone_vectorcall Log(FVector4A v)
    {
        return FVector4A{Private::Vector4F_Log(v.Inner)};
    }

    inline FVector4A anemone_vectorcall Log(FVector4A x, FVector4A base)
    {
        return FVector4A{Private::Vector4F_Log(x.Inner, base.Inner)};
    }

    inline FVector4A anemone_vectorcall Log2(FVector4A v)
    {
        return FVector4A{Private::Vector4F_Log2(v.Inner)};
    }

    inline FVector4A anemone_vectorcall Log10(FVector4A v)
    {
        return FVector4A{Private::Vector4F_Log10(v.Inner)};
    }

    inline SinCosResult<FVector4A> anemone_vectorcall SinCos(FVector4A v)
    {
        SinCosResult<FVector4A> result;
        Private::Vector4F_SinCos(result.Sin.Inner, result.Cos.Inner, v.Inner);
        return result;
    }

    inline FVector4A anemone_vectorcall Sin(FVector4A v)
    {
        return FVector4A{Private::Vector4F_Sin(v.Inner)};
    }

    inline FVector4A anemone_vectorcall Cos(FVector4A v)
    {
        return FVector4A{Private::Vector4F_Cos(v.Inner)};
    }

    inline FVector4A anemone_vectorcall Tan(FVector4A v)
    {
        return FVector4A{Private::Vector4F_Tan(v.Inner)};
    }

    inline FVector4A anemone_vectorcall Asin(FVector4A v)
    {
        return FVector4A{Private::Vector4F_Asin(v.Inner)};
    }

    inline FVector4A anemone_vectorcall Acos(FVector4A v)
    {
        return FVector4A{Private::Vector4F_Acos(v.Inner)};
    }

    inline FVector4A anemone_vectorcall Atan(FVector4A v)
    {
        return FVector4A{Private::Vector4F_Atan(v.Inner)};
    }

    inline FVector4A anemone_vectorcall Atan2(FVector4A y, FVector4A x)
    {
        return FVector4A{Private::Vector4F_Atan2(y.Inner, x.Inner)};
    }
}

// Masked operations
namespace Anemone::Numerics
{
    inline FMask4A anemone_vectorcall CompareEqual(FVector4A a, FVector4A b)
    {
        return FMask4A{Private::Vector4F_CompareEqual(a.Inner, b.Inner)};
    }

    inline FMask4A anemone_vectorcall CompareNotEqual(FVector4A a, FVector4A b)
    {
        return FMask4A{Private::Vector4F_CompareNotEqual(a.Inner, b.Inner)};
    }

    inline FMask4A anemone_vectorcall CompareLessThan(FVector4A a, FVector4A b)
    {
        return FMask4A{Private::Vector4F_CompareLessThan(a.Inner, b.Inner)};
    }

    inline FMask4A anemone_vectorcall CompareLessEqual(FVector4A a, FVector4A b)
    {
        return FMask4A{Private::Vector4F_CompareLessEqual(a.Inner, b.Inner)};
    }

    inline FMask4A anemone_vectorcall CompareGreaterThan(FVector4A a, FVector4A b)
    {
        return FMask4A{Private::Vector4F_CompareGreaterThan(a.Inner, b.Inner)};
    }

    inline FMask4A anemone_vectorcall CompareGreaterEqual(FVector4A a, FVector4A b)
    {
        return FMask4A{Private::Vector4F_CompareGreaterEqual(a.Inner, b.Inner)};
    }

    inline FMask4A anemone_vectorcall CompareNearEqual(FVector4A a, FVector4A b, FVector4A tolerance)
    {
        return FMask4A{Private::Vector4F_CompareNearEqual(a.Inner, b.Inner, tolerance.Inner)};
    }

    inline FMask4A anemone_vectorcall CompareNearEqual(FVector4A a, FVector4A b, float tolerance)
    {
        return FMask4A{Private::Vector4F_CompareNearEqual(a.Inner, b.Inner, tolerance)};
    }

    inline FMask4A anemone_vectorcall CompareNearEqual(FVector4A a, FVector4A b)
    {
        return FMask4A{Private::Vector4F_CompareNearEqual(a.Inner, b.Inner)};
    }

    inline FMask4A anemone_vectorcall CompareNearEqualPrecise(FVector4A a, FVector4A b)
    {
        return FMask4A{Private::Vector4F_CompareNearEqualPrecise(a.Inner, b.Inner)};
    }

    inline FMask4A anemone_vectorcall CompareNearZero(FVector4A v, FVector4A tolerance)
    {
        return FMask4A{Private::Vector4F_CompareNearZero(v.Inner, tolerance.Inner)};
    }

    inline FMask4A anemone_vectorcall CompareNearZero(FVector4A v, float tolerance)
    {
        return FMask4A{Private::Vector4F_CompareNearZero(v.Inner, tolerance)};
    }

    inline FMask4A anemone_vectorcall CompareNearZero(FVector4A v)
    {
        return FMask4A{Private::Vector4F_CompareNearZero(v.Inner)};
    }

    inline FMask4A anemone_vectorcall CompareNearZeroPrecise(FVector4A v)
    {
        return FMask4A{Private::Vector4F_CompareNearZeroPrecise(v.Inner)};
    }
}

namespace Anemone::Numerics
{
    inline FVector4A anemone_vectorcall PreciseLerp(FVector4A a, FVector4A b, FVector4A t)
    {
        return FVector4A{Private::Vector4F_PreciseLerp(a.Inner, b.Inner, t.Inner)};
    }

    inline FVector4A anemone_vectorcall PreciseLerp(FVector4A a, FVector4A b, float t)
    {
        return FVector4A{Private::Vector4F_PreciseLerp(a.Inner, b.Inner, t)};
    }

    inline FVector4A anemone_vectorcall Lerp(FVector4A a, FVector4A b, FVector4A t)
    {
        return FVector4A{Private::Vector4F_Lerp(a.Inner, b.Inner, t.Inner)};
    }

    inline FVector4A anemone_vectorcall Lerp(FVector4A a, FVector4A b, float t)
    {
        return FVector4A{Private::Vector4F_Lerp(a.Inner, b.Inner, t)};
    }

    inline FVector4A anemone_vectorcall PreciseBilinearLerp(FVector4A ax, FVector4A bx, FVector4A ay, FVector4A by, FVector4A tx, FVector4A ty)
    {
        return FVector4A{Private::Vector4F_PreciseBilinearLerp(ax.Inner, bx.Inner, ay.Inner, by.Inner, tx.Inner, ty.Inner)};
    }

    inline FVector4A anemone_vectorcall PreciseBilinearLerp(FVector4A ax, FVector4A bx, FVector4A ay, FVector4A by, float tx, float ty)
    {
        return FVector4A{Private::Vector4F_PreciseBilinearLerp(ax.Inner, bx.Inner, ay.Inner, by.Inner, tx, ty)};
    }

    inline FVector4A anemone_vectorcall BilinearLerp(FVector4A ax, FVector4A bx, FVector4A ay, FVector4A by, FVector4A tx, FVector4A ty)
    {
        return FVector4A{Private::Vector4F_BilinearLerp(ax.Inner, bx.Inner, ay.Inner, by.Inner, tx.Inner, ty.Inner)};
    }

    inline FVector4A anemone_vectorcall BilinearLerp(FVector4A ax, FVector4A bx, FVector4A ay, FVector4A by, float tx, float ty)
    {
        return FVector4A{Private::Vector4F_BilinearLerp(ax.Inner, bx.Inner, ay.Inner, by.Inner, tx, ty)};
    }

    inline FVector4A anemone_vectorcall Barycentric(FVector4A a, FVector4A b, FVector4A c, FVector4A u, FVector4A v)
    {
        return FVector4A{Private::Vector4F_Barycentric(a.Inner, b.Inner, c.Inner, u.Inner, v.Inner)};
    }

    inline FVector4A anemone_vectorcall Barycentric(FVector4A a, FVector4A b, FVector4A c, float u, float v)
    {
        return FVector4A{Private::Vector4F_Barycentric(a.Inner, b.Inner, c.Inner, u, v)};
    }

    inline FVector4A anemone_vectorcall Unlerp(FVector4A value, FVector4A min, FVector4A max)
    {
        return FVector4A{Private::Vector4F_Unlerp(value.Inner, min.Inner, max.Inner)};
    }

    inline FVector4A anemone_vectorcall Map(FVector4A value, FVector4A fromLower, FVector4A fromUpper, FVector4A toLower, FVector4A toUpper)
    {
        return FVector4A{Private::Vector4F_Map(value.Inner, fromLower.Inner, fromUpper.Inner, toLower.Inner, toUpper.Inner)};
    }

    inline FVector4A anemone_vectorcall PreciseMap(FVector4A value, FVector4A fromLower, FVector4A fromUpper, FVector4A toLower, FVector4A toUpper)
    {
        return FVector4A{Private::Vector4F_PreciseMap(value.Inner, fromLower.Inner, fromUpper.Inner, toLower.Inner, toUpper.Inner)};
    }

    inline FVector4A anemone_vectorcall Bezier(FVector4A a, FVector4A b, FVector4A c, FVector4A t)
    {
        return FVector4A{Private::Vector4F_BezierPosition(a.Inner, b.Inner, c.Inner, t.Inner)};
    }

    inline FVector4A anemone_vectorcall Bezier(FVector4A a, FVector4A b, FVector4A c, float t)
    {
        return FVector4A{Private::Vector4F_BezierPosition(a.Inner, b.Inner, c.Inner, t)};
    }

    inline FVector4A anemone_vectorcall Bezier(FVector4A a, FVector4A b, FVector4A c, FVector4A d, FVector4A t)
    {
        return FVector4A{Private::Vector4F_BezierPosition(a.Inner, b.Inner, c.Inner, d.Inner, t.Inner)};
    }

    inline FVector4A anemone_vectorcall Bezier(FVector4A a, FVector4A b, FVector4A c, FVector4A d, float t)
    {
        return FVector4A{Private::Vector4F_BezierPosition(a.Inner, b.Inner, c.Inner, d.Inner, t)};
    }

    inline FVector4A anemone_vectorcall Hermite(FVector4A point0, FVector4A tangent0, FVector4A point1, FVector4A tangent1, FVector4A t)
    {
        return FVector4A{Private::Vector4F_Hermite(point0.Inner, tangent0.Inner, point1.Inner, tangent1.Inner, t.Inner)};
    }

    inline FVector4A anemone_vectorcall Hermite(FVector4A point0, FVector4A tangent0, FVector4A point1, FVector4A tangent1, float t)
    {
        return FVector4A{Private::Vector4F_Hermite(point0.Inner, tangent0.Inner, point1.Inner, tangent1.Inner, t)};
    }

    inline FVector4A anemone_vectorcall CatmullRom(FVector4A p0, FVector4A p1, FVector4A p2, FVector4A p3, FVector4A t)
    {
        return FVector4A{Private::Vector4F_CatmullRom(p0.Inner, p1.Inner, p2.Inner, p3.Inner, t.Inner)};
    }

    inline FVector4A anemone_vectorcall CatmullRom(FVector4A p0, FVector4A p1, FVector4A p2, FVector4A p3, float t)
    {
        return FVector4A{Private::Vector4F_CatmullRom(p0.Inner, p1.Inner, p2.Inner, p3.Inner, t)};
    }
}
