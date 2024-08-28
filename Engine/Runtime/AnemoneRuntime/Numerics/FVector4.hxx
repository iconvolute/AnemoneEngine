#pragma once
#include "AnemoneRuntime/Numerics/Types.hxx"
#include "AnemoneRuntime/Numerics/Simd.hxx"

// Type
namespace Anemone::Numerics
{
    struct FVector4;
    struct FMask4A;

    struct FVector4A final
    {
        Private::SimdVector4F Inner;

        FVector4A(Private::SimdVector4F inner)
            : Inner{inner}
        {
        }

        FVector4A() = default;
        explicit FVector4A(float x, float y, float z, float w);
        explicit FVector4A(float value);

        static FVector4A anemone_vectorcall Create(float x, float y, float z, float w);
        static FVector4A anemone_vectorcall Splat(float value);

        static FVector4A anemone_vectorcall Zero();
        static FVector4A anemone_vectorcall One();

        static FVector4A anemone_vectorcall UnitX();
        static FVector4A anemone_vectorcall UnitY();
        static FVector4A anemone_vectorcall UnitZ();
        static FVector4A anemone_vectorcall UnitW();

        static FVector4A anemone_vectorcall NegativeUnitX();
        static FVector4A anemone_vectorcall NegativeUnitY();
        static FVector4A anemone_vectorcall NegativeUnitZ();
        static FVector4A anemone_vectorcall NegativeUnitW();

        static FVector4A anemone_vectorcall Load(FVector4 const& source);
        static void anemone_vectorcall Store(FVector4& destination, FVector4A source);
    };

    FVector4A anemone_vectorcall FromVector4(FVector4 const& source);
    FVector4 anemone_vectorcall ToVector4(FVector4A source);
}

// Component-wise operations
namespace Anemone::Numerics
{
    template <size_t N>
    float anemone_vectorcall Extract(FVector4A a)
        requires(N < 4);

    template <size_t N>
    FVector4A anemone_vectorcall Insert(FVector4A a, float b)
        requires(N < 4);

    template <size_t N>
    FVector4A anemone_vectorcall Broadcast(FVector4A a)
        requires(N < 4);

    template <size_t X, size_t Y, size_t Z, size_t W>
    FVector4A anemone_vectorcall Permute(FVector4A a)
        requires((X < 4) and (Y < 4) and (Z < 4) and (W < 4));

    FVector4A anemone_vectorcall Select(FMask4A mask, FVector4A whenFalse, FVector4A whenTrue);

    template <bool X, bool Y, bool Z, bool W>
    FVector4A anemone_vectorcall Select(FVector4A whenFalse, FVector4A whenTrue);
}

// Specific operations
namespace Anemone::Numerics
{
    FVector4A anemone_vectorcall Dot(FVector4A a, FVector4A b);
    FVector4A anemone_vectorcall Cross(FVector4A a, FVector4A b, FVector4A c);
    FVector4A anemone_vectorcall Length(FVector4A v);
    FVector4A anemone_vectorcall LengthSquared(FVector4A v);
    FVector4A anemone_vectorcall Normalize(FVector4A v);
}

// Arithmetic operations
namespace Anemone::Numerics
{
    FVector4A anemone_vectorcall Add(FVector4A a, FVector4A b);
    FVector4A anemone_vectorcall Subtract(FVector4A a, FVector4A b);
    FVector4A anemone_vectorcall Multiply(FVector4A a, FVector4A b);
    FVector4A anemone_vectorcall Divide(FVector4A a, FVector4A b);
    FVector4A anemone_vectorcall Negate(FVector4A v);
    FVector4A anemone_vectorcall Reciprocal(FVector4A v);
    FVector4A anemone_vectorcall ReciprocalEst(FVector4A v);
    FVector4A anemone_vectorcall MultiplyAdd(FVector4A a, FVector4A b, FVector4A c);
    FVector4A anemone_vectorcall MultiplySubtract(FVector4A a, FVector4A b, FVector4A c);
    FVector4A anemone_vectorcall NegateMultiplyAdd(FVector4A a, FVector4A b, FVector4A c);
    FVector4A anemone_vectorcall NegateMultiplySubtract(FVector4A a, FVector4A b, FVector4A c);

    FVector4A anemone_vectorcall Min(FVector4A a, FVector4A b);
    FVector4A anemone_vectorcall Max(FVector4A a, FVector4A b);
    FVector4A anemone_vectorcall Abs(FVector4A v);
    FVector4A anemone_vectorcall Square(FVector4A v);
    FVector4A anemone_vectorcall SignedSquare(FVector4A v);
    FVector4A anemone_vectorcall Clamp(FVector4A v, FVector4A lower, FVector4A upper);
    FVector4A anemone_vectorcall Saturate(FVector4A v);
    FVector4A anemone_vectorcall Sqrt(FVector4A v);
    FVector4A anemone_vectorcall SqrtEst(FVector4A v);
    FVector4A anemone_vectorcall ReciprocalSqrt(FVector4A v);
    FVector4A anemone_vectorcall ReciprocalSqrtEst(FVector4A v);
    FVector4A anemone_vectorcall Ceil(FVector4A v);
    FVector4A anemone_vectorcall Floor(FVector4A v);
    FVector4A anemone_vectorcall Truncate(FVector4A v);
    FVector4A anemone_vectorcall Round(FVector4A v);
    FVector4A anemone_vectorcall Fraction(FVector4A v);
    FVector4A anemone_vectorcall Remainder(FVector4A x, FVector4A y);
    FVector4A anemone_vectorcall Repeat(FVector4A v, FVector4A length);
    FVector4A anemone_vectorcall Wrap(FVector4A v, FVector4A lower, FVector4A upper);
    FVector4A anemone_vectorcall Power(FVector4A x, FVector4A y);
    FVector4A anemone_vectorcall Exp(FVector4A v);
    FVector4A anemone_vectorcall Exp2(FVector4A v);
    FVector4A anemone_vectorcall Exp10(FVector4A v);
    FVector4A anemone_vectorcall Log(FVector4A v);
    FVector4A anemone_vectorcall Log(FVector4A x, FVector4A base);
    FVector4A anemone_vectorcall Log2(FVector4A v);
    FVector4A anemone_vectorcall Log10(FVector4A v);
    SinCosResult<FVector4A> anemone_vectorcall SinCos(FVector4A v);
    FVector4A anemone_vectorcall Sin(FVector4A v);
    FVector4A anemone_vectorcall Cos(FVector4A v);
    FVector4A anemone_vectorcall Tan(FVector4A v);
    FVector4A anemone_vectorcall Asin(FVector4A v);
    FVector4A anemone_vectorcall Acos(FVector4A v);
    FVector4A anemone_vectorcall Atan(FVector4A v);
    FVector4A anemone_vectorcall Atan2(FVector4A y, FVector4A x);

}

// Masked operations
namespace Anemone::Numerics
{
    FMask4A anemone_vectorcall CompareEqual(FVector4A a, FVector4A b);
    FMask4A anemone_vectorcall CompareNotEqual(FVector4A a, FVector4A b);
    FMask4A anemone_vectorcall CompareLessThan(FVector4A a, FVector4A b);
    FMask4A anemone_vectorcall CompareLessEqual(FVector4A a, FVector4A b);
    FMask4A anemone_vectorcall CompareGreaterThan(FVector4A a, FVector4A b);
    FMask4A anemone_vectorcall CompareGreaterEqual(FVector4A a, FVector4A b);

    FMask4A anemone_vectorcall CompareInRange(FVector4A value, FVector4A lower, FVector4A upper);
    FMask4A anemone_vectorcall CompareInBounds(FVector4A value, FVector4A bound);

    FMask4A anemone_vectorcall CompareNearEqual(FVector4A a, FVector4A b, FVector4A tolerance);
    FMask4A anemone_vectorcall CompareNearEqual(FVector4A a, FVector4A b, float tolerance);
    FMask4A anemone_vectorcall CompareNearEqual(FVector4A a, FVector4A b);
    FMask4A anemone_vectorcall CompareNearEqualPrecise(FVector4A a, FVector4A b);
    FMask4A anemone_vectorcall CompareNearZero(FVector4A v, FVector4A tolerance);
    FMask4A anemone_vectorcall CompareNearZero(FVector4A v, float tolerance);
    FMask4A anemone_vectorcall CompareNearZero(FVector4A v);
    FMask4A anemone_vectorcall CompareNearZeroPrecise(FVector4A v);
}

namespace Anemone::Numerics
{
    FVector4A anemone_vectorcall PreciseLerp(FVector4A a, FVector4A b, FVector4A t);
    FVector4A anemone_vectorcall PreciseLerp(FVector4A a, FVector4A b, float t);

    FVector4A anemone_vectorcall Lerp(FVector4A a, FVector4A b, FVector4A t);
    FVector4A anemone_vectorcall Lerp(FVector4A a, FVector4A b, float t);

    FVector4A anemone_vectorcall PreciseBilinearLerp(FVector4A ax, FVector4A bx, FVector4A ay, FVector4A by, FVector4A tx, FVector4A ty);
    FVector4A anemone_vectorcall PreciseBilinearLerp(FVector4A ax, FVector4A bx, FVector4A ay, FVector4A by, float tx, float ty);

    FVector4A anemone_vectorcall BilinearLerp(FVector4A ax, FVector4A bx, FVector4A ay, FVector4A by, FVector4A tx, FVector4A ty);
    FVector4A anemone_vectorcall BilinearLerp(FVector4A ax, FVector4A bx, FVector4A ay, FVector4A by, float tx, float ty);

    FVector4A anemone_vectorcall Barycentric(FVector4A a, FVector4A b, FVector4A c, FVector4A u, FVector4A v);
    FVector4A anemone_vectorcall Barycentric(FVector4A a, FVector4A b, FVector4A c, float u, float v);

    FVector4A anemone_vectorcall Unlerp(FVector4A value, FVector4A min, FVector4A max);

    FVector4A anemone_vectorcall Map(FVector4A value, FVector4A fromLower, FVector4A fromUpper, FVector4A toLower, FVector4A toUpper);

    FVector4A anemone_vectorcall PreciseMap(FVector4A value, FVector4A fromLower, FVector4A fromUpper, FVector4A toLower, FVector4A toUpper);

    FVector4A anemone_vectorcall Bezier(FVector4A a, FVector4A b, FVector4A c, FVector4A t);
    FVector4A anemone_vectorcall Bezier(FVector4A a, FVector4A b, FVector4A c, float t);

    FVector4A anemone_vectorcall Bezier(FVector4A a, FVector4A b, FVector4A c, FVector4A d, FVector4A t);
    FVector4A anemone_vectorcall Bezier(FVector4A a, FVector4A b, FVector4A c, FVector4A d, float t);

    FVector4A anemone_vectorcall Hermite(FVector4A point0, FVector4A tangent0, FVector4A point1, FVector4A tangent1, FVector4A t);
    FVector4A anemone_vectorcall Hermite(FVector4A point0, FVector4A tangent0, FVector4A point1, FVector4A tangent1, float t);

    FVector4A anemone_vectorcall CatmullRom(FVector4A p0, FVector4A p1, FVector4A p2, FVector4A p3, FVector4A t);
    FVector4A anemone_vectorcall CatmullRom(FVector4A p0, FVector4A p1, FVector4A p2, FVector4A p3, float t);
}

#include "AnemoneRuntime/Numerics/Private/FVector4.Avx.hxx"

namespace Anemone::Numerics
{
    inline FVector4A anemone_vectorcall Reflect(FVector4A incident, FVector4A normal)
    {
        return FVector4A{Private::Vector4F_Reflect4(incident.Inner, normal.Inner)};
    }

    inline FVector4A anemone_vectorcall Refract(FVector4A incident, FVector4A normal, FVector4A index)
    {
        return FVector4A{Private::Vector4F_Refract4(incident.Inner, normal.Inner, index.Inner)};
    }
}
