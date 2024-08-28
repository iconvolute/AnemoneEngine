#pragma once
#include "AnemoneRuntime/Numerics/Types.hxx"
#include "AnemoneRuntime/Numerics/Simd.hxx"

// Type
namespace Anemone::Numerics
{
    struct FVector3;
    struct FVector4;
    struct FQuaternionA;
    struct FMatrix4x4A;
    struct FMask3A;

    struct FVector3A final
    {
        Private::SimdVector4F Inner;

        FVector3A(Private::SimdVector4F inner)
            : Inner{inner}
        {
        }

        FVector3A() = default;
        explicit FVector3A(float x, float y, float z);
        explicit FVector3A(float value);

        static FVector3A anemone_vectorcall Create(float x, float y, float z);
        static FVector3A anemone_vectorcall Splat(float value);

        static FVector3A anemone_vectorcall Zero();
        static FVector3A anemone_vectorcall One();

        static FVector3A anemone_vectorcall UnitX();
        static FVector3A anemone_vectorcall UnitY();
        static FVector3A anemone_vectorcall UnitZ();

        static FVector3A anemone_vectorcall NegativeUnitX();
        static FVector3A anemone_vectorcall NegativeUnitY();
        static FVector3A anemone_vectorcall NegativeUnitZ();

        static FVector3A anemone_vectorcall Load(FVector3 const& source);
        static void anemone_vectorcall Store(FVector3& destination, FVector3A source);
    };

    FVector3A anemone_vectorcall FromVector3(FVector3 const& source);
    FVector3 anemone_vectorcall ToVector3(FVector3A source);
}

// Component-wise operations
namespace Anemone::Numerics
{
    template <size_t N>
    float anemone_vectorcall Extract(FVector3A a)
        requires(N < 3);

    template <size_t N>
    FVector3A anemone_vectorcall Insert(FVector3A a, float b)
        requires(N < 3);

    template <size_t N>
    FVector3A anemone_vectorcall Broadcast(FVector3A a)
        requires(N < 3);

    template <size_t X, size_t Y, size_t Z>
    FVector3A anemone_vectorcall Permute(FVector3A a)
        requires((X < 3) and (Y < 3) and (Z < 3));

    FVector3A anemone_vectorcall Select(FMask3A mask, FVector3A whenFalse, FVector3A whenTrue);

    template <bool X, bool Y, bool Z>
    FVector3A anemone_vectorcall Select(FVector3A whenFalse, FVector3A whenTrue);
}

// Specific operations
namespace Anemone::Numerics
{
    FVector3A anemone_vectorcall Dot(FVector3A a, FVector3A b);
    FVector3A anemone_vectorcall Cross(FVector3A a, FVector3A b);
    FVector3A anemone_vectorcall Length(FVector3A v);
    FVector3A anemone_vectorcall LengthSquared(FVector3A v);
    FVector3A anemone_vectorcall Normalize(FVector3A v);
    FVector3A anemone_vectorcall Rotate(FQuaternionA q, FVector3A v);
    FVector3A anemone_vectorcall InverseRotate(FQuaternionA q, FVector3A v);
    FVector3A anemone_vectorcall Transform(FVector3A v, FMatrix4x4A m);
}

// Arithmetic operations
namespace Anemone::Numerics
{
    FVector3A anemone_vectorcall Add(FVector3A a, FVector3A b);
    FVector3A anemone_vectorcall Subtract(FVector3A a, FVector3A b);
    FVector3A anemone_vectorcall Multiply(FVector3A a, FVector3A b);
    FVector3A anemone_vectorcall Divide(FVector3A a, FVector3A b);
    FVector3A anemone_vectorcall Negate(FVector3A v);
    FVector3A anemone_vectorcall Reciprocal(FVector3A v);
    FVector3A anemone_vectorcall ReciprocalEst(FVector3A v);
    FVector3A anemone_vectorcall MultiplyAdd(FVector3A a, FVector3A b, FVector3A c);
    FVector3A anemone_vectorcall MultiplySubtract(FVector3A a, FVector3A b, FVector3A c);
    FVector3A anemone_vectorcall NegateMultiplyAdd(FVector3A a, FVector3A b, FVector3A c);
    FVector3A anemone_vectorcall NegateMultiplySubtract(FVector3A a, FVector3A b, FVector3A c);

    FVector3A anemone_vectorcall Min(FVector3A a, FVector3A b);
    FVector3A anemone_vectorcall Max(FVector3A a, FVector3A b);
    FVector3A anemone_vectorcall Abs(FVector3A v);
    FVector3A anemone_vectorcall Square(FVector3A v);
    FVector3A anemone_vectorcall SignedSquare(FVector3A v);
    FVector3A anemone_vectorcall Clamp(FVector3A v, FVector3A lower, FVector3A upper);
    FVector3A anemone_vectorcall Saturate(FVector3A v);
    FVector3A anemone_vectorcall Sqrt(FVector3A v);
    FVector3A anemone_vectorcall SqrtEst(FVector3A v);
    FVector3A anemone_vectorcall ReciprocalSqrt(FVector3A v);
    FVector3A anemone_vectorcall ReciprocalSqrtEst(FVector3A v);
    FVector3A anemone_vectorcall Ceil(FVector3A v);
    FVector3A anemone_vectorcall Floor(FVector3A v);
    FVector3A anemone_vectorcall Truncate(FVector3A v);
    FVector3A anemone_vectorcall Round(FVector3A v);
    FVector3A anemone_vectorcall Fraction(FVector3A v);
    FVector3A anemone_vectorcall Remainder(FVector3A x, FVector3A y);
    FVector3A anemone_vectorcall Repeat(FVector3A v, FVector3A length);
    FVector3A anemone_vectorcall Wrap(FVector3A v, FVector3A lower, FVector3A upper);
    FVector3A anemone_vectorcall Power(FVector3A x, FVector3A y);
    FVector3A anemone_vectorcall Exp(FVector3A v);
    FVector3A anemone_vectorcall Exp2(FVector3A v);
    FVector3A anemone_vectorcall Exp10(FVector3A v);
    FVector3A anemone_vectorcall Log(FVector3A v);
    FVector3A anemone_vectorcall Log(FVector3A x, FVector3A base);
    FVector3A anemone_vectorcall Log2(FVector3A v);
    FVector3A anemone_vectorcall Log10(FVector3A v);
    SinCosResult<FVector3A> anemone_vectorcall SinCos(FVector3A v);
    FVector3A anemone_vectorcall Sin(FVector3A v);
    FVector3A anemone_vectorcall Cos(FVector3A v);
    FVector3A anemone_vectorcall Tan(FVector3A v);
    FVector3A anemone_vectorcall Asin(FVector3A v);
    FVector3A anemone_vectorcall Acos(FVector3A v);
    FVector3A anemone_vectorcall Atan(FVector3A v);
    FVector3A anemone_vectorcall Atan2(FVector3A y, FVector3A x);

}

// Masked operations
namespace Anemone::Numerics
{
    FMask3A anemone_vectorcall CompareEqual(FVector3A a, FVector3A b);
    FMask3A anemone_vectorcall CompareNotEqual(FVector3A a, FVector3A b);
    FMask3A anemone_vectorcall CompareLessThan(FVector3A a, FVector3A b);
    FMask3A anemone_vectorcall CompareLessEqual(FVector3A a, FVector3A b);
    FMask3A anemone_vectorcall CompareGreaterThan(FVector3A a, FVector3A b);
    FMask3A anemone_vectorcall CompareGreaterEqual(FVector3A a, FVector3A b);

    FMask3A anemone_vectorcall CompareInBounds(FVector3A value, FVector3A lower, FVector3A upper);
    FMask3A anemone_vectorcall CompareInBounds(FVector3A value, FVector3A bound);

    FMask3A anemone_vectorcall CompareNearEqual(FVector3A a, FVector3A b, FVector3A tolerance);
    FMask3A anemone_vectorcall CompareNearEqual(FVector3A a, FVector3A b, float tolerance);
    FMask3A anemone_vectorcall CompareNearEqual(FVector3A a, FVector3A b);
    FMask3A anemone_vectorcall CompareNearEqualPrecise(FVector3A a, FVector3A b);
    FMask3A anemone_vectorcall CompareNearZero(FVector3A v, FVector3A tolerance);
    FMask3A anemone_vectorcall CompareNearZero(FVector3A v, float tolerance);
    FMask3A anemone_vectorcall CompareNearZero(FVector3A v);
    FMask3A anemone_vectorcall CompareNearZeroPrecise(FVector3A v);
}

#include "AnemoneRuntime/Numerics/Private/FVector3.Avx.hxx"

namespace Anemone::Numerics
{
    anemone_noinline inline FVector3A anemone_vectorcall Reflect(FVector3A incident, FVector3A normal)
    {
        return FVector3A{Private::Vector4F_Reflect3(incident.Inner, normal.Inner)};
    }

    anemone_noinline inline FVector3A anemone_vectorcall Refract(FVector3A incident, FVector3A normal, FVector3A index)
    {
        return FVector3A{Private::Vector4F_Refract3(incident.Inner, normal.Inner, index.Inner)};
    }
}
