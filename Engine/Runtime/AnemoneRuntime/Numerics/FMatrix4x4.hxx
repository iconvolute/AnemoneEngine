#pragma once
#include "AnemoneRuntime/Numerics/Types.hxx"
#include "AnemoneRuntime/Numerics/Simd.hxx"
#include "AnemoneRuntime/Numerics/FVector4.hxx"

namespace Anemone::Numerics
{
    struct FVector4A;
    struct FVector3A;
    struct FQuaternionA;

    struct FMatrix4x4A final
    {
        Private::SimdMatrix4x4F Inner;

        static FMatrix4x4A anemone_vectorcall Identity();

        static FMatrix4x4A anemone_vectorcall CreateTranslation(float x, float y, float z);
        static FMatrix4x4A anemone_vectorcall CreateTranslation(FVector3A translation);

        static FMatrix4x4A anemone_vectorcall CreateScale(float x, float y, float z);
        static FMatrix4x4A anemone_vectorcall CreateScale(FVector3A scale);

        static FMatrix4x4A anemone_vectorcall CreateRotationX(float angle);
        static FMatrix4x4A anemone_vectorcall CreateRotationY(float angle);
        static FMatrix4x4A anemone_vectorcall CreateRotationZ(float angle);

        static FMatrix4x4A anemone_vectorcall FromAxisAngle(FVector3A axis, float angle);
        static FMatrix4x4A anemone_vectorcall FromNormalAngle(FVector3A normal, float angle);
        static FMatrix4x4A anemone_vectorcall FromEulerAngles(float pitch, float yaw, float roll);
        static FMatrix4x4A anemone_vectorcall FromQuaternion(FQuaternionA quaternion);

        static FMatrix4x4A anemone_vectorcall Load(FMatrix4x4 const& source);
        static void anemone_vectorcall Store(FMatrix4x4& destination, FMatrix4x4A source);

        static FMatrix4x4A anemone_vectorcall Load(FMatrix4x3 const& source);
        static void anemone_vectorcall Store(FMatrix4x3& destination, FMatrix4x4A source);

        static FMatrix4x4A anemone_vectorcall Load(FMatrix3x4 const& source);
        static void anemone_vectorcall Store(FMatrix3x4& destination, FMatrix4x4A source);

        static FMatrix4x4A anemone_vectorcall Load(FMatrix3x3 const& source);
        static void anemone_vectorcall Store(FMatrix3x3& destination, FMatrix4x4A source);

        static FMatrix4x4A anemone_vectorcall LoadTranspose(FMatrix4x4 const& source);
        static void anemone_vectorcall StoreTranspose(FMatrix4x4& destination, FMatrix4x4A source);
    };
}

namespace Anemone::Numerics
{
    template <size_t N>
    FVector4A anemone_vectorcall Extract(FMatrix4x4A m)
        requires(N < 4);

    template <size_t N>
    FMatrix4x4A anemone_vectorcall Insert(FMatrix4x4A m, FVector4A v)
        requires(N < 4);

    FMatrix4x4A anemone_vectorcall Multiply(FMatrix4x4A a, FMatrix4x4A b);
    FMatrix4x4A anemone_vectorcall MultiplyTranspose(FMatrix4x4A a, FMatrix4x4A b);
    FMatrix4x4A anemone_vectorcall Transpose(FMatrix4x4A m);
    FVector4A anemone_vectorcall Diagonal(FMatrix4x4A m);
    FVector4A anemone_vectorcall Trace(FMatrix4x4A m);
    FMatrix4x4A anemone_vectorcall TensorProduct(FVector4A a, FVector4A b);
}

#include "AnemoneRuntime/Numerics/Private/FMatrix4x4.Avx.hxx"
