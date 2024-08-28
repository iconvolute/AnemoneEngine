#if false
#include <AnemoneCore/Numerics/Simd.hxx>

#include <catch_amalgamated.hpp>

TEST_CASE("FMatrix4x4A / LoadStore")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Numerics;
    using namespace Anemone;

    SECTION("Store operations")
    {
        FMatrix4x4 const reference{
            11.0f, 12.0f, 13.0f, 14.0f,
            21.0f, 22.0f, 23.0f, 24.0f,
            31.0f, 32.0f, 33.0f, 34.0f,
            41.0f, 42.0f, 43.0f, 44.0f};

        FMatrix4x4A const s = FMatrix4x4A::Load(reference);

        SECTION("Store FMatrix4x3")
        {
            FMatrix4x3 destination;
            FMatrix4x4A::Store(destination, s);

            CHECK_THAT(destination.M11, WithinAbs(reference.M11, 0.0001));
            CHECK_THAT(destination.M12, WithinAbs(reference.M12, 0.0001));
            CHECK_THAT(destination.M13, WithinAbs(reference.M13, 0.0001));

            CHECK_THAT(destination.M21, WithinAbs(reference.M21, 0.0001));
            CHECK_THAT(destination.M22, WithinAbs(reference.M22, 0.0001));
            CHECK_THAT(destination.M23, WithinAbs(reference.M23, 0.0001));

            CHECK_THAT(destination.M31, WithinAbs(reference.M31, 0.0001));
            CHECK_THAT(destination.M32, WithinAbs(reference.M32, 0.0001));
            CHECK_THAT(destination.M33, WithinAbs(reference.M33, 0.0001));

            CHECK_THAT(destination.M41, WithinAbs(reference.M41, 0.0001));
            CHECK_THAT(destination.M42, WithinAbs(reference.M42, 0.0001));
            CHECK_THAT(destination.M43, WithinAbs(reference.M43, 0.0001));
        }
    }

    SECTION("Load operations")
    {
        SECTION("FMatrix4x3")
        {
            FMatrix4x3 const reference{
                11.0f, 12.0f, 13.0f,
                21.0f, 22.0f, 23.0f,
                31.0f, 32.0f, 33.0f,
                41.0f, 42.0f, 43.0f};

            FMatrix4x4A const loaded = FMatrix4x4A::Load(reference);

            FMatrix4x4 const expected{
                11.0f, 12.0f, 13.0f, 0.0f,
                21.0f, 22.0f, 23.0f, 0.0f,
                31.0f, 32.0f, 33.0f, 0.0f,
                41.0f, 42.0f, 43.0f, 1.0f};

            FMatrix4x4 result;

            FMatrix4x4A::Store(result, loaded);

            CHECK_THAT(result.M11, WithinAbs(expected.M11, 0.0001));
            CHECK_THAT(result.M12, WithinAbs(expected.M12, 0.0001));
            CHECK_THAT(result.M13, WithinAbs(expected.M13, 0.0001));
            CHECK_THAT(result.M14, WithinAbs(expected.M14, 0.0001));

            CHECK_THAT(result.M21, WithinAbs(expected.M21, 0.0001));
            CHECK_THAT(result.M22, WithinAbs(expected.M22, 0.0001));
            CHECK_THAT(result.M23, WithinAbs(expected.M23, 0.0001));
            CHECK_THAT(result.M24, WithinAbs(expected.M24, 0.0001));

            CHECK_THAT(result.M31, WithinAbs(expected.M31, 0.0001));
            CHECK_THAT(result.M32, WithinAbs(expected.M32, 0.0001));
            CHECK_THAT(result.M33, WithinAbs(expected.M33, 0.0001));
            CHECK_THAT(result.M34, WithinAbs(expected.M34, 0.0001));

            CHECK_THAT(result.M41, WithinAbs(expected.M41, 0.0001));
            CHECK_THAT(result.M42, WithinAbs(expected.M42, 0.0001));
            CHECK_THAT(result.M43, WithinAbs(expected.M43, 0.0001));
            CHECK_THAT(result.M44, WithinAbs(expected.M44, 0.0001));
        }

        SECTION("FMatrix3x4")
        {
            FMatrix3x4 const reference{
                11.0f, 12.0f, 13.0f, 14.0f,
                21.0f, 22.0f, 23.0f, 24.0f,
                31.0f, 32.0f, 33.0f, 34.0f};

            FMatrix4x4A const loaded = FMatrix4x4A::Load(reference);

            FMatrix4x4 const expected{
                11.0f, 21.0f, 31.0f, 0.0f,
                12.0f, 22.0f, 32.0f, 0.0f,
                13.0f, 23.0f, 33.0f, 0.0f,
                14.0f, 24.0f, 34.0f, 1.0f};

            FMatrix4x4 result;

            FMatrix4x4A::Store(result, loaded);

            CHECK_THAT(result.M11, WithinAbs(expected.M11, 0.0001));
            CHECK_THAT(result.M12, WithinAbs(expected.M12, 0.0001));
            CHECK_THAT(result.M13, WithinAbs(expected.M13, 0.0001));
            CHECK_THAT(result.M14, WithinAbs(expected.M14, 0.0001));

            CHECK_THAT(result.M21, WithinAbs(expected.M21, 0.0001));
            CHECK_THAT(result.M22, WithinAbs(expected.M22, 0.0001));
            CHECK_THAT(result.M23, WithinAbs(expected.M23, 0.0001));
            CHECK_THAT(result.M24, WithinAbs(expected.M24, 0.0001));

            CHECK_THAT(result.M31, WithinAbs(expected.M31, 0.0001));
            CHECK_THAT(result.M32, WithinAbs(expected.M32, 0.0001));
            CHECK_THAT(result.M33, WithinAbs(expected.M33, 0.0001));
            CHECK_THAT(result.M34, WithinAbs(expected.M34, 0.0001));

            CHECK_THAT(result.M41, WithinAbs(expected.M41, 0.0001));
            CHECK_THAT(result.M42, WithinAbs(expected.M42, 0.0001));
            CHECK_THAT(result.M43, WithinAbs(expected.M43, 0.0001));
            CHECK_THAT(result.M44, WithinAbs(expected.M44, 0.0001));
        }

        SECTION("FMatrix3x3")
        {
            FMatrix3x3 const reference{
                11.0f, 12.0f, 13.0f,
                21.0f, 22.0f, 23.0f,
                31.0f, 32.0f, 33.0f};

            FMatrix4x4A const loaded = FMatrix4x4A::Load(reference);

            FMatrix4x4 const expected{
                11.0f, 12.0f, 13.0f, 0.0f,
                21.0f, 22.0f, 23.0f, 0.0f,
                31.0f, 32.0f, 33.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f};

            FMatrix4x4 result;

            FMatrix4x4A::Store(result, loaded);

            CHECK_THAT(result.M11, WithinAbs(expected.M11, 0.0001));
            CHECK_THAT(result.M12, WithinAbs(expected.M12, 0.0001));
            CHECK_THAT(result.M13, WithinAbs(expected.M13, 0.0001));
            CHECK_THAT(result.M14, WithinAbs(expected.M14, 0.0001));

            CHECK_THAT(result.M21, WithinAbs(expected.M21, 0.0001));
            CHECK_THAT(result.M22, WithinAbs(expected.M22, 0.0001));
            CHECK_THAT(result.M23, WithinAbs(expected.M23, 0.0001));
            CHECK_THAT(result.M24, WithinAbs(expected.M24, 0.0001));

            CHECK_THAT(result.M31, WithinAbs(expected.M31, 0.0001));
            CHECK_THAT(result.M32, WithinAbs(expected.M32, 0.0001));
            CHECK_THAT(result.M33, WithinAbs(expected.M33, 0.0001));
            CHECK_THAT(result.M34, WithinAbs(expected.M34, 0.0001));

            CHECK_THAT(result.M41, WithinAbs(expected.M41, 0.0001));
            CHECK_THAT(result.M42, WithinAbs(expected.M42, 0.0001));
            CHECK_THAT(result.M43, WithinAbs(expected.M43, 0.0001));
            CHECK_THAT(result.M44, WithinAbs(expected.M44, 0.0001));
        }
    }
}
#endif
