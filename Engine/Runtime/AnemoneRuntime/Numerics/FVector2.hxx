#pragma once
#include "AnemoneRuntime/Numerics/Types.hxx"
#include "AnemoneRuntime/Numerics/Simd.hxx"

// Type
namespace Anemone::Numerics
{
    struct FVector2;
    struct FMask2A;

    struct FVector2A final
    {
        Private::SimdVector4F Inner;

        FVector2A(Private::SimdVector4F inner)
            : Inner{inner}
        {
        }

        FVector2A() = default;
        explicit FVector2A(float x, float y);
        explicit FVector2A(float value);

        static FVector2A anemone_vectorcall Create(float x, float y);
        static FVector2A anemone_vectorcall Splat(float value);

        static FVector2A anemone_vectorcall Zero();
        static FVector2A anemone_vectorcall One();

        static FVector2A anemone_vectorcall UnitX();
        static FVector2A anemone_vectorcall UnitY();

        static FVector2A anemone_vectorcall NegativeUnitX();
        static FVector2A anemone_vectorcall NegativeUnitY();

        static FVector2A anemone_vectorcall Load(FVector2 const& source);
        static void anemone_vectorcall Store(FVector2& destination, FVector2A source);
    };

    FVector2A anemone_vectorcall FromVector2(FVector2 const& source);
    FVector2 anemone_vectorcall ToVector2(FVector2A source);
}

// Component-wise operations
namespace Anemone::Numerics
{
    template <size_t N>
    float anemone_vectorcall Extract(FVector2A a)
        requires(N < 2);

    template <size_t N>
    FVector2A anemone_vectorcall Insert(FVector2A a, float b)
        requires(N < 2);

    template <size_t N>
    FVector2A anemone_vectorcall Broadcast(FVector2A a)
        requires(N < 2);

    template <size_t X, size_t Y>
    FVector2A anemone_vectorcall Permute(FVector2A a)
        requires((X < 2) and (Y < 2));

    FVector2A anemone_vectorcall Select(FMask2A mask, FVector2A whenFalse, FVector2A whenTrue);

    template <bool X, bool Y>
    FVector2A anemone_vectorcall Select(FVector2A whenFalse, FVector2A whenTrue);
}

// Specific operations
namespace Anemone::Numerics
{
    FVector2A anemone_vectorcall Dot(FVector2A a, FVector2A b);
    FVector2A anemone_vectorcall Cross(FVector2A a, FVector2A b);
    FVector2A anemone_vectorcall Length(FVector2A v);
    FVector2A anemone_vectorcall LengthSquared(FVector2A v);
    FVector2A anemone_vectorcall Normalize(FVector2A v);
}

// Arithmetic operations
namespace Anemone::Numerics
{
    FVector2A anemone_vectorcall Add(FVector2A a, FVector2A b);
    FVector2A anemone_vectorcall Subtract(FVector2A a, FVector2A b);
    FVector2A anemone_vectorcall Multiply(FVector2A a, FVector2A b);
    FVector2A anemone_vectorcall Divide(FVector2A a, FVector2A b);
    FVector2A anemone_vectorcall Negate(FVector2A v);
    FVector2A anemone_vectorcall Reciprocal(FVector2A v);
    FVector2A anemone_vectorcall ReciprocalEst(FVector2A v);
    FVector2A anemone_vectorcall MultiplyAdd(FVector2A a, FVector2A b, FVector2A c);
    FVector2A anemone_vectorcall MultiplySubtract(FVector2A a, FVector2A b, FVector2A c);
    FVector2A anemone_vectorcall NegateMultiplyAdd(FVector2A a, FVector2A b, FVector2A c);
    FVector2A anemone_vectorcall NegateMultiplySubtract(FVector2A a, FVector2A b, FVector2A c);

    FVector2A anemone_vectorcall Min(FVector2A a, FVector2A b);
    FVector2A anemone_vectorcall Max(FVector2A a, FVector2A b);
    FVector2A anemone_vectorcall Abs(FVector2A v);
    FVector2A anemone_vectorcall Square(FVector2A v);
    FVector2A anemone_vectorcall SignedSquare(FVector2A v);
    FVector2A anemone_vectorcall Clamp(FVector2A v, FVector2A lower, FVector2A upper);
    FVector2A anemone_vectorcall Saturate(FVector2A v);
    FVector2A anemone_vectorcall Sqrt(FVector2A v);
    FVector2A anemone_vectorcall SqrtEst(FVector2A v);
    FVector2A anemone_vectorcall ReciprocalSqrt(FVector2A v);
    FVector2A anemone_vectorcall ReciprocalSqrtEst(FVector2A v);
    FVector2A anemone_vectorcall Ceil(FVector2A v);
    FVector2A anemone_vectorcall Floor(FVector2A v);
    FVector2A anemone_vectorcall Truncate(FVector2A v);
    FVector2A anemone_vectorcall Round(FVector2A v);
    FVector2A anemone_vectorcall Fraction(FVector2A v);
    FVector2A anemone_vectorcall Remainder(FVector2A x, FVector2A y);
    FVector2A anemone_vectorcall Repeat(FVector2A v, FVector2A length);
    FVector2A anemone_vectorcall Wrap(FVector2A v, FVector2A lower, FVector2A upper);
    FVector2A anemone_vectorcall Power(FVector2A x, FVector2A y);
    FVector2A anemone_vectorcall Exp(FVector2A v);
    FVector2A anemone_vectorcall Exp2(FVector2A v);
    FVector2A anemone_vectorcall Exp10(FVector2A v);
    FVector2A anemone_vectorcall Log(FVector2A v);
    FVector2A anemone_vectorcall Log(FVector2A x, FVector2A base);
    FVector2A anemone_vectorcall Log2(FVector2A v);
    FVector2A anemone_vectorcall Log10(FVector2A v);
    SinCosResult<FVector2A> anemone_vectorcall SinCos(FVector2A v);
    FVector2A anemone_vectorcall Sin(FVector2A v);
    FVector2A anemone_vectorcall Cos(FVector2A v);
    FVector2A anemone_vectorcall Tan(FVector2A v);
    FVector2A anemone_vectorcall Asin(FVector2A v);
    FVector2A anemone_vectorcall Acos(FVector2A v);
    FVector2A anemone_vectorcall Atan(FVector2A v);
    FVector2A anemone_vectorcall Atan2(FVector2A y, FVector2A x);

}

// Masked operations
namespace Anemone::Numerics
{
    FMask2A anemone_vectorcall CompareEqual(FVector2A a, FVector2A b);
    FMask2A anemone_vectorcall CompareNotEqual(FVector2A a, FVector2A b);
    FMask2A anemone_vectorcall CompareLessThan(FVector2A a, FVector2A b);
    FMask2A anemone_vectorcall CompareLessEqual(FVector2A a, FVector2A b);
    FMask2A anemone_vectorcall CompareGreaterThan(FVector2A a, FVector2A b);
    FMask2A anemone_vectorcall CompareGreaterEqual(FVector2A a, FVector2A b);

    FMask2A anemone_vectorcall CompareNearEqual(FVector2A a, FVector2A b, FVector2A tolerance);
    FMask2A anemone_vectorcall CompareNearEqual(FVector2A a, FVector2A b, float tolerance);
    FMask2A anemone_vectorcall CompareNearEqual(FVector2A a, FVector2A b);
    FMask2A anemone_vectorcall CompareNearEqualPrecise(FVector2A a, FVector2A b);
    FMask2A anemone_vectorcall CompareNearZero(FVector2A v, FVector2A tolerance);
    FMask2A anemone_vectorcall CompareNearZero(FVector2A v, float tolerance);
    FMask2A anemone_vectorcall CompareNearZero(FVector2A v);
    FMask2A anemone_vectorcall CompareNearZeroPrecise(FVector2A v);
}

#include "AnemoneRuntime/Numerics/Private/FVector2.Avx.hxx"

namespace Anemone::Numerics
{
    inline FVector2A anemone_vectorcall Reflect(FVector2A incident, FVector2A normal)
    {
        return FVector2A{Private::Vector4F_Reflect2(incident.Inner, normal.Inner)};
    }

    inline FVector2A anemone_vectorcall Refract(FVector2A incident, FVector2A normal, FVector2A index)
    {
        return FVector2A{Private::Vector4F_Refract2(incident.Inner, normal.Inner, index.Inner)};
    }
}
