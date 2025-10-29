#include "AnemoneMath/Detail/SimdFloat.hxx"
#include "AnemoneMath/Random.hxx"

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

ANEMONE_EXTERNAL_HEADERS_END

TEST_CASE("Matrix4x4F_Multiply")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math::Detail;
    using namespace Anemone::Math;

    static constexpr float tolerance = 0.001f;

    float const a[16]{
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f};

    float const b[16]{
        11.0f, 12.0f, 13.0f, 14.0f,
        21.0f, 22.0f, 23.0f, 24.0f,
        31.0f, 32.0f, 33.0f, 34.0f,
        41.0f, 42.0f, 43.0f, 44.0f};
    SimdMatrix4x4F const identity = Matrix4x4F_Identity();

    SECTION("identity * identity")
    {
        SimdMatrix4x4F const m = Matrix4x4F_Multiply(identity, identity);

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

        SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

        SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

        SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);
        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
    }

    SECTION("a * identity")
    {
        SimdMatrix4x4F const m = Matrix4x4F_Multiply(Matrix4x4F_LoadUnalignedFloat4x4(a), identity);

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(2.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(3.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(4.0f, tolerance));

        SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(5.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(6.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(7.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(8.0f, tolerance));

        SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(9.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(10.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(11.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(12.0f, tolerance));

        SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);
        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(13.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(14.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(15.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(16.0f, tolerance));
    }

    SECTION("identity * b")
    {
        SimdMatrix4x4F const m = Matrix4x4F_Multiply(identity, Matrix4x4F_LoadUnalignedFloat4x4(b));

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(11.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(12.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(13.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(14.0f, tolerance));

        SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(21.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(22.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(23.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(24.0f, tolerance));

        SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(31.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(32.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(33.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(34.0f, tolerance));

        SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);
        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(41.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(42.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(43.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(44.0f, tolerance));
    }

    SECTION("a * b")
    {
        SimdMatrix4x4F const m = Matrix4x4F_Multiply(Matrix4x4F_LoadUnalignedFloat4x4(a), Matrix4x4F_LoadUnalignedFloat4x4(b));

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(310.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(320.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(330.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(340.0f, tolerance));

        SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(726.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(752.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(778.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(804.0f, tolerance));

        SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(1142.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(1184.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1226.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(1268.0f, tolerance));

        SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);
        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(1558.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(1616.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(1674.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1732.0f, tolerance));
    }
}

TEST_CASE("Matrix4x4F_MultiplyTranspose")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math::Detail;
    using namespace Anemone::Math;

    static constexpr float tolerance = 0.001f;

    float const a[16]{
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f};

    float const b[16]{
        11.0f, 12.0f, 13.0f, 14.0f,
        21.0f, 22.0f, 23.0f, 24.0f,
        31.0f, 32.0f, 33.0f, 34.0f,
        41.0f, 42.0f, 43.0f, 44.0f};
    SimdMatrix4x4F const identity = Matrix4x4F_Identity();

    SECTION("identity * identity")
    {
        SimdMatrix4x4F const m = Matrix4x4F_MultiplyTranspose(identity, identity);

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

        SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

        SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

        SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);
        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
    }

    SECTION("a * identity")
    {
        SimdMatrix4x4F const m = Matrix4x4F_MultiplyTranspose(Matrix4x4F_LoadUnalignedFloat4x4(a), identity);

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(5.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(9.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(13.0f, tolerance));

        SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(2.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(6.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(10.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(14.0f, tolerance));

        SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(3.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(7.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(11.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(15.0f, tolerance));

        SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);
        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(4.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(8.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(12.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(16.0f, tolerance));
    }

    SECTION("identity * b")
    {
        SimdMatrix4x4F const m = Matrix4x4F_MultiplyTranspose(identity, Matrix4x4F_LoadUnalignedFloat4x4(b));

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(11.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(21.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(31.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(41.0f, tolerance));

        SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(12.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(22.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(32.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(42.0f, tolerance));

        SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(13.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(23.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(33.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(43.0f, tolerance));

        SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);
        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(14.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(24.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(34.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(44.0f, tolerance));
    }

    SECTION("a * b")
    {
        SimdMatrix4x4F const m = Matrix4x4F_MultiplyTranspose(Matrix4x4F_LoadUnalignedFloat4x4(a), Matrix4x4F_LoadUnalignedFloat4x4(b));

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(310.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(726.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(1142.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(1558.0f, tolerance));

        SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(320.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(752.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(1184.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(1616.0f, tolerance));

        SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(330.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(778.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1226.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(1674.0f, tolerance));

        SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);
        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(340.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(804.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(1268.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1732.0f, tolerance));
    }
}

template <typename T>
T Helper_Matrix4x4_ComputeDeterminant(T const* m)
{
    T const m00 = m[0 * 4 + 0];
    T const m01 = m[0 * 4 + 1];
    T const m02 = m[0 * 4 + 2];
    T const m03 = m[0 * 4 + 3];

    T const m10 = m[1 * 4 + 0];
    T const m11 = m[1 * 4 + 1];
    T const m12 = m[1 * 4 + 2];
    T const m13 = m[1 * 4 + 3];

    T const m20 = m[2 * 4 + 0];
    T const m21 = m[2 * 4 + 1];
    T const m22 = m[2 * 4 + 2];
    T const m23 = m[2 * 4 + 3];

    T const m30 = m[3 * 4 + 0];
    T const m31 = m[3 * 4 + 1];
    T const m32 = m[3 * 4 + 2];
    T const m33 = m[3 * 4 + 3];

    T const result =
        m03 * m12 * m21 * m30 - m02 * m13 * m21 * m30 -
        m03 * m11 * m22 * m30 + m01 * m13 * m22 * m30 +
        m02 * m11 * m23 * m30 - m01 * m12 * m23 * m30 -
        m03 * m12 * m20 * m31 + m02 * m13 * m20 * m31 +
        m03 * m10 * m22 * m31 - m00 * m13 * m22 * m31 -
        m02 * m10 * m23 * m31 + m00 * m12 * m23 * m31 +
        m03 * m11 * m20 * m32 - m01 * m13 * m20 * m32 -
        m03 * m10 * m21 * m32 + m00 * m13 * m21 * m32 +
        m01 * m10 * m23 * m32 - m00 * m11 * m23 * m32 -
        m02 * m11 * m20 * m33 + m01 * m12 * m20 * m33 +
        m02 * m10 * m21 * m33 - m00 * m12 * m21 * m33 -
        m01 * m10 * m22 * m33 + m00 * m11 * m22 * m33;
    return result;
}

TEST_CASE("Matrix4x4F_Determinant")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math::Detail;
    using namespace Anemone::Math;

    static constexpr float tolerance = 0.05f;

    float const a[16]{
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f};

    float const b[16]{
        1, -1, 0, 4,
        5, -1, 2, 0,
        4, 1, 2, -2,
        0, 1, 1, 5};

    SimdMatrix4x4F const identity = Matrix4x4F_Identity();

    SECTION("Identity")
    {
        SimdVector4F const d = Matrix4x4F_Determinant(identity);
        CHECK_THAT(Vector4F_Extract<0>(d), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(d), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(d), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(d), WithinAbs(1.0f, tolerance));
    }

    SECTION("a")
    {
        SimdVector4F const d = Matrix4x4F_Determinant(Matrix4x4F_LoadUnalignedFloat4x4(a));
        CHECK_THAT(Vector4F_Extract<0>(d), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(d), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(d), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(d), WithinAbs(0.0f, tolerance));
    }

    SECTION("b")
    {
        SimdVector4F const d = Matrix4x4F_Determinant(Matrix4x4F_LoadUnalignedFloat4x4(b));
        CHECK_THAT(Vector4F_Extract<0>(d), WithinAbs(-34.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(d), WithinAbs(-34.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(d), WithinAbs(-34.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(d), WithinAbs(-34.0f, tolerance));
    }

    SECTION("Random")
    {
        UniformDistribution<float> uniform{};
        Random random{2137};

        for (size_t i = 0; i < 100; ++i)
        {
            float m[16];

            for (float& c : m)
            {
                c = uniform(random, -10.0f, 10.0f);
            }

            float const expected = Helper_Matrix4x4_ComputeDeterminant(m);

            SimdVector4F const d = Matrix4x4F_Determinant(Matrix4x4F_LoadUnalignedFloat4x4(m));
            CHECK_THAT(Vector4F_Extract<0>(d), WithinAbs(expected, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(d), WithinAbs(expected, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(d), WithinAbs(expected, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(d), WithinAbs(expected, tolerance));
        }
    }
}

template <typename T>
T Helper_Matrix4x4_ComputeInverse(T const* matrix, T* result)
{
    // Use the Cramer's rule to compute the inverse of a 4x4 matrix.
    //
    // M^-1 = 1 / det(M) * adj(M)
    //
    // adj(M) = C^T
    //
    // Cij = (-1)^(i+j) * det(Mij)

    float const a = matrix[0 * 4 + 0];
    float const b = matrix[0 * 4 + 1];
    float const c = matrix[0 * 4 + 2];
    float const d = matrix[0 * 4 + 3];

    float const e = matrix[1 * 4 + 0];
    float const f = matrix[1 * 4 + 1];
    float const g = matrix[1 * 4 + 2];
    float const h = matrix[1 * 4 + 3];

    float const i = matrix[2 * 4 + 0];
    float const j = matrix[2 * 4 + 1];
    float const k = matrix[2 * 4 + 2];
    float const l = matrix[2 * 4 + 3];

    float const m = matrix[3 * 4 + 0];
    float const n = matrix[3 * 4 + 1];
    float const o = matrix[3 * 4 + 2];
    float const p = matrix[3 * 4 + 3];

    float const kp_lo = (k * p) - (l * o);
    float const jp_ln = (j * p) - (l * n);
    float const jo_kn = (j * o) - (k * n);
    float const ip_lm = (i * p) - (l * m);
    float const io_km = (i * o) - (k * m);
    float const in_jm = (i * n) - (j * m);

    float const a11 = +((f * kp_lo) - (g * jp_ln) + (h * jo_kn));
    float const a12 = -((e * kp_lo) - (g * ip_lm) + (h * io_km));
    float const a13 = +((e * jp_ln) - (f * ip_lm) + (h * in_jm));
    float const a14 = -((e * jo_kn) - (f * io_km) + (g * in_jm));

    float const det = (a * a11) + (b * a12) + (c * a13) + (d * a14);

    if (std::abs(det) <= std::numeric_limits<float>::epsilon())
    {
        for (size_t index = 0; index < 16; ++index)
        {
            result[index] = std::numeric_limits<float>::quiet_NaN();
        }

        return det;
    }

    float const invDet = 1.0f / det;

    result[0 * 4 + 0] = a11 * invDet;
    result[1 * 4 + 0] = a12 * invDet;
    result[2 * 4 + 0] = a13 * invDet;
    result[3 * 4 + 0] = a14 * invDet;

    result[0 * 4 + 1] = -((b * kp_lo) - (c * jp_ln) + (d * jo_kn)) * invDet;
    result[1 * 4 + 1] = +((a * kp_lo) - (c * ip_lm) + (d * io_km)) * invDet;
    result[2 * 4 + 1] = -((a * jp_ln) - (b * ip_lm) + (d * in_jm)) * invDet;
    result[3 * 4 + 1] = +((a * jo_kn) - (b * io_km) + (c * in_jm)) * invDet;

    float const gp_ho = (g * p) - (h * o);
    float const fp_hn = (f * p) - (h * n);
    float const fo_gn = (f * o) - (g * n);
    float const ep_hm = (e * p) - (h * m);
    float const eo_gm = (e * o) - (g * m);
    float const en_fm = (e * n) - (f * m);

    result[0 * 4 + 2] = +((b * gp_ho) - (c * fp_hn) + (d * fo_gn)) * invDet;
    result[1 * 4 + 2] = -((a * gp_ho) - (c * ep_hm) + (d * eo_gm)) * invDet;
    result[2 * 4 + 2] = +((a * fp_hn) - (b * ep_hm) + (d * en_fm)) * invDet;
    result[3 * 4 + 2] = -((a * fo_gn) - (b * eo_gm) + (c * en_fm)) * invDet;

    float const gl_hk = (g * l) - (h * k);
    float const fl_hj = (f * l) - (h * j);
    float const fk_gj = (f * k) - (g * j);
    float const el_hi = (e * l) - (h * i);
    float const ek_gi = (e * k) - (g * i);
    float const ej_fi = (e * j) - (f * i);

    result[0 * 4 + 3] = -((b * gl_hk) - (c * fl_hj) + (d * fk_gj)) * invDet;
    result[1 * 4 + 3] = +((a * gl_hk) - (c * el_hi) + (d * ek_gi)) * invDet;
    result[2 * 4 + 3] = -((a * fl_hj) - (b * el_hi) + (d * ej_fi)) * invDet;
    result[3 * 4 + 3] = +((a * fk_gj) - (b * ek_gi) + (c * ej_fi)) * invDet;

    return det;
}

TEST_CASE("Matrix4x4F_Inverse")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math::Detail;
    using namespace Anemone::Math;

    static constexpr float tolerance = 0.01f;

    SECTION("Scratch")
    {
        float source[16]{
            00.0f, 10.0f, 20.0f, 30.0f,
            01.0f, 11.0f, 21.0f, 31.0f,
            02.0f, 12.0f, 22.0f, 32.0f,
            03.0f, 13.0f, 23.0f, 33.0f};

        SimdMatrix4x4F const matrix = Matrix4x4F_LoadUnalignedFloat4x4(source);

        SimdVector4F determinant;
        SimdMatrix4x4F const inverse = Matrix4x4F_Inverse(matrix, determinant);
        (void)inverse;
    }

    SECTION("Identity")
    {
        SimdMatrix4x4F const identity = Matrix4x4F_Identity();

        SimdVector4F determinant;
        SimdMatrix4x4F const inverse = Matrix4x4F_Inverse(identity, determinant);

        CHECK_THAT(Vector4F_Extract<0>(determinant), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(determinant), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(determinant), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(determinant), WithinAbs(1.0f, tolerance));

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(inverse);
        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

        SimdVector4F const r1 = Matrix4x4F_Extract<1>(inverse);
        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

        SimdVector4F const r2 = Matrix4x4F_Extract<2>(inverse);
        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

        SimdVector4F const r3 = Matrix4x4F_Extract<3>(inverse);
        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
    }

    SECTION("Matrix with dependent vectors as rows")
    {
        float source[16]{
            1.0f, 2.0f, 3.0f, 4.0f,
            2.0f, 4.0f, 6.0f, 8.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};

        SimdMatrix4x4F const matrix = Matrix4x4F_LoadUnalignedFloat4x4(source);

        SimdVector4F determinant;
        SimdMatrix4x4F const inverse = Matrix4x4F_Inverse(matrix, determinant);

        CHECK_THAT(Vector4F_Extract<0>(determinant), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(determinant), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(determinant), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(determinant), WithinAbs(0.0f, tolerance));

        // The determinant is zero, so the inverse is not valid.
        // Depending on CPU architecture, the result may be NaN or Inf.
        SimdVector4F const r0 = Matrix4x4F_Extract<0>(inverse);
        CHECK(!std::isfinite(Vector4F_Extract<0>(r0)));
        CHECK(!std::isfinite(Vector4F_Extract<1>(r0)));
        CHECK(!std::isfinite(Vector4F_Extract<2>(r0)));
        CHECK(!std::isfinite(Vector4F_Extract<3>(r0)));

        SimdVector4F const r1 = Matrix4x4F_Extract<1>(inverse);
        CHECK(!std::isfinite(Vector4F_Extract<0>(r1)));
        CHECK(!std::isfinite(Vector4F_Extract<1>(r1)));
        CHECK(!std::isfinite(Vector4F_Extract<2>(r1)));
        CHECK(!std::isfinite(Vector4F_Extract<3>(r1)));

        SimdVector4F const r2 = Matrix4x4F_Extract<2>(inverse);
        CHECK(!std::isfinite(Vector4F_Extract<0>(r2)));
        CHECK(!std::isfinite(Vector4F_Extract<1>(r2)));
        CHECK(!std::isfinite(Vector4F_Extract<2>(r2)));
        CHECK(!std::isfinite(Vector4F_Extract<3>(r2)));

        SimdVector4F const r3 = Matrix4x4F_Extract<3>(inverse);
        CHECK(!std::isfinite(Vector4F_Extract<0>(r3)));
        CHECK(!std::isfinite(Vector4F_Extract<1>(r3)));
        CHECK(!std::isfinite(Vector4F_Extract<2>(r3)));
        CHECK(!std::isfinite(Vector4F_Extract<3>(r3)));
    }

    SECTION("Matrix with dependent vectors as columns")
    {
        float source[16]{
            1.0f, 2.0f, 0.0f, 0.0f,
            2.0f, 4.0f, 0.0f, 0.0f,
            3.0f, 6.0f, 1.0f, 0.0f,
            4.0f, 8.0f, 0.0f, 1.0f};
        SimdMatrix4x4F const matrix = Matrix4x4F_LoadUnalignedFloat4x4(source);

        SimdVector4F determinant;
        SimdMatrix4x4F const inverse = Matrix4x4F_Inverse(matrix, determinant);

        CHECK_THAT(Vector4F_Extract<0>(determinant), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(determinant), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(determinant), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(determinant), WithinAbs(0.0f, tolerance));

        // The determinant is zero, so the inverse is not valid.
        // Depending on CPU architecture, the result may be NaN or Inf.
        SimdVector4F const r0 = Matrix4x4F_Extract<0>(inverse);
        CHECK(!std::isfinite(Vector4F_Extract<0>(r0)));
        CHECK(!std::isfinite(Vector4F_Extract<1>(r0)));
        CHECK(!std::isfinite(Vector4F_Extract<2>(r0)));
        CHECK(!std::isfinite(Vector4F_Extract<3>(r0)));

        SimdVector4F const r1 = Matrix4x4F_Extract<1>(inverse);
        CHECK(!std::isfinite(Vector4F_Extract<0>(r1)));
        CHECK(!std::isfinite(Vector4F_Extract<1>(r1)));
        CHECK(!std::isfinite(Vector4F_Extract<2>(r1)));
        CHECK(!std::isfinite(Vector4F_Extract<3>(r1)));

        SimdVector4F const r2 = Matrix4x4F_Extract<2>(inverse);
        CHECK(!std::isfinite(Vector4F_Extract<0>(r2)));
        CHECK(!std::isfinite(Vector4F_Extract<1>(r2)));
        CHECK(!std::isfinite(Vector4F_Extract<2>(r2)));
        CHECK(!std::isfinite(Vector4F_Extract<3>(r2)));

        SimdVector4F const r3 = Matrix4x4F_Extract<3>(inverse);
        CHECK(!std::isfinite(Vector4F_Extract<0>(r3)));
        CHECK(!std::isfinite(Vector4F_Extract<1>(r3)));
        CHECK(!std::isfinite(Vector4F_Extract<2>(r3)));
        CHECK(!std::isfinite(Vector4F_Extract<3>(r3)));
    }

    SECTION("Random")
    {
        UniformDistribution<float> uniform{};
        Random random{2137};

        for (size_t i = 0; i < 100; ++i)
        {
            float m[16];

            for (float& c : m)
            {
                c = uniform(random, -10.0f, 10.0f);
            }

            float expected_inverse[16];
            float const ed = Helper_Matrix4x4_ComputeInverse(m, expected_inverse);
            float const expected_determinant = Helper_Matrix4x4_ComputeDeterminant(m);

            CHECK_THAT(ed, WithinAbs(expected_determinant, tolerance));

            SimdVector4F determinant;
            SimdMatrix4x4F const inverse = Matrix4x4F_Inverse(Matrix4x4F_LoadUnalignedFloat4x4(m), determinant);

            if (IsNearZero(Vector4F_Extract<0>(determinant)))
            {
                CHECK(Matrix4x4F_IsNaN(inverse));
            }
            else
            {
                CHECK_THAT(Vector4F_Extract<0>(determinant), WithinAbs(expected_determinant, tolerance));
                CHECK_THAT(Vector4F_Extract<1>(determinant), WithinAbs(expected_determinant, tolerance));
                CHECK_THAT(Vector4F_Extract<2>(determinant), WithinAbs(expected_determinant, tolerance));
                CHECK_THAT(Vector4F_Extract<3>(determinant), WithinAbs(expected_determinant, tolerance));

                {
                    SimdVector4F const r0 = Matrix4x4F_Extract<0>(inverse);
                    CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(expected_inverse[0 * 4 + 0], tolerance));
                    CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(expected_inverse[0 * 4 + 1], tolerance));
                    CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(expected_inverse[0 * 4 + 2], tolerance));
                    CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(expected_inverse[0 * 4 + 3], tolerance));

                    SimdVector4F const r1 = Matrix4x4F_Extract<1>(inverse);
                    CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(expected_inverse[1 * 4 + 0], tolerance));
                    CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(expected_inverse[1 * 4 + 1], tolerance));
                    CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(expected_inverse[1 * 4 + 2], tolerance));
                    CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(expected_inverse[1 * 4 + 3], tolerance));

                    SimdVector4F const r2 = Matrix4x4F_Extract<2>(inverse);
                    CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(expected_inverse[2 * 4 + 0], tolerance));
                    CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(expected_inverse[2 * 4 + 1], tolerance));
                    CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(expected_inverse[2 * 4 + 2], tolerance));
                    CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(expected_inverse[2 * 4 + 3], tolerance));

                    SimdVector4F const r3 = Matrix4x4F_Extract<3>(inverse);
                    CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(expected_inverse[3 * 4 + 0], tolerance));
                    CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(expected_inverse[3 * 4 + 1], tolerance));
                    CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(expected_inverse[3 * 4 + 2], tolerance));
                    CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(expected_inverse[3 * 4 + 3], tolerance));
                }

                // Inverse of the matrix multiplied by the original matrix should be the identity matrix.
                SimdMatrix4x4F const product = Matrix4x4F_Multiply(inverse, Matrix4x4F_LoadUnalignedFloat4x4(m));
                {
                    SimdVector4F const r0 = Matrix4x4F_Extract<0>(product);
                    CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, tolerance));
                    CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
                    CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
                    CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

                    SimdVector4F const r1 = Matrix4x4F_Extract<1>(product);
                    CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
                    CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(1.0f, tolerance));
                    CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
                    CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

                    SimdVector4F const r2 = Matrix4x4F_Extract<2>(product);
                    CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
                    CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
                    CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1.0f, tolerance));
                    CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

                    SimdVector4F const r3 = Matrix4x4F_Extract<3>(product);
                    CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
                    CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
                    CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
                    CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
                }
            }
        }
    }
}

TEST_CASE("Matrix4x4F_CreateFromQuaternion")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math::Detail;
    using namespace Anemone::Math;

    static constexpr float tolerance = 0.001f;

    SECTION("x-axis, 0 degrees")
    {
        SimdVector4F const rotation = QuaternionF_CreateFromAxisAngle(
            Vector4F_PositiveUnitX(),
            DegreesToRadians(0.0f));

        SimdMatrix4x4F const m = Matrix4x4F_CreateFromQuaternion(rotation);

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

        SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

        SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

        SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);
        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
    }

    SECTION("x-axis, 90 degrees")
    {
        SimdVector4F const rotation = QuaternionF_CreateFromAxisAngle(
            Vector4F_PositiveUnitX(),
            DegreesToRadians(90.0f));

        SimdMatrix4x4F const m = Matrix4x4F_CreateFromQuaternion(rotation);

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

        SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

        SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(-1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

        SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);
        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
    }

    SECTION("y-axis, 90 degrees")
    {
        SimdVector4F const rotation = QuaternionF_CreateFromAxisAngle(
            Vector4F_PositiveUnitY(),
            DegreesToRadians(90.0f));

        SimdMatrix4x4F const m = Matrix4x4F_CreateFromQuaternion(rotation);

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(-1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

        SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

        SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

        SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);
        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
    }

    SECTION("z-axis, 90 degrees")
    {
        SimdVector4F const rotation = QuaternionF_CreateFromAxisAngle(
            Vector4F_PositiveUnitZ(),
            DegreesToRadians(90.0f));

        SimdMatrix4x4F const m = Matrix4x4F_CreateFromQuaternion(rotation);

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

        SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(-1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

        SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

        SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);
        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
    }

    SECTION("custom axis and angle")
    {
        SimdVector4F const rotation = QuaternionF_CreateFromAxisAngle(
            Vector4F_Create(1.0f, 2.0f, 3.0f, 4.0f),
            DegreesToRadians(30.0f));

        SimdMatrix4x4F const m = Matrix4x4F_CreateFromQuaternion(rotation);

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(0.8756f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.42003f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(-0.23855f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

        SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(-0.38175f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(0.9043f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.19105f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

        SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.29597f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(-0.07621f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(0.95215f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

        SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);
        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
    }
}

TEST_CASE("Matrix4x4F_CreateFromPitchYawRoll")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    SECTION("Pitch = 0, Yaw = 0, Roll = 0")
    {
        SimdMatrix4x4F const xform = Matrix4x4F_CreateFromPitchYawRoll(
            DegreesToRadians(0.0f),
            DegreesToRadians(0.0f),
            DegreesToRadians(0.0f));

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(xform);
        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, 0.0001));

        SimdVector4F const r1 = Matrix4x4F_Extract<1>(xform);
        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, 0.0001));

        SimdVector4F const r2 = Matrix4x4F_Extract<2>(xform);
        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, 0.0001));

        SimdVector4F const r3 = Matrix4x4F_Extract<3>(xform);
        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, 0.0001));
    }

    SECTION("Pitch = 90, Yaw = 90, Roll = 90")
    {
        SimdMatrix4x4F const xform = Matrix4x4F_CreateFromPitchYawRoll(
            DegreesToRadians(90.0f),
            DegreesToRadians(90.0f),
            DegreesToRadians(90.0f));

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(xform);
        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, 0.0001));

        SimdVector4F const r1 = Matrix4x4F_Extract<1>(xform);
        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, 0.0001));

        SimdVector4F const r2 = Matrix4x4F_Extract<2>(xform);
        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(-1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, 0.0001));

        SimdVector4F const r3 = Matrix4x4F_Extract<3>(xform);
        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, 0.0001));
    }

    SECTION("Pitch = 90, Yaw = 0, Roll = 0")
    {
        SimdMatrix4x4F const xform = Matrix4x4F_CreateFromPitchYawRoll(
            DegreesToRadians(90.0f),
            DegreesToRadians(0.0f),
            DegreesToRadians(0.0f));

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(xform);
        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, 0.0001));

        SimdVector4F const r1 = Matrix4x4F_Extract<1>(xform);
        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, 0.0001));

        SimdVector4F const r2 = Matrix4x4F_Extract<2>(xform);
        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(-1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, 0.0001));

        SimdVector4F const r3 = Matrix4x4F_Extract<3>(xform);
        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, 0.0001));
    }

    SECTION("Pitch = 0, Yaw = 90, Roll = 0")
    {
        SimdMatrix4x4F const xform = Matrix4x4F_CreateFromPitchYawRoll(
            DegreesToRadians(0.0f),
            DegreesToRadians(90.0f),
            DegreesToRadians(0.0f));

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(xform);
        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(-1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, 0.0001));

        SimdVector4F const r1 = Matrix4x4F_Extract<1>(xform);
        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, 0.0001));

        SimdVector4F const r2 = Matrix4x4F_Extract<2>(xform);
        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, 0.0001));

        SimdVector4F const r3 = Matrix4x4F_Extract<3>(xform);
        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, 0.0001));
    }

    SECTION("Pitch = 0, Yaw = 0, Roll = 90")
    {
        SimdMatrix4x4F const xform = Matrix4x4F_CreateFromPitchYawRoll(
            DegreesToRadians(0.0f),
            DegreesToRadians(0.0f),
            DegreesToRadians(90.0f));

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(xform);
        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, 0.0001));

        SimdVector4F const r1 = Matrix4x4F_Extract<1>(xform);
        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(-1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, 0.0001));

        SimdVector4F const r2 = Matrix4x4F_Extract<2>(xform);
        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, 0.0001));

        SimdVector4F const r3 = Matrix4x4F_Extract<3>(xform);
        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, 0.0001));
    }
}

TEST_CASE("Matrix4x4F_CreateAffineTransform3")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    SECTION("Affine2 - Identity")
    {
        SimdMatrix4x4F xform = Matrix4x4F_CreateAffineTransform3(
            Vector4F_Replicate(2.0f),
            Vector4F_Zero(),
            QuaternionF_Identity(),
            Vector4F_Zero());


        SimdVector4F const v = Vector4F_Create(1.0f, 2.0f, 3.0f, 0.0f);

        SimdVector4F const r = Vector4F_Transform3(v, xform);

        CHECK_THAT(Vector4F_Extract<0>(r), WithinAbs(2.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r), WithinAbs(4.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r), WithinAbs(6.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r), WithinAbs(1.0f, 0.0001));
    }

    SECTION("Affine2 - Custom")
    {
        SimdMatrix4x4F xform = Matrix4x4F_CreateAffineTransform3(
            Vector4F_Replicate(2.0f),
            Vector4F_Create(1.0f, 1.0f, 1.0f, 1.0f),
            QuaternionF_CreateFromAxisAngle(Vector4F_PositiveUnitZ(), DegreesToRadians(90.0f)),
            Vector4F_Create(2.0f, 3.0f, 4.0f, 5.0f));


        SimdVector4F const v = Vector4F_Create(1.0f, 2.0f, 3.0f, 0.0f);

        SimdVector4F const r = Vector4F_Transform3(v, xform);

        CHECK_THAT(Vector4F_Extract<0>(r), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r), WithinAbs(5.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r), WithinAbs(10.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r), WithinAbs(1.0f, 0.0001));
    }
}

TEST_CASE("Matrix4x4F_CreateAffineTransform2")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    SECTION("Affine2 - Identity")
    {
        SimdMatrix4x4F xform = Matrix4x4F_CreateAffineTransform2(
            Vector4F_Replicate(1.0f),
            Vector4F_Zero(),
            0.0f,
            Vector4F_Zero());


        SimdVector4F const v = Vector4F_Create(1.0f, 2.0f, 3.0f, 0.0f);

        SimdVector4F const r = Vector4F_Transform3(v, xform);

        CHECK_THAT(Vector4F_Extract<0>(r), WithinAbs(1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r), WithinAbs(2.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r), WithinAbs(3.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r), WithinAbs(1.0f, 0.0001));
    }

    SECTION("Affine2 - Custom")
    {
        SimdMatrix4x4F xform = Matrix4x4F_CreateAffineTransform2(
            Vector4F_Replicate(2.0f),
            Vector4F_Create(-1.0f, -2.0f, -3.0f, -4.0f),
            DegreesToRadians(90.0f),
            Vector4F_Create(2.0f, 3.0f, 3.0f, 4.0f));


        SimdVector4F const v = Vector4F_Create(1.0f, 2.0f, 3.0f, 0.0f);

        SimdVector4F const r = Vector4F_Transform3(v, xform);

        CHECK_THAT(Vector4F_Extract<0>(r), WithinAbs(-5.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r), WithinAbs(4.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r), WithinAbs(3.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r), WithinAbs(1.0f, 0.0001));
    }
}

TEST_CASE("Matrix4x4F_Transpose")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    static constexpr float tolerance = 0.0001f;

    float const source[16]{
        11.0f, 12.0f, 13.0f, 14.0f,
        21.0f, 22.0f, 23.0f, 24.0f,
        31.0f, 32.0f, 33.0f, 34.0f,
        41.0f, 42.0f, 43.0f, 44.0f};

    SimdMatrix4x4F const m = Matrix4x4F_LoadUnalignedFloat4x4(source);
    SimdMatrix4x4F const r = Matrix4x4F_Transpose(m);

    SimdVector4F const r0 = Matrix4x4F_Extract<0>(r);
    SimdVector4F const r1 = Matrix4x4F_Extract<1>(r);
    SimdVector4F const r2 = Matrix4x4F_Extract<2>(r);
    SimdVector4F const r3 = Matrix4x4F_Extract<3>(r);

    CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(11.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(21.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(31.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(41.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(12.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(22.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(32.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(42.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(13.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(23.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(33.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(43.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(14.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(24.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(34.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(44.0f, tolerance));
}

TEST_CASE("Matrix4x4F_Diagonal")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    static constexpr float tolerance = 0.0001f;

    float const source[16]{
        11.0f, 12.0f, 13.0f, 14.0f,
        21.0f, 22.0f, 23.0f, 24.0f,
        31.0f, 32.0f, 33.0f, 34.0f,
        41.0f, 42.0f, 43.0f, 44.0f};

    SimdMatrix4x4F const m = Matrix4x4F_LoadUnalignedFloat4x4(source);

    SimdVector4F const r = Matrix4x4F_Diagonal(m);

    CHECK_THAT(Vector4F_Extract<0>(r), WithinAbs(11.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r), WithinAbs(22.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r), WithinAbs(33.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r), WithinAbs(44.0f, tolerance));
}

TEST_CASE("Matrix4x4F_Trace")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    static constexpr float tolerance = 0.0001f;

    float const source[16]{
        11.0f, 12.0f, 13.0f, 14.0f,
        21.0f, 22.0f, 23.0f, 24.0f,
        31.0f, 32.0f, 33.0f, 34.0f,
        41.0f, 42.0f, 43.0f, 44.0f};

    SimdMatrix4x4F const m = Matrix4x4F_LoadUnalignedFloat4x4(source);

    SimdVector4F const r = Matrix4x4F_Trace(m);

    float const expected = 11.0f + 22.0f + 33.0f + 44.0f;
    CHECK_THAT(Vector4F_Extract<0>(r), WithinAbs(expected, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r), WithinAbs(expected, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r), WithinAbs(expected, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r), WithinAbs(expected, tolerance));
}

TEST_CASE("Matrix4x4F_CreateTranslation / scalar")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    static constexpr float tolerance = 0.0001f;

    auto const r = Matrix4x4F_CreateTranslation(2.0f, 3.0f, 4.0f);

    SimdVector4F const r0 = Matrix4x4F_Extract<0>(r);
    SimdVector4F const r1 = Matrix4x4F_Extract<1>(r);
    SimdVector4F const r2 = Matrix4x4F_Extract<2>(r);
    SimdVector4F const r3 = Matrix4x4F_Extract<3>(r);

    CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(1.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(2.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(3.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(4.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
}

TEST_CASE("Matrix4x4F_CreateTranslation / vector")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    static constexpr float tolerance = 0.0001f;

    auto const m = Matrix4x4F_CreateTranslation(Vector4F_Create(2.0f, 3.0f, 4.0f, 0.0f));

    SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
    SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
    SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
    SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);

    CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(1.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(2.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(3.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(4.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
}

TEST_CASE("Matrix4x4F_CreateScale / scalar")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    static constexpr float tolerance = 0.0001f;

    auto const r = Matrix4x4F_CreateScale(2.0f, 3.0f, 4.0f);

    SimdVector4F const r0 = Matrix4x4F_Extract<0>(r);
    SimdVector4F const r1 = Matrix4x4F_Extract<1>(r);
    SimdVector4F const r2 = Matrix4x4F_Extract<2>(r);
    SimdVector4F const r3 = Matrix4x4F_Extract<3>(r);

    CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(2.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(3.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(4.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
}

TEST_CASE("Matrix4x4F_CreateScale / vector")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    static constexpr float tolerance = 0.0001f;

    auto const m = Matrix4x4F_CreateScale(Vector4F_Create(2.0f, 3.0f, 4.0f, 0.0f));

    SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
    SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
    SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
    SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);

    CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(2.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(3.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(4.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
}

TEST_CASE("Matrix4x4F_CreateRotationX")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    static constexpr float tolerance = 0.0001f;

    SECTION("angle = 0")
    {
        auto const m = Matrix4x4F_CreateRotationX(DegreesToRadians(0.0f));

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
        SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
        SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
        SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);

        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
    }

    SECTION("angle = -30")
    {
        auto const m = Matrix4x4F_CreateRotationX(DegreesToRadians(-30.0f));

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
        SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
        SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
        SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);

        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(0.86603f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(-0.5f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.5f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(0.86603f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
    }

    SECTION("angle = 45")
    {
        auto const m = Matrix4x4F_CreateRotationX(DegreesToRadians(45.0f));

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
        SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
        SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
        SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);

        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(0.70711f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.70711f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(-0.70711f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(0.70711f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
    }

    SECTION("angle = 90")
    {
        auto const m = Matrix4x4F_CreateRotationX(DegreesToRadians(90.0f));

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
        SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
        SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
        SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);

        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(-1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
    }
}

TEST_CASE("Matrix4x4F_CreateRotationY")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    static constexpr float tolerance = 0.0001f;

    SECTION("angle = 0")
    {
        auto const m = Matrix4x4F_CreateRotationY(DegreesToRadians(0.0f));

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
        SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
        SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
        SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);

        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
    }

    SECTION("angle = -30")
    {
        auto const m = Matrix4x4F_CreateRotationY(DegreesToRadians(-30.0f));

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
        SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
        SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
        SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);

        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(0.86603f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.5f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(-0.5f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(0.86603f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
    }

    SECTION("angle = 45")
    {
        auto const m = Matrix4x4F_CreateRotationY(DegreesToRadians(45.0f));

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
        SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
        SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
        SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);

        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(0.70711f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(-0.70711f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.70711f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(0.70711f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
    }

    SECTION("angle = 90")
    {
        auto const m = Matrix4x4F_CreateRotationY(DegreesToRadians(90.0f));

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
        SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
        SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
        SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);

        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(-1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
    }
}

TEST_CASE("Matrix4x4F_CreateRotationZ")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    static constexpr float tolerance = 0.0001f;

    SECTION("angle = 0")
    {
        auto const m = Matrix4x4F_CreateRotationZ(DegreesToRadians(0.0f));

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
        SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
        SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
        SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);

        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
    }

    SECTION("angle = -30")
    {
        auto const m = Matrix4x4F_CreateRotationZ(DegreesToRadians(-30.0f));

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
        SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
        SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
        SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);

        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(0.86603f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(-0.5f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.5f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(0.86603f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
    }

    SECTION("angle = 45")
    {
        auto const m = Matrix4x4F_CreateRotationZ(DegreesToRadians(45.0f));

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
        SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
        SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
        SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);

        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(0.70711f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.70711f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(-0.70711f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(0.70711f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
    }

    SECTION("angle = 90")
    {
        auto const m = Matrix4x4F_CreateRotationZ(DegreesToRadians(90.0f));

        SimdVector4F const r0 = Matrix4x4F_Extract<0>(m);
        SimdVector4F const r1 = Matrix4x4F_Extract<1>(m);
        SimdVector4F const r2 = Matrix4x4F_Extract<2>(m);
        SimdVector4F const r3 = Matrix4x4F_Extract<3>(m);

        CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(-1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

        CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
    }
}

TEST_CASE("Matrix4x4F_LoadAlignedFloat4x4")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    static constexpr float tolerance = 0.0001f;

    Aligned::Matrix4x4F source;
    source.M11 = 11.0f;
    source.M12 = 12.0f;
    source.M13 = 13.0f;
    source.M14 = 14.0f;
    source.M21 = 21.0f;
    source.M22 = 22.0f;
    source.M23 = 23.0f;
    source.M24 = 24.0f;
    source.M31 = 31.0f;
    source.M32 = 32.0f;
    source.M33 = 33.0f;
    source.M34 = 34.0f;
    source.M41 = 41.0f;
    source.M42 = 42.0f;
    source.M43 = 43.0f;
    source.M44 = 44.0f;

    SimdMatrix4x4F const loaded = Matrix4x4F_LoadAlignedFloat4x4(reinterpret_cast<float const*>(&source));

    SimdVector4F const r0 = Matrix4x4F_Extract<0>(loaded);
    SimdVector4F const r1 = Matrix4x4F_Extract<1>(loaded);
    SimdVector4F const r2 = Matrix4x4F_Extract<2>(loaded);
    SimdVector4F const r3 = Matrix4x4F_Extract<3>(loaded);

    CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(11.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(12.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(13.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(14.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(21.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(22.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(23.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(24.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(31.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(32.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(33.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(34.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(41.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(42.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(43.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(44.0f, tolerance));
}

TEST_CASE("Matrix4x4F_LoadAlignedFloat4x3")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    static constexpr float tolerance = 0.0001f;

    Aligned::Matrix4x3F source;
    source.M11 = 11.0f;
    source.M12 = 12.0f;
    source.M13 = 13.0f;
    source.M21 = 21.0f;
    source.M22 = 22.0f;
    source.M23 = 23.0f;
    source.M31 = 31.0f;
    source.M32 = 32.0f;
    source.M33 = 33.0f;
    source.M41 = 41.0f;
    source.M42 = 42.0f;
    source.M43 = 43.0f;

    SimdMatrix4x4F const loaded = Matrix4x4F_LoadAlignedFloat4x3(reinterpret_cast<float const*>(&source));

    SimdVector4F const r0 = Matrix4x4F_Extract<0>(loaded);
    SimdVector4F const r1 = Matrix4x4F_Extract<1>(loaded);
    SimdVector4F const r2 = Matrix4x4F_Extract<2>(loaded);
    SimdVector4F const r3 = Matrix4x4F_Extract<3>(loaded);

    CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(11.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(12.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(13.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(21.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(22.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(23.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(31.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(32.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(33.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(41.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(42.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(43.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
}

TEST_CASE("Matrix4x4F_LoadAlignedFloat3x4")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    static constexpr float tolerance = 0.0001f;

    Aligned::Matrix3x4F source;
    source.M11 = 11.0f;
    source.M12 = 12.0f;
    source.M13 = 13.0f;
    source.M14 = 14.0f;
    source.M21 = 21.0f;
    source.M22 = 22.0f;
    source.M23 = 23.0f;
    source.M24 = 24.0f;
    source.M31 = 31.0f;
    source.M32 = 32.0f;
    source.M33 = 33.0f;
    source.M34 = 34.0f;

    SimdMatrix4x4F const loaded = Matrix4x4F_LoadAlignedFloat3x4(reinterpret_cast<float const*>(&source));

    SimdVector4F const r0 = Matrix4x4F_Extract<0>(loaded);
    SimdVector4F const r1 = Matrix4x4F_Extract<1>(loaded);
    SimdVector4F const r2 = Matrix4x4F_Extract<2>(loaded);
    SimdVector4F const r3 = Matrix4x4F_Extract<3>(loaded);

    CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(11.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(21.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(31.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(12.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(22.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(32.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(13.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(23.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(33.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(14.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(24.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(34.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
}

TEST_CASE("Matrix4x4F_LoadUnalignedFloat4x4")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    static constexpr float tolerance = 0.0001f;

    Packed::Matrix4x4F source;
    source.M11 = 11.0f;
    source.M12 = 12.0f;
    source.M13 = 13.0f;
    source.M14 = 14.0f;
    source.M21 = 21.0f;
    source.M22 = 22.0f;
    source.M23 = 23.0f;
    source.M24 = 24.0f;
    source.M31 = 31.0f;
    source.M32 = 32.0f;
    source.M33 = 33.0f;
    source.M34 = 34.0f;
    source.M41 = 41.0f;
    source.M42 = 42.0f;
    source.M43 = 43.0f;
    source.M44 = 44.0f;

    SimdMatrix4x4F const loaded = Matrix4x4F_LoadUnalignedFloat4x4(reinterpret_cast<float const*>(&source));

    SimdVector4F const r0 = Matrix4x4F_Extract<0>(loaded);
    SimdVector4F const r1 = Matrix4x4F_Extract<1>(loaded);
    SimdVector4F const r2 = Matrix4x4F_Extract<2>(loaded);
    SimdVector4F const r3 = Matrix4x4F_Extract<3>(loaded);

    CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(11.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(12.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(13.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(14.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(21.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(22.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(23.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(24.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(31.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(32.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(33.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(34.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(41.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(42.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(43.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(44.0f, tolerance));
}

TEST_CASE("Matrix4x4F_LoadUnalignedFloat4x3")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    static constexpr float tolerance = 0.0001f;

    Packed::Matrix4x3F source;
    source.M11 = 11.0f;
    source.M12 = 12.0f;
    source.M13 = 13.0f;
    source.M21 = 21.0f;
    source.M22 = 22.0f;
    source.M23 = 23.0f;
    source.M31 = 31.0f;
    source.M32 = 32.0f;
    source.M33 = 33.0f;
    source.M41 = 41.0f;
    source.M42 = 42.0f;
    source.M43 = 43.0f;

    SimdMatrix4x4F const loaded = Matrix4x4F_LoadUnalignedFloat4x3(reinterpret_cast<float const*>(&source));

    SimdVector4F const r0 = Matrix4x4F_Extract<0>(loaded);
    SimdVector4F const r1 = Matrix4x4F_Extract<1>(loaded);
    SimdVector4F const r2 = Matrix4x4F_Extract<2>(loaded);
    SimdVector4F const r3 = Matrix4x4F_Extract<3>(loaded);

    CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(11.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(12.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(13.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(21.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(22.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(23.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(31.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(32.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(33.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(41.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(42.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(43.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
}

TEST_CASE("Matrix4x4F_LoadUnalignedFloat3x4")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    static constexpr float tolerance = 0.0001f;

    Packed::Matrix3x4F source;
    source.M11 = 11.0f;
    source.M12 = 12.0f;
    source.M13 = 13.0f;
    source.M14 = 14.0f;
    source.M21 = 21.0f;
    source.M22 = 22.0f;
    source.M23 = 23.0f;
    source.M24 = 24.0f;
    source.M31 = 31.0f;
    source.M32 = 32.0f;
    source.M33 = 33.0f;
    source.M34 = 34.0f;

    SimdMatrix4x4F const loaded = Matrix4x4F_LoadUnalignedFloat3x4(reinterpret_cast<float const*>(&source));

    SimdVector4F const r0 = Matrix4x4F_Extract<0>(loaded);
    SimdVector4F const r1 = Matrix4x4F_Extract<1>(loaded);
    SimdVector4F const r2 = Matrix4x4F_Extract<2>(loaded);
    SimdVector4F const r3 = Matrix4x4F_Extract<3>(loaded);

    CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(11.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(21.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(31.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(12.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(22.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(32.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(13.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(23.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(33.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(14.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(24.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(34.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
}

TEST_CASE("Matrix4x4F_LoadUnalignedFloat3x3")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    static constexpr float tolerance = 0.0001f;

    Packed::Matrix3x3F source;
    source.M11 = 11.0f;
    source.M12 = 12.0f;
    source.M13 = 13.0f;
    source.M21 = 21.0f;
    source.M22 = 22.0f;
    source.M23 = 23.0f;
    source.M31 = 31.0f;
    source.M32 = 32.0f;
    source.M33 = 33.0f;

    SimdMatrix4x4F const loaded = Matrix4x4F_LoadUnalignedFloat3x3(reinterpret_cast<float const*>(&source));

    SimdVector4F const r0 = Matrix4x4F_Extract<0>(loaded);
    SimdVector4F const r1 = Matrix4x4F_Extract<1>(loaded);
    SimdVector4F const r2 = Matrix4x4F_Extract<2>(loaded);
    SimdVector4F const r3 = Matrix4x4F_Extract<3>(loaded);

    CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(11.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(12.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(13.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(21.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(22.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(23.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(31.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(32.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(33.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

    CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
}

TEST_CASE("Matrix4x4F_StoreAlignedFloat4x4")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    static constexpr float tolerance = 0.0001f;

    Aligned::Matrix4x4F source;
    source.M11 = 11.0f;
    source.M12 = 12.0f;
    source.M13 = 13.0f;
    source.M14 = 14.0f;
    source.M21 = 21.0f;
    source.M22 = 22.0f;
    source.M23 = 23.0f;
    source.M24 = 24.0f;
    source.M31 = 31.0f;
    source.M32 = 32.0f;
    source.M33 = 33.0f;
    source.M34 = 34.0f;
    source.M41 = 41.0f;
    source.M42 = 42.0f;
    source.M43 = 43.0f;
    source.M44 = 44.0f;

    SimdMatrix4x4F const loaded = Matrix4x4F_LoadAlignedFloat4x4(reinterpret_cast<float const*>(&source));

    Aligned::Matrix4x4F destination{};

    Matrix4x4F_StoreAlignedFloat4x4(reinterpret_cast<float*>(&destination), loaded);

    CHECK_THAT(destination.M11, WithinAbs(11.0f, tolerance));
    CHECK_THAT(destination.M12, WithinAbs(12.0f, tolerance));
    CHECK_THAT(destination.M13, WithinAbs(13.0f, tolerance));
    CHECK_THAT(destination.M14, WithinAbs(14.0f, tolerance));
    CHECK_THAT(destination.M21, WithinAbs(21.0f, tolerance));
    CHECK_THAT(destination.M22, WithinAbs(22.0f, tolerance));
    CHECK_THAT(destination.M23, WithinAbs(23.0f, tolerance));
    CHECK_THAT(destination.M24, WithinAbs(24.0f, tolerance));
    CHECK_THAT(destination.M31, WithinAbs(31.0f, tolerance));
    CHECK_THAT(destination.M32, WithinAbs(32.0f, tolerance));
    CHECK_THAT(destination.M33, WithinAbs(33.0f, tolerance));
    CHECK_THAT(destination.M34, WithinAbs(34.0f, tolerance));
    CHECK_THAT(destination.M41, WithinAbs(41.0f, tolerance));
    CHECK_THAT(destination.M42, WithinAbs(42.0f, tolerance));
    CHECK_THAT(destination.M43, WithinAbs(43.0f, tolerance));
    CHECK_THAT(destination.M44, WithinAbs(44.0f, tolerance));
}

TEST_CASE("Matrix4x4F_StoreAlignedFloat4x3")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    static constexpr float tolerance = 0.0001f;

    Aligned::Matrix4x4F source;
    source.M11 = 11.0f;
    source.M12 = 12.0f;
    source.M13 = 13.0f;
    source.M14 = 14.0f;
    source.M21 = 21.0f;
    source.M22 = 22.0f;
    source.M23 = 23.0f;
    source.M24 = 24.0f;
    source.M31 = 31.0f;
    source.M32 = 32.0f;
    source.M33 = 33.0f;
    source.M34 = 34.0f;
    source.M41 = 41.0f;
    source.M42 = 42.0f;
    source.M43 = 43.0f;
    source.M44 = 44.0f;

    SimdMatrix4x4F const loaded = Matrix4x4F_LoadAlignedFloat4x4(reinterpret_cast<float const*>(&source));

    Aligned::Matrix4x3F destination{};

    Matrix4x4F_StoreAlignedFloat4x3(reinterpret_cast<float*>(&destination), loaded);

    CHECK_THAT(destination.M11, WithinAbs(11.0f, tolerance));
    CHECK_THAT(destination.M12, WithinAbs(12.0f, tolerance));
    CHECK_THAT(destination.M13, WithinAbs(13.0f, tolerance));
    CHECK_THAT(destination.M21, WithinAbs(21.0f, tolerance));
    CHECK_THAT(destination.M22, WithinAbs(22.0f, tolerance));
    CHECK_THAT(destination.M23, WithinAbs(23.0f, tolerance));
    CHECK_THAT(destination.M31, WithinAbs(31.0f, tolerance));
    CHECK_THAT(destination.M32, WithinAbs(32.0f, tolerance));
    CHECK_THAT(destination.M33, WithinAbs(33.0f, tolerance));
    CHECK_THAT(destination.M41, WithinAbs(41.0f, tolerance));
    CHECK_THAT(destination.M42, WithinAbs(42.0f, tolerance));
    CHECK_THAT(destination.M43, WithinAbs(43.0f, tolerance));
}

TEST_CASE("Matrix4x4F_StoreAlignedFloat3x4")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    static constexpr float tolerance = 0.0001f;

    Aligned::Matrix4x4F source;
    source.M11 = 11.0f;
    source.M12 = 12.0f;
    source.M13 = 13.0f;
    source.M14 = 14.0f;
    source.M21 = 21.0f;
    source.M22 = 22.0f;
    source.M23 = 23.0f;
    source.M24 = 24.0f;
    source.M31 = 31.0f;
    source.M32 = 32.0f;
    source.M33 = 33.0f;
    source.M34 = 34.0f;
    source.M41 = 41.0f;
    source.M42 = 42.0f;
    source.M43 = 43.0f;
    source.M44 = 44.0f;

    SimdMatrix4x4F const loaded = Matrix4x4F_LoadAlignedFloat4x4(reinterpret_cast<float const*>(&source));

    Aligned::Matrix3x4F destination{};

    Matrix4x4F_StoreAlignedFloat3x4(reinterpret_cast<float*>(&destination), loaded);

    CHECK_THAT(destination.M11, WithinAbs(11.0f, tolerance));
    CHECK_THAT(destination.M12, WithinAbs(21.0f, tolerance));
    CHECK_THAT(destination.M13, WithinAbs(31.0f, tolerance));
    CHECK_THAT(destination.M14, WithinAbs(41.0f, tolerance));
    CHECK_THAT(destination.M21, WithinAbs(12.0f, tolerance));
    CHECK_THAT(destination.M22, WithinAbs(22.0f, tolerance));
    CHECK_THAT(destination.M23, WithinAbs(32.0f, tolerance));
    CHECK_THAT(destination.M24, WithinAbs(42.0f, tolerance));
    CHECK_THAT(destination.M31, WithinAbs(13.0f, tolerance));
    CHECK_THAT(destination.M32, WithinAbs(23.0f, tolerance));
    CHECK_THAT(destination.M33, WithinAbs(33.0f, tolerance));
    CHECK_THAT(destination.M34, WithinAbs(43.0f, tolerance));
}

TEST_CASE("Matrix4x4F_StoreUnalignedFloat4x4")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    static constexpr float tolerance = 0.0001f;

    Aligned::Matrix4x4F source;
    source.M11 = 11.0f;
    source.M12 = 12.0f;
    source.M13 = 13.0f;
    source.M14 = 14.0f;
    source.M21 = 21.0f;
    source.M22 = 22.0f;
    source.M23 = 23.0f;
    source.M24 = 24.0f;
    source.M31 = 31.0f;
    source.M32 = 32.0f;
    source.M33 = 33.0f;
    source.M34 = 34.0f;
    source.M41 = 41.0f;
    source.M42 = 42.0f;
    source.M43 = 43.0f;
    source.M44 = 44.0f;

    SimdMatrix4x4F const loaded = Matrix4x4F_LoadAlignedFloat4x4(reinterpret_cast<float const*>(&source));

    Packed::Matrix4x4F destination{};

    Matrix4x4F_StoreUnalignedFloat4x4(reinterpret_cast<float*>(&destination), loaded);

    CHECK_THAT(destination.M11, WithinAbs(11.0f, tolerance));
    CHECK_THAT(destination.M12, WithinAbs(12.0f, tolerance));
    CHECK_THAT(destination.M13, WithinAbs(13.0f, tolerance));
    CHECK_THAT(destination.M14, WithinAbs(14.0f, tolerance));
    CHECK_THAT(destination.M21, WithinAbs(21.0f, tolerance));
    CHECK_THAT(destination.M22, WithinAbs(22.0f, tolerance));
    CHECK_THAT(destination.M23, WithinAbs(23.0f, tolerance));
    CHECK_THAT(destination.M24, WithinAbs(24.0f, tolerance));
    CHECK_THAT(destination.M31, WithinAbs(31.0f, tolerance));
    CHECK_THAT(destination.M32, WithinAbs(32.0f, tolerance));
    CHECK_THAT(destination.M33, WithinAbs(33.0f, tolerance));
    CHECK_THAT(destination.M34, WithinAbs(34.0f, tolerance));
    CHECK_THAT(destination.M41, WithinAbs(41.0f, tolerance));
    CHECK_THAT(destination.M42, WithinAbs(42.0f, tolerance));
    CHECK_THAT(destination.M43, WithinAbs(43.0f, tolerance));
    CHECK_THAT(destination.M44, WithinAbs(44.0f, tolerance));
}

TEST_CASE("Matrix4x4F_StoreUnalignedFloat4x3")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    static constexpr float tolerance = 0.0001f;

    Aligned::Matrix4x4F source;
    source.M11 = 11.0f;
    source.M12 = 12.0f;
    source.M13 = 13.0f;
    source.M14 = 14.0f;
    source.M21 = 21.0f;
    source.M22 = 22.0f;
    source.M23 = 23.0f;
    source.M24 = 24.0f;
    source.M31 = 31.0f;
    source.M32 = 32.0f;
    source.M33 = 33.0f;
    source.M34 = 34.0f;
    source.M41 = 41.0f;
    source.M42 = 42.0f;
    source.M43 = 43.0f;
    source.M44 = 44.0f;

    SimdMatrix4x4F const loaded = Matrix4x4F_LoadAlignedFloat4x4(reinterpret_cast<float const*>(&source));

    Packed::Matrix4x3F destination{};

    Matrix4x4F_StoreUnalignedFloat4x3(reinterpret_cast<float*>(&destination), loaded);

    CHECK_THAT(destination.M11, WithinAbs(11.0f, tolerance));
    CHECK_THAT(destination.M12, WithinAbs(12.0f, tolerance));
    CHECK_THAT(destination.M13, WithinAbs(13.0f, tolerance));
    CHECK_THAT(destination.M21, WithinAbs(21.0f, tolerance));
    CHECK_THAT(destination.M22, WithinAbs(22.0f, tolerance));
    CHECK_THAT(destination.M23, WithinAbs(23.0f, tolerance));
    CHECK_THAT(destination.M31, WithinAbs(31.0f, tolerance));
    CHECK_THAT(destination.M32, WithinAbs(32.0f, tolerance));
    CHECK_THAT(destination.M33, WithinAbs(33.0f, tolerance));
    CHECK_THAT(destination.M41, WithinAbs(41.0f, tolerance));
    CHECK_THAT(destination.M42, WithinAbs(42.0f, tolerance));
    CHECK_THAT(destination.M43, WithinAbs(43.0f, tolerance));
}

TEST_CASE("Matrix4x4F_StoreUnalignedFloat3x4")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    static constexpr float tolerance = 0.0001f;

    Aligned::Matrix4x4F source;
    source.M11 = 11.0f;
    source.M12 = 12.0f;
    source.M13 = 13.0f;
    source.M14 = 14.0f;
    source.M21 = 21.0f;
    source.M22 = 22.0f;
    source.M23 = 23.0f;
    source.M24 = 24.0f;
    source.M31 = 31.0f;
    source.M32 = 32.0f;
    source.M33 = 33.0f;
    source.M34 = 34.0f;
    source.M41 = 41.0f;
    source.M42 = 42.0f;
    source.M43 = 43.0f;
    source.M44 = 44.0f;

    SimdMatrix4x4F const loaded = Matrix4x4F_LoadAlignedFloat4x4(reinterpret_cast<float const*>(&source));

    Packed::Matrix3x4F destination{};

    Matrix4x4F_StoreUnalignedFloat3x4(reinterpret_cast<float*>(&destination), loaded);

    CHECK_THAT(destination.M11, WithinAbs(11.0f, tolerance));
    CHECK_THAT(destination.M12, WithinAbs(21.0f, tolerance));
    CHECK_THAT(destination.M13, WithinAbs(31.0f, tolerance));
    CHECK_THAT(destination.M14, WithinAbs(41.0f, tolerance));
    CHECK_THAT(destination.M21, WithinAbs(12.0f, tolerance));
    CHECK_THAT(destination.M22, WithinAbs(22.0f, tolerance));
    CHECK_THAT(destination.M23, WithinAbs(32.0f, tolerance));
    CHECK_THAT(destination.M24, WithinAbs(42.0f, tolerance));
    CHECK_THAT(destination.M31, WithinAbs(13.0f, tolerance));
    CHECK_THAT(destination.M32, WithinAbs(23.0f, tolerance));
    CHECK_THAT(destination.M33, WithinAbs(33.0f, tolerance));
    CHECK_THAT(destination.M34, WithinAbs(43.0f, tolerance));
}

TEST_CASE("Matrix4x4F_StoreUnalignedFloat3x3")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math;
    using namespace Anemone::Math::Detail;

    static constexpr float tolerance = 0.0001f;

    Aligned::Matrix4x4F source;
    source.M11 = 11.0f;
    source.M12 = 12.0f;
    source.M13 = 13.0f;
    source.M14 = 14.0f;
    source.M21 = 21.0f;
    source.M22 = 22.0f;
    source.M23 = 23.0f;
    source.M24 = 24.0f;
    source.M31 = 31.0f;
    source.M32 = 32.0f;
    source.M33 = 33.0f;
    source.M34 = 34.0f;
    source.M41 = 41.0f;
    source.M42 = 42.0f;
    source.M43 = 43.0f;
    source.M44 = 44.0f;

    SimdMatrix4x4F const loaded = Matrix4x4F_LoadAlignedFloat4x4(reinterpret_cast<float const*>(&source));

    Packed::Matrix3x3F destination{};

    Matrix4x4F_StoreUnalignedFloat3x3(reinterpret_cast<float*>(&destination), loaded);

    CHECK_THAT(destination.M11, WithinAbs(11.0f, tolerance));
    CHECK_THAT(destination.M12, WithinAbs(12.0f, tolerance));
    CHECK_THAT(destination.M13, WithinAbs(13.0f, tolerance));
    CHECK_THAT(destination.M21, WithinAbs(21.0f, tolerance));
    CHECK_THAT(destination.M22, WithinAbs(22.0f, tolerance));
    CHECK_THAT(destination.M23, WithinAbs(23.0f, tolerance));
    CHECK_THAT(destination.M31, WithinAbs(31.0f, tolerance));
    CHECK_THAT(destination.M32, WithinAbs(32.0f, tolerance));
    CHECK_THAT(destination.M33, WithinAbs(33.0f, tolerance));
}
