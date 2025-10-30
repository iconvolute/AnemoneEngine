#include "AnemoneMath/Detail/SimdFloat.hxx"
#include "AnemoneRandom/Generator.hxx"
#include "AnemoneRandom/Distribution.hxx"

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

ANEMONE_EXTERNAL_HEADERS_END

TEST_CASE("RotorF_CreateFromAxisAngle")
{
    using namespace Anemone::Math::Detail;
    using namespace Anemone::Math;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.0001f;

    SECTION("axis = X")
    {
        SimdVector4F const axis = Vector4F_PositiveUnitX();

        SECTION("angle = -30")
        {
            SimdVector4F const rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians<float>(-30.0f));

            CHECK_THAT(Vector4F_Extract<0>(rotor), WithinAbs(0.25882f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(rotor), WithinAbs(0.96593f, tolerance));
        }

        SECTION("angle = 0")
        {
            SimdVector4F const rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians<float>(0.0f));

            CHECK_THAT(Vector4F_Extract<0>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(rotor), WithinAbs(1.0f, tolerance));
        }

        SECTION("angle = 45")
        {
            SimdVector4F const rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians<float>(45.0f));

            CHECK_THAT(Vector4F_Extract<0>(rotor), WithinAbs(-0.38268f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(rotor), WithinAbs(0.92380f, tolerance));
        }

        SECTION("angle = 90")
        {
            SimdVector4F const rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians<float>(90.0f));

            CHECK_THAT(Vector4F_Extract<0>(rotor), WithinAbs(-0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(rotor), WithinAbs(0.70711f, tolerance));
        }

        SECTION("angle = 270")
        {
            SimdVector4F const rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians<float>(270.0f));

            CHECK_THAT(Vector4F_Extract<0>(rotor), WithinAbs(-0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(rotor), WithinAbs(-0.70711f, tolerance));
        }
    }

    SECTION("axis = Y")
    {
        SimdVector4F const axis = Vector4F_PositiveUnitY();

        SECTION("angle = -30")
        {
            SimdVector4F const rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians<float>(-30.0f));

            CHECK_THAT(Vector4F_Extract<0>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(rotor), WithinAbs(0.25882f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(rotor), WithinAbs(0.96593f, tolerance));
        }

        SECTION("angle = 0")
        {
            SimdVector4F const rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians<float>(0.0f));

            CHECK_THAT(Vector4F_Extract<0>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(rotor), WithinAbs(1.0f, tolerance));
        }

        SECTION("angle = 45")
        {
            SimdVector4F const rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians<float>(45.0f));

            CHECK_THAT(Vector4F_Extract<0>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(rotor), WithinAbs(-0.38268f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(rotor), WithinAbs(0.92388f, tolerance));
        }

        SECTION("angle = 90")
        {
            SimdVector4F const rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians<float>(90.0f));

            CHECK_THAT(Vector4F_Extract<0>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(rotor), WithinAbs(-0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(rotor), WithinAbs(0.70711f, tolerance));
        }

        SECTION("angle = 270")
        {
            SimdVector4F const rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians<float>(270.0f));

            CHECK_THAT(Vector4F_Extract<0>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(rotor), WithinAbs(-0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(rotor), WithinAbs(-0.70711f, tolerance));
        }
    }

    SECTION("axis = Z")
    {
        SimdVector4F const axis = Vector4F_PositiveUnitZ();

        SECTION("angle = -30")
        {
            SimdVector4F const rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians<float>(-30.0f));

            CHECK_THAT(Vector4F_Extract<0>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(rotor), WithinAbs(0.25882f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(rotor), WithinAbs(0.96593f, tolerance));
        }

        SECTION("angle = 0")
        {
            SimdVector4F const rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians<float>(0.0f));

            CHECK_THAT(Vector4F_Extract<0>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(rotor), WithinAbs(1.0f, tolerance));
        }

        SECTION("angle = 45")
        {
            SimdVector4F const rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians<float>(45.0f));

            CHECK_THAT(Vector4F_Extract<0>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(rotor), WithinAbs(-0.38268f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(rotor), WithinAbs(0.92388f, tolerance));
        }

        SECTION("angle = 90")
        {
            SimdVector4F const rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians<float>(90.0f));

            CHECK_THAT(Vector4F_Extract<0>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(rotor), WithinAbs(-0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(rotor), WithinAbs(0.70711f, tolerance));
        }

        SECTION("angle = 270")
        {
            SimdVector4F const rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians<float>(270.0f));

            CHECK_THAT(Vector4F_Extract<0>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(rotor), WithinAbs(-0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(rotor), WithinAbs(-0.70711f, tolerance));
        }
    }

    SECTION("axis = (2.0f, 3.0f, 5.0f)")
    {
        SimdVector4F const axis = Vector4F_Create(2.0f, 3.0f, 5.0f, 0.0f);

        SECTION("angle = -30")
        {
            SimdVector4F const rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians<float>(-30.0f));

            CHECK_THAT(Vector4F_Extract<0>(rotor), WithinAbs(0.08397f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(rotor), WithinAbs(0.12596f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(rotor), WithinAbs(0.20993f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(rotor), WithinAbs(0.96593f, tolerance));
        }

        SECTION("angle = 0")
        {
            SimdVector4F const rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians<float>(0.0f));

            CHECK_THAT(Vector4F_Extract<0>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(rotor), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(rotor), WithinAbs(1.0f, tolerance));
        }

        SECTION("angle = 45")
        {
            SimdVector4F const rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians<float>(45.0f));

            CHECK_THAT(Vector4F_Extract<0>(rotor), WithinAbs(-0.12416f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(rotor), WithinAbs(-0.18624f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(rotor), WithinAbs(-0.3104f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(rotor), WithinAbs(0.92388f, tolerance));
        }

        SECTION("angle = 90")
        {
            SimdVector4F const rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians<float>(90.0f));

            CHECK_THAT(Vector4F_Extract<0>(rotor), WithinAbs(-0.22942f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(rotor), WithinAbs(-0.34412f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(rotor), WithinAbs(-0.57354f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(rotor), WithinAbs(0.70711f, tolerance));
        }

        SECTION("angle = 270")
        {
            SimdVector4F const rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians<float>(270.0f));

            CHECK_THAT(Vector4F_Extract<0>(rotor), WithinAbs(-0.22942f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(rotor), WithinAbs(-0.34412f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(rotor), WithinAbs(-0.57354f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(rotor), WithinAbs(-0.70711f, tolerance));
        }
    }
}

TEST_CASE("RotorF_Rotate3")
{
    using namespace Anemone::Math::Detail;
    using namespace Anemone::Math;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.0001f;

    SECTION("Simple case")
    {
        auto const rotor = RotorF_RotationBetween(Vector4F_PositiveUnitX(), Vector4F_PositiveUnitY());

        CHECK_THAT(Vector4F_Extract<0>(rotor), WithinAbs(-0.70711f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(rotor), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(rotor), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(rotor), WithinAbs(0.70711f, tolerance));

        auto const v = Vector4F_PositiveUnitX();

        auto const v_rotated = RotorF_Rotate3(rotor, v);

        CHECK_THAT(Vector4F_Extract<0>(v_rotated), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(v_rotated), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(v_rotated), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(v_rotated), WithinAbs(0.0f, tolerance));

        auto const v_reversed = RotorF_InverseRotate3(rotor, v_rotated);

        CHECK_THAT(Vector4F_Extract<0>(v_reversed), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(v_reversed), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(v_reversed), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(v_reversed), WithinAbs(0.0f, tolerance));
    }

    SECTION("Random")
    {
        Random random{2137};
        UniformDistribution<float> uniform{};

        // Pick random unit vector
        for (size_t i = 0; i < 16; ++i)
        {
            float const axis_x = uniform(random, -1.0f, 1.0f);
            float const axis_y = uniform(random, -1.0f, 1.0f);
            float const axis_z = 1.0f - (axis_x * axis_x + axis_y * axis_y);

            SimdVector4F const axis = Vector4F_Normalize3(Vector4F_Create(axis_x, axis_y, axis_z, 0.0f));

            float const unit_x = uniform(random, -1.0f, 1.0f);
            float const unit_y = uniform(random, -1.0f, 1.0f);
            float const unit_z = 1.0f - (unit_x * unit_x + unit_y * unit_y);

            SimdVector4F const unit = Vector4F_Normalize3(Vector4F_Create(unit_x, unit_y, unit_z, 0.0f));

            float const angle = uniform(random, -Pi2<float>, Pi2<float>);

            auto const rotation = RotorF_CreateFromAxisAngle(axis, angle);

            auto const unit_rotated = RotorF_Rotate3(rotation, unit);

            auto const unit_reversed = RotorF_InverseRotate3(rotation, unit_rotated);

            CHECK_THAT(Vector4F_Extract<0>(unit_reversed), WithinAbs(Vector4F_Extract<0>(unit), tolerance));
            CHECK_THAT(Vector4F_Extract<1>(unit_reversed), WithinAbs(Vector4F_Extract<1>(unit), tolerance));
            CHECK_THAT(Vector4F_Extract<2>(unit_reversed), WithinAbs(Vector4F_Extract<2>(unit), tolerance));
        }
    }
}

TEST_CASE("RotorF_ToMatrix4x4F")
{
    using namespace Anemone::Math::Detail;
    using namespace Anemone::Math;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.0001f;

    SECTION("axis = X")
    {
        SimdVector4F const axis = Vector4F_PositiveUnitX();
        SECTION("angle = -30")
        {
            SimdVector4F rotor = RotorF_CreateFromNormalAngle(axis, DegreesToRadians(-30.0f));
            SimdMatrix4x4F matrix = RotorF_ToMatrix4x4F(rotor);

            SimdVector4F const r0 = Matrix4x4F_Extract<0>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(0.86603f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(-0.5f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

            SimdVector4F const r1 = Matrix4x4F_Extract<1>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.5f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(0.86603f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

            SimdVector4F const r2 = Matrix4x4F_Extract<2>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

            SimdVector4F const r3 = Matrix4x4F_Extract<3>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
        }

        SECTION("angle = 0")
        {
            SimdVector4F rotor = RotorF_CreateFromNormalAngle(axis, DegreesToRadians(0.0f));
            SimdMatrix4x4F matrix = RotorF_ToMatrix4x4F(rotor);

            SimdVector4F const r0 = Matrix4x4F_Extract<0>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

            SimdVector4F const r1 = Matrix4x4F_Extract<1>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

            SimdVector4F const r2 = Matrix4x4F_Extract<2>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

            SimdVector4F const r3 = Matrix4x4F_Extract<3>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
        }

        SECTION("angle = 45")
        {
            SimdVector4F rotor = RotorF_CreateFromNormalAngle(axis, DegreesToRadians(45.0f));
            SimdMatrix4x4F matrix = RotorF_ToMatrix4x4F(rotor);

            SimdVector4F const r0 = Matrix4x4F_Extract<0>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

            SimdVector4F const r1 = Matrix4x4F_Extract<1>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(-0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

            SimdVector4F const r2 = Matrix4x4F_Extract<2>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

            SimdVector4F const r3 = Matrix4x4F_Extract<3>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
        }

        SECTION("angle = 90")
        {
            SimdVector4F rotor = RotorF_CreateFromNormalAngle(axis, DegreesToRadians(90.0f));
            SimdMatrix4x4F matrix = RotorF_ToMatrix4x4F(rotor);

            SimdVector4F const r0 = Matrix4x4F_Extract<0>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

            SimdVector4F const r1 = Matrix4x4F_Extract<1>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(-1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

            SimdVector4F const r2 = Matrix4x4F_Extract<2>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

            SimdVector4F const r3 = Matrix4x4F_Extract<3>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
        }

        SECTION("angle = 135")
        {
            SimdVector4F rotor = RotorF_CreateFromNormalAngle(axis, DegreesToRadians(135.0f));
            SimdMatrix4x4F matrix = RotorF_ToMatrix4x4F(rotor);

            SimdVector4F const r0 = Matrix4x4F_Extract<0>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(-0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

            SimdVector4F const r1 = Matrix4x4F_Extract<1>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(-0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(-0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

            SimdVector4F const r2 = Matrix4x4F_Extract<2>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

            SimdVector4F const r3 = Matrix4x4F_Extract<3>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
        }

        SECTION("angle = 270")
        {
            SimdVector4F rotor = RotorF_CreateFromNormalAngle(axis, DegreesToRadians(270.0f));
            SimdMatrix4x4F matrix = RotorF_ToMatrix4x4F(rotor);

            SimdVector4F const r0 = Matrix4x4F_Extract<0>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(-1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

            SimdVector4F const r1 = Matrix4x4F_Extract<1>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

            SimdVector4F const r2 = Matrix4x4F_Extract<2>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

            SimdVector4F const r3 = Matrix4x4F_Extract<3>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
        }
    }

    SECTION("axis = Y")
    {
        SimdVector4F const axis = Vector4F_PositiveUnitY();
        SECTION("angle = -30")
        {
            SimdVector4F rotor = RotorF_CreateFromNormalAngle(axis, DegreesToRadians(-30.0f));
            SimdMatrix4x4F matrix = RotorF_ToMatrix4x4F(rotor);

            SimdVector4F const r0 = Matrix4x4F_Extract<0>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(0.86603f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(-0.5f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

            SimdVector4F const r1 = Matrix4x4F_Extract<1>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

            SimdVector4F const r2 = Matrix4x4F_Extract<2>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.5f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(0.86603f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

            SimdVector4F const r3 = Matrix4x4F_Extract<3>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
        }

        SECTION("angle = 0")
        {
            SimdVector4F rotor = RotorF_CreateFromNormalAngle(axis, DegreesToRadians(0.0f));
            SimdMatrix4x4F matrix = RotorF_ToMatrix4x4F(rotor);

            SimdVector4F const r0 = Matrix4x4F_Extract<0>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

            SimdVector4F const r1 = Matrix4x4F_Extract<1>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

            SimdVector4F const r2 = Matrix4x4F_Extract<2>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

            SimdVector4F const r3 = Matrix4x4F_Extract<3>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
        }

        SECTION("angle = 45")
        {
            SimdVector4F rotor = RotorF_CreateFromNormalAngle(axis, DegreesToRadians(45.0f));
            SimdMatrix4x4F matrix = RotorF_ToMatrix4x4F(rotor);

            SimdVector4F const r0 = Matrix4x4F_Extract<0>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

            SimdVector4F const r1 = Matrix4x4F_Extract<1>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

            SimdVector4F const r2 = Matrix4x4F_Extract<2>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(-0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

            SimdVector4F const r3 = Matrix4x4F_Extract<3>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
        }

        SECTION("angle = 90")
        {
            SimdVector4F rotor = RotorF_CreateFromNormalAngle(axis, DegreesToRadians(90.0f));
            SimdMatrix4x4F matrix = RotorF_ToMatrix4x4F(rotor);

            SimdVector4F const r0 = Matrix4x4F_Extract<0>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

            SimdVector4F const r1 = Matrix4x4F_Extract<1>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

            SimdVector4F const r2 = Matrix4x4F_Extract<2>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(-1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

            SimdVector4F const r3 = Matrix4x4F_Extract<3>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
        }

        SECTION("angle = 135")
        {
            SimdVector4F rotor = RotorF_CreateFromNormalAngle(axis, DegreesToRadians(135.0f));
            SimdMatrix4x4F matrix = RotorF_ToMatrix4x4F(rotor);

            SimdVector4F const r0 = Matrix4x4F_Extract<0>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(-0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

            SimdVector4F const r1 = Matrix4x4F_Extract<1>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

            SimdVector4F const r2 = Matrix4x4F_Extract<2>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(-0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(-0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

            SimdVector4F const r3 = Matrix4x4F_Extract<3>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
        }

        SECTION("angle = 270")
        {
            SimdVector4F rotor = RotorF_CreateFromNormalAngle(axis, DegreesToRadians(270.0f));
            SimdMatrix4x4F matrix = RotorF_ToMatrix4x4F(rotor);

            SimdVector4F const r0 = Matrix4x4F_Extract<0>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(-1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

            SimdVector4F const r1 = Matrix4x4F_Extract<1>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

            SimdVector4F const r2 = Matrix4x4F_Extract<2>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

            SimdVector4F const r3 = Matrix4x4F_Extract<3>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
        }
    }

    SECTION("axis = Z")
    {
        SimdVector4F const axis = Vector4F_PositiveUnitZ();
        SECTION("angle = -30")
        {
            SimdVector4F rotor = RotorF_CreateFromNormalAngle(axis, DegreesToRadians(-30.0f));
            SimdMatrix4x4F matrix = RotorF_ToMatrix4x4F(rotor);

            SimdVector4F const r0 = Matrix4x4F_Extract<0>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

            SimdVector4F const r1 = Matrix4x4F_Extract<1>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(0.86603f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(-0.5f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

            SimdVector4F const r2 = Matrix4x4F_Extract<2>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.5f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(0.86603f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

            SimdVector4F const r3 = Matrix4x4F_Extract<3>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
        }

        SECTION("angle = 0")
        {
            SimdVector4F rotor = RotorF_CreateFromNormalAngle(axis, DegreesToRadians(0.0f));
            SimdMatrix4x4F matrix = RotorF_ToMatrix4x4F(rotor);

            SimdVector4F const r0 = Matrix4x4F_Extract<0>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

            SimdVector4F const r1 = Matrix4x4F_Extract<1>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

            SimdVector4F const r2 = Matrix4x4F_Extract<2>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

            SimdVector4F const r3 = Matrix4x4F_Extract<3>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
        }

        SECTION("angle = 45")
        {
            SimdVector4F rotor = RotorF_CreateFromNormalAngle(axis, DegreesToRadians(45.0f));
            SimdMatrix4x4F matrix = RotorF_ToMatrix4x4F(rotor);

            SimdVector4F const r0 = Matrix4x4F_Extract<0>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

            SimdVector4F const r1 = Matrix4x4F_Extract<1>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

            SimdVector4F const r2 = Matrix4x4F_Extract<2>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(-0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

            SimdVector4F const r3 = Matrix4x4F_Extract<3>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
        }

        SECTION("angle = 90")
        {
            SimdVector4F rotor = RotorF_CreateFromNormalAngle(axis, DegreesToRadians(90.0f));
            SimdMatrix4x4F matrix = RotorF_ToMatrix4x4F(rotor);

            SimdVector4F const r0 = Matrix4x4F_Extract<0>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

            SimdVector4F const r1 = Matrix4x4F_Extract<1>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

            SimdVector4F const r2 = Matrix4x4F_Extract<2>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(-1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

            SimdVector4F const r3 = Matrix4x4F_Extract<3>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
        }

        SECTION("angle = 135")
        {
            SimdVector4F rotor = RotorF_CreateFromNormalAngle(axis, DegreesToRadians(135.0f));
            SimdMatrix4x4F matrix = RotorF_ToMatrix4x4F(rotor);

            SimdVector4F const r0 = Matrix4x4F_Extract<0>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

            SimdVector4F const r1 = Matrix4x4F_Extract<1>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(-0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

            SimdVector4F const r2 = Matrix4x4F_Extract<2>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(-0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(-0.70711f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

            SimdVector4F const r3 = Matrix4x4F_Extract<3>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
        }

        SECTION("angle = 270")
        {
            SimdVector4F rotor = RotorF_CreateFromNormalAngle(axis, DegreesToRadians(270.0f));
            SimdMatrix4x4F matrix = RotorF_ToMatrix4x4F(rotor);

            SimdVector4F const r0 = Matrix4x4F_Extract<0>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

            SimdVector4F const r1 = Matrix4x4F_Extract<1>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(-1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

            SimdVector4F const r2 = Matrix4x4F_Extract<2>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

            SimdVector4F const r3 = Matrix4x4F_Extract<3>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
        }
    }

    SECTION("axis = custom")
    {
        SimdVector4F const axis = Vector4F_Create(2.0f, 3.0f, 5.0f, 0.0f);
        SECTION("angle = -30")
        {
            SimdVector4F rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians(-30.0f));
            SimdMatrix4x4F matrix = RotorF_ToMatrix4x4F(rotor);

            SimdVector4F const r0 = Matrix4x4F_Extract<0>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(0.95417f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(-0.21511f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(-0.20808f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

            SimdVector4F const r1 = Matrix4x4F_Extract<1>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.10934f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(0.89776f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(-0.42671f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

            SimdVector4F const r2 = Matrix4x4F_Extract<2>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.27859f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.3844f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(0.88013f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

            SimdVector4F const r3 = Matrix4x4F_Extract<3>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
        }

        SECTION("angle = 0")
        {
            SimdVector4F rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians(0.0f));
            SimdMatrix4x4F matrix = RotorF_ToMatrix4x4F(rotor);

            SimdVector4F const r0 = Matrix4x4F_Extract<0>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

            SimdVector4F const r1 = Matrix4x4F_Extract<1>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

            SimdVector4F const r2 = Matrix4x4F_Extract<2>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(1.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

            SimdVector4F const r3 = Matrix4x4F_Extract<3>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
        }

        SECTION("angle = 45")
        {
            SimdVector4F rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians(45.0f));
            SimdMatrix4x4F matrix = RotorF_ToMatrix4x4F(rotor);

            SimdVector4F const r0 = Matrix4x4F_Extract<0>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(0.8998f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(0.1138f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.4212f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

            SimdVector4F const r1 = Matrix4x4F_Extract<1>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(-0.34503f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(0.77648f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.52729f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

            SimdVector4F const r2 = Matrix4x4F_Extract<2>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(-0.26705f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(-0.61979f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(0.73794f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

            SimdVector4F const r3 = Matrix4x4F_Extract<3>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
        }

        SECTION("angle = 90")
        {
            SimdVector4F rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians(90.0f));
            SimdMatrix4x4F matrix = RotorF_ToMatrix4x4F(rotor);

            SimdVector4F const r0 = Matrix4x4F_Extract<0>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(0.65789f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(-0.07029f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.74982f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

            SimdVector4F const r1 = Matrix4x4F_Extract<1>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(-0.71918f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(0.23684f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.65321f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

            SimdVector4F const r2 = Matrix4x4F_Extract<2>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(-0.22351f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(-0.969f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(0.10526f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

            SimdVector4F const r3 = Matrix4x4F_Extract<3>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
        }

        SECTION("angle = 135")
        {
            SimdVector4F rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians(135.0f));
            SimdMatrix4x4F matrix = RotorF_ToMatrix4x4F(rotor);

            SimdVector4F const r0 = Matrix4x4F_Extract<0>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(0.41599f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(-0.44444f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(0.79336f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

            SimdVector4F const r1 = Matrix4x4F_Extract<1>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(-0.90327f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(-0.30279f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(0.304f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

            SimdVector4F const r2 = Matrix4x4F_Extract<2>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.10512f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(-0.84308f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(-0.52741f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

            SimdVector4F const r3 = Matrix4x4F_Extract<3>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
        }

        SECTION("angle = 270")
        {
            SimdVector4F rotor = RotorF_CreateFromAxisAngle(axis, DegreesToRadians(270.0f));
            SimdMatrix4x4F matrix = RotorF_ToMatrix4x4F(rotor);

            SimdVector4F const r0 = Matrix4x4F_Extract<0>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(0.65789f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(-0.71918f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(-0.22351f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

            SimdVector4F const r1 = Matrix4x4F_Extract<1>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(-0.07029f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(0.23684f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(-0.969f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

            SimdVector4F const r2 = Matrix4x4F_Extract<2>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(0.74982f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(0.65321f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(0.10526f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

            SimdVector4F const r3 = Matrix4x4F_Extract<3>(matrix);
            CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
            CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
        }
    }
}

TEST_CASE("RotorF_Slerp")
{
    using namespace Anemone::Math::Detail;
    using namespace Anemone::Math;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.0001f;

    SimdVector4F const v = Vector4F_PositiveUnitY();

    SimdVector4F const rotor1 = RotorF_CreateFromNormalAngle(Vector4F_PositiveUnitX(), DegreesToRadians(0.0f));
    SimdVector4F const rotor2 = RotorF_CreateFromNormalAngle(Vector4F_PositiveUnitX(), DegreesToRadians(180.0f));

    SECTION("t = 0/6")
    {
        SimdVector4F result = RotorF_Slerp(rotor1, rotor2, 0.0f);
        CHECK_THAT(Vector4F_Extract<0>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinAbs(1.0f, tolerance));

        SimdVector4F const vt = RotorF_Rotate3(result, v);
        CHECK_THAT(Vector4F_Extract<0>(vt), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vt), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vt), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vt), WithinAbs(0.0f, tolerance));

        auto const vr = RotorF_InverseRotate3(result, vt);

        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vr), WithinAbs(0.0f, tolerance));
    }

    SECTION("t = 1/6")
    {
        SimdVector4F result = RotorF_Slerp(rotor1, rotor2, 1.0f / 6.0f);
        CHECK_THAT(Vector4F_Extract<0>(result), WithinAbs(0.25882f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinAbs(0.96593f, tolerance));

        SimdVector4F const vt = RotorF_Rotate3(result, v);
        CHECK_THAT(Vector4F_Extract<0>(vt), WithinAbs(0.5f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vt), WithinAbs(0.86603f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vt), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vt), WithinAbs(0.0f, tolerance));

        auto const vr = RotorF_InverseRotate3(result, vt);

        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vr), WithinAbs(0.0f, tolerance));
    }

    SECTION("t = 2/6")
    {
        SimdVector4F result = RotorF_Slerp(rotor1, rotor2, 2.0f / 6.0f);
        CHECK_THAT(Vector4F_Extract<0>(result), WithinAbs(0.5f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinAbs(0.86603f, tolerance));

        SimdVector4F const vt = RotorF_Rotate3(result, v);
        CHECK_THAT(Vector4F_Extract<0>(vt), WithinAbs(0.86603f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vt), WithinAbs(0.5f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vt), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vt), WithinAbs(0.0f, tolerance));

        auto const vr = RotorF_InverseRotate3(result, vt);

        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vr), WithinAbs(0.0f, tolerance));
    }

    SECTION("t = 3/6")
    {
        SimdVector4F result = RotorF_Slerp(rotor1, rotor2, 3.0f / 6.0f);
        CHECK_THAT(Vector4F_Extract<0>(result), WithinAbs(0.70711f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinAbs(0.70711f, tolerance));

        SimdVector4F const vt = RotorF_Rotate3(result, v);
        CHECK_THAT(Vector4F_Extract<0>(vt), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vt), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vt), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vt), WithinAbs(0.0f, tolerance));

        auto const vr = RotorF_InverseRotate3(result, vt);

        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vr), WithinAbs(0.0f, tolerance));
    }

    SECTION("t = 4/6")
    {
        SimdVector4F result = RotorF_Slerp(rotor1, rotor2, 4.0f / 6.0f);
        CHECK_THAT(Vector4F_Extract<0>(result), WithinAbs(0.86603f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinAbs(0.5f, tolerance));

        SimdVector4F const vt = RotorF_Rotate3(result, v);
        CHECK_THAT(Vector4F_Extract<0>(vt), WithinAbs(0.86603f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vt), WithinAbs(-0.5f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vt), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vt), WithinAbs(0.0f, tolerance));

        auto const vr = RotorF_InverseRotate3(result, vt);

        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vr), WithinAbs(0.0f, tolerance));
    }

    SECTION("t = 5/6")
    {
        SimdVector4F result = RotorF_Slerp(rotor1, rotor2, 5.0f / 6.0f);
        CHECK_THAT(Vector4F_Extract<0>(result), WithinAbs(0.96593f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinAbs(0.25882f, tolerance));

        SimdVector4F const vt = RotorF_Rotate3(result, v);
        CHECK_THAT(Vector4F_Extract<0>(vt), WithinAbs(0.5f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vt), WithinAbs(-0.86603f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vt), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vt), WithinAbs(0.0f, tolerance));

        auto const vr = RotorF_InverseRotate3(result, vt);

        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vr), WithinAbs(0.0f, tolerance));
    }

    SECTION("t = 6/6")
    {
        SimdVector4F result = RotorF_Slerp(rotor1, rotor2, 1.0f);
        CHECK_THAT(Vector4F_Extract<0>(result), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinAbs(0.0f, tolerance));

        SimdVector4F const vt = RotorF_Rotate3(result, v);
        CHECK_THAT(Vector4F_Extract<0>(vt), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vt), WithinAbs(-1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vt), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vt), WithinAbs(0.0f, tolerance));

        auto const vr = RotorF_InverseRotate3(result, vt);

        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vr), WithinAbs(0.0f, tolerance));
    }
}

TEST_CASE("RotorF_ToMatrix4x4F / scratch")
{
    using namespace Anemone::Math::Detail;
    using namespace Anemone::Math;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.0001f;

    SimdVector4F const rotor = Vector4F_Create(3.0f, 5.0f, 7.0f, 11.0f);
    SimdMatrix4x4F matrix = RotorF_ToMatrix4x4F(rotor);

    SimdVector4F const r0 = Matrix4x4F_Extract<0>(matrix);
    CHECK_THAT(Vector4F_Extract<0>(r0), WithinAbs(136.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r0), WithinAbs(-136.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r0), WithinAbs(-68.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r0), WithinAbs(0.0f, tolerance));

    SimdVector4F const r1 = Matrix4x4F_Extract<1>(matrix);
    CHECK_THAT(Vector4F_Extract<0>(r1), WithinAbs(-4.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r1), WithinAbs(88.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r1), WithinAbs(-184.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r1), WithinAbs(0.0f, tolerance));

    SimdVector4F const r2 = Matrix4x4F_Extract<2>(matrix);
    CHECK_THAT(Vector4F_Extract<0>(r2), WithinAbs(152.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r2), WithinAbs(124.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r2), WithinAbs(56.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r2), WithinAbs(0.0f, tolerance));

    SimdVector4F const r3 = Matrix4x4F_Extract<3>(matrix);
    CHECK_THAT(Vector4F_Extract<0>(r3), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<1>(r3), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<2>(r3), WithinAbs(0.0f, tolerance));
    CHECK_THAT(Vector4F_Extract<3>(r3), WithinAbs(1.0f, tolerance));
}
