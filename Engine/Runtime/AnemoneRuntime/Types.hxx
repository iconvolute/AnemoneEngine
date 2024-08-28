#pragma once
#include <cstdint>
#include <cstddef>

namespace Anemone
{
    struct Half final
    {
        uint16_t Inner;
    };

    template <typename T>
    struct SphericalCoordinates final
    {
        T Radius;
        T Theta;
        T Phi;
    };
    template <typename T>
    struct CylindricalCoordinates final
    {
        T Radius;
        T Angle;
        T Elevation;
    };
    template <typename T>
    struct PolarCoordinates final
    {
        T Radius;
        T Angle;
    };
}

namespace Anemone::Experimental
{
    template <typename T, size_t N>
    struct Vector final
    {
        using Element = T;
        static constexpr size_t Count = N;

        T Inner[Count];
    };

    template <typename T, size_t N, size_t M>
    struct Matrix final
    {
        using Element = T;
        static constexpr size_t Rows = N;
        static constexpr size_t Columns = M;

        T Inner[Rows][Columns];
    };

    using Half2 = Vector<Half, 2>;
    using Half3 = Vector<Half, 3>;
    using Half4 = Vector<Half, 4>;

    using Float1 = Vector<float, 1>;
    using Float2 = Vector<float, 2>;
    using Float3 = Vector<float, 3>;
    using Float4 = Vector<float, 4>;

    using Float8 = Vector<float, 8>;

    using Float4x4 = Matrix<float, 4, 4>;
    using Float4x3 = Matrix<float, 4, 3>;
    using Float3x4 = Matrix<float, 3, 4>;
    using Float3x3 = Matrix<float, 3, 3>;

    using Double1 = Vector<double, 1>;
    using Double2 = Vector<double, 2>;
    using Double3 = Vector<double, 3>;
    using Double4 = Vector<double, 4>;

    using Double4x4 = Matrix<double, 4, 4>;
    using Double4x3 = Matrix<double, 4, 3>;
    using Double3x4 = Matrix<double, 3, 4>;
    using Double3x3 = Matrix<double, 3, 3>;

    using UInt8x16 = Vector<uint8_t, 16>;
    using UInt8x32 = Vector<uint8_t, 32>;
    using UInt16x8 = Vector<uint16_t, 8>;
    using UInt16x16 = Vector<uint16_t, 16>;
    using UInt32x4 = Vector<uint32_t, 4>;
    using UInt32x8 = Vector<uint32_t, 8>;
    using UInt64x2 = Vector<uint64_t, 2>;
    using UInt64x4 = Vector<uint64_t, 4>;
    using Int8x16 = Vector<int8_t, 16>;
    using Int8x32 = Vector<int8_t, 32>;
    using Int16x8 = Vector<int16_t, 8>;
    using Int16x16 = Vector<int16_t, 16>;
    using Int32x4 = Vector<int32_t, 4>;
    using Int32x8 = Vector<int32_t, 8>;
    using Int64x2 = Vector<int64_t, 2>;
    using Int64x4 = Vector<int64_t, 4>;
}

namespace Anemone
{
    struct Half2
    {
        Half X;
        Half Y;
    };

    struct Half3
    {
        Half X;
        Half Y;
        Half Z;
    };

    struct Half4
    {
        Half X;
        Half Y;
        Half Z;
        Half W;
    };

    struct Float2
    {
        float X;
        float Y;
    };

    struct Float3
    {
        float X;
        float Y;
        float Z;
    };

    struct Float4
    {
        float X;
        float Y;
        float Z;
        float W;
    };

    struct Float4x4
    {
        float M11;
        float M12;
        float M13;
        float M14;
        float M21;
        float M22;
        float M23;
        float M24;
        float M31;
        float M32;
        float M33;
        float M34;
        float M41;
        float M42;
        float M43;
        float M44;
    };

    struct Float4x3
    {
        float M11;
        float M12;
        float M13;
        float M14;
        float M21;
        float M22;
        float M23;
        float M24;
        float M31;
        float M32;
        float M33;
        float M34;
    };

    struct Float3x3
    {
        float M11;
        float M12;
        float M13;
        float M21;
        float M22;
        float M23;
        float M31;
        float M32;
        float M33;
    };

    struct Double2
    {
        double X;
        double Y;
    };

    struct Double3
    {
        double X;
        double Y;
        double Z;
    };

    struct Double4
    {
        double X;
        double Y;
        double Z;
        double W;
    };

    struct Double4x4
    {
        double M11;
        double M12;
        double M13;
        double M14;
        double M21;
        double M22;
        double M23;
        double M24;
        double M31;
        double M32;
        double M33;
        double M34;
        double M41;
        double M42;
        double M43;
        double M44;
    };

    struct Double4x3
    {
        double M11;
        double M12;
        double M13;
        double M14;
        double M21;
        double M22;
        double M23;
        double M24;
        double M31;
        double M32;
        double M33;
        double M34;
    };

    struct Double3x3
    {
        double M11;
        double M12;
        double M13;
        double M21;
        double M22;
        double M23;
        double M31;
        double M32;
        double M33;
    };

    struct UInt8x16
    {
        uint8_t Inner[16];
    };

    struct UInt8x32
    {
        uint8_t Inner[32];
    };

    struct UInt16x8
    {
        uint16_t Inner[8];
    };

    struct UInt16x16
    {
        uint16_t Inner[16];
    };

    struct UInt32x4
    {
        uint32_t Inner[4];
    };

    struct UInt32x8
    {
        uint32_t Inner[8];
    };

    struct UInt64x2
    {
        uint64_t Inner[2];
    };

    struct UInt64x4
    {
        uint64_t Inner[4];
    };

    struct Int8x16
    {
        int8_t Inner[16];
    };

    struct Int8x32
    {
        int8_t Inner[32];
    };

    struct Int16x8
    {
        int16_t Inner[8];
    };

    struct Int16x16
    {
        int16_t Inner[16];
    };

    struct Int32x4
    {
        int32_t Inner[4];
    };

    struct Int32x8
    {
        int32_t Inner[8];
    };

    struct Int64x2
    {
        int64_t Inner[2];
    };

    struct Int64x4
    {
        int64_t Inner[4];
    };

    struct Int_10_10_10_2
    {
        int32_t X : 10;
        int32_t Y : 10;
        int32_t Z : 10;
        int32_t W : 2;
    };

    struct UInt_10_10_10_2
    {
        uint32_t X : 10;
        uint32_t Y : 10;
        uint32_t Z : 10;
        uint32_t W : 2;
    };
}
