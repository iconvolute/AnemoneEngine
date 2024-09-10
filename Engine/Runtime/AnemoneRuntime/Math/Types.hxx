#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Math/Aligned.hxx"
#include "AnemoneRuntime/Math/Packed.hxx"
#include "AnemoneRuntime/Numerics/Simd.hxx"

namespace Anemone::Math
{
    struct Matrix4x4F;
    struct Vector2F;
    struct Vector3F;
    struct Vector4F;

    struct Matrix4x4F
    {
        Numerics::Private::SimdMatrix4x4F Inner;

        [[nodiscard]] static Matrix4x4F anemone_vectorcall CreateTranslation(Vector3F v);
    };

    struct Matrix4x4D
    {
        Numerics::Private::SimdMatrix4x4D Inner;
    };

    struct Vector2F
    {
        Numerics::Private::SimdVector4F Inner;
    };

    struct Vector2D
    {
        Numerics::Private::SimdVector4D Inner;
    };

    struct Vector3F
    {
        Numerics::Private::SimdVector4F Inner;
    };

    struct Vector3D
    {
        Numerics::Private::SimdVector4D Inner;
    };

    struct Vector4F
    {
        Numerics::Private::SimdVector4F Inner;
    };

    struct Vector4D
    {
        Numerics::Private::SimdVector4D Inner;
    };

    inline Matrix4x4F anemone_vectorcall Matrix4x4F::CreateTranslation(Vector3F v)
    {
        return {Numerics::Private::Matrix4x4F_CreateTranslation(v.Inner)};
    }

    inline Vector3F anemone_vectorcall Transform(Matrix4x4F matrix, Vector3F v)
    {
        return {Numerics::Private::Vector4F_Transform3(v.Inner, matrix.Inner)};
    }

    inline Vector2F anemone_vectorcall Add(Vector2F left, Vector2F right)
    {
        return {Numerics::Private::Vector4F_Add(left.Inner, right.Inner)};
    }

    inline Vector3F anemone_vectorcall Add(Vector3F left, Vector3F right)
    {
        return {Numerics::Private::Vector4F_Add(left.Inner, right.Inner)};
    }

    inline Vector4F anemone_vectorcall Add(Vector4F left, Vector4F right)
    {
        return {Numerics::Private::Vector4F_Add(left.Inner, right.Inner)};
    }
}
