#pragma once
#include "AnemoneRuntime/Numerics/FQuaternion.hxx"
#include "AnemoneRuntime/Numerics/FVector3.hxx"
#include "AnemoneRuntime/Numerics/FVector4.hxx"
#include "AnemoneRuntime/Numerics/FMatrix4x4.hxx"
#include "AnemoneRuntime/Numerics/Functions.hxx"
#include "AnemoneRuntime/Numerics/Simd.hxx"

namespace Anemone::Numerics
{
    inline FQuaternionA::FQuaternionA(float x, float y, float z, float w)
        : Inner{Private::Vector4F_Create(x, y, z, w)}
    {
    }

    inline FQuaternionA::FQuaternionA(float value)
        : Inner{Private::Vector4F_Replicate(value)}
    {
    }

    inline FQuaternionA anemone_vectorcall FQuaternionA::Create(float x, float y, float z, float w)
    {
        return FQuaternionA{Private::Vector4F_Create(x, y, z, w)};
    }

    inline FQuaternionA anemone_vectorcall FQuaternionA::Splat(float value)
    {
        return FQuaternionA{Private::Vector4F_Replicate(value)};
    }

    inline FQuaternionA anemone_vectorcall FQuaternionA::Identity()
    {
        return FQuaternionA{Private::QuaternionF_Identity()};
    }

    inline FQuaternionA anemone_vectorcall FQuaternionA::FromAxisAngle(FVector3A axis, float angle)
    {
        return FQuaternionA{Private::QuaternionF_FromAxisAngle(axis.Inner, angle)};
    }

    inline FQuaternionA anemone_vectorcall FQuaternionA::FromNormalAngle(FVector3A normal, float angle)
    {
        return FQuaternionA{Private::QuaternionF_FromNormalAngle(normal.Inner, angle)};
    }

    inline FQuaternionA anemone_vectorcall FQuaternionA::FromEulerAngles(float pitch, float yaw, float roll)
    {
        return FQuaternionA{Private::QuaternionF_FromEulerAngles(pitch, yaw, roll)};
    }

    inline FQuaternionA anemone_vectorcall FQuaternionA::FromEulerAngles(FVector3A pitchYawRoll)
    {
        return FQuaternionA{Private::QuaternionF_FromEulerAngles(pitchYawRoll.Inner)};
    }

    inline FQuaternionA anemone_vectorcall FQuaternionA::Load(FQuaternion const& source)
    {
        return FQuaternionA{Private::Vector4F_LoadFloat4(reinterpret_cast<float const*>(&source))};
    }

    inline void anemone_vectorcall FQuaternionA::Store(FQuaternion& destination, FQuaternionA source)
    {
        Private::Vector4F_StoreFloat4(reinterpret_cast<float*>(&destination), source.Inner);
    }
}

namespace Anemone::Numerics
{
    template <size_t N>
    float anemone_vectorcall Extract(FQuaternionA a)
        requires(N < 4)
    {
        return Private::Vector4F_Extract<N>(a.Inner);
    }

    template <size_t N>
    FQuaternionA anemone_vectorcall Insert(FQuaternionA a, float b)
        requires(N < 4)
    {
        return FQuaternionA{Private::Vector4F_Insert<N>(a.Inner, b)};
    }

    inline FMask4A anemone_vectorcall CompareEqual(FQuaternionA a, FQuaternionA b)
    {
        return FMask4A{Private::QuaternionF_CompareEqual(a.Inner, b.Inner)};
    }

    inline FMask4A anemone_vectorcall CompareNotEqual(FQuaternionA a, FQuaternionA b)
    {
        return FMask4A{Private::QuaternionF_CompareNotEqual(a.Inner, b.Inner)};
    }

    inline FMask4A anemone_vectorcall CompareNaN(FQuaternionA a)
    {
        return FMask4A{Private::QuaternionF_CompareNaN(a.Inner)};
    }

    inline FMask4A anemone_vectorcall CompareInfinite(FQuaternionA a)
    {
        return FMask4A{Private::QuaternionF_CompareInfinite(a.Inner)};
    }

    inline FMask4A anemone_vectorcall CompareIdentity(FQuaternionA a)
    {
        return FMask4A{Private::QuaternionF_CompareIdentity(a.Inner)};
    }

    inline bool anemone_vectorcall IsEqual(FQuaternionA a, FQuaternionA b)
    {
        return Private::QuaternionF_IsEqual(a.Inner, b.Inner);
    }

    inline bool anemone_vectorcall IsNotEqual(FQuaternionA a, FQuaternionA b)
    {
        return Private::QuaternionF_IsNotEqual(a.Inner, b.Inner);
    }

    inline bool anemone_vectorcall IsNaN(FQuaternionA a)
    {
        return Private::QuaternionF_IsNaN(a.Inner);
    }

    inline bool anemone_vectorcall IsInfinite(FQuaternionA a)
    {
        return Private::QuaternionF_IsInfinite(a.Inner);
    }

    inline bool anemone_vectorcall IsIdentity(FQuaternionA a)
    {
        return Private::QuaternionF_IsIdentity(a.Inner);
    }

    inline FQuaternionA anemone_vectorcall Dot(FQuaternionA a, FQuaternionA b)
    {
        return FQuaternionA{Private::Vector4F_Dot4(a.Inner, b.Inner)};
    }

    inline FQuaternionA anemone_vectorcall Multiply(FQuaternionA a, FQuaternionA b)
    {
        return FQuaternionA{Private::QuaternionF_Multiply(a.Inner, b.Inner)};
    }

    inline FVector4A anemone_vectorcall Length(FQuaternionA a)
    {
        return FVector4A{Private::Vector4F_Length4(a.Inner)};
    }

    inline FVector4A anemone_vectorcall LengthSquared(FQuaternionA a)
    {
        return FVector4A{Private::Vector4F_LengthSquared4(a.Inner)};
    }

    inline FQuaternionA anemone_vectorcall Normalize(FQuaternionA a)
    {
        return FQuaternionA{Private::Vector4F_Normalize4(a.Inner)};
    }

    inline FQuaternionA anemone_vectorcall Conjugate(FQuaternionA a)
    {
        return FQuaternionA{Private::QuaternionF_Conjugate(a.Inner)};
    }

    inline FQuaternionA anemone_vectorcall Inverse(FQuaternionA a)
    {
        return FQuaternionA{Private::QuaternionF_Inverse(a.Inner)};
    }

    // inline FQuaternionA anemone_vectorcall Barycentric(FQuaternionA a, FQuaternionA b, FQuaternionA c, float u, float v);
    // inline FQuaternionA anemone_vectorcall Barycentric(FQuaternionA a, FQuaternionA b, FQuaternionA c, FVector4A u, FVector4A v);

    inline void anemone_vectorcall ToAxisAngle(FVector3A& axis, float& angle, FQuaternionA a)
    {
        Private::QuaternionF_ToAxisAngle(axis.Inner, angle, a.Inner);
    }
}
