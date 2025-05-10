#pragma once

// Defines aligned math types

namespace Anemone::Math::Aligned
{
    struct alignas(16) Vector2F
    {
        float X;
        float Y;
    };

    struct alignas(16) Vector2D
    {
        double X;
        double Y;
    };

    struct alignas(16) Vector3F
    {
        float X;
        float Y;
        float Z;
    };

    struct alignas(32) Vector3D
    {
        double X;
        double Y;
        double Z;
    };

    struct alignas(16) Vector4F
    {
        float X;
        float Y;
        float Z;
        float W;
    };

    struct alignas(32) Vector4D
    {
        double X;
        double Y;
        double Z;
        double W;
    };

    struct alignas(16) QuaternionF
    {
        float X;
        float Y;
        float Z;
        float W;
    };

    struct alignas(32) QuaternionD
    {
        double X;
        double Y;
        double Z;
        double W;
    };

    struct alignas(16) RotorF
    {
        float XY;
        float XZ;
        float YZ;
        float Scalar;
    };

    struct alignas(32) RotorD
    {
        double XY;
        double XZ;
        double YZ;
        double Scalar;
    };

    struct alignas(16) PlaneF
    {
        float A;
        float B;
        float C;
        float D;
    };

    struct alignas(32) PlaneD
    {
        double A;
        double B;
        double C;
        double D;
    };

    struct alignas(16) SphericalCoordinateF final
    {
        float Radius;
        float Theta;
        float Phi;
    };

    struct alignas(32) SphericalCoordinateD final
    {
        double Radius;
        double Theta;
        double Phi;
    };

    struct alignas(16) CylindricalCoordinateF final
    {
        float Radius;
        float Angle;
        float Elevation;
    };

    struct alignas(32) CylindricalCoordinateD final
    {
        double Radius;
        double Angle;
        double Elevation;
    };

    struct alignas(16) PolarCoordinateF final
    {
        float Radius;
        float Angle;
    };

    struct alignas(16) PolarCoordinateD final
    {
        double Radius;
        double Angle;
    };

    struct alignas(16) Ray2F
    {
        Vector2F Origin;
        Vector2F Direction;
    };

    struct alignas(16) Ray2D
    {
        Vector2D Origin;
        Vector2D Direction;
    };

    struct alignas(16) Ray3F
    {
        Vector3F Origin;
        Vector3F Direction;
    };

    struct alignas(32) Ray3D
    {
        Vector3D Origin;
        Vector3D Direction;
    };

    struct alignas(16) BoundingSphereF
    {
        float X;
        float Y;
        float Z;
        float R;
    };

    struct alignas(32) BoundingSphereD
    {
        double X;
        double Y;
        double Z;
        double R;
    };

    struct alignas(16) BoundingCircleF
    {
        float X;
        float Y;
        float R;
    };

    struct alignas(32) BoundingCircleD
    {
        double X;
        double Y;
        double R;
    };

    struct alignas(16) BoundingBox2F
    {
        Vector2F Center;
        Vector2F Extents;
    };

    struct alignas(16) BoundingBox2D
    {
        Vector2D Center;
        Vector2D Extents;
    };

    struct alignas(16) BoundingBox3F
    {
        Vector3F Center;
        Vector3F Extents;
    };

    struct alignas(32) BoundingBox3D
    {
        Vector3D Center;
        Vector3D Extents;
    };

    struct alignas(16) BoundingOrientedBox2F
    {
        Vector2F Center;
        Vector2F Extents;
        float Angle;
    };

    struct alignas(16) BoundingOrientedBox2D
    {
        Vector2D Center;
        Vector2D Extents;
        double Angle;
    };

    struct alignas(16) BoundingOrientedBox3F
    {
        QuaternionF Orientation;
        Vector3F Center;
        Vector3F Extents;
    };

    struct alignas(32) BoundingOrientedBox3D
    {
        QuaternionD Orientation;
        Vector3D Center;
        Vector3D Extents;
    };

    struct alignas(16) BoundingFrustumF
    {
        PlaneF Near;
        PlaneF Far;
        PlaneF Left;
        PlaneF Right;
        PlaneF Top;
        PlaneF Bottom;
    };

    struct alignas(32) BoundingFrustumD
    {
        PlaneD Near;
        PlaneD Far;
        PlaneD Left;
        PlaneD Right;
        PlaneD Top;
        PlaneD Bottom;
    };

    struct alignas(16) Matrix4x4F
    {
        float M11, M12, M13, M14;
        float M21, M22, M23, M24;
        float M31, M32, M33, M34;
        float M41, M42, M43, M44;
    };

    struct alignas(32) Matrix4x4D
    {
        double M11, M12, M13, M14;
        double M21, M22, M23, M24;
        double M31, M32, M33, M34;
        double M41, M42, M43, M44;
    };

    struct alignas(16) Matrix4x3F
    {
        float M11, M12, M13;
        float M21, M22, M23;
        float M31, M32, M33;
        float M41, M42, M43;
    };

    struct alignas(32) Matrix4x3D
    {
        double M11, M12, M13;
        double M21, M22, M23;
        double M31, M32, M33;
        double M41, M42, M43;
    };

    struct alignas(16) Matrix3x4F
    {
        float M11, M12, M13, M14;
        float M21, M22, M23, M24;
        float M31, M32, M33, M34;
    };

    struct alignas(32) Matrix3x4D
    {
        double M11, M12, M13, M14;
        double M21, M22, M23, M24;
        double M31, M32, M33, M34;
    };

    struct alignas(16) Matrix3x3F
    {
        float M11, M12, M13;
        float M21, M22, M23;
        float M31, M32, M33;
    };

    struct alignas(16) Matrix3x3D
    {
        double M11, M12, M13;
        double M21, M22, M23;
        double M31, M32, M33;
    };
}
