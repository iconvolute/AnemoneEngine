#include "AnemoneRuntime/Math/Detail/SimdFloat.hxx"
#include "AnemoneRuntime/Math/Random.hxx"

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

ANEMONE_EXTERNAL_HEADERS_END

TEST_CASE("QuaternionF_CreateFromAxisAngle")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math::Detail;
    using namespace Anemone::Math;
    using namespace Anemone;

    SECTION("axis = x, angle = 90")
    {
        SimdVector4F const q = QuaternionF_CreateFromAxisAngle(Vector4F_PositiveUnitX(), DegreesToRadians(90.0f));

        CHECK_THAT(Vector4F_Extract<0>(q), WithinAbs(0.70711f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(q), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(q), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(q), WithinAbs(0.70711f, 0.0001));
    }

    SECTION("axis = y, angle = 90")
    {
        SimdVector4F const q = QuaternionF_CreateFromAxisAngle(Vector4F_PositiveUnitY(), DegreesToRadians(90.0f));

        CHECK_THAT(Vector4F_Extract<0>(q), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(q), WithinAbs(0.70711f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(q), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(q), WithinAbs(0.70711f, 0.0001));
    }


    SECTION("axis = z, angle = 90")
    {
        SimdVector4F const q = QuaternionF_CreateFromAxisAngle(Vector4F_PositiveUnitZ(), DegreesToRadians(90.0f));

        CHECK_THAT(Vector4F_Extract<0>(q), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(q), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(q), WithinAbs(0.70711f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(q), WithinAbs(0.70711f, 0.0001));
    }
}

TEST_CASE("QuaternionF_Rotate3")
{
    using namespace Anemone::Math::Detail;
    using namespace Anemone::Math;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.0001f;

    SECTION("Random")
    {
        Random random{2137};

        // Pick random unit vector
        for (size_t i = 0; i < 16; ++i)
        {
            float const axis_x = random.NextFloat(-1.0f, 1.0f);
            float const axis_y = random.NextFloat(-1.0f, 1.0f);
            float const axis_z = 1.0f - (axis_x * axis_x + axis_y * axis_y);

            SimdVector4F const axis = Vector4F_Normalize3(Vector4F_Create(axis_x, axis_y, axis_z, 0.0f));

            float const unit_x = random.NextFloat(-1.0f, 1.0f);
            float const unit_y = random.NextFloat(-1.0f, 1.0f);
            float const unit_z = 1.0f - (unit_x * unit_x + unit_y * unit_y);

            SimdVector4F const unit = Vector4F_Normalize3(Vector4F_Create(unit_x, unit_y, unit_z, 0.0f));

            float const angle = random.NextFloat(-Pi2<float>, Pi2<float>);

            auto const rotation = QuaternionF_CreateFromAxisAngle(axis, angle);

            auto const unit_rotated = QuaternionF_Rotate3(rotation, unit);

            auto const unit_reversed = QuaternionF_InverseRotate3(rotation, unit_rotated);

            CHECK_THAT(Vector4F_Extract<0>(unit_reversed), WithinAbs(Vector4F_Extract<0>(unit), tolerance));
            CHECK_THAT(Vector4F_Extract<1>(unit_reversed), WithinAbs(Vector4F_Extract<1>(unit), tolerance));
            CHECK_THAT(Vector4F_Extract<2>(unit_reversed), WithinAbs(Vector4F_Extract<2>(unit), tolerance));
        }
    }
}

TEST_CASE("QuaternionF_Slerp")
{
    using namespace Anemone::Math::Detail;
    using namespace Anemone::Math;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.0001f;

    SimdVector4F const v = Vector4F_PositiveUnitY();

    SimdVector4F const rotor1 = QuaternionF_CreateFromNormalAngle(Vector4F_PositiveUnitX(), DegreesToRadians(0.0f));
    SimdVector4F const rotor2 = QuaternionF_CreateFromNormalAngle(Vector4F_PositiveUnitX(), DegreesToRadians(180.0f));

    SECTION("t = 0/6")
    {
        SimdVector4F result = QuaternionF_Slerp(rotor1, rotor2, 0.0f);
        CHECK_THAT(Vector4F_Extract<0>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinAbs(1.0f, tolerance));

        SimdVector4F const vt = QuaternionF_Rotate3(result, v);
        CHECK_THAT(Vector4F_Extract<0>(vt), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vt), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vt), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vt), WithinAbs(0.0f, tolerance));

        SimdVector4F const vr = QuaternionF_InverseRotate3(result, v);
        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vr), WithinAbs(0.0f, tolerance));
    }

    SECTION("t = 1/6")
    {
        SimdVector4F result = QuaternionF_Slerp(rotor1, rotor2, 1.0f / 6.0f);
        CHECK_THAT(Vector4F_Extract<0>(result), WithinAbs(0.25882f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinAbs(0.96593f, tolerance));

        SimdVector4F const vt = QuaternionF_Rotate3(result, v);
        CHECK_THAT(Vector4F_Extract<0>(vt), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vt), WithinAbs(0.86603f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vt), WithinAbs(0.5f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vt), WithinAbs(0.0f, tolerance));

        SimdVector4F const vr = QuaternionF_InverseRotate3(result, vt);
        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vr), WithinAbs(0.0f, tolerance));
    }

    SECTION("t = 2/6")
    {
        SimdVector4F result = QuaternionF_Slerp(rotor1, rotor2, 2.0f / 6.0f);
        CHECK_THAT(Vector4F_Extract<0>(result), WithinAbs(0.5f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinAbs(0.86603f, tolerance));

        SimdVector4F const vt = QuaternionF_Rotate3(result, v);
        CHECK_THAT(Vector4F_Extract<0>(vt), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vt), WithinAbs(0.5f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vt), WithinAbs(0.86603f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vt), WithinAbs(0.0f, tolerance));

        SimdVector4F const vr = QuaternionF_InverseRotate3(result, vt);
        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vr), WithinAbs(0.0f, tolerance));
    }

    SECTION("t = 3/6")
    {
        SimdVector4F result = QuaternionF_Slerp(rotor1, rotor2, 3.0f / 6.0f);
        CHECK_THAT(Vector4F_Extract<0>(result), WithinAbs(0.70711f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinAbs(0.70711f, tolerance));

        SimdVector4F const vt = QuaternionF_Rotate3(result, v);
        CHECK_THAT(Vector4F_Extract<0>(vt), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vt), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vt), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vt), WithinAbs(0.0f, tolerance));

        SimdVector4F const vr = QuaternionF_InverseRotate3(result, vt);
        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vr), WithinAbs(0.0f, tolerance));
    }

    SECTION("t = 4/6")
    {
        SimdVector4F result = QuaternionF_Slerp(rotor1, rotor2, 4.0f / 6.0f);
        CHECK_THAT(Vector4F_Extract<0>(result), WithinAbs(0.86603f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinAbs(0.5f, tolerance));

        SimdVector4F const vt = QuaternionF_Rotate3(result, v);
        CHECK_THAT(Vector4F_Extract<0>(vt), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vt), WithinAbs(-0.5f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vt), WithinAbs(0.86603f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vt), WithinAbs(0.0f, tolerance));

        SimdVector4F const vr = QuaternionF_InverseRotate3(result, vt);
        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vr), WithinAbs(0.0f, tolerance));
    }

    SECTION("t = 5/6")
    {
        SimdVector4F result = QuaternionF_Slerp(rotor1, rotor2, 5.0f / 6.0f);
        CHECK_THAT(Vector4F_Extract<0>(result), WithinAbs(0.96593f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinAbs(0.25882f, tolerance));

        SimdVector4F const vt = QuaternionF_Rotate3(result, v);
        CHECK_THAT(Vector4F_Extract<0>(vt), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vt), WithinAbs(-0.86603f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vt), WithinAbs(0.5f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vt), WithinAbs(0.0f, tolerance));

        SimdVector4F const vr = QuaternionF_InverseRotate3(result, vt);
        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vr), WithinAbs(0.0f, tolerance));
    }

    SECTION("t = 6/6")
    {
        SimdVector4F result = QuaternionF_Slerp(rotor1, rotor2, 1.0f);
        CHECK_THAT(Vector4F_Extract<0>(result), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinAbs(0.0f, tolerance));

        SimdVector4F const vt = QuaternionF_Rotate3(result, v);
        CHECK_THAT(Vector4F_Extract<0>(vt), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vt), WithinAbs(-1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vt), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vt), WithinAbs(0.0f, tolerance));

        SimdVector4F const vr = QuaternionF_InverseRotate3(result, vt);
        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vr), WithinAbs(0.0f, tolerance));
    }
}
