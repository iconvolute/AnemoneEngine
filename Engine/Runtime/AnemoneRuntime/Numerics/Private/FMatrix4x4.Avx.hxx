#pragma once
#include "AnemoneRuntime/Numerics/FMatrix4x4.hxx"
#include "AnemoneRuntime/Numerics/FVector4.hxx"
#include "AnemoneRuntime/Numerics/FVector3.hxx"
#include "AnemoneRuntime/Numerics/FQuaternion.hxx"
#include "AnemoneRuntime/Numerics/FMask4.hxx"
#include "AnemoneRuntime/Numerics/Functions.hxx"
#include "AnemoneRuntime/Numerics/Simd.hxx"

namespace Anemone::Numerics
{
    inline FMatrix4x4A anemone_vectorcall FMatrix4x4A::Identity()
    {
        return FMatrix4x4A{Private::Matrix4x4F_Identity()};
    }

    inline FMatrix4x4A anemone_vectorcall FMatrix4x4A::CreateTranslation(float x, float y, float z)
    {
        return FMatrix4x4A{Private::Matrix4x4F_CreateTranslation(x, y, z)};
    }

    inline FMatrix4x4A anemone_vectorcall FMatrix4x4A::CreateTranslation(FVector3A translation)
    {
        return FMatrix4x4A{Private::Matrix4x4F_CreateTranslation(translation.Inner)};
    }

    inline FMatrix4x4A anemone_vectorcall FMatrix4x4A::CreateScale(float x, float y, float z)
    {
        return FMatrix4x4A{Private::Matrix4x4F_CreateScale(x, y, z)};
    }

    inline FMatrix4x4A anemone_vectorcall FMatrix4x4A::CreateScale(FVector3A scale)
    {
        return FMatrix4x4A{Private::Matrix4x4F_CreateScale(scale.Inner)};
    }

    inline FMatrix4x4A anemone_vectorcall FMatrix4x4A::CreateRotationX(float angle)
    {
        return FMatrix4x4A{Private::Matrix4x4F_CreateRotationX(angle)};
    }

    inline FMatrix4x4A anemone_vectorcall FMatrix4x4A::CreateRotationY(float angle)
    {
        return FMatrix4x4A{Private::Matrix4x4F_CreateRotationY(angle)};
    }

    inline FMatrix4x4A anemone_vectorcall FMatrix4x4A::CreateRotationZ(float angle)
    {
        return FMatrix4x4A{Private::Matrix4x4F_CreateRotationZ(angle)};
    }

    // static FMatrix4x4A anemone_vectorcall FMatrix4x4A::FromAxisAngle(FVector3A axis, float angle);
    // static FMatrix4x4A anemone_vectorcall FMatrix4x4A::FromNormalAngle(FVector3A normal, float angle);
    // static FMatrix4x4A anemone_vectorcall FMatrix4x4A::FromEulerAngles(float pitch, float yaw, float roll);
    // static FMatrix4x4A anemone_vectorcall FMatrix4x4A::FromQuaternion(FQuaternionA quaternion);

    inline FMatrix4x4A anemone_vectorcall FMatrix4x4A::Load(FMatrix4x4 const& source)
    {
        return FMatrix4x4A{Private::Matrix4x4F_LoadFloat4x4(reinterpret_cast<float const*>(&source))};
    }

    inline void anemone_vectorcall FMatrix4x4A::Store(FMatrix4x4& destination, FMatrix4x4A source)
    {
        Private::Matrix4x4F_StoreFloat4x4(reinterpret_cast<float*>(&destination), source.Inner);
    }

    inline FMatrix4x4A anemone_vectorcall FMatrix4x4A::Load(FMatrix4x3 const& source)
    {
        return FMatrix4x4A{Private::Matrix4x4F_LoadFloat4x3(reinterpret_cast<float const*>(&source))};
    }

    inline void anemone_vectorcall FMatrix4x4A::Store(FMatrix4x3& destination, FMatrix4x4A source)
    {
        Private::Matrix4x4F_StoreFloat4x3(reinterpret_cast<float*>(&destination), source.Inner);
    }

    inline FMatrix4x4A anemone_vectorcall FMatrix4x4A::Load(FMatrix3x4 const& source)
    {
        return FMatrix4x4A{Private::Matrix4x4F_LoadFloat3x4(reinterpret_cast<float const*>(&source))};
    }

    inline void anemone_vectorcall FMatrix4x4A::Store(FMatrix3x4& destination, FMatrix4x4A source)
    {
        Private::Matrix4x4F_StoreFloat3x4(reinterpret_cast<float*>(&destination), source.Inner);
    }

    inline FMatrix4x4A anemone_vectorcall FMatrix4x4A::Load(FMatrix3x3 const& source)
    {
        return FMatrix4x4A{Private::Matrix4x4F_LoadFloat3x3(reinterpret_cast<float const*>(&source))};
    }

    inline void anemone_vectorcall Store(FMatrix3x3& destination, FMatrix4x4A source)
    {
        Private::Matrix4x4F_StoreFloat3x3(reinterpret_cast<float*>(&destination), source.Inner);
    }

    inline FMatrix4x4A anemone_vectorcall FMatrix4x4A::LoadTranspose(FMatrix4x4 const& source)
    {
        return Transpose(Load(source));
    }

    inline void anemone_vectorcall FMatrix4x4A::StoreTranspose(FMatrix4x4& destination, FMatrix4x4A source)
    {
        Store(destination, Transpose(source));
    }
}

namespace Anemone::Numerics
{
    template <size_t N>
    FVector4A anemone_vectorcall Extract(FMatrix4x4A m)
        requires(N < 4)
    {
        return FVector4A{Private::Matrix4x4F_Extract<N>(m.Inner)};
    }

    template <size_t N>
    FMatrix4x4A anemone_vectorcall Insert(FMatrix4x4A m, FVector4A v)
        requires(N < 4)
    {
        return FMatrix4x4A{Private::Matrix4x4F_Insert<N>(m.Inner, v.Inner)};
    }

    inline FMatrix4x4A anemone_vectorcall Multiply(FMatrix4x4A a, FMatrix4x4A b)
    {
        return FMatrix4x4A{Private::Matrix4x4F_Multiply(a.Inner, b.Inner)};
    }

    // inline FMatrix4x4A anemone_vectorcall MultiplyTranspose(FMatrix4x4A a, FMatrix4x4A b)
    // {
    // }

    inline FMatrix4x4A anemone_vectorcall Transpose(FMatrix4x4A m)
    {
        return FMatrix4x4A{Private::Matrix4x4F_Transpose(m.Inner)};
    }

    inline FVector4A anemone_vectorcall Diagonal(FMatrix4x4A m)
    {
        return FVector4A{Private::Matrix4x4F_Diagonal(m.Inner)};
    }

    inline FVector4A anemone_vectorcall Trace(FMatrix4x4A m)
    {
        return FVector4A{Private::Matrix4x4F_Trace(m.Inner)};
    }

    inline FMatrix4x4A anemone_vectorcall TensorProduct(FVector4A a, FVector4A b)
    {
        return FMatrix4x4A{Private::Matrix4x4F_TensorProduct(a.Inner, b.Inner)};
    }
}
