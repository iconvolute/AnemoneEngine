#include "AnemoneRuntime/Math/Functions.hxx"

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

ANEMONE_EXTERNAL_HEADERS_END

TEST_CASE("Float / Unwinding angles")
{
    using Catch::Matchers::WithinAbs;
    using namespace Anemone::Math;

    SECTION("Unwind Radians")
    {
        CHECK_THAT(UnwindRadians(PiOver2<float>), WithinAbs(PiOver2<float>, 0.001));
        CHECK_THAT(UnwindRadians(PiOver2<float> + Pi2<float> * -1), WithinAbs(PiOver2<float>, 0.001));
        CHECK_THAT(UnwindRadians(PiOver2<float> + Pi2<float> * 1), WithinAbs(PiOver2<float>, 0.001));
        CHECK_THAT(UnwindRadians(PiOver2<float> + Pi2<float> * 2), WithinAbs(PiOver2<float>, 0.001));
    }

    SECTION("Unwind Degrees Signed")
    {
        CHECK_THAT(UnwindDegrees(45.0f + 90.0f * -5.0f), WithinAbs(-45, 0.001));
        CHECK_THAT(UnwindDegrees(45.0f + 90.0f * -4.0f), WithinAbs(45, 0.001));
        CHECK_THAT(UnwindDegrees(45.0f + 90.0f * -3.0f), WithinAbs(135, 0.001));
        CHECK_THAT(UnwindDegrees(45.0f + 90.0f * -2.0f), WithinAbs(-135, 0.001));
        CHECK_THAT(UnwindDegrees(45.0f + 90.0f * -1.0f), WithinAbs(-45, 0.001));
        CHECK_THAT(UnwindDegrees(45.0f + 90.0f * +0.0f), WithinAbs(45, 0.001));
        CHECK_THAT(UnwindDegrees(45.0f + 90.0f * +1.0f), WithinAbs(135, 0.001));
        CHECK_THAT(UnwindDegrees(45.0f + 90.0f * +2.0f), WithinAbs(-135, 0.001));
        CHECK_THAT(UnwindDegrees(45.0f + 90.0f * +3.0f), WithinAbs(-45, 0.001));
        CHECK_THAT(UnwindDegrees(45.0f + 90.0f * +4.0f), WithinAbs(45, 0.001));
        CHECK_THAT(UnwindDegrees(45.0f + 90.0f * +5.0f), WithinAbs(135, 0.001));
    }
}

TEST_CASE("Float / IsNearZeroPrecise")
{
    using namespace Anemone::Math;

    SECTION("0.0f - 16 ULP")
    {
        float const value = std::bit_cast<float>(std::bit_cast<int32_t>(0.0f) - 16);
        REQUIRE_FALSE(IsNearZeroPrecise(value));
    }

    SECTION("0.0f - 5 ULP")
    {
        float const value = std::bit_cast<float>(std::bit_cast<int32_t>(0.0f) - 5);
        REQUIRE_FALSE(IsNearZeroPrecise(value));
    }

    SECTION("0.0f - 4 ULP")
    {
        float const value = std::bit_cast<float>(std::bit_cast<int32_t>(0.0f) - 4);
        REQUIRE(IsNearZeroPrecise(value));
    }

    SECTION("0.0f - 3 ULP")
    {
        float const value = std::bit_cast<float>(std::bit_cast<int32_t>(0.0f) - 3);
        REQUIRE(IsNearZeroPrecise(value));
    }

    SECTION("0.0f - 2 ULP")
    {
        float const value = std::bit_cast<float>(std::bit_cast<int32_t>(0.0f) - 2);
        REQUIRE(IsNearZeroPrecise(value));
    }

    SECTION("0.0f - 1 ULP")
    {
        float const value = std::bit_cast<float>(std::bit_cast<int32_t>(0.0f) - 1);
        REQUIRE(IsNearZeroPrecise(value));
    }

    SECTION("0.0f + 0 ULP")
    {
        float const value = 0.0f;

        REQUIRE(IsNearZeroPrecise(value));
    }

    SECTION("0.0f + 1 ULP")
    {
        float const value = std::bit_cast<float>(std::bit_cast<int32_t>(0.0f) + 1);
        REQUIRE(IsNearZeroPrecise(value));
    }

    SECTION("0.0f + 2 ULP")
    {
        float const value = std::bit_cast<float>(std::bit_cast<int32_t>(0.0f) + 2);
        REQUIRE(IsNearZeroPrecise(value));
    }

    SECTION("0.0f + 3 ULP")
    {
        float const value = std::bit_cast<float>(std::bit_cast<int32_t>(0.0f) + 3);
        REQUIRE(IsNearZeroPrecise(value));
    }

    SECTION("0.0f + 4 ULP")
    {
        float const value = std::bit_cast<float>(std::bit_cast<int32_t>(0.0f) + 4);
        REQUIRE(IsNearZeroPrecise(value));
    }

    SECTION("0.0f + 5 ULP")
    {
        float const value = std::bit_cast<float>(std::bit_cast<int32_t>(0.0f) + 5);
        REQUIRE_FALSE(IsNearZeroPrecise(value));
    }

    SECTION("0.0f + 16 ULP")
    {
        float const value = std::bit_cast<float>(std::bit_cast<uint32_t>(0.0f) + 16);
        REQUIRE_FALSE(IsNearZeroPrecise(value));
    }
}

TEST_CASE("Float / IsNearEqualPrecise")
{
    using namespace Anemone::Math;

    SECTION("diff <= epsilon")
    {
        float const a = 0;
        float const a1 = std::nextafter(a, std::numeric_limits<float>::infinity());
        REQUIRE_FALSE(a == a1);
        REQUIRE(IsNearEqual(a, a1));
        REQUIRE(IsNearEqualPrecise(a, a1));

        float const a2 = std::nextafter(a1, std::numeric_limits<float>::infinity());
        REQUIRE_FALSE(a == a2);
        REQUIRE(IsNearEqual(a, a2));
        REQUIRE(IsNearEqualPrecise(a, a2));

        float const a3 = std::nextafter(a2, std::numeric_limits<float>::infinity());
        REQUIRE_FALSE(a == a3);
        REQUIRE(IsNearEqual(a, a3));
        REQUIRE(IsNearEqualPrecise(a, a3));

        float const a4 = std::nextafter(a3, std::numeric_limits<float>::infinity());
        REQUIRE_FALSE(a == a4);
        REQUIRE(IsNearEqual(a, a4));
        REQUIRE(IsNearEqualPrecise(a, a4));

        float const a5 = std::nextafter(a4, std::numeric_limits<float>::infinity());
        REQUIRE_FALSE(a == a5);
        REQUIRE(IsNearEqual(a, a5));
        REQUIRE(IsNearEqualPrecise(a, a5));
    }

    SECTION("diff > epsilon")
    {
        float const a = 1e+11;
        float const a1 = std::nextafter(a, std::numeric_limits<float>::infinity());
        REQUIRE_FALSE(a == a1);
        REQUIRE_FALSE(IsNearEqual(a, a1));
        REQUIRE(IsNearEqualPrecise(a, a1));

        float const a2 = std::nextafter(a1, std::numeric_limits<float>::infinity());
        REQUIRE_FALSE(a == a2);
        REQUIRE_FALSE(IsNearEqual(a, a2));
        REQUIRE(IsNearEqualPrecise(a, a2));

        float const a3 = std::nextafter(a2, std::numeric_limits<float>::infinity());
        REQUIRE_FALSE(a == a3);
        REQUIRE_FALSE(IsNearEqual(a, a3));
        REQUIRE(IsNearEqualPrecise(a, a3));

        float const a4 = std::nextafter(a3, std::numeric_limits<float>::infinity());
        REQUIRE_FALSE(a == a4);
        REQUIRE_FALSE(IsNearEqual(a, a4));
        REQUIRE(IsNearEqualPrecise(a, a4));

        float const a5 = std::nextafter(a4, std::numeric_limits<float>::infinity());
        REQUIRE_FALSE(a == a5);
        REQUIRE_FALSE(IsNearEqual(a, a5));
        REQUIRE_FALSE(IsNearEqualPrecise(a, a5));
    }
}
