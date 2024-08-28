#pragma once
#include "AnemoneRuntime/Numerics/Types.hxx"
#include "AnemoneRuntime/Numerics/Simd.hxx"

namespace Anemone::Numerics
{
    struct FVector4A;
    struct FVector3A;
    struct FMask4A;

    struct FQuaternionA final
    {
        Private::SimdVector4F Inner;

        FQuaternionA(Private::SimdVector4F inner)
            : Inner{inner}
        {
        }

        FQuaternionA() = default;
        explicit FQuaternionA(float x, float y, float z, float w);
        explicit FQuaternionA(float value);

        static FQuaternionA anemone_vectorcall Create(float x, float y, float z, float w);
        static FQuaternionA anemone_vectorcall Splat(float value);

        static FQuaternionA anemone_vectorcall Identity();

        static FQuaternionA anemone_vectorcall FromAxisAngle(FVector3A axis, float angle);
        static FQuaternionA anemone_vectorcall FromNormalAngle(FVector3A normal, float angle);
        static FQuaternionA anemone_vectorcall FromEulerAngles(float pitch, float yaw, float roll);
        static FQuaternionA anemone_vectorcall FromEulerAngles(FVector3A pitchYawRoll);

        static FQuaternionA anemone_vectorcall Load(FQuaternion const& source);
        static void anemone_vectorcall Store(FQuaternion& destination, FQuaternionA source);
    };

    template <size_t N>
    float anemone_vectorcall Extract(FQuaternionA a)
        requires(N < 4);

    template <size_t N>
    FQuaternionA anemone_vectorcall Insert(FQuaternionA a, float b)
        requires(N < 4);

    FMask4A anemone_vectorcall CompareEqual(FQuaternionA a, FQuaternionA b);
    FMask4A anemone_vectorcall CompareNotEqual(FQuaternionA a, FQuaternionA b);
    FMask4A anemone_vectorcall CompareNaN(FQuaternionA a);
    FMask4A anemone_vectorcall CompareInfinite(FQuaternionA a);
    FMask4A anemone_vectorcall CompareIdentity(FQuaternionA a);

    bool anemone_vectorcall IsEqual(FQuaternionA a, FQuaternionA b);
    bool anemone_vectorcall IsNotEqual(FQuaternionA a, FQuaternionA b);
    bool anemone_vectorcall IsNaN(FQuaternionA a);
    bool anemone_vectorcall IsInfinite(FQuaternionA a);
    bool anemone_vectorcall IsIdentity(FQuaternionA a);

    FQuaternionA anemone_vectorcall Dot(FQuaternionA a, FQuaternionA b);

    FQuaternionA anemone_vectorcall Multiply(FQuaternionA a, FQuaternionA b);

    FVector4A anemone_vectorcall Length(FQuaternionA a);
    FVector4A anemone_vectorcall LengthSquared(FQuaternionA a);
    FQuaternionA anemone_vectorcall Normalize(FQuaternionA a);
    FQuaternionA anemone_vectorcall Conjugate(FQuaternionA a);
    FQuaternionA anemone_vectorcall Inverse(FQuaternionA a);

    FQuaternionA anemone_vectorcall Barycentric(FQuaternionA a, FQuaternionA b, FQuaternionA c, float u, float v);
    FQuaternionA anemone_vectorcall Barycentric(FQuaternionA a, FQuaternionA b, FQuaternionA c, FVector4A u, FVector4A v);

    void anemone_vectorcall ToAxisAngle(FVector3A& axis, float& angle, FQuaternionA a);
}

#include "AnemoneRuntime/Numerics/Private/FQuaternion.Avx.hxx"
