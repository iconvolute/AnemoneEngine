#include "AnemoneRuntime/Numerics/FMask4.hxx"

#include <catch_amalgamated.hpp>

TEST_CASE("FMask4 / Reduce")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Numerics;
    using namespace Anemone;

    SECTION("0 0 0 0")
    {
        FMask4A m = FMask4A::Create(false, false, false, false);

        CHECK_FALSE(All(m));
        CHECK_FALSE(Any(m));
        CHECK(None(m));
    }

    SECTION("1 1 1 1")
    {
        FMask4A m = FMask4A::Create(true, true, true, true);

        CHECK(All(m));
        CHECK(Any(m));
        CHECK_FALSE(None(m));
    }

    SECTION("1 0 0 0")
    {
        FMask4A m = FMask4A::Create(true, false, false, false);

        CHECK_FALSE(All(m));
        CHECK(Any(m));
        CHECK_FALSE(None(m));
    }

    SECTION("1 0 1 0")
    {
        FMask4A m = FMask4A::Create(true, false, true, false);

        CHECK_FALSE(All(m));
        CHECK(Any(m));
        CHECK_FALSE(None(m));
    }
}

TEST_CASE("FMask4 / Logical operations")
{
    using namespace Catch::Matchers;
    using namespace Anemone::Numerics;
    using namespace Anemone;

    SECTION("And")
    {
        // TODO: implement extract or load/store for masks
        //[[maybe_unused]] FMask4A m1{false, true, false, false};
        //[[maybe_unused]] FMask4A m2{false, true, false, true};

        //{
        //    FMask4A r = And(m1, m2);
        //    r.
        //}
    }
}
