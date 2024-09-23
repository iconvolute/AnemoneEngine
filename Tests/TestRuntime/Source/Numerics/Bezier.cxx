#include "AnemoneRuntime/Math/Detail/SimdFloat.hxx"

#include <catch_amalgamated.hpp>

template <typename Callback>
anemone_noinline Anemone::Math::Detail::SimdVector4F anemone_vectorcall Integrate(
    float step,
    float start,
    float end,
    Callback callback)
{
    using namespace Anemone::Math::Detail;

    SimdVector4F result{};

    for (float t = start; t <= end; t += step)
    {
        result = Vector4F_Add(result, Vector4F_Multiply(callback(t), Vector4F_Replicate(step)));
    }

    return result;
}


TEST_CASE("Bezier - Integrate")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math::Detail;

    constexpr float step = 1.0f / 1024.0f;

    // Quite large margin, because the integration is not very precise.
    constexpr float margin = 0.1f;

    SimdVector4F const p0 = Vector4F_Create(1.0f, 8.0f, 13.0f, 3.0f);
    SimdVector4F const p1 = Vector4F_Create(6.0f, 12.0f, 7.0f, 11.0f);
    SimdVector4F const p2 = Vector4F_Create(16.0f, 14.0f, 2.0f, 15.0f);
    SimdVector4F const p3 = Vector4F_Create(10.0f, 5.0f, 9.0f, 4.0f);

    SECTION("Position -> Tangent")
    {
        auto const callback = [&](float t)
        {
            return Vector4F_BezierTangent(p0, p1, p2, p3, t);
        };

        SimdVector4F const result = Integrate(step, 0.0f, 1.0f, callback);
        SimdVector4F const expected = Vector4F_Subtract(
            Vector4F_BezierPosition(p0, p1, p2, p3, 1.0f),
            Vector4F_BezierPosition(p0, p1, p2, p3, 0.0f));

        CHECK_THAT(Vector4F_Extract<0>(result), WithinAbs(Vector4F_Extract<0>(expected), margin));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinAbs(Vector4F_Extract<1>(expected), margin));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinAbs(Vector4F_Extract<2>(expected), margin));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinAbs(Vector4F_Extract<3>(expected), margin));
    }

    SECTION("Tangent -> Acceleration")
    {
        auto const callback = [&](float t)
        {
            return Vector4F_BezierAcceleration(p0, p1, p2, p3, t);
        };

        SimdVector4F const result = Integrate(step, 0.0f, 1.0f, callback);
        SimdVector4F const expected = Vector4F_Subtract(
            Vector4F_BezierTangent(p0, p1, p2, p3, 1.0f),
            Vector4F_BezierTangent(p0, p1, p2, p3, 0.0f));

        CHECK_THAT(Vector4F_Extract<0>(result), WithinAbs(Vector4F_Extract<0>(expected), margin));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinAbs(Vector4F_Extract<1>(expected), margin));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinAbs(Vector4F_Extract<2>(expected), margin));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinAbs(Vector4F_Extract<3>(expected), margin));
    }
}

TEST_CASE("Bezier4 - calculate length")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Math::Detail;

    SECTION("Custom")
    {
        SimdVector4F const p0 = Vector4F_Create(0.0f, 0.0f, 0.0f, 0.0f);
        SimdVector4F const p1 = Vector4F_Create(0.0f, 1.0f, 1.0f, 0.0f);
        SimdVector4F const p2 = Vector4F_Create(1.0f, 1.0f, 0.0f, 0.0f);
        SimdVector4F const p3 = Vector4F_Create(1.0f, 1.0f, 1.0f, 0.0f);

        DistanceToInterpolation lookup[5]{};
        SimdVector4F const approxLength = Bezier4_CalculateLengthSegments(p0, p1, p2, p3, lookup, 5);

        CHECK_THAT(Vector4F_Extract<0>(approxLength), WithinAbs(2.03107f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(approxLength), WithinAbs(2.03107f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(approxLength), WithinAbs(2.03107f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(approxLength), WithinAbs(2.03107f, 0.0001));

        CHECK_THAT(lookup[0].Distance, WithinAbs(0.0f, 0.0001));
        CHECK_THAT(lookup[0].Value, WithinAbs(0.0f, 0.0001));

        CHECK_THAT(lookup[1].Distance, WithinAbs(0.74165f, 0.0001));
        CHECK_THAT(lookup[1].Value, WithinAbs(0.25f, 0.0001));

        CHECK_THAT(lookup[2].Distance, WithinAbs(1.20013f, 0.0001));
        CHECK_THAT(lookup[2].Value, WithinAbs(0.5f, 0.0001));

        CHECK_THAT(lookup[3].Distance, WithinAbs(1.56624f, 0.0001));
        CHECK_THAT(lookup[3].Value, WithinAbs(0.75f, 0.0001));

        CHECK_THAT(lookup[4].Distance, WithinAbs(2.03107f, 0.0001));
        CHECK_THAT(lookup[4].Value, WithinAbs(1.0f, 0.0001));

        SimdVector4F const preciseLength = Bezier4_CalculateLength(p0, p1, p2, p3, 1000);

        CHECK_THAT(Vector4F_Extract<0>(preciseLength), WithinAbs(2.078f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(preciseLength), WithinAbs(2.078f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(preciseLength), WithinAbs(2.078f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(preciseLength), WithinAbs(2.078f, 0.0001));
    }

    SECTION("Sanity check")
    {
        SimdVector4F const p0 = Vector4F_Create(1.0f, 0.0f, 0.0f, 0.0f);
        SimdVector4F const p1 = Vector4F_Create(2.0f, 0.0f, 0.0f, 0.0f);
        SimdVector4F const p2 = Vector4F_Create(3.0f, 0.0f, 0.0f, 0.0f);
        SimdVector4F const p3 = Vector4F_Create(4.0f, 0.0f, 0.0f, 0.0f);

        DistanceToInterpolation lookup[4]{};
        SimdVector4F const approxLength = Bezier4_CalculateLengthSegments(p0, p1, p2, p3, lookup, 4);

        CHECK_THAT(Vector4F_Extract<0>(approxLength), WithinAbs(3.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(approxLength), WithinAbs(3.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(approxLength), WithinAbs(3.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(approxLength), WithinAbs(3.0f, 0.0001));

        CHECK_THAT(lookup[0].Distance, WithinAbs(0.0f, 0.0001));
        CHECK_THAT(lookup[0].Value, WithinAbs(0.0f, 0.0001));

        CHECK_THAT(lookup[1].Distance, WithinAbs(1.0f, 0.0001));
        CHECK_THAT(lookup[1].Value, WithinAbs(0.33333f, 0.0001));

        CHECK_THAT(lookup[2].Distance, WithinAbs(2.0f, 0.0001));
        CHECK_THAT(lookup[2].Value, WithinAbs(0.666667f, 0.0001));

        CHECK_THAT(lookup[3].Distance, WithinAbs(3.0f, 0.0001));
        CHECK_THAT(lookup[3].Value, WithinAbs(1.0f, 0.0001));

        SimdVector4F const preciseLength = Bezier4_CalculateLength(p0, p1, p2, p3, 1000);

        CHECK_THAT(Vector4F_Extract<0>(preciseLength), WithinAbs(3.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(preciseLength), WithinAbs(3.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(preciseLength), WithinAbs(3.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(preciseLength), WithinAbs(3.0f, 0.0001));
    }
}
