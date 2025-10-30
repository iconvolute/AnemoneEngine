#include "AnemoneMath/Detail/SimdFloat.hxx"
#include "AnemoneRandom/Generator.hxx"
#include "AnemoneRandom/Distribution.hxx"

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

ANEMONE_EXTERNAL_HEADERS_END

TEST_CASE("Vector4F_LoadUnalignedFloat4")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    float const data[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    SimdVector4F const v = Vector4F_LoadUnalignedFloat4(data);

    REQUIRE_THAT(Vector4F_Extract<0>(v), WithinRel(1.0f));
    REQUIRE_THAT(Vector4F_Extract<1>(v), WithinRel(2.0f));
    REQUIRE_THAT(Vector4F_Extract<2>(v), WithinRel(3.0f));
    REQUIRE_THAT(Vector4F_Extract<3>(v), WithinRel(4.0f));
}

TEST_CASE("Vector4F_StoreUnalignedFloat4")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    float data[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    SimdVector4F const v = Vector4F_Create(1.0f, 2.0f, 3.0f, 4.0f);
    Vector4F_StoreUnalignedFloat4(data, v);

    REQUIRE_THAT(data[0], WithinRel(1.0f));
    REQUIRE_THAT(data[1], WithinRel(2.0f));
    REQUIRE_THAT(data[2], WithinRel(3.0f));
    REQUIRE_THAT(data[3], WithinRel(4.0f));
}

TEST_CASE("Vector4F_LoadUnalignedFloat3")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    float const data[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    SimdVector4F const v = Vector4F_LoadUnalignedFloat3(data);

    REQUIRE_THAT(Vector4F_Extract<0>(v), WithinRel(1.0f));
    REQUIRE_THAT(Vector4F_Extract<1>(v), WithinRel(2.0f));
    REQUIRE_THAT(Vector4F_Extract<2>(v), WithinRel(3.0f));
    REQUIRE_THAT(Vector4F_Extract<3>(v), WithinRel(0.0f));
}

TEST_CASE("Vector4F_StoreUnalignedFloat3")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    float data[4] = {0.0f, 0.0f, 0.0f, 4.0f};
    SimdVector4F const v = Vector4F_Create(1.0f, 2.0f, 3.0f, 0.0f);
    Vector4F_StoreUnalignedFloat3(data, v);

    REQUIRE_THAT(data[0], WithinRel(1.0f));
    REQUIRE_THAT(data[1], WithinRel(2.0f));
    REQUIRE_THAT(data[2], WithinRel(3.0f));
    REQUIRE_THAT(data[3], WithinRel(4.0f));
}

TEST_CASE("Vector4F_LoadUnalignedFloat2")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    float const data[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    SimdVector4F const v = Vector4F_LoadUnalignedFloat2(data);

    REQUIRE_THAT(Vector4F_Extract<0>(v), WithinRel(1.0f));
    REQUIRE_THAT(Vector4F_Extract<1>(v), WithinRel(2.0f));
    REQUIRE_THAT(Vector4F_Extract<2>(v), WithinRel(0.0f));
    REQUIRE_THAT(Vector4F_Extract<3>(v), WithinRel(0.0f));
}

TEST_CASE("Vector4F_StoreUnalignedFloat2")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    float data[4] = {0.0f, 0.0f, 3.0f, 4.0f};
    SimdVector4F const v = Vector4F_Create(1.0f, 2.0f, 0.0f, 0.0f);
    Vector4F_StoreUnalignedFloat2(data, v);

    REQUIRE_THAT(data[0], WithinRel(1.0f));
    REQUIRE_THAT(data[1], WithinRel(2.0f));
    REQUIRE_THAT(data[2], WithinRel(3.0f));
    REQUIRE_THAT(data[3], WithinRel(4.0f));
}

TEST_CASE("Vector4F_LoadUnalignedFloat1")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    float const data[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    SimdVector4F const v = Vector4F_LoadUnalignedFloat1(data);

    REQUIRE_THAT(Vector4F_Extract<0>(v), WithinRel(1.0f));
    REQUIRE_THAT(Vector4F_Extract<1>(v), WithinRel(0.0f));
    REQUIRE_THAT(Vector4F_Extract<2>(v), WithinRel(0.0f));
    REQUIRE_THAT(Vector4F_Extract<3>(v), WithinRel(0.0f));
}

TEST_CASE("Vector4F_StoreUnalignedFloat1")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    float data[4] = {0.0f, 2.0f, 3.0f, 4.0f};
    SimdVector4F const v = Vector4F_Create(1.0f, 0.0f, 0.0f, 0.0f);
    Vector4F_StoreUnalignedFloat1(data, v);

    REQUIRE_THAT(data[0], WithinRel(1.0f));
    REQUIRE_THAT(data[1], WithinRel(2.0f));
    REQUIRE_THAT(data[2], WithinRel(3.0f));
    REQUIRE_THAT(data[3], WithinRel(4.0f));
}

TEST_CASE("Vector4F_Create")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const v = Vector4F_Create(1.0f, 2.0f, 3.0f, 4.0f);

    REQUIRE_THAT(Vector4F_Extract<0>(v), WithinRel(1.0f));
    REQUIRE_THAT(Vector4F_Extract<1>(v), WithinRel(2.0f));
    REQUIRE_THAT(Vector4F_Extract<2>(v), WithinRel(3.0f));
    REQUIRE_THAT(Vector4F_Extract<3>(v), WithinRel(4.0f));
}

TEST_CASE("Vector4F_Replicate")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const v = Vector4F_Replicate(1.0f);

    REQUIRE_THAT(Vector4F_Extract<0>(v), WithinRel(1.0f));
    REQUIRE_THAT(Vector4F_Extract<1>(v), WithinRel(1.0f));
    REQUIRE_THAT(Vector4F_Extract<2>(v), WithinRel(1.0f));
    REQUIRE_THAT(Vector4F_Extract<3>(v), WithinRel(1.0f));
}

TEST_CASE("Vector4F_Zero")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const v = Vector4F_Zero();

    REQUIRE_THAT(Vector4F_Extract<0>(v), WithinRel(0.0f));
    REQUIRE_THAT(Vector4F_Extract<1>(v), WithinRel(0.0f));
    REQUIRE_THAT(Vector4F_Extract<2>(v), WithinRel(0.0f));
    REQUIRE_THAT(Vector4F_Extract<3>(v), WithinRel(0.0f));
}

TEST_CASE("Vector4F_PositiveUnitX")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const v = Vector4F_PositiveUnitX();

    REQUIRE_THAT(Vector4F_Extract<0>(v), WithinRel(1.0f));
    REQUIRE_THAT(Vector4F_Extract<1>(v), WithinRel(0.0f));
    REQUIRE_THAT(Vector4F_Extract<2>(v), WithinRel(0.0f));
    REQUIRE_THAT(Vector4F_Extract<3>(v), WithinRel(0.0f));
}

TEST_CASE("Vector4F_PositiveUnitY")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const v = Vector4F_PositiveUnitY();

    REQUIRE_THAT(Vector4F_Extract<0>(v), WithinRel(0.0f));
    REQUIRE_THAT(Vector4F_Extract<1>(v), WithinRel(1.0f));
    REQUIRE_THAT(Vector4F_Extract<2>(v), WithinRel(0.0f));
    REQUIRE_THAT(Vector4F_Extract<3>(v), WithinRel(0.0f));
}

TEST_CASE("Vector4F_PositiveUnitZ")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const v = Vector4F_PositiveUnitZ();

    REQUIRE_THAT(Vector4F_Extract<0>(v), WithinRel(0.0f));
    REQUIRE_THAT(Vector4F_Extract<1>(v), WithinRel(0.0f));
    REQUIRE_THAT(Vector4F_Extract<2>(v), WithinRel(1.0f));
    REQUIRE_THAT(Vector4F_Extract<3>(v), WithinRel(0.0f));
}

TEST_CASE("Vector4F_PositiveUnitW")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const v = Vector4F_PositiveUnitW();

    REQUIRE_THAT(Vector4F_Extract<0>(v), WithinRel(0.0f));
    REQUIRE_THAT(Vector4F_Extract<1>(v), WithinRel(0.0f));
    REQUIRE_THAT(Vector4F_Extract<2>(v), WithinRel(0.0f));
    REQUIRE_THAT(Vector4F_Extract<3>(v), WithinRel(1.0f));
}

TEST_CASE("Vector4F_NegativeUnitX")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const v = Vector4F_NegativeUnitX();

    REQUIRE_THAT(Vector4F_Extract<0>(v), WithinRel(-1.0f));
    REQUIRE_THAT(Vector4F_Extract<1>(v), WithinRel(0.0f));
    REQUIRE_THAT(Vector4F_Extract<2>(v), WithinRel(0.0f));
    REQUIRE_THAT(Vector4F_Extract<3>(v), WithinRel(0.0f));
}

TEST_CASE("Vector4F_NegativeUnitY")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const v = Vector4F_NegativeUnitY();

    REQUIRE_THAT(Vector4F_Extract<0>(v), WithinRel(0.0f));
    REQUIRE_THAT(Vector4F_Extract<1>(v), WithinRel(-1.0f));
    REQUIRE_THAT(Vector4F_Extract<2>(v), WithinRel(0.0f));
    REQUIRE_THAT(Vector4F_Extract<3>(v), WithinRel(0.0f));
}

TEST_CASE("Vector4F_NegativeUnitZ")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const v = Vector4F_NegativeUnitZ();

    REQUIRE_THAT(Vector4F_Extract<0>(v), WithinRel(0.0f));
    REQUIRE_THAT(Vector4F_Extract<1>(v), WithinRel(0.0f));
    REQUIRE_THAT(Vector4F_Extract<2>(v), WithinRel(-1.0f));
    REQUIRE_THAT(Vector4F_Extract<3>(v), WithinRel(0.0f));
}

TEST_CASE("Vector4F_NegativeUnitW")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const v = Vector4F_NegativeUnitW();

    REQUIRE_THAT(Vector4F_Extract<0>(v), WithinRel(0.0f));
    REQUIRE_THAT(Vector4F_Extract<1>(v), WithinRel(0.0f));
    REQUIRE_THAT(Vector4F_Extract<2>(v), WithinRel(0.0f));
    REQUIRE_THAT(Vector4F_Extract<3>(v), WithinRel(-1.0f));
}

TEST_CASE("Vector4F_Extract")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const v = Vector4F_Create(1.0f, 2.0f, 3.0f, 4.0f);

    REQUIRE_THAT(Vector4F_Extract<0>(v), WithinRel(1.0f));
    REQUIRE_THAT(Vector4F_Extract<1>(v), WithinRel(2.0f));
    REQUIRE_THAT(Vector4F_Extract<2>(v), WithinRel(3.0f));
    REQUIRE_THAT(Vector4F_Extract<3>(v), WithinRel(4.0f));
}

TEST_CASE("Vector4F_Insert")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const v = Vector4F_Create(1.0f, 2.0f, 3.0f, 4.0f);

    SECTION("Insert 0")
    {
        SimdVector4F const result = Vector4F_Insert<0>(v, -1.0f);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(-1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(2.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(3.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(4.0f));
    }

    SECTION("Insert 1")
    {
        SimdVector4F const result = Vector4F_Insert<1>(v, -1.0f);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(-1.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(3.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(4.0f));
    }

    SECTION("Insert 2")
    {
        SimdVector4F const result = Vector4F_Insert<2>(v, -1.0f);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(2.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(-1.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(4.0f));
    }

    SECTION("Insert 3")
    {
        SimdVector4F const result = Vector4F_Insert<3>(v, -1.0f);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(2.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(3.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(-1.0f));
    }
}

TEST_CASE("Vector4F_Broadcast")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const v = Vector4F_Create(1.0f, 2.0f, 3.0f, 4.0f);

    SECTION("Broadcast 0")
    {
        SimdVector4F const result = Vector4F_Broadcast<0>(v);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(1.0f));
    }

    SECTION("Broadcast 1")
    {
        SimdVector4F const result = Vector4F_Broadcast<1>(v);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(2.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(2.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(2.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(2.0f));
    }

    SECTION("Broadcast 2")
    {
        SimdVector4F const result = Vector4F_Broadcast<2>(v);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(3.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(3.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(3.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(3.0f));
    }

    SECTION("Broadcast 3")
    {
        SimdVector4F const result = Vector4F_Broadcast<3>(v);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(4.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(4.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(4.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(4.0f));
    }
}

TEST_CASE("Vector4F_Permute / Single Vector")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const v = Vector4F_Create(1.0f, 2.0f, 3.0f, 4.0f);

    SECTION("Permute 0, 0, 0, 0")
    {
        SimdVector4F const result = Vector4F_Permute<0, 0, 0, 0>(v);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(1.0f));
    }

    SECTION("Permute 0, 1, 2, 3")
    {
        SimdVector4F const result = Vector4F_Permute<0, 1, 2, 3>(v);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(2.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(3.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(4.0f));
    }

    SECTION("Permute 1, 0, 3, 2")
    {
        SimdVector4F const result = Vector4F_Permute<1, 0, 3, 2>(v);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(2.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(4.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(3.0f));
    }

    SECTION("Permute 1, 1, 1, 1")
    {
        SimdVector4F const result = Vector4F_Permute<1, 1, 1, 1>(v);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(2.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(2.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(2.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(2.0f));
    }

    SECTION("Permute 1, 2, 3, 0")
    {
        SimdVector4F const result = Vector4F_Permute<1, 2, 3, 0>(v);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(2.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(3.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(4.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(1.0f));
    }

    SECTION("Permute 2, 2, 1, 3")
    {
        SimdVector4F const result = Vector4F_Permute<2, 2, 1, 3>(v);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(3.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(3.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(2.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(4.0f));
    }

    SECTION("Permute 2, 2, 2, 2")
    {
        SimdVector4F const result = Vector4F_Permute<2, 2, 2, 2>(v);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(3.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(3.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(3.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(3.0f));
    }

    SECTION("Permute 2, 3, 0, 1")
    {
        SimdVector4F const result = Vector4F_Permute<2, 3, 0, 1>(v);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(3.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(4.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(2.0f));
    }

    SECTION("Permute 3, 0, 1, 2")
    {
        SimdVector4F const result = Vector4F_Permute<3, 0, 1, 2>(v);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(4.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(2.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(3.0f));
    }

    SECTION("Permute 3, 2, 1, 0")
    {
        SimdVector4F const result = Vector4F_Permute<3, 2, 1, 0>(v);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(4.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(3.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(2.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(1.0f));
    }

    SECTION("Permute 3, 3, 3, 3")
    {
        SimdVector4F const result = Vector4F_Permute<3, 3, 3, 3>(v);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(4.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(4.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(4.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(4.0f));
    }
}

TEST_CASE("Vector4F_Permute / Two vectors")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const v1 = Vector4F_Create(1.0f, 2.0f, 3.0f, 4.0f);
    SimdVector4F const v2 = Vector4F_Create(5.0f, 6.0f, 7.0f, 8.0f);

    SECTION("Permute 0, 0, 0, 0")
    {
        SimdVector4F const result = Vector4F_Permute<0, 0, 0, 0>(v1, v2);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(1.0f));
    }

    SECTION("Permute 0, 1, 2, 3")
    {
        SimdVector4F const result = Vector4F_Permute<0, 1, 2, 3>(v1, v2);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(2.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(3.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(4.0f));
    }

    SECTION("Permute 0, 5, 2, 7")
    {
        SimdVector4F const result = Vector4F_Permute<0, 5, 2, 7>(v1, v2);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(6.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(3.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(8.0f));
    }

    SECTION("Permute 4, 1, 6, 3")
    {
        SimdVector4F const result = Vector4F_Permute<4, 1, 6, 3>(v1, v2);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(5.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(2.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(7.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(4.0f));
    }

    SECTION("Permute 0, 0, 4, 4")
    {
        SimdVector4F const result = Vector4F_Permute<0, 0, 4, 4>(v1, v2);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(5.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(5.0f));
    }

    SECTION("Permute 2, 3, 6, 7")
    {
        SimdVector4F const result = Vector4F_Permute<2, 3, 6, 7>(v1, v2);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(3.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(4.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(7.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(8.0f));
    }

    SECTION("Permute 4, 3, 7, 0")
    {
        SimdVector4F const result = Vector4F_Permute<4, 3, 7, 0>(v1, v2);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(5.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(4.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(8.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(1.0f));
    }
}

TEST_CASE("Vector4F_Select / With Mask")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const v1 = Vector4F_Create(1.0f, 2.0f, 3.0f, 4.0f);
    SimdVector4F const v2 = Vector4F_Create(5.0f, 6.0f, 7.0f, 8.0f);

    SECTION("Select 0, 1, 0, 1")
    {
        SimdMask4F const mask = Mask4F_Create(false, true, false, true);
        SimdVector4F const result = Vector4F_Select(mask, v1, v2);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(6.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(3.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(8.0f));
    }

    SECTION("Select 1, 0, 1, 0")
    {
        SimdMask4F const mask = Mask4F_Create(true, false, true, false);
        SimdVector4F const result = Vector4F_Select(mask, v1, v2);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(5.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(2.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(7.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(4.0f));
    }

    SECTION("Select 1, 1, 1, 1")
    {
        SimdMask4F const mask = Mask4F_Create(true, true, true, true);
        SimdVector4F const result = Vector4F_Select(mask, v1, v2);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(5.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(6.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(7.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(8.0f));
    }

    SECTION("Select 0, 0, 0, 0")
    {
        SimdMask4F const mask = Mask4F_Create(false, false, false, false);
        SimdVector4F const result = Vector4F_Select(mask, v1, v2);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(2.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(3.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(4.0f));
    }
}

TEST_CASE("Vector4F_MultiplyAdd")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.001f;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        float const b[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        float const c[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);
        SimdVector4F const vb = Vector4F_LoadUnalignedFloat4(b);
        SimdVector4F const vc = Vector4F_LoadUnalignedFloat4(c);

        SimdVector4F const result = Vector4F_MultiplyAdd(va, vb, vc);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinAbs((a[0] * b[0]) + c[0], tolerance));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinAbs((a[1] * b[1]) + c[1], tolerance));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinAbs((a[2] * b[2]) + c[2], tolerance));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinAbs((a[3] * b[3]) + c[3], tolerance));
    }
}

TEST_CASE("Vector4F_MultiplySubtract")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.001f;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        float const b[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        float const c[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);
        SimdVector4F const vb = Vector4F_LoadUnalignedFloat4(b);
        SimdVector4F const vc = Vector4F_LoadUnalignedFloat4(c);

        SimdVector4F const result = Vector4F_MultiplySubtract(va, vb, vc);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinAbs((a[0] * b[0]) - c[0], tolerance));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinAbs((a[1] * b[1]) - c[1], tolerance));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinAbs((a[2] * b[2]) - c[2], tolerance));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinAbs((a[3] * b[3]) - c[3], tolerance));
    }
}

TEST_CASE("Vector4F_NegateMultiplyAdd")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.001f;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        float const b[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        float const c[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);
        SimdVector4F const vb = Vector4F_LoadUnalignedFloat4(b);
        SimdVector4F const vc = Vector4F_LoadUnalignedFloat4(c);

        SimdVector4F const result = Vector4F_NegateMultiplyAdd(va, vb, vc);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinAbs(-(a[0] * b[0]) + c[0], tolerance));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinAbs(-(a[1] * b[1]) + c[1], tolerance));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinAbs(-(a[2] * b[2]) + c[2], tolerance));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinAbs(-(a[3] * b[3]) + c[3], tolerance));
    }
}

TEST_CASE("Vector4F_NegateMultiplySubtract")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.001f;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        float const b[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        float const c[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);
        SimdVector4F const vb = Vector4F_LoadUnalignedFloat4(b);
        SimdVector4F const vc = Vector4F_LoadUnalignedFloat4(c);

        SimdVector4F const result = Vector4F_NegateMultiplySubtract(va, vb, vc);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinAbs(-(a[0] * b[0]) - c[0], tolerance));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinAbs(-(a[1] * b[1]) - c[1], tolerance));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinAbs(-(a[2] * b[2]) - c[2], tolerance));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinAbs(-(a[3] * b[3]) - c[3], tolerance));
    }
}

TEST_CASE("Vector4F_Add")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        float const b[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);
        SimdVector4F const vb = Vector4F_LoadUnalignedFloat4(b);

        SimdVector4F const result = Vector4F_Add(va, vb);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(a[0] + b[0]));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(a[1] + b[1]));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(a[2] + b[2]));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(a[3] + b[3]));
    }
}

TEST_CASE("Vector4F_Subtract")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        float const b[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);
        SimdVector4F const vb = Vector4F_LoadUnalignedFloat4(b);

        SimdVector4F const result = Vector4F_Subtract(va, vb);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(a[0] - b[0]));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(a[1] - b[1]));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(a[2] - b[2]));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(a[3] - b[3]));
    }
}

TEST_CASE("Vector4F_Multiply")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.001f;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        float const b[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);
        SimdVector4F const vb = Vector4F_LoadUnalignedFloat4(b);

        SimdVector4F const result = Vector4F_Multiply(va, vb);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinAbs(a[0] * b[0], tolerance));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinAbs(a[1] * b[1], tolerance));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinAbs(a[2] * b[2], tolerance));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinAbs(a[3] * b[3], tolerance));
    }
}

TEST_CASE("Vector4F_Divide")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        float const b[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);
        SimdVector4F const vb = Vector4F_LoadUnalignedFloat4(b);

        SimdVector4F const result = Vector4F_Divide(va, vb);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(a[0] / b[0]));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(a[1] / b[1]));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(a[2] / b[2]));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(a[3] / b[3]));
    }
}

TEST_CASE("Vector4F_Negate")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_Negate(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(-a[0]));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(-a[1]));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(-a[2]));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(-a[3]));
    }
}

TEST_CASE("Vector4F_Reciprocal")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_Reciprocal(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(1.0f / a[0]));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(1.0f / a[1]));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(1.0f / a[2]));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(1.0f / a[3]));
    }
}

TEST_CASE("Vector4F_ReciprocalEst")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.1f;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_ReciprocalEst(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinAbs(1.0f / a[0], tolerance));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinAbs(1.0f / a[1], tolerance));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinAbs(1.0f / a[2], tolerance));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinAbs(1.0f / a[3], tolerance));
    }
}

TEST_CASE("Vector4F_Min")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        float const b[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);
        SimdVector4F const vb = Vector4F_LoadUnalignedFloat4(b);

        SimdVector4F const result = Vector4F_Min(va, vb);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(std::min<float>(a[0], b[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(std::min<float>(a[1], b[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(std::min<float>(a[2], b[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(std::min<float>(a[3], b[3])));
    }
}

TEST_CASE("Vector4F_Max")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        float const b[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);
        SimdVector4F const vb = Vector4F_LoadUnalignedFloat4(b);

        SimdVector4F const result = Vector4F_Max(va, vb);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(std::max<float>(a[0], b[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(std::max<float>(a[1], b[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(std::max<float>(a[2], b[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(std::max<float>(a[3], b[3])));
    }
}

TEST_CASE("Vector4F_Abs")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_Abs(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(std::abs(a[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(std::abs(a[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(std::abs(a[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(std::abs(a[3])));
    }
}

TEST_CASE("Vector4F_Square")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_Square(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(a[0] * a[0]));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(a[1] * a[1]));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(a[2] * a[2]));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(a[3] * a[3]));
    }
}

TEST_CASE("Vector4F_SignedSquare")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_SignedSquare(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(a[0] * std::abs(a[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(a[1] * std::abs(a[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(a[2] * std::abs(a[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(a[3] * std::abs(a[3])));
    }
}

TEST_CASE("Vector4F_Clamp")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 200; ++i)
    {
        float const a[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        float const b[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        float const c[4] = {
            b[0] + uniform(r, 10.0f),
            b[1] + uniform(r, 10.0f),
            b[2] + uniform(r, 10.0f),
            b[3] + uniform(r, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);
        SimdVector4F const vb = Vector4F_LoadUnalignedFloat4(b);
        SimdVector4F const vc = Vector4F_LoadUnalignedFloat4(c);

        SimdVector4F const result = Vector4F_Clamp(va, vb, vc);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(Clamp<float>(a[0], b[0], c[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(Clamp<float>(a[1], b[1], c[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(Clamp<float>(a[2], b[2], c[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(Clamp<float>(a[3], b[3], c[3])));
    }
}

TEST_CASE("Vector4F_Saturate")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, -1.0f, 2.0f),
            uniform(r, -1.0f, 2.0f),
            uniform(r, -1.0f, 2.0f),
            uniform(r, -1.0f, 2.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_Saturate(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(Saturate<float>(a[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(Saturate<float>(a[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(Saturate<float>(a[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(Saturate<float>(a[3])));
    }
}

TEST_CASE("Vector4F_SquareRoot")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, 0.0f, 49.0f),
            uniform(r, 0.0f, 49.0f),
            uniform(r, 0.0f, 49.0f),
            uniform(r, 0.0f, 49.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_SquareRoot(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(Sqrt<float>(a[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(Sqrt<float>(a[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(Sqrt<float>(a[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(Sqrt<float>(a[3])));
    }
}

TEST_CASE("Vector4F_SquareRootEst")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.005f;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, 0.0f, 49.0f),
            uniform(r, 0.0f, 49.0f),
            uniform(r, 0.0f, 49.0f),
            uniform(r, 0.0f, 49.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_SquareRootEst(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinAbs(Sqrt<float>(a[0]), tolerance));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinAbs(Sqrt<float>(a[1]), tolerance));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinAbs(Sqrt<float>(a[2]), tolerance));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinAbs(Sqrt<float>(a[3]), tolerance));
    }
}

TEST_CASE("Vector4F_ReciprocalSquareRoot")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, 0.0f, 49.0f),
            uniform(r, 0.0f, 49.0f),
            uniform(r, 0.0f, 49.0f),
            uniform(r, 0.0f, 49.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_ReciprocalSquareRoot(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(ReciprocalSqrt<float>(a[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(ReciprocalSqrt<float>(a[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(ReciprocalSqrt<float>(a[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(ReciprocalSqrt<float>(a[3])));
    }
}

TEST_CASE("Vector4F_ReciprocalSquareRootEst")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.0075f;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, 0.0f, 49.0f),
            uniform(r, 0.0f, 49.0f),
            uniform(r, 0.0f, 49.0f),
            uniform(r, 0.0f, 49.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_ReciprocalSquareRootEst(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinAbs(ReciprocalSqrtEst<float>(a[0]), tolerance));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinAbs(ReciprocalSqrtEst<float>(a[1]), tolerance));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinAbs(ReciprocalSqrtEst<float>(a[2]), tolerance));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinAbs(ReciprocalSqrtEst<float>(a[3]), tolerance));
    }
}

TEST_CASE("Vector4F_Ceil")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, -50.0f, 50.0f),
            uniform(r, -50.0f, 50.0f),
            uniform(r, -50.0f, 50.0f),
            uniform(r, -50.0f, 50.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_Ceil(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(Ceil<float>(a[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(Ceil<float>(a[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(Ceil<float>(a[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(Ceil<float>(a[3])));
    }
}

TEST_CASE("Vector4F_Floor")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, -50.0f, 50.0f),
            uniform(r, -50.0f, 50.0f),
            uniform(r, -50.0f, 50.0f),
            uniform(r, -50.0f, 50.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_Floor(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(Floor<float>(a[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(Floor<float>(a[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(Floor<float>(a[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(Floor<float>(a[3])));
    }
}

TEST_CASE("Vector4F_Truncate")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, -50.0f, 50.0f),
            uniform(r, -50.0f, 50.0f),
            uniform(r, -50.0f, 50.0f),
            uniform(r, -50.0f, 50.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_Truncate(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(Truncate<float>(a[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(Truncate<float>(a[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(Truncate<float>(a[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(Truncate<float>(a[3])));
    }
}

TEST_CASE("Vector4F_Round")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, -50.0f, 50.0f),
            uniform(r, -50.0f, 50.0f),
            uniform(r, -50.0f, 50.0f),
            uniform(r, -50.0f, 50.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_Round(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(Round<float>(a[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(Round<float>(a[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(Round<float>(a[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(Round<float>(a[3])));
    }
}

TEST_CASE("Vector4F_Fraction")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        float const a[4] = {
            uniform(r, -50.0f, 50.0f),
            uniform(r, -50.0f, 50.0f),
            uniform(r, -50.0f, 50.0f),
            uniform(r, -50.0f, 50.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_Fraction(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(Fraction<float>(a[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(Fraction<float>(a[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(Fraction<float>(a[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(Fraction<float>(a[3])));
    }
}

TEST_CASE("Vector4F_Remainder")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 200; ++i)
    {
        float const a[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        float const b[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);
        SimdVector4F const vb = Vector4F_LoadUnalignedFloat4(b);

        SimdVector4F const result = Vector4F_Remainder(va, vb);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(Remainder<float>(a[0], b[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(Remainder<float>(a[1], b[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(Remainder<float>(a[2], b[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(Remainder<float>(a[3], b[3])));
    }
}

TEST_CASE("Vector4F_Repeat")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.005f;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 200; ++i)
    {
        float const a[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        float const b[4] = {
            uniform(r, 0.1f, 10.0f),
            uniform(r, 0.1f, 10.0f),
            uniform(r, 0.1f, 10.0f),
            uniform(r, 0.1f, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);
        SimdVector4F const vb = Vector4F_LoadUnalignedFloat4(b);

        SimdVector4F const result = Vector4F_Repeat(va, vb);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinAbs(Repeat<float>(a[0], b[0]), tolerance));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinAbs(Repeat<float>(a[1], b[1]), tolerance));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinAbs(Repeat<float>(a[2], b[2]), tolerance));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinAbs(Repeat<float>(a[3], b[3]), tolerance));
    }
}

TEST_CASE("Vector4F_Wrap")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.005f;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 200; ++i)
    {
        float const a[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        float const b[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        float const c[4] = {
            b[0] + uniform(r, 10.0f),
            b[1] + uniform(r, 10.0f),
            b[2] + uniform(r, 10.0f),
            b[3] + uniform(r, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);
        SimdVector4F const vb = Vector4F_LoadUnalignedFloat4(b);
        SimdVector4F const vc = Vector4F_LoadUnalignedFloat4(c);

        SimdVector4F const result = Vector4F_Wrap(va, vb, vc);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinAbs(Wrap<float>(a[0], b[0], c[0]), tolerance));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinAbs(Wrap<float>(a[1], b[1], c[1]), tolerance));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinAbs(Wrap<float>(a[2], b[2], c[2]), tolerance));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinAbs(Wrap<float>(a[3], b[3], c[3]), tolerance));
    }
}

TEST_CASE("Vector4F_Power")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 200; ++i)
    {
        float const a[4] = {
            uniform(r, 0.1f, 10.0f),
            uniform(r, 0.1f, 10.0f),
            uniform(r, 0.1f, 10.0f),
            uniform(r, 0.1f, 10.0f),
        };

        float const b[4] = {
            uniform(r, 0.01f, 3.0f) * ((r.Next() & 1) ? 1.0f : -1.0f),
            uniform(r, 0.01f, 3.0f) * ((r.Next() & 1) ? 1.0f : -1.0f),
            uniform(r, 0.01f, 3.0f) * ((r.Next() & 1) ? 1.0f : -1.0f),
            uniform(r, 0.01f, 3.0f) * ((r.Next() & 1) ? 1.0f : -1.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);
        SimdVector4F const vb = Vector4F_LoadUnalignedFloat4(b);

        SimdVector4F const result = Vector4F_Power(va, vb);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(Power<float>(a[0], b[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(Power<float>(a[1], b[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(Power<float>(a[2], b[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(Power<float>(a[3], b[3])));
    }
}

TEST_CASE("Vector4F_Exp")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 200; ++i)
    {
        float const a[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_Exp(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(Exp<float>(a[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(Exp<float>(a[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(Exp<float>(a[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(Exp<float>(a[3])));
    }
}

TEST_CASE("Vector4F_Exp2")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 200; ++i)
    {
        float const a[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_Exp2(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(Exp2<float>(a[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(Exp2<float>(a[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(Exp2<float>(a[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(Exp2<float>(a[3])));
    }
}

TEST_CASE("Vector4F_Exp10")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 200; ++i)
    {
        float const a[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_Exp10(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(Exp10<float>(a[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(Exp10<float>(a[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(Exp10<float>(a[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(Exp10<float>(a[3])));
    }
}

TEST_CASE("Vector4F_Log")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 200; ++i)
    {
        float const a[4] = {
            uniform(r, 0.001f, 10.0f),
            uniform(r, 0.001f, 10.0f),
            uniform(r, 0.001f, 10.0f),
            uniform(r, 0.001f, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_Log(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(Log<float>(a[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(Log<float>(a[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(Log<float>(a[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(Log<float>(a[3])));
    }
}

TEST_CASE("Vector4F_Log with base")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 200; ++i)
    {
        float const a[4] = {
            uniform(r, 0.001f, 10.0f),
            uniform(r, 0.001f, 10.0f),
            uniform(r, 0.001f, 10.0f),
            uniform(r, 0.001f, 10.0f),
        };

        float const b = uniform(r, 2.0f, 20.0f);

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);
        SimdVector4F const vb = Vector4F_Replicate(b);

        SimdVector4F const result = Vector4F_Log(va, vb);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(Log<float>(a[0], b)));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(Log<float>(a[1], b)));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(Log<float>(a[2], b)));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(Log<float>(a[3], b)));
    }
}

TEST_CASE("Vector4F_Log2")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 200; ++i)
    {
        float const a[4] = {
            uniform(r, 0.001f, 10.0f),
            uniform(r, 0.001f, 10.0f),
            uniform(r, 0.001f, 10.0f),
            uniform(r, 0.001f, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_Log2(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(Log2<float>(a[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(Log2<float>(a[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(Log2<float>(a[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(Log2<float>(a[3])));
    }
}

TEST_CASE("Vector4F_Log10")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 200; ++i)
    {
        float const a[4] = {
            uniform(r, 0.001f, 10.0f),
            uniform(r, 0.001f, 10.0f),
            uniform(r, 0.001f, 10.0f),
            uniform(r, 0.001f, 10.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_Log10(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(Log10<float>(a[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(Log10<float>(a[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(Log10<float>(a[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(Log10<float>(a[3])));
    }
}

TEST_CASE("Vector4F_SinCos")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 200; ++i)
    {
        float const a[4] = {
            uniform(r, -Pi2<float>, Pi2<float>),
            uniform(r, -Pi2<float>, Pi2<float>),
            uniform(r, -Pi2<float>, Pi2<float>),
            uniform(r, -Pi2<float>, Pi2<float>),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F vSin;
        SimdVector4F vCos;

        Vector4F_SinCos(vSin, vCos, va);

        REQUIRE_THAT(Vector4F_Extract<0>(vSin), WithinRel(Sin<float>(a[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(vSin), WithinRel(Sin<float>(a[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(vSin), WithinRel(Sin<float>(a[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(vSin), WithinRel(Sin<float>(a[3])));

        REQUIRE_THAT(Vector4F_Extract<0>(vCos), WithinRel(Cos<float>(a[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(vCos), WithinRel(Cos<float>(a[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(vCos), WithinRel(Cos<float>(a[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(vCos), WithinRel(Cos<float>(a[3])));
    }
}

TEST_CASE("Vector4F_Sin")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 200; ++i)
    {
        float const a[4] = {
            uniform(r, -Pi2<float>, Pi2<float>),
            uniform(r, -Pi2<float>, Pi2<float>),
            uniform(r, -Pi2<float>, Pi2<float>),
            uniform(r, -Pi2<float>, Pi2<float>),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_Sin(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(Sin<float>(a[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(Sin<float>(a[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(Sin<float>(a[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(Sin<float>(a[3])));
    }
}

TEST_CASE("Vector4F_Cos")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 200; ++i)
    {
        float const a[4] = {
            uniform(r, -Pi2<float>, Pi2<float>),
            uniform(r, -Pi2<float>, Pi2<float>),
            uniform(r, -Pi2<float>, Pi2<float>),
            uniform(r, -Pi2<float>, Pi2<float>),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_Cos(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(Cos<float>(a[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(Cos<float>(a[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(Cos<float>(a[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(Cos<float>(a[3])));
    }
}

TEST_CASE("Vector4F_Tan")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 200; ++i)
    {
        float const a[4] = {
            uniform(r, -Pi2<float>, Pi2<float>),
            uniform(r, -Pi2<float>, Pi2<float>),
            uniform(r, -Pi2<float>, Pi2<float>),
            uniform(r, -Pi2<float>, Pi2<float>),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_Tan(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(Tan<float>(a[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(Tan<float>(a[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(Tan<float>(a[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(Tan<float>(a[3])));
    }
}

TEST_CASE("Vector4F_Asin")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 200; ++i)
    {
        float const a[4] = {
            uniform(r, -1.0f, 1.0f),
            uniform(r, -1.0f, 1.0f),
            uniform(r, -1.0f, 1.0f),
            uniform(r, -1.0f, 1.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_Asin(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(Asin<float>(a[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(Asin<float>(a[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(Asin<float>(a[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(Asin<float>(a[3])));
    }
}

TEST_CASE("Vector4F_Acos")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 200; ++i)
    {
        float const a[4] = {
            uniform(r, -1.0f, 1.0f),
            uniform(r, -1.0f, 1.0f),
            uniform(r, -1.0f, 1.0f),
            uniform(r, -1.0f, 1.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_Acos(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(Acos<float>(a[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(Acos<float>(a[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(Acos<float>(a[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(Acos<float>(a[3])));
    }
}

TEST_CASE("Vector4F_Atan")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 200; ++i)
    {
        float const a[4] = {
            uniform(r, -1.0f, 1.0f),
            uniform(r, -1.0f, 1.0f),
            uniform(r, -1.0f, 1.0f),
            uniform(r, -1.0f, 1.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);

        SimdVector4F const result = Vector4F_Atan(va);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(Atan<float>(a[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(Atan<float>(a[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(Atan<float>(a[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(Atan<float>(a[3])));
    }
}

TEST_CASE("Vector4F_Atan2")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 200; ++i)
    {
        float const a[4] = {
            uniform(r, -1.0f, 1.0f),
            uniform(r, -1.0f, 1.0f),
            uniform(r, -1.0f, 1.0f),
            uniform(r, -1.0f, 1.0f),
        };

        float const b[4] = {
            uniform(r, -1.0f, 1.0f),
            uniform(r, -1.0f, 1.0f),
            uniform(r, -1.0f, 1.0f),
            uniform(r, -1.0f, 1.0f),
        };

        SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);
        SimdVector4F const vb = Vector4F_LoadUnalignedFloat4(b);

        SimdVector4F const result = Vector4F_Atan2(va, vb);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(Atan2<float>(a[0], b[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(Atan2<float>(a[1], b[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(Atan2<float>(a[2], b[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(Atan2<float>(a[3], b[3])));
    }
}

TEST_CASE("Vector4F_PreciseLerp")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.01f;

    UniformDistribution<float> uniform{};
    Random r{2137};

    SECTION("Separate T")
    {
        for (size_t i = 0; i < 200; ++i)
        {
            float const a[4] = {
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
            };

            float const b[4] = {
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
            };

            float const t[4] = {
                uniform(r, 0.0f, 1.0f),
                uniform(r, 0.0f, 1.0f),
                uniform(r, 0.0f, 1.0f),
                uniform(r, 0.0f, 1.0f),
            };

            SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);
            SimdVector4F const vb = Vector4F_LoadUnalignedFloat4(b);
            SimdVector4F const vt = Vector4F_LoadUnalignedFloat4(t);

            SimdVector4F const result = Vector4F_PreciseLerp(va, vb, vt);

            REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(PreciseLerp<float>(a[0], b[0], t[0]), tolerance));
            REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(PreciseLerp<float>(a[1], b[1], t[1]), tolerance));
            REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(PreciseLerp<float>(a[2], b[2], t[2]), tolerance));
            REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(PreciseLerp<float>(a[3], b[3], t[3]), tolerance));
        }
    }

    SECTION("Single T")
    {
        for (size_t i = 0; i < 200; ++i)
        {
            float const a[4] = {
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
            };

            float const b[4] = {
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
            };

            float const t = uniform(r, 0.0f, 1.0f);

            SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);
            SimdVector4F const vb = Vector4F_LoadUnalignedFloat4(b);

            SimdVector4F const result = Vector4F_PreciseLerp(va, vb, t);

            REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(PreciseLerp<float>(a[0], b[0], t)));
            REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(PreciseLerp<float>(a[1], b[1], t)));
            REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(PreciseLerp<float>(a[2], b[2], t)));
            REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(PreciseLerp<float>(a[3], b[3], t)));
        }
    }
}

TEST_CASE("Vector4F_Lerp")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.001f;

    UniformDistribution<float> uniform{};
    Random r{2137};

    SECTION("Separate T")
    {
        for (size_t i = 0; i < 200; ++i)
        {
            float const a[4] = {
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
            };

            float const b[4] = {
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
            };

            float const t[4] = {
                uniform(r, 0.0f, 1.0f),
                uniform(r, 0.0f, 1.0f),
                uniform(r, 0.0f, 1.0f),
                uniform(r, 0.0f, 1.0f),
            };

            SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);
            SimdVector4F const vb = Vector4F_LoadUnalignedFloat4(b);
            SimdVector4F const vt = Vector4F_LoadUnalignedFloat4(t);

            SimdVector4F const result = Vector4F_Lerp(va, vb, vt);

            REQUIRE_THAT(Vector4F_Extract<0>(result), WithinAbs(Lerp<float>(a[0], b[0], t[0]), tolerance));
            REQUIRE_THAT(Vector4F_Extract<1>(result), WithinAbs(Lerp<float>(a[1], b[1], t[1]), tolerance));
            REQUIRE_THAT(Vector4F_Extract<2>(result), WithinAbs(Lerp<float>(a[2], b[2], t[2]), tolerance));
            REQUIRE_THAT(Vector4F_Extract<3>(result), WithinAbs(Lerp<float>(a[3], b[3], t[3]), tolerance));
        }
    }

    SECTION("Single T")
    {
        for (size_t i = 0; i < 200; ++i)
        {
            float const a[4] = {
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
            };

            float const b[4] = {
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
            };

            float const t = uniform(r, 0.0f, 1.0f);

            SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);
            SimdVector4F const vb = Vector4F_LoadUnalignedFloat4(b);

            SimdVector4F const result = Vector4F_Lerp(va, vb, t);

            REQUIRE_THAT(Vector4F_Extract<0>(result), WithinAbs(Lerp<float>(a[0], b[0], t), tolerance));
            REQUIRE_THAT(Vector4F_Extract<1>(result), WithinAbs(Lerp<float>(a[1], b[1], t), tolerance));
            REQUIRE_THAT(Vector4F_Extract<2>(result), WithinAbs(Lerp<float>(a[2], b[2], t), tolerance));
            REQUIRE_THAT(Vector4F_Extract<3>(result), WithinAbs(Lerp<float>(a[3], b[3], t), tolerance));
        }
    }
}

TEST_CASE("Vector4F_PreciseBilinearLerp")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.01f;

    UniformDistribution<float> uniform{};
    Random r{2137};

    SECTION("Separate T")
    {
        for (size_t i = 0; i < 200; ++i)
        {
            float const a[4] = {
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
            };

            float const b[4] = {
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
            };

            float const c[4] = {
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
            };

            float const d[4] = {
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
            };

            float const u[4] = {
                uniform(r, 0.0f, 1.0f),
                uniform(r, 0.0f, 1.0f),
                uniform(r, 0.0f, 1.0f),
                uniform(r, 0.0f, 1.0f),
            };

            float const v[4] = {
                uniform(r, 0.0f, 1.0f),
                uniform(r, 0.0f, 1.0f),
                uniform(r, 0.0f, 1.0f),
                uniform(r, 0.0f, 1.0f),
            };

            SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);
            SimdVector4F const vb = Vector4F_LoadUnalignedFloat4(b);
            SimdVector4F const vc = Vector4F_LoadUnalignedFloat4(c);
            SimdVector4F const vd = Vector4F_LoadUnalignedFloat4(d);
            SimdVector4F const vu = Vector4F_LoadUnalignedFloat4(u);
            SimdVector4F const vv = Vector4F_LoadUnalignedFloat4(v);

            SimdVector4F const result = Vector4F_PreciseBilinearLerp(va, vb, vc, vd, vu, vv);

            REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(PreciseBilinearLerp<float>(a[0], b[0], c[0], d[0], u[0], v[0]), tolerance));
            REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(PreciseBilinearLerp<float>(a[1], b[1], c[1], d[1], u[1], v[1]), tolerance));
            REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(PreciseBilinearLerp<float>(a[2], b[2], c[2], d[2], u[2], v[2]), tolerance));
            REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(PreciseBilinearLerp<float>(a[3], b[3], c[3], d[3], u[3], v[3]), tolerance));
        }
    }

    SECTION("Single T")
    {
        for (size_t i = 0; i < 200; ++i)
        {
            float const a[4] = {
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
            };

            float const b[4] = {
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
            };

            float const c[4] = {
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
            };

            float const d[4] = {
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
            };

            float const u = uniform(r, 0.0f, 1.0f);

            float const v = uniform(r, 0.0f, 1.0f);

            SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);
            SimdVector4F const vb = Vector4F_LoadUnalignedFloat4(b);
            SimdVector4F const vc = Vector4F_LoadUnalignedFloat4(c);
            SimdVector4F const vd = Vector4F_LoadUnalignedFloat4(d);

            SimdVector4F const result = Vector4F_PreciseBilinearLerp(va, vb, vc, vd, u, v);

            REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(PreciseBilinearLerp<float>(a[0], b[0], c[0], d[0], u, v)));
            REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(PreciseBilinearLerp<float>(a[1], b[1], c[1], d[1], u, v)));
            REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(PreciseBilinearLerp<float>(a[2], b[2], c[2], d[2], u, v)));
            REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(PreciseBilinearLerp<float>(a[3], b[3], c[3], d[3], u, v)));
        }
    }
}

TEST_CASE("Vector4F_BilinearLerp")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.001f;

    UniformDistribution<float> uniform{};
    Random r{2137};

    SECTION("Separate T")
    {
        for (size_t i = 0; i < 200; ++i)
        {
            float const a[4] = {
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
            };

            float const b[4] = {
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
            };

            float const c[4] = {
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
            };

            float const d[4] = {
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
            };

            float const u[4] = {
                uniform(r, 0.0f, 1.0f),
                uniform(r, 0.0f, 1.0f),
                uniform(r, 0.0f, 1.0f),
                uniform(r, 0.0f, 1.0f),
            };

            float const v[4] = {
                uniform(r, 0.0f, 1.0f),
                uniform(r, 0.0f, 1.0f),
                uniform(r, 0.0f, 1.0f),
                uniform(r, 0.0f, 1.0f),
            };

            SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);
            SimdVector4F const vb = Vector4F_LoadUnalignedFloat4(b);
            SimdVector4F const vc = Vector4F_LoadUnalignedFloat4(c);
            SimdVector4F const vd = Vector4F_LoadUnalignedFloat4(d);
            SimdVector4F const vu = Vector4F_LoadUnalignedFloat4(u);
            SimdVector4F const vv = Vector4F_LoadUnalignedFloat4(v);

            SimdVector4F const result = Vector4F_BilinearLerp(va, vb, vc, vd, vu, vv);

            REQUIRE_THAT(Vector4F_Extract<0>(result), WithinAbs(BilinearLerp<float>(a[0], b[0], c[0], d[0], u[0], v[0]), tolerance));
            REQUIRE_THAT(Vector4F_Extract<1>(result), WithinAbs(BilinearLerp<float>(a[1], b[1], c[1], d[1], u[1], v[1]), tolerance));
            REQUIRE_THAT(Vector4F_Extract<2>(result), WithinAbs(BilinearLerp<float>(a[2], b[2], c[2], d[2], u[2], v[2]), tolerance));
            REQUIRE_THAT(Vector4F_Extract<3>(result), WithinAbs(BilinearLerp<float>(a[3], b[3], c[3], d[3], u[3], v[3]), tolerance));
        }
    }

    SECTION("Single T")
    {
        for (size_t i = 0; i < 200; ++i)
        {
            float const a[4] = {
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
            };

            float const b[4] = {
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
            };

            float const c[4] = {
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
            };

            float const d[4] = {
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
                uniform(r, -10.0f, 10.0f),
            };

            float const u = uniform(r, 0.0f, 1.0f);

            float const v = uniform(r, 0.0f, 1.0f);

            SimdVector4F const va = Vector4F_LoadUnalignedFloat4(a);
            SimdVector4F const vb = Vector4F_LoadUnalignedFloat4(b);
            SimdVector4F const vc = Vector4F_LoadUnalignedFloat4(c);
            SimdVector4F const vd = Vector4F_LoadUnalignedFloat4(d);

            SimdVector4F const result = Vector4F_BilinearLerp(va, vb, vc, vd, u, v);

            REQUIRE_THAT(Vector4F_Extract<0>(result), WithinAbs(BilinearLerp<float>(a[0], b[0], c[0], d[0], u, v), tolerance));
            REQUIRE_THAT(Vector4F_Extract<1>(result), WithinAbs(BilinearLerp<float>(a[1], b[1], c[1], d[1], u, v), tolerance));
            REQUIRE_THAT(Vector4F_Extract<2>(result), WithinAbs(BilinearLerp<float>(a[2], b[2], c[2], d[2], u, v), tolerance));
            REQUIRE_THAT(Vector4F_Extract<3>(result), WithinAbs(BilinearLerp<float>(a[3], b[3], c[3], d[3], u, v), tolerance));
        }
    }
}

TEST_CASE("Vector4F_Barycentric")
{
}

TEST_CASE("Vector4F_Unlerp")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 200; ++i)
    {
        float const lower[4] = {
            uniform(r, -1.0f, 1.0f),
            uniform(r, -1.0f, 1.0f),
            uniform(r, -1.0f, 1.0f),
            uniform(r, -1.0f, 1.0f),
        };

        float const upper[4] = {
            lower[0] + uniform(r, 1.0f, 10.0f),
            lower[1] + uniform(r, 1.0f, 10.0f),
            lower[2] + uniform(r, 1.0f, 10.0f),
            lower[3] + uniform(r, 1.0f, 10.0f),
        };

        float const t[4] = {
            uniform(r, lower[0], upper[0]),
            uniform(r, lower[0], upper[0]),
            uniform(r, lower[0], upper[0]),
            uniform(r, lower[0], upper[0]),
        };

        SimdVector4F const vLower = Vector4F_LoadUnalignedFloat4(lower);
        SimdVector4F const vUpper = Vector4F_LoadUnalignedFloat4(upper);
        SimdVector4F const vt = Vector4F_LoadUnalignedFloat4(t);

        SimdVector4F const result = Vector4F_Unlerp(vt, vLower, vUpper);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(Unlerp<float>(t[0], lower[0], upper[0])));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(Unlerp<float>(t[1], lower[1], upper[1])));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(Unlerp<float>(t[2], lower[2], upper[2])));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(Unlerp<float>(t[3], lower[3], upper[3])));
    }
}

TEST_CASE("Vector4F_Map")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.001f;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 200; ++i)
    {
        float const sourceLower[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        float const sourceUpper[4] = {
            sourceLower[0] + uniform(r, 1.0f, 10.0f),
            sourceLower[1] + uniform(r, 1.0f, 10.0f),
            sourceLower[2] + uniform(r, 1.0f, 10.0f),
            sourceLower[3] + uniform(r, 1.0f, 10.0f),
        };

        float const targetLower[4] = {
            uniform(r, -5.0f, 15.0f),
            uniform(r, -5.0f, 15.0f),
            uniform(r, -5.0f, 15.0f),
            uniform(r, -5.0f, 15.0f),
        };

        float const targetUpper[4] = {
            sourceLower[0] + uniform(r, 5.0f, 20.0f),
            sourceLower[1] + uniform(r, 5.0f, 20.0f),
            sourceLower[2] + uniform(r, 5.0f, 20.0f),
            sourceLower[3] + uniform(r, 5.0f, 20.0f),
        };

        float const value[4] = {
            uniform(r, sourceLower[0], sourceUpper[0]),
            uniform(r, sourceLower[1], sourceUpper[1]),
            uniform(r, sourceLower[2], sourceUpper[2]),
            uniform(r, sourceLower[3], sourceUpper[3]),
        };

        SimdVector4F vSourceLower = Vector4F_LoadUnalignedFloat4(sourceLower);
        SimdVector4F vSourceUpper = Vector4F_LoadUnalignedFloat4(sourceUpper);
        SimdVector4F vTargetLower = Vector4F_LoadUnalignedFloat4(targetLower);
        SimdVector4F vTargetUpper = Vector4F_LoadUnalignedFloat4(targetUpper);
        SimdVector4F vValue = Vector4F_LoadUnalignedFloat4(value);

        SimdVector4F result = Vector4F_Map(vValue, vSourceLower, vSourceUpper, vTargetLower, vTargetUpper);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinAbs(Map<float>(value[0], sourceLower[0], sourceUpper[0], targetLower[0], targetUpper[0]), tolerance));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinAbs(Map<float>(value[1], sourceLower[1], sourceUpper[1], targetLower[1], targetUpper[1]), tolerance));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinAbs(Map<float>(value[2], sourceLower[2], sourceUpper[2], targetLower[2], targetUpper[2]), tolerance));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinAbs(Map<float>(value[3], sourceLower[3], sourceUpper[3], targetLower[3], targetUpper[3]), tolerance));
    }
}

TEST_CASE("Vector4F_PreciseMap")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.001f;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 200; ++i)
    {
        float const sourceLower[4] = {
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
        };

        float const sourceUpper[4] = {
            sourceLower[0] + uniform(r, 1.0f, 10.0f),
            sourceLower[1] + uniform(r, 1.0f, 10.0f),
            sourceLower[2] + uniform(r, 1.0f, 10.0f),
            sourceLower[3] + uniform(r, 1.0f, 10.0f),
        };

        float const targetLower[4] = {
            uniform(r, -5.0f, 15.0f),
            uniform(r, -5.0f, 15.0f),
            uniform(r, -5.0f, 15.0f),
            uniform(r, -5.0f, 15.0f),
        };

        float const targetUpper[4] = {
            sourceLower[0] + uniform(r, 5.0f, 20.0f),
            sourceLower[1] + uniform(r, 5.0f, 20.0f),
            sourceLower[2] + uniform(r, 5.0f, 20.0f),
            sourceLower[3] + uniform(r, 5.0f, 20.0f),
        };

        float const value[4] = {
            uniform(r, sourceLower[0], sourceUpper[0]),
            uniform(r, sourceLower[1], sourceUpper[1]),
            uniform(r, sourceLower[2], sourceUpper[2]),
            uniform(r, sourceLower[3], sourceUpper[3]),
        };

        SimdVector4F vSourceLower = Vector4F_LoadUnalignedFloat4(sourceLower);
        SimdVector4F vSourceUpper = Vector4F_LoadUnalignedFloat4(sourceUpper);
        SimdVector4F vTargetLower = Vector4F_LoadUnalignedFloat4(targetLower);
        SimdVector4F vTargetUpper = Vector4F_LoadUnalignedFloat4(targetUpper);
        SimdVector4F vValue = Vector4F_LoadUnalignedFloat4(value);

        SimdVector4F result = Vector4F_PreciseMap(vValue, vSourceLower, vSourceUpper, vTargetLower, vTargetUpper);

        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinAbs(PreciseMap<float>(value[0], sourceLower[0], sourceUpper[0], targetLower[0], targetUpper[0]), tolerance));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinAbs(PreciseMap<float>(value[1], sourceLower[1], sourceUpper[1], targetLower[1], targetUpper[1]), tolerance));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinAbs(PreciseMap<float>(value[2], sourceLower[2], sourceUpper[2], targetLower[2], targetUpper[2]), tolerance));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinAbs(PreciseMap<float>(value[3], sourceLower[3], sourceUpper[3], targetLower[3], targetUpper[3]), tolerance));
    }
}

TEST_CASE("Vector4F_UnwindRadians")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        SimdVector4F const v = Vector4F_Create(
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f));

        SimdVector4F const result = Vector4F_UnwindRadians(v);

        CHECK_THAT(Vector4F_Extract<0>(result), WithinRel(UnwindRadians(Vector4F_Extract<0>(v))));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinRel(UnwindRadians(Vector4F_Extract<1>(v))));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinRel(UnwindRadians(Vector4F_Extract<2>(v))));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinRel(UnwindRadians(Vector4F_Extract<3>(v))));
    }
}

TEST_CASE("Vector4F_UnwindDegrees")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.01f;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        SimdVector4F const v = Vector4F_Create(
            uniform(r, -700.0f, 700.0f),
            uniform(r, -700.0f, 700.0f),
            uniform(r, -700.0f, 700.0f),
            uniform(r, -700.0f, 700.0f));

        SimdVector4F const result = Vector4F_UnwindDegrees(v);

        CHECK_THAT(Vector4F_Extract<0>(result), WithinRel(UnwindDegrees(Vector4F_Extract<0>(v)), tolerance));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinRel(UnwindDegrees(Vector4F_Extract<1>(v)), tolerance));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinRel(UnwindDegrees(Vector4F_Extract<2>(v)), tolerance));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinRel(UnwindDegrees(Vector4F_Extract<3>(v)), tolerance));
    }
}

TEST_CASE("Vector4F_AngleDifferenceRadians")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.01f;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        SimdVector4F const a = Vector4F_Create(
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f));

        SimdVector4F const b = Vector4F_Create(
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f));

        SimdVector4F const result = Vector4F_AngleDifferenceRadians(a, b);

        CHECK_THAT(Vector4F_Extract<0>(result), WithinRel(AngleDifferenceRadians(Vector4F_Extract<0>(a), Vector4F_Extract<0>(b)), tolerance));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinRel(AngleDifferenceRadians(Vector4F_Extract<1>(a), Vector4F_Extract<1>(b)), tolerance));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinRel(AngleDifferenceRadians(Vector4F_Extract<2>(a), Vector4F_Extract<2>(b)), tolerance));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinRel(AngleDifferenceRadians(Vector4F_Extract<3>(a), Vector4F_Extract<3>(b)), tolerance));
    }
}

TEST_CASE("Vector4F_AngleDifferenceDegrees")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        SimdVector4F const a = Vector4F_Create(
            uniform(r, -700.0f, 700.0f),
            uniform(r, -700.0f, 700.0f),
            uniform(r, -700.0f, 700.0f),
            uniform(r, -700.0f, 700.0f));

        SimdVector4F const b = Vector4F_Create(
            uniform(r, -700.0f, 700.0f),
            uniform(r, -700.0f, 700.0f),
            uniform(r, -700.0f, 700.0f),
            uniform(r, -700.0f, 700.0f));

        SimdVector4F const result = Vector4F_AngleDifferenceDegrees(a, b);

        CHECK_THAT(Vector4F_Extract<0>(result), WithinRel(AngleDifferenceDegrees(Vector4F_Extract<0>(a), Vector4F_Extract<0>(b))));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinRel(AngleDifferenceDegrees(Vector4F_Extract<1>(a), Vector4F_Extract<1>(b))));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinRel(AngleDifferenceDegrees(Vector4F_Extract<2>(a), Vector4F_Extract<2>(b))));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinRel(AngleDifferenceDegrees(Vector4F_Extract<3>(a), Vector4F_Extract<3>(b))));
    }
}

TEST_CASE("Vector4F_RevolutionsToDegrees")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        SimdVector4F const v = Vector4F_Create(
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f));

        SimdVector4F const result = Vector4F_RevolutionsToDegrees(v);

        CHECK_THAT(Vector4F_Extract<0>(result), WithinRel(RevolutionsToDegrees(Vector4F_Extract<0>(v))));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinRel(RevolutionsToDegrees(Vector4F_Extract<1>(v))));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinRel(RevolutionsToDegrees(Vector4F_Extract<2>(v))));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinRel(RevolutionsToDegrees(Vector4F_Extract<3>(v))));
    }
}

TEST_CASE("Vector4F_RevolutionsToRadians")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        SimdVector4F const v = Vector4F_Create(
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f));

        SimdVector4F const result = Vector4F_RevolutionsToRadians(v);

        CHECK_THAT(Vector4F_Extract<0>(result), WithinRel(RevolutionsToRadians(Vector4F_Extract<0>(v))));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinRel(RevolutionsToRadians(Vector4F_Extract<1>(v))));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinRel(RevolutionsToRadians(Vector4F_Extract<2>(v))));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinRel(RevolutionsToRadians(Vector4F_Extract<3>(v))));
    }
}

TEST_CASE("Vector4F_DegreesToRevolutions")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        SimdVector4F const v = Vector4F_Create(
            uniform(r, -700.0f, 700.0f),
            uniform(r, -700.0f, 700.0f),
            uniform(r, -700.0f, 700.0f),
            uniform(r, -700.0f, 700.0f));

        SimdVector4F const result = Vector4F_DegreesToRevolutions(v);

        CHECK_THAT(Vector4F_Extract<0>(result), WithinRel(DegreesToRevolutions(Vector4F_Extract<0>(v))));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinRel(DegreesToRevolutions(Vector4F_Extract<1>(v))));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinRel(DegreesToRevolutions(Vector4F_Extract<2>(v))));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinRel(DegreesToRevolutions(Vector4F_Extract<3>(v))));
    }
}

TEST_CASE("Vector4F_RadiansToRevolutions")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        SimdVector4F const v = Vector4F_Create(
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f));

        SimdVector4F const result = Vector4F_RadiansToRevolutions(v);

        CHECK_THAT(Vector4F_Extract<0>(result), WithinRel(RadiansToRevolutions(Vector4F_Extract<0>(v))));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinRel(RadiansToRevolutions(Vector4F_Extract<1>(v))));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinRel(RadiansToRevolutions(Vector4F_Extract<2>(v))));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinRel(RadiansToRevolutions(Vector4F_Extract<3>(v))));
    }
}

TEST_CASE("Vector4F_RadiansToDegrees")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.01f;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        SimdVector4F const v = Vector4F_Create(
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f),
            uniform(r, -10.0f, 10.0f));

        SimdVector4F const result = Vector4F_RadiansToDegrees(v);

        CHECK_THAT(Vector4F_Extract<0>(result), WithinRel(RadiansToDegrees(Vector4F_Extract<0>(v)), tolerance));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinRel(RadiansToDegrees(Vector4F_Extract<1>(v)), tolerance));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinRel(RadiansToDegrees(Vector4F_Extract<2>(v)), tolerance));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinRel(RadiansToDegrees(Vector4F_Extract<3>(v)), tolerance));
    }
}

TEST_CASE("Vector4F_DegreesToRadians")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.01f;

    UniformDistribution<float> uniform{};
    Random r{2137};

    for (size_t i = 0; i < 20; ++i)
    {
        SimdVector4F const v = Vector4F_Create(
            uniform(r, -700.0f, 700.0f),
            uniform(r, -700.0f, 700.0f),
            uniform(r, -700.0f, 700.0f),
            uniform(r, -700.0f, 700.0f));

        SimdVector4F const result = Vector4F_DegreesToRadians(v);

        CHECK_THAT(Vector4F_Extract<0>(result), WithinRel(DegreesToRadians(Vector4F_Extract<0>(v)), tolerance));
        CHECK_THAT(Vector4F_Extract<1>(result), WithinRel(DegreesToRadians(Vector4F_Extract<1>(v)), tolerance));
        CHECK_THAT(Vector4F_Extract<2>(result), WithinRel(DegreesToRadians(Vector4F_Extract<2>(v)), tolerance));
        CHECK_THAT(Vector4F_Extract<3>(result), WithinRel(DegreesToRadians(Vector4F_Extract<3>(v)), tolerance));
    }
}

TEST_CASE("Vector4F_Dot4")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SECTION("dot4(+X, +X)")
    {
        SimdVector4F const result = Vector4F_Dot4(Vector4F_PositiveUnitX(), Vector4F_PositiveUnitX());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(1.0f));
    }

    SECTION("dot4(+Y, +Y)")
    {
        SimdVector4F const result = Vector4F_Dot4(Vector4F_PositiveUnitY(), Vector4F_PositiveUnitY());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(1.0f));
    }

    SECTION("dot4(+Z, +Z)")
    {
        SimdVector4F const result = Vector4F_Dot4(Vector4F_PositiveUnitZ(), Vector4F_PositiveUnitZ());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(1.0f));
    }

    SECTION("dot4(+W, +W)")
    {
        SimdVector4F const result = Vector4F_Dot4(Vector4F_PositiveUnitW(), Vector4F_PositiveUnitW());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(1.0f));
    }

    SECTION("dot4(+X, +Y)")
    {
        SimdVector4F const result = Vector4F_Dot4(Vector4F_PositiveUnitX(), Vector4F_PositiveUnitY());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(0.0f));
    }

    SECTION("dot4(+Y, +X)")
    {
        SimdVector4F const result = Vector4F_Dot4(Vector4F_PositiveUnitY(), Vector4F_PositiveUnitX());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(0.0f));
    }

    SECTION("dot4(+X, +Z)")
    {
        SimdVector4F const result = Vector4F_Dot4(Vector4F_PositiveUnitX(), Vector4F_PositiveUnitZ());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(0.0f));
    }

    SECTION("dot4(+X, +W)")
    {
        SimdVector4F const result = Vector4F_Dot4(Vector4F_PositiveUnitX(), Vector4F_PositiveUnitW());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(0.0f));
    }

    SECTION("dot4(+W, +Z)")
    {
        SimdVector4F const result = Vector4F_Dot4(Vector4F_PositiveUnitW(), Vector4F_PositiveUnitZ());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(0.0f));
    }

    SECTION("dot4(+Z, +W)")
    {
        SimdVector4F const result = Vector4F_Dot4(Vector4F_PositiveUnitZ(), Vector4F_PositiveUnitW());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(0.0f));
    }
}

TEST_CASE("Vector4F_Dot3")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SECTION("dot3(+X, +X)")
    {
        SimdVector4F const result = Vector4F_Dot3(Vector4F_PositiveUnitX(), Vector4F_PositiveUnitX());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(1.0f));
    }

    SECTION("dot3(+X, +Y)")
    {
        SimdVector4F const result = Vector4F_Dot3(Vector4F_PositiveUnitX(), Vector4F_PositiveUnitY());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(0.0f));
    }

    SECTION("dot3(+X, +Z)")
    {
        SimdVector4F const result = Vector4F_Dot3(Vector4F_PositiveUnitX(), Vector4F_PositiveUnitZ());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(0.0f));
    }

    SECTION("dot3(+Y, +X)")
    {
        SimdVector4F const result = Vector4F_Dot3(Vector4F_PositiveUnitY(), Vector4F_PositiveUnitX());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(0.0f));
    }

    SECTION("dot3(+Y, +Y)")
    {
        SimdVector4F const result = Vector4F_Dot3(Vector4F_PositiveUnitY(), Vector4F_PositiveUnitY());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(1.0f));
    }

    SECTION("dot3(+Y, +Z)")
    {
        SimdVector4F const result = Vector4F_Dot3(Vector4F_PositiveUnitY(), Vector4F_PositiveUnitZ());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(0.0f));
    }

    SECTION("dot3(+Z, +X)")
    {
        SimdVector4F const result = Vector4F_Dot3(Vector4F_PositiveUnitZ(), Vector4F_PositiveUnitX());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(0.0f));
    }

    SECTION("dot3(+Z, +Y)")
    {
        SimdVector4F const result = Vector4F_Dot3(Vector4F_PositiveUnitZ(), Vector4F_PositiveUnitY());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(0.0f));
    }

    SECTION("dot3(+Z, +Z)")
    {
        SimdVector4F const result = Vector4F_Dot3(Vector4F_PositiveUnitZ(), Vector4F_PositiveUnitZ());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(1.0f));
    }

    SECTION("dot3(custom) ")
    {
        SimdVector4F const va = Vector4F_Create(1.0f, 2.0f, 3.0f, 0.0f);
        SimdVector4F const vb = Vector4F_Create(5.0f, 6.0f, 7.0f, 0.0f);

        SimdVector4F const result = Vector4F_Dot3(va, vb);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(38.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(38.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(38.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(38.0f));
    }
}

TEST_CASE("Vector4F_Dot2")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SECTION("dot2(+X, +X)")
    {
        SimdVector4F const result = Vector4F_Dot2(Vector4F_PositiveUnitX(), Vector4F_PositiveUnitX());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(1.0f));
    }

    SECTION("dot2(+X, +Y)")
    {
        SimdVector4F const result = Vector4F_Dot2(Vector4F_PositiveUnitX(), Vector4F_PositiveUnitY());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(0.0f));
    }

    SECTION("dot2(+Y, +X)")
    {
        SimdVector4F const result = Vector4F_Dot2(Vector4F_PositiveUnitY(), Vector4F_PositiveUnitX());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(0.0f));
    }

    SECTION("dot2(+Y, +Y)")
    {
        SimdVector4F const result = Vector4F_Dot2(Vector4F_PositiveUnitY(), Vector4F_PositiveUnitY());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(1.0f));
    }

    SECTION("dot2(+X, -X)")
    {
        SimdVector4F const result = Vector4F_Dot2(Vector4F_PositiveUnitX(), Vector4F_NegativeUnitX());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(-1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(-1.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(-1.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(-1.0f));
    }

    SECTION("dot2(+X, -Y)")
    {
        SimdVector4F const result = Vector4F_Dot2(Vector4F_PositiveUnitX(), Vector4F_NegativeUnitY());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(0.0f));
    }

    SECTION("dot2(+Y, -X)")
    {
        SimdVector4F const result = Vector4F_Dot2(Vector4F_PositiveUnitY(), Vector4F_NegativeUnitX());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(0.0f));
    }

    SECTION("dot2(+Y, -Y)")
    {
        SimdVector4F const result = Vector4F_Dot2(Vector4F_PositiveUnitY(), Vector4F_NegativeUnitY());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(-1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(-1.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(-1.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(-1.0f));
    }

    SECTION("dot2(-X, +X)")
    {
        SimdVector4F const result = Vector4F_Dot2(Vector4F_NegativeUnitX(), Vector4F_PositiveUnitX());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(-1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(-1.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(-1.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(-1.0f));
    }

    SECTION("dot2(-X, +Y)")
    {
        SimdVector4F const result = Vector4F_Dot2(Vector4F_NegativeUnitX(), Vector4F_PositiveUnitY());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(0.0f));
    }

    SECTION("dot2(-Y, +X)")
    {
        SimdVector4F const result = Vector4F_Dot2(Vector4F_NegativeUnitY(), Vector4F_PositiveUnitX());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(0.0f));
    }

    SECTION("dot2(-Y, +Y)")
    {
        SimdVector4F const result = Vector4F_Dot2(Vector4F_NegativeUnitY(), Vector4F_PositiveUnitY());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(-1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(-1.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(-1.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(-1.0f));
    }

    SECTION("dot2(-X, -X)")
    {
        SimdVector4F const result = Vector4F_Dot2(Vector4F_NegativeUnitX(), Vector4F_NegativeUnitX());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(1.0f));
    }

    SECTION("dot2(-X, -Y)")
    {
        SimdVector4F const result = Vector4F_Dot2(Vector4F_NegativeUnitX(), Vector4F_NegativeUnitY());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(0.0f));
    }

    SECTION("dot2(-Y, -X)")
    {
        SimdVector4F const result = Vector4F_Dot2(Vector4F_NegativeUnitY(), Vector4F_NegativeUnitX());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(0.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(0.0f));
    }

    SECTION("dot2(-Y, -Y)")
    {
        SimdVector4F const result = Vector4F_Dot2(Vector4F_NegativeUnitY(), Vector4F_NegativeUnitY());
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(1.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(1.0f));
    }

    SECTION("dot2(custom)")
    {
        SimdVector4F const va = Vector4F_Create(1.0f, 2.0f, 0.0f, 0.0f);
        SimdVector4F const vb = Vector4F_Create(5.0f, 6.0f, 0.0f, 0.0f);

        SimdVector4F const result = Vector4F_Dot2(va, vb);
        REQUIRE_THAT(Vector4F_Extract<0>(result), WithinRel(17.0f));
        REQUIRE_THAT(Vector4F_Extract<1>(result), WithinRel(17.0f));
        REQUIRE_THAT(Vector4F_Extract<2>(result), WithinRel(17.0f));
        REQUIRE_THAT(Vector4F_Extract<3>(result), WithinRel(17.0f));
    }
}

TEST_CASE("Vector4F_Cross4")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.0001f;

    SimdVector4F const v1000 = Vector4F_Create(1.0f, 0.0f, 0.0f, 0.0f);
    SimdVector4F const v0100 = Vector4F_Create(0.0f, 1.0f, 0.0f, 0.0f);
    SimdVector4F const v0010 = Vector4F_Create(0.0f, 0.0f, 1.0f, 0.0f);
    SimdVector4F const v0001 = Vector4F_Create(0.0f, 0.0f, 0.0f, 1.0f);

    {
        SimdVector4F const vr = Vector4F_Cross4(
            v1000,
            v0100,
            v0010);

        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vr), WithinAbs(-1.0f, tolerance));
    }
    {
        SimdVector4F const vr = Vector4F_Cross4(
            v1000,
            v0100,
            v0001);

        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vr), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vr), WithinAbs(0.0f, tolerance));
    }
    {
        SimdVector4F const vr = Vector4F_Cross4(
            v1000,
            v0010,
            v0001);

        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(-1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vr), WithinAbs(0.0f, tolerance));
    }
    {
        SimdVector4F const vr = Vector4F_Cross4(
            v0100,
            v0010,
            v0001);

        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(1.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<2>(vr), WithinAbs(0.0f, tolerance));
        CHECK_THAT(Vector4F_Extract<3>(vr), WithinAbs(0.0f, tolerance));
    }
}

TEST_CASE("Vector4F_Cross3")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const v1000 = Vector4F_Create(1.0f, 0.0f, 0.0f, 0.0f);
    SimdVector4F const v0100 = Vector4F_Create(0.0f, 1.0f, 0.0f, 0.0f);
    SimdVector4F const v0010 = Vector4F_Create(0.0f, 0.0f, 1.0f, 0.0f);

    {
        SimdVector4F const vr = Vector4F_Cross3(
            v1000,
            v0100);

        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(vr), WithinAbs(1.0f, 0.0001));
    }
    {
        SimdVector4F const vr = Vector4F_Cross3(
            v1000,
            v0010);


        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(-1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(vr), WithinAbs(0.0f, 0.0001));
    }
    {
        SimdVector4F const vr = Vector4F_Cross3(
            v0100,
            v1000);

        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(vr), WithinAbs(-1.0f, 0.0001));
    }
    {
        SimdVector4F const vr = Vector4F_Cross3(
            v0010,
            v0100);

        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(-1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(vr), WithinAbs(0.0f, 0.0001));
    }
    {
        SimdVector4F const vr = Vector4F_Cross3(
            Vector4F_Create(3.0f, 2.0f, 1.0f, 0.0f),
            Vector4F_Create(1.0f, 5.0f, 3.0f, 0.0f));

        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(-8.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(vr), WithinAbs(13.0f, 0.0001));
    }
}

TEST_CASE("Vector4F_Cross2")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const v1000 = Vector4F_Create(1.0f, 0.0f, 0.0f, 0.0f);
    SimdVector4F const v0100 = Vector4F_Create(0.0f, 1.0f, 0.0f, 0.0f);

    {
        SimdVector4F const vr = Vector4F_Cross2(
            v1000,
            v0100);

        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(1.0f, 0.0001));
    }
    {
        SimdVector4F const vr = Vector4F_Cross2(
            v0100,
            v1000);

        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(-1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(-1.0f, 0.0001));
    }
    {
        SimdVector4F const vr = Vector4F_Cross2(
            Vector4F_Create(3.0f, 2.0f, 0.0f, 0.0f),
            Vector4F_Create(1.0f, 5.0f, 0.0f, 0.0f));

        CHECK_THAT(Vector4F_Extract<0>(vr), WithinAbs(13.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(vr), WithinAbs(13.0f, 0.0001));
    }
}

TEST_CASE("Vector4F_LengthSquared4")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.0001f;

    SimdVector4F const va = Vector4F_Create(1.0f, 2.0f, 3.0f, 4.0f);

    SimdVector4F const result = Vector4F_LengthSquared4(va);
    REQUIRE_THAT(Vector4F_Extract<0>(result), WithinAbs(30.0f, tolerance));
    REQUIRE_THAT(Vector4F_Extract<1>(result), WithinAbs(30.0f, tolerance));
    REQUIRE_THAT(Vector4F_Extract<2>(result), WithinAbs(30.0f, tolerance));
    REQUIRE_THAT(Vector4F_Extract<3>(result), WithinAbs(30.0f, tolerance));
}

TEST_CASE("Vector4F_LengthSquared3")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.0001f;

    SimdVector4F const va = Vector4F_Create(1.0f, 2.0f, 3.0f, 4.0f);

    SimdVector4F const result = Vector4F_LengthSquared3(va);
    REQUIRE_THAT(Vector4F_Extract<0>(result), WithinAbs(14.0f, tolerance));
    REQUIRE_THAT(Vector4F_Extract<1>(result), WithinAbs(14.0f, tolerance));
    REQUIRE_THAT(Vector4F_Extract<2>(result), WithinAbs(14.0f, tolerance));
    REQUIRE_THAT(Vector4F_Extract<3>(result), WithinAbs(14.0f, tolerance));
}

TEST_CASE("Vector4F_LengthSquared2")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.0001f;

    SimdVector4F const va = Vector4F_Create(1.0f, 2.0f, 3.0f, 4.0f);

    SimdVector4F const result = Vector4F_LengthSquared2(va);
    REQUIRE_THAT(Vector4F_Extract<0>(result), WithinAbs(5.0f, tolerance));
    REQUIRE_THAT(Vector4F_Extract<1>(result), WithinAbs(5.0f, tolerance));
    REQUIRE_THAT(Vector4F_Extract<2>(result), WithinAbs(5.0f, tolerance));
    REQUIRE_THAT(Vector4F_Extract<3>(result), WithinAbs(5.0f, tolerance));
}

TEST_CASE("Vector4F_Length4")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.0001f;

    SimdVector4F const va = Vector4F_Create(1.0f, 2.0f, 3.0f, 4.0f);

    SimdVector4F const result = Vector4F_Length4(va);
    REQUIRE_THAT(Vector4F_Extract<0>(result), WithinAbs(5.4772255750516611345696978280085f, tolerance));
    REQUIRE_THAT(Vector4F_Extract<1>(result), WithinAbs(5.4772255750516611345696978280085f, tolerance));
    REQUIRE_THAT(Vector4F_Extract<2>(result), WithinAbs(5.4772255750516611345696978280085f, tolerance));
    REQUIRE_THAT(Vector4F_Extract<3>(result), WithinAbs(5.4772255750516611345696978280085f, tolerance));
}

TEST_CASE("Vector4F_Length3")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.0001f;

    SimdVector4F const va = Vector4F_Create(1.0f, 2.0f, 3.0f, 4.0f);

    SimdVector4F const result = Vector4F_Length3(va);
    REQUIRE_THAT(Vector4F_Extract<0>(result), WithinAbs(3.74166f, tolerance));
    REQUIRE_THAT(Vector4F_Extract<1>(result), WithinAbs(3.74166f, tolerance));
    REQUIRE_THAT(Vector4F_Extract<2>(result), WithinAbs(3.74166f, tolerance));
    REQUIRE_THAT(Vector4F_Extract<3>(result), WithinAbs(3.74166f, tolerance));
}

TEST_CASE("Vector4F_Length2")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    static constexpr float tolerance = 0.0001f;

    SimdVector4F const va = Vector4F_Create(1.0f, 2.0f, 3.0f, 4.0f);

    SimdVector4F const result = Vector4F_Length2(va);
    REQUIRE_THAT(Vector4F_Extract<0>(result), WithinAbs(2.23607f, tolerance));
    REQUIRE_THAT(Vector4F_Extract<1>(result), WithinAbs(2.23607f, tolerance));
    REQUIRE_THAT(Vector4F_Extract<2>(result), WithinAbs(2.23607f, tolerance));
    REQUIRE_THAT(Vector4F_Extract<3>(result), WithinAbs(2.23607f, tolerance));
}

TEST_CASE("Vector4F_Normalize4")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const va = Vector4F_Create(1.0f, 2.0f, 3.0f, 4.0f);
    SimdVector4F const result = Vector4F_Normalize4(va);

    CHECK_THAT(Vector4F_Extract<0>(result), WithinAbs(0.1825741858350553711523232609336f, 0.0001));
    CHECK_THAT(Vector4F_Extract<1>(result), WithinAbs(0.3651483716701107423046465218672f, 0.0001));
    CHECK_THAT(Vector4F_Extract<2>(result), WithinAbs(0.5477225575051661134569697828008f, 0.0001));
    CHECK_THAT(Vector4F_Extract<3>(result), WithinAbs(0.7302967433402214846092930437344f, 0.0001));
}

TEST_CASE("Vector4F_Normalize3")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const va = Vector4F_Create(1.0f, 2.0f, 3.0f, 4.0f);
    SimdVector4F const result = Vector4F_Normalize3(va);

    CHECK_THAT(Vector4F_Extract<0>(result), WithinAbs(0.26726f, 0.0001));
    CHECK_THAT(Vector4F_Extract<1>(result), WithinAbs(0.53452f, 0.0001));
    CHECK_THAT(Vector4F_Extract<2>(result), WithinAbs(0.80178f, 0.0001));
}

TEST_CASE("Vector4F_Normalize2")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const va = Vector4F_Create(1.0f, 2.0f, 3.0f, 4.0f);
    SimdVector4F const result = Vector4F_Normalize2(va);

    CHECK_THAT(Vector4F_Extract<0>(result), WithinAbs(0.44721f, 0.0001));
    CHECK_THAT(Vector4F_Extract<1>(result), WithinAbs(0.89443f, 0.0001));
}

TEST_CASE("Vector4F_Reflect4")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const v = Vector4F_Create(1.0f, 2.0f, 3.0f, 4.0f);
    SimdVector4F const n = Vector4F_Create(0.0f, 1.0f, 0.0f, 0.0f);

    SimdVector4F const result = Vector4F_Reflect4(v, n);

    CHECK_THAT(Vector4F_Extract<0>(result), WithinAbs(1.0f, 0.0001));
    CHECK_THAT(Vector4F_Extract<1>(result), WithinAbs(-2.0f, 0.0001));
    CHECK_THAT(Vector4F_Extract<2>(result), WithinAbs(3.0f, 0.0001));
    CHECK_THAT(Vector4F_Extract<3>(result), WithinAbs(4.0f, 0.0001));
}

TEST_CASE("Vector4F_Reflect3")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const v = Vector4F_Create(1.0f, 2.0f, 3.0f, 0.0f);
    SimdVector4F const n = Vector4F_Create(0.0f, 1.0f, 0.0f, 0.0f);

    SimdVector4F const result = Vector4F_Reflect3(v, n);

    CHECK_THAT(Vector4F_Extract<0>(result), WithinAbs(1.0f, 0.0001));
    CHECK_THAT(Vector4F_Extract<1>(result), WithinAbs(-2.0f, 0.0001));
    CHECK_THAT(Vector4F_Extract<2>(result), WithinAbs(3.0f, 0.0001));
}

TEST_CASE("Vector4F_Reflect2")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const v = Vector4F_Create(1.0f, 2.0f, 3.0f, 0.0f);
    SimdVector4F const n = Vector4F_Create(0.0f, 1.0f, 0.0f, 0.0f);

    SimdVector4F const result = Vector4F_Reflect2(v, n);

    CHECK_THAT(Vector4F_Extract<0>(result), WithinAbs(1.0f, 0.0001));
    CHECK_THAT(Vector4F_Extract<1>(result), WithinAbs(-2.0f, 0.0001));
}

TEST_CASE("Vector4F_Refract4")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    {
        SimdVector4F const i = Vector4F_Create(1.0f, 1.0f, 0.0f, 0.0f);
        SimdVector4F const n = Vector4F_Create(0.0f, 1.0f, 0.0f, 0.0f);
        SimdVector4F const idx = Vector4F_Replicate(1.0f);

        SimdVector4F const r = Vector4F_Refract4(i, n, idx);

        CHECK_THAT(Vector4F_Extract<0>(r), WithinAbs(1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r), WithinAbs(-1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r), WithinAbs(0.0f, 0.0001));
    }

    {
        SimdVector4F const i = Vector4F_Create(1.0f, 1.0f, 0.0f, 1.0f);
        SimdVector4F const n = Vector4F_Create(0.0f, 1.0f, 0.0f, 1.0f);
        SimdVector4F const idx = Vector4F_Replicate(0.5f);

        SimdVector4F const r = Vector4F_Refract4(i, n, idx);

        CHECK_THAT(Vector4F_Extract<0>(r), WithinAbs(0.5f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r), WithinAbs(-1.82288f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r), WithinAbs(-1.82288f, 0.0001));
    }
}

TEST_CASE("Vector4F_Refract3")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    {
        SimdVector4F const i = Vector4F_Create(1.0f, 1.0f, 0.0f, 0.0f);
        SimdVector4F const n = Vector4F_Create(0.0f, 1.0f, 0.0f, 0.0f);
        SimdVector4F const idx = Vector4F_Replicate(1.0f);

        SimdVector4F const r = Vector4F_Refract3(i, n, idx);


        CHECK_THAT(Vector4F_Extract<0>(r), WithinAbs(1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r), WithinAbs(-1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r), WithinAbs(0.0f, 0.0001));
    }

    {
        SimdVector4F const i = Vector4F_Create(1.0f, 1.0f, 0.0f, 0.0f);
        SimdVector4F const n = Vector4F_Create(0.0f, 1.0f, 0.0f, 0.0f);
        SimdVector4F const idx = Vector4F_Replicate(0.5f);

        SimdVector4F const r = Vector4F_Refract3(i, n, idx);

        CHECK_THAT(Vector4F_Extract<0>(r), WithinAbs(0.5f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r), WithinAbs(-1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r), WithinAbs(0.0f, 0.0001));
    }
}

TEST_CASE("Vector4F_Refract2")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    {
        SimdVector4F const i = Vector4F_Create(1.0f, 1.0f, 0.0f, 0.0f);
        SimdVector4F const n = Vector4F_Create(0.0f, 1.0f, 0.0f, 0.0f);
        SimdVector4F const idx = Vector4F_Replicate(1.0f);

        SimdVector4F const r = Vector4F_Refract2(i, n, idx);


        CHECK_THAT(Vector4F_Extract<0>(r), WithinAbs(1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r), WithinAbs(-1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r), WithinAbs(0.0f, 0.0001));
    }

    {
        SimdVector4F const i = Vector4F_Create(1.0f, 1.0f, 0.0f, 0.0f);
        SimdVector4F const n = Vector4F_Create(0.0f, 1.0f, 0.0f, 0.0f);
        SimdVector4F const idx = Vector4F_Replicate(0.5f);

        SimdVector4F const r = Vector4F_Refract2(i, n, idx);

        CHECK_THAT(Vector4F_Extract<0>(r), WithinAbs(0.5f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r), WithinAbs(-1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r), WithinAbs(0.0f, 0.0001));
    }
}

TEST_CASE("QuaternionF_Rotate3 / QuaternionF_InverseRotate3")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const source = Vector4F_PositiveUnitX();

    SECTION("axis = x, angle = 90")
    {
        SimdVector4F const q = QuaternionF_CreateFromAxisAngle(Vector4F_PositiveUnitX(), DegreesToRadians(90.0f));

        SimdVector4F const transformed = QuaternionF_Rotate3(q, source);

        CHECK_THAT(Vector4F_Extract<0>(transformed), WithinAbs(1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(transformed), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(transformed), WithinAbs(0.0f, 0.0001));

        SimdVector4F const reversed = QuaternionF_InverseRotate3(q, transformed);

        CHECK_THAT(Vector4F_Extract<0>(reversed), WithinAbs(1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(reversed), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(reversed), WithinAbs(0.0f, 0.0001));
    }

    SECTION("axis = y, angle = 90")
    {
        SimdVector4F const q = QuaternionF_CreateFromAxisAngle(Vector4F_PositiveUnitY(), DegreesToRadians(90.0f));

        SimdVector4F const transformed = QuaternionF_Rotate3(q, source);

        CHECK_THAT(Vector4F_Extract<0>(transformed), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(transformed), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(transformed), WithinAbs(-1.0f, 0.0001));

        SimdVector4F const reversed = QuaternionF_InverseRotate3(q, transformed);

        CHECK_THAT(Vector4F_Extract<0>(reversed), WithinAbs(1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(reversed), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(reversed), WithinAbs(0.0f, 0.0001));
    }

    SECTION("axis = z, angle = 90")
    {
        SimdVector4F const q = QuaternionF_CreateFromAxisAngle(Vector4F_PositiveUnitZ(), DegreesToRadians(90.0f));

        SimdVector4F const transformed = QuaternionF_Rotate3(q, source);

        CHECK_THAT(Vector4F_Extract<0>(transformed), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(transformed), WithinAbs(1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(transformed), WithinAbs(0.0f, 0.0001));

        SimdVector4F const reversed = QuaternionF_InverseRotate3(q, transformed);

        CHECK_THAT(Vector4F_Extract<0>(reversed), WithinAbs(1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(reversed), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(reversed), WithinAbs(0.0f, 0.0001));
    }
}

TEST_CASE("Vector4F_Transform3")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SECTION("Identity")
    {
        SimdMatrix4x4F const xform = Matrix4x4F_Identity();


        SimdVector4F const v = Vector4F_Create(1.0f, 2.0f, 3.0f, 0.0f);

        SimdVector4F const r = Vector4F_Transform3(v, xform);

        CHECK_THAT(Vector4F_Extract<0>(r), WithinAbs(1.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r), WithinAbs(2.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r), WithinAbs(3.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r), WithinAbs(1.0f, 0.0001));
    }

    SECTION("Affine2 - Custom")
    {
        SimdMatrix4x4F const xform = Matrix4x4F_CreateAffineTransform2(
            Vector4F_Replicate(2.0f),
            Vector4F_Create(1.0f, 1.0f, 0.0f, 0.0f),
            DegreesToRadians(90.0f),
            Vector4F_Create(2.0f, 3.0f, 0.0f, 0.0f));


        SimdVector4F const v = Vector4F_Create(1.0f, 2.0f, 3.0f, 0.0f);

        SimdVector4F const r = Vector4F_Transform3(v, xform);

        CHECK_THAT(Vector4F_Extract<0>(r), WithinAbs(0.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<1>(r), WithinAbs(5.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<2>(r), WithinAbs(3.0f, 0.0001));
        CHECK_THAT(Vector4F_Extract<3>(r), WithinAbs(1.0f, 0.0001));
    }
}

TEST_CASE("Vector4F_CompareEqual")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const a = Vector4F_Create(1.0f, 2.0f, 1.0f, std::numeric_limits<float>::infinity());
    SimdVector4F const b = Vector4F_Create(1.0f, 1.0f, 3.0f, 4.0f);

    SimdMask4F const r = Vector4F_CompareEqual(a, b);

    REQUIRE(Mask4F_Extract<0>(r) != 0);
    REQUIRE(Mask4F_Extract<1>(r) == 0);
    REQUIRE(Mask4F_Extract<2>(r) == 0);
    REQUIRE(Mask4F_Extract<3>(r) == 0);
}

TEST_CASE("Vector4F_CompareNotEqual")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const a = Vector4F_Create(1.0f, 2.0f, 1.0f, std::numeric_limits<float>::infinity());
    SimdVector4F const b = Vector4F_Create(1.0f, 1.0f, 3.0f, 4.0f);

    SimdMask4F const r = Vector4F_CompareNotEqual(a, b);

    REQUIRE(Mask4F_Extract<0>(r) == 0);
    REQUIRE(Mask4F_Extract<1>(r) != 0);
    REQUIRE(Mask4F_Extract<2>(r) != 0);
    REQUIRE(Mask4F_Extract<3>(r) != 0);
}

TEST_CASE("Vector4F_CompareLessThan")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const a = Vector4F_Create(1.0f, 2.0f, 1.0f, std::numeric_limits<float>::infinity());
    SimdVector4F const b = Vector4F_Create(1.0f, 1.0f, 3.0f, 4.0f);

    SimdMask4F const r = Vector4F_CompareLessThan(a, b);

    REQUIRE(Mask4F_Extract<0>(r) == 0);
    REQUIRE(Mask4F_Extract<1>(r) == 0);
    REQUIRE(Mask4F_Extract<2>(r) != 0);
    REQUIRE(Mask4F_Extract<3>(r) == 0);
}

TEST_CASE("Vector4F_CompareLessEqual")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const a = Vector4F_Create(1.0f, 2.0f, 1.0f, std::numeric_limits<float>::infinity());
    SimdVector4F const b = Vector4F_Create(1.0f, 1.0f, 3.0f, 4.0f);

    SimdMask4F const r = Vector4F_CompareLessEqual(a, b);

    REQUIRE(Mask4F_Extract<0>(r) != 0);
    REQUIRE(Mask4F_Extract<1>(r) == 0);
    REQUIRE(Mask4F_Extract<2>(r) != 0);
    REQUIRE(Mask4F_Extract<3>(r) == 0);
}

TEST_CASE("Vector4F_CompareGreaterThan")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const a = Vector4F_Create(1.0f, 2.0f, 1.0f, std::numeric_limits<float>::infinity());
    SimdVector4F const b = Vector4F_Create(1.0f, 1.0f, 3.0f, 4.0f);

    SimdMask4F const r = Vector4F_CompareGreaterThan(a, b);

    REQUIRE(Mask4F_Extract<0>(r) == 0);
    REQUIRE(Mask4F_Extract<1>(r) != 0);
    REQUIRE(Mask4F_Extract<2>(r) == 0);
    REQUIRE(Mask4F_Extract<3>(r) != 0);
}

TEST_CASE("Vector4F_CompareGreaterEqual")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const a = Vector4F_Create(1.0f, 2.0f, 1.0f, std::numeric_limits<float>::infinity());
    SimdVector4F const b = Vector4F_Create(1.0f, 1.0f, 3.0f, 4.0f);

    SimdMask4F const r = Vector4F_CompareGreaterEqual(a, b);

    REQUIRE(Mask4F_Extract<0>(r) != 0);
    REQUIRE(Mask4F_Extract<1>(r) != 0);
    REQUIRE(Mask4F_Extract<2>(r) == 0);
    REQUIRE(Mask4F_Extract<3>(r) != 0);
}

TEST_CASE("Vector4F_CompareNaN")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    {
        SimdVector4F const v = Vector4F_Create(static_cast<float>(rand()) + std::numeric_limits<float>::quiet_NaN(), 2.0f, 1.0f, std::numeric_limits<float>::infinity());
        SimdMask4F const r = Vector4F_CompareNaN(v);

        REQUIRE(Mask4F_Extract<0>(r) != 0);
        REQUIRE(Mask4F_Extract<1>(r) == 0);
        REQUIRE(Mask4F_Extract<2>(r) == 0);
        REQUIRE(Mask4F_Extract<3>(r) == 0);
    }

    {
        SimdVector4F const v = Vector4F_Create(1.0f, std::numeric_limits<float>::infinity(), std::numeric_limits<float>::quiet_NaN(), 4.0f);
        SimdMask4F const r = Vector4F_CompareNaN(v);

        REQUIRE(Mask4F_Extract<0>(r) == 0);
        REQUIRE(Mask4F_Extract<1>(r) == 0);
        REQUIRE(Mask4F_Extract<2>(r) != 0);
        REQUIRE(Mask4F_Extract<3>(r) == 0);
    }
}

TEST_CASE("Vector4F_CompareInfinite")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    {
        SimdVector4F const v = Vector4F_Create(std::numeric_limits<float>::quiet_NaN(), 2.0f, 1.0f, std::numeric_limits<float>::infinity());
        SimdMask4F const r = Vector4F_CompareInfinite(v);

        REQUIRE(Mask4F_Extract<0>(r) == 0);
        REQUIRE(Mask4F_Extract<1>(r) == 0);
        REQUIRE(Mask4F_Extract<2>(r) == 0);
        REQUIRE(Mask4F_Extract<3>(r) != 0);
    }

    {
        SimdVector4F const v = Vector4F_Create(1.0f, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::quiet_NaN(), 4.0f);
        SimdMask4F const r = Vector4F_CompareInfinite(v);

        REQUIRE(Mask4F_Extract<0>(r) == 0);
        REQUIRE(Mask4F_Extract<1>(r) != 0);
        REQUIRE(Mask4F_Extract<2>(r) == 0);
        REQUIRE(Mask4F_Extract<3>(r) == 0);
    }
}
//
// bool anemone_vectorcall Vector4F_IsEqual4(SimdVector4F a, SimdVector4F b);
// bool anemone_vectorcall Vector4F_IsEqual3(SimdVector4F a, SimdVector4F b);
// bool anemone_vectorcall Vector4F_IsEqual2(SimdVector4F a, SimdVector4F b);
//
// bool anemone_vectorcall Vector4F_IsNotEqual4(SimdVector4F a, SimdVector4F b);
// bool anemone_vectorcall Vector4F_IsNotEqual3(SimdVector4F a, SimdVector4F b);
// bool anemone_vectorcall Vector4F_IsNotEqual2(SimdVector4F a, SimdVector4F b);
//
// bool anemone_vectorcall Vector4F_IsLessThan4(SimdVector4F a, SimdVector4F b);
// bool anemone_vectorcall Vector4F_IsLessThan3(SimdVector4F a, SimdVector4F b);
// bool anemone_vectorcall Vector4F_IsLessThan2(SimdVector4F a, SimdVector4F b);
//
// bool anemone_vectorcall Vector4F_IsLessEqual4(SimdVector4F a, SimdVector4F b);
// bool anemone_vectorcall Vector4F_IsLessEqual3(SimdVector4F a, SimdVector4F b);
// bool anemone_vectorcall Vector4F_IsLessEqual2(SimdVector4F a, SimdVector4F b);
//
// bool anemone_vectorcall Vector4F_IsGreaterThan4(SimdVector4F a, SimdVector4F b);
// bool anemone_vectorcall Vector4F_IsGreaterThan3(SimdVector4F a, SimdVector4F b);
// bool anemone_vectorcall Vector4F_IsGreaterThan2(SimdVector4F a, SimdVector4F b);
//
// bool anemone_vectorcall Vector4F_IsGreaterEqual4(SimdVector4F a, SimdVector4F b);
// bool anemone_vectorcall Vector4F_IsGreaterEqual3(SimdVector4F a, SimdVector4F b);
// bool anemone_vectorcall Vector4F_IsGreaterEqual2(SimdVector4F a, SimdVector4F b);
//
// bool anemone_vectorcall Vector4F_IsNaN4(SimdVector4F v);
// bool anemone_vectorcall Vector4F_IsNaN3(SimdVector4F v);
// bool anemone_vectorcall Vector4F_IsNaN2(SimdVector4F v);
//
// SimdMask4F anemone_vectorcall Vector4F_CompareInfinite(SimdVector4F v);
// bool anemone_vectorcall Vector4F_IsInfinite4(SimdVector4F v);
// bool anemone_vectorcall Vector4F_IsInfinite3(SimdVector4F v);
// bool anemone_vectorcall Vector4F_IsInfinite2(SimdVector4F v);
//
// SimdMask4F anemone_vectorcall Vector4F_CompareInRange(SimdVector4F v, SimdVector4F lower, SimdVector4F upper);
// bool anemone_vectorcall Vector4F_InRange4(SimdVector4F v, SimdVector4F lower, SimdVector4F upper);
// bool anemone_vectorcall Vector4F_InRange3(SimdVector4F v, SimdVector4F lower, SimdVector4F upper);
// bool anemone_vectorcall Vector4F_InRange2(SimdVector4F v, SimdVector4F lower, SimdVector4F upper);


TEST_CASE("Vector4F_CompareNearZero")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    {
        SimdVector4F const v = Vector4F_Create(-1.0f, -0.0f, 0.0f, 1.0f);
        SimdMask4F const r = Vector4F_CompareNearZero(v);

        REQUIRE(Mask4F_Extract<0>(r) == 0);
        REQUIRE(Mask4F_Extract<1>(r) != 0);
        REQUIRE(Mask4F_Extract<2>(r) != 0);
        REQUIRE(Mask4F_Extract<3>(r) == 0);
    }
    {
        float const epsilon = std::numeric_limits<float>::epsilon();

        float const e0 = epsilon;
        float const e1 = std::nextafter(e0, 1.0f);
        float const e2 = std::nextafter(e1, 1.0f);
        float const e3 = std::nextafter(e2, 1.0f);

        SimdVector4F const v = Vector4F_Create(e0, e1, e2, e3);
        SimdMask4F const r = Vector4F_CompareNearZero(v);

        REQUIRE(Mask4F_Extract<0>(r) != 0);
        REQUIRE(Mask4F_Extract<1>(r) == 0);
        REQUIRE(Mask4F_Extract<2>(r) == 0);
        REQUIRE(Mask4F_Extract<3>(r) == 0);
    }
}

TEST_CASE("Vector4F_CompareNearZeroPrecise")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    float const zero = 0.0f;

    {
        SimdVector4F const v1 = Vector4F_Create(
            zero,
            std::bit_cast<float>(std::bit_cast<int32_t>(zero) + 1),
            std::bit_cast<float>(std::bit_cast<int32_t>(zero) + 2),
            std::bit_cast<float>(std::bit_cast<int32_t>(zero) + 3));

        SimdMask4F const r1 = Vector4F_CompareNearZeroPrecise(v1);

        REQUIRE(Mask4F_Extract<0>(r1) != 0);
        REQUIRE(Mask4F_Extract<1>(r1) != 0);
        REQUIRE(Mask4F_Extract<2>(r1) != 0);
        REQUIRE(Mask4F_Extract<3>(r1) != 0);

        SimdVector4F const v2 = Vector4F_Create(
            std::bit_cast<float>(std::bit_cast<int32_t>(zero) + 4),
            std::bit_cast<float>(std::bit_cast<int32_t>(zero) + 5),
            std::bit_cast<float>(std::bit_cast<int32_t>(zero) + 6),
            std::bit_cast<float>(std::bit_cast<int32_t>(zero) + 7));

        SimdMask4F const r2 = Vector4F_CompareNearZeroPrecise(v2);

        REQUIRE(Mask4F_Extract<0>(r2) != 0);
        REQUIRE(Mask4F_Extract<1>(r2) == 0);
        REQUIRE(Mask4F_Extract<2>(r2) == 0);
        REQUIRE(Mask4F_Extract<3>(r2) == 0);
    }
}

TEST_CASE("Vector4F_CompareNearEqual")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;
}

TEST_CASE("Vector4F_CompareNearEqualPrecise")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;
}
