#pragma once
#include <cstdint>
#include <cstddef>

// Requirements:
// - storage types different from operational types
// - storage types only store data
// - operational types allow to perform operations in more optimal way

namespace Anemone::Numerics
{
#if false
    using bool32 = uint32_t;

    struct Mask1F
    {
        bool32 X;
    };

    struct Mask2F
    {
        bool32 X;
        bool32 Y;
    };

    struct Mask3F
    {
        bool32 X;
        bool32 Y;
        bool32 Z;
    };

    struct Mask4F
    {
        bool32 X;
        bool32 Y;
        bool32 Z;
        bool32 W;
    };

    struct Vector1F
    {
        float X;
    };

    struct Vector2F
    {
        float X;
        float Y;
    };

    struct Vector3F
    {
        float X;
        float Y;
        float Z;
    };

    struct Vector4F
    {
        float X;
        float Y;
        float Z;
        float W;
    };

    struct QuaternionF
    {
        float X;
        float Y;
        float Z;
        float W;
    };

    struct RotorF
    {
        float XY;
        float XZ;
        float YZ;
        float Scalar;
    };

    struct Matrix3x3F
    {
        float M11, M12, M13;
        float M21, M22, M23;
        float M31, M32, M33;
    };

    struct Matrix3x4F
    {
        float M11, M12, M13, M14;
        float M21, M22, M23, M24;
        float M31, M32, M33, M34;
    };

    struct Matrix4x3F
    {
        float M11, M12, M13;
        float M21, M22, M23;
        float M31, M32, M33;
        float M41, M42, M43;
    };

    struct Matrix4x4F
    {
        float M11, M12, M13, M14;
        float M21, M22, M23, M24;
        float M31, M32, M33, M34;
        float M41, M42, M43, M44;
    };

    struct SphereF
    {
        Vector3F Center;
        float Radius;
    };

    struct PlaneF
    {
        float A;
        float B;
        float C;
        float D;
    };

    struct OpVector4F
    {
        //Private::SimdVector4F V;
    };
#endif

    struct FMask2
    {
        bool X;
        bool Y;
    };

    struct FMask3
    {
        bool X;
        bool Y;
        bool Z;
    };

    struct FMask4
    {
        bool X;
        bool Y;
        bool Z;
        bool W;
    };

    struct FVector2
    {
        float X;
        float Y;
    };

    struct FVector3
    {
        float X;
        float Y;
        float Z;
    };

    struct FVector4
    {
        float X;
        float Y;
        float Z;
        float W;
    };

    struct FQuaternion
    {
        float X;
        float Y;
        float Z;
        float W;
    };

    struct FMatrix3x3
    {
        float M11, M12, M13;
        float M21, M22, M23;
        float M31, M32, M33;
    };

    struct FMatrix3x4
    {
        float M11, M12, M13, M14;
        float M21, M22, M23, M24;
        float M31, M32, M33, M34;
    };

    struct FMatrix4x3
    {
        float M11, M12, M13;
        float M21, M22, M23;
        float M31, M32, M33;
        float M41, M42, M43;
    };

    struct FMatrix4x4
    {
        float M11, M12, M13, M14;
        float M21, M22, M23, M24;
        float M31, M32, M33, M34;
        float M41, M42, M43, M44;
    };

    struct FSphere
    {
        FVector3 Center;
        float Radius;
    };

    struct FPlane
    {
        float A;
        float B;
        float C;
        float D;
    };

    template <typename T, size_t Lanes, size_t Alignment = 16>
    struct Vector
    {
        alignas(Alignment) T Elements[Lanes];

        template <typename U>
        U const* As() const
        {
            return reinterpret_cast<U const*>(this);
        }
    };

    using VectorF32x4 = Vector<float, 4>;
    using VectorF32x8 = Vector<float, 8>;
    using VectorI32x4 = Vector<int32_t, 4>;
    using VectorI16x8 = Vector<int16_t, 8>;

}
