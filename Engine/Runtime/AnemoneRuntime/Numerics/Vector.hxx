#pragma once
#include "AnemoneRuntime/Numerics/Functions.hxx"

namespace Anemone::Numerics
{
    template <typename T>
    struct Vector2
    {
        T X;
        T Y;
    };

    using Vector2F = Vector2<float>;
    using Vector2D = Vector2<double>;

    template <typename T>
    struct Vector3
    {
        T X;
        T Y;
        T Z;
    };

    using Vector3F = Vector3<float>;
    using Vector3D = Vector3<double>;

    template <typename T>
    struct Vector4
    {
        T X;
        T Y;
        T Z;
        T W;
    };

    using Vector4F = Vector4<float>;
    using Vector4D = Vector4<double>;

    template <typename T>
    struct Quaternion
    {
        T X;
        T Y;
        T Z;
        T W;
    };

    using QuaternionF = Quaternion<float>;
    using QuaternionD = Quaternion<double>;

    template <typename T>
    struct Plane
    {
        T X;
        T Y;
        T Z;
        T W;
    };

    using PlaneF = Plane<float>;
    using PlaneD = Plane<double>;

    template <typename T>
    struct Matrix4x4
    {
        T M00, M01, M02, M03;
        T M10, M11, M12, M13;
        T M20, M21, M22, M23;
        T M30, M31, M32, M33;
    };

    using Matrix4x4F = Matrix4x4<float>;
    using Matrix4x4D = Matrix4x4<double>;

    template <typename T>
    struct Matrix4x3
    {
        T M00, M01, M02, M03;
        T M10, M11, M12, M13;
        T M20, M21, M22, M23;
    };

    using Matrix4x3F = Matrix4x3<float>;
    using Matrix4x3D = Matrix4x3<double>;

    template <typename T>
    struct Matrix3x3
    {
        T M00, M01, M02;
        T M10, M11, M12;
        T M20, M21, M22;
    };

    using Matrix3x3F = Matrix3x3<float>;
    using Matrix3x3D = Matrix3x3<double>;

    template <typename T>
    struct Matrix3x2
    {
        T M00, M01, M02;
        T M10, M11, M12;
    };

    using Matrix3x2F = Matrix3x2<float>;
    using Matrix3x2D = Matrix3x2<double>;
}

namespace Anemone::Numerics
{
    template <typename T>
    Vector2<T> Add(Vector2<T> a, Vector2<T> b)
    {
        return {
            a.X + b.X,
            a.Y + b.Y,
        };
    }
}

namespace Anemone::Numerics
{
    template <typename T>
    Vector3<T> Add(Vector3<T> a, Vector3<T> b)
    {
        return {
            a.X + b.X,
            a.Y + b.Y,
            a.Z + b.Z,
        };
    }
}

namespace Anemone::Numerics
{
    template <typename T>
    Vector4<T> Add(Vector4<T> a, Vector4<T> b)
    {
        return {
            a.X + b.X,
            a.Y + b.Y,
            a.Z + b.Z,
            a.W + b.W,
        };
    }
}
