#pragma once

// Defines packed math types.

namespace Anemone::Math::Packed
{
    struct Vector2F
    {
        float X;
        float Y;
    };

    struct Vector2D
    {
        double X;
        double Y;
    };

    struct Vector3F
    {
        float X;
        float Y;
        float Z;
    };

    struct Vector3D
    {
        double X;
        double Y;
        double Z;
    };

    struct Vector4F
    {
        float X;
        float Y;
        float Z;
        float W;
    };

    struct Vector4D
    {
        double X;
        double Y;
        double Z;
        double W;
    };

    struct QuaternionF
    {
        float X;
        float Y;
        float Z;
        float W;
    };

    struct QuaternionD
    {
        double X;
        double Y;
        double Z;
        double W;
    };

    struct RotorF
    {
        float XY;
        float XZ;
        float YZ;
        float Scalar;
    };

    struct RotorD
    {
        double XY;
        double XZ;
        double YZ;
        double Scalar;
    };

    struct PlaneF
    {
        float A;
        float B;
        float C;
        float D;
    };

    struct PlaneD
    {
        double A;
        double B;
        double C;
        double D;
    };

    struct SphericalCoordinateF final
    {
        float Radius;
        float Theta;
        float Phi;
    };

    struct SphericalCoordinateD final
    {
        double Radius;
        double Theta;
        double Phi;
    };

    struct CylindricalCoordinateF final
    {
        float Radius;
        float Angle;
        float Elevation;
    };

    struct CylindricalCoordinateD final
    {
        double Radius;
        double Angle;
        double Elevation;
    };

    struct PolarCoordinateF final
    {
        float Radius;
        float Angle;
    };

    struct PolarCoordinateD final
    {
        double Radius;
        double Angle;
    };

    struct Ray2F
    {
        Vector2F Origin;
        Vector2F Direction;
    };

    struct Ray2D
    {
        Vector2D Origin;
        Vector2D Direction;
    };

    struct Ray3F
    {
        Vector3F Origin;
        Vector3F Direction;
    };

    struct Ray3D
    {
        Vector3D Origin;
        Vector3D Direction;
    };

    struct BoundingSphereF
    {
        float X;
        float Y;
        float Z;
        float R;
    };

    struct BoundingSphereD
    {
        double X;
        double Y;
        double Z;
        double R;
    };

    struct BoundingCircleF
    {
        float X;
        float Y;
        float R;
    };

    struct BoundingCircleD
    {
        double X;
        double Y;
        double R;
    };

    struct BoundingBox2F
    {
        Vector2F Center;
        Vector2F Extents;
    };

    struct BoundingBox2D
    {
        Vector2D Center;
        Vector2D Extents;
    };

    struct BoundingBox3F
    {
        Vector3F Center;
        Vector3F Extents;
    };

    struct BoundingBox3D
    {
        Vector3D Center;
        Vector3D Extents;
    };

    struct BoundingOrientedBox2F
    {
        Vector2F Center;
        Vector2F Extents;
        float Angle;
    };

    struct BoundingOrientedBox2D
    {
        Vector2D Center;
        Vector2D Extents;
        double Angle;
    };

    struct BoundingOrientedBox3F
    {
        QuaternionF Orientation;
        Vector3F Center;
        Vector3F Extents;
    };

    struct BoundingOrientedBox3D
    {
        QuaternionD Orientation;
        Vector3D Center;
        Vector3D Extents;
    };

    struct BoundingFrustumF
    {
        PlaneF Near;
        PlaneF Far;
        PlaneF Left;
        PlaneF Right;
        PlaneF Top;
        PlaneF Bottom;
    };

    struct BoundingFrustumD
    {
        PlaneD Near;
        PlaneD Far;
        PlaneD Left;
        PlaneD Right;
        PlaneD Top;
        PlaneD Bottom;
    };

    struct Matrix4x4F
    {
        float M11, M12, M13, M14;
        float M21, M22, M23, M24;
        float M31, M32, M33, M34;
        float M41, M42, M43, M44;
    };

    struct Matrix4x4D
    {
        double M11, M12, M13, M14;
        double M21, M22, M23, M24;
        double M31, M32, M33, M34;
        double M41, M42, M43, M44;
    };

    struct Matrix4x3F
    {
        float M11, M12, M13;
        float M21, M22, M23;
        float M31, M32, M33;
        float M41, M42, M43;
    };

    struct Matrix4x3D
    {
        double M11, M12, M13;
        double M21, M22, M23;
        double M31, M32, M33;
        double M41, M42, M43;
    };

    struct Matrix3x4F
    {
        float M11, M12, M13, M14;
        float M21, M22, M23, M24;
        float M31, M32, M33, M34;
    };

    struct Matrix3x4D
    {
        double M11, M12, M13, M14;
        double M21, M22, M23, M24;
        double M31, M32, M33, M34;
    };

    struct Matrix3x3F
    {
        float M11, M12, M13;
        float M21, M22, M23;
        float M31, M32, M33;
    };

    struct Matrix3x3D
    {
        double M11, M12, M13;
        double M21, M22, M23;
        double M31, M32, M33;
    };
}
