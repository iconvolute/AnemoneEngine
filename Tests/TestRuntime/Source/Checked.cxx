#include <catch_amalgamated.hpp>

#include "AnemoneBase/Checked.hxx"

TEST_CASE("Checked Add Overflow")
{
    using namespace Anemone;

    SECTION("Unsigned")
    {
        constexpr uint32_t maxValue = std::numeric_limits<uint32_t>::max();
        {
            uint32_t result = 0;
            CHECK_FALSE(CheckedAdd<uint32_t>(0, maxValue, result));
            CHECK(result == maxValue);
        }
        {
            uint32_t result = 0;
            CHECK_FALSE(CheckedAdd<uint32_t>(maxValue, 0, result));
            CHECK(result == maxValue);
        }
        {
            uint32_t result = 0;
            CHECK(CheckedAdd<uint32_t>(maxValue, maxValue, result));
            CHECK(result == (maxValue - 1));
        }
        {
            uint32_t result = 0;
            CHECK(CheckedAdd<uint32_t>(maxValue, 1, result));
            CHECK(result == 0);
        }
        {
            uint32_t result = 0;
            CHECK(CheckedAdd<uint32_t>(1, maxValue, result));
            CHECK(result == 0);
        }
    }

    SECTION("Signed")
    {
        constexpr int32_t maxValue = std::numeric_limits<int32_t>::max();
        constexpr int32_t minValue = std::numeric_limits<int32_t>::min();
        {
            int32_t result = 0;
            CHECK_FALSE(CheckedAdd<int32_t>(0, maxValue, result));
            CHECK(result == maxValue);
        }
        {
            int32_t result = 0;
            CHECK_FALSE(CheckedAdd<int32_t>(maxValue, 0, result));
            CHECK(result == maxValue);
        }
        {
            int32_t result = 0;
            CHECK_FALSE(CheckedAdd<int32_t>(0, minValue, result));
            CHECK(result == minValue);
        }
        {
            int32_t result = 0;
            CHECK_FALSE(CheckedAdd<int32_t>(minValue, 0, result));
            CHECK(result == minValue);
        }
        {
            int32_t result = 0;
            CHECK(CheckedAdd<int32_t>(maxValue, maxValue, result));
            CHECK(result == -2);
        }
        {
            int32_t result = 0;
            CHECK(CheckedAdd<int32_t>(minValue, minValue, result));
            CHECK(result == 0);
        }
        {
            int32_t result = 0;
            CHECK(CheckedAdd<int32_t>(maxValue, 1, result));
            CHECK(result == minValue);
        }
        {
            int32_t result = 0;
            CHECK(CheckedAdd<int32_t>(minValue, -1, result));
            CHECK(result == maxValue);
        }
        {
            int32_t result = 0;
            CHECK(CheckedAdd<int32_t>(1, maxValue, result));
            CHECK(result == minValue);
        }
        {
            int32_t result = 0;
            CHECK(CheckedAdd<int32_t>(-1, minValue, result));
            CHECK(result == maxValue);
        }
    }
}

TEST_CASE("Checked Sub Overflow")
{
    using namespace Anemone;

    SECTION("Unsigned")
    {
        constexpr uint32_t maxValue = std::numeric_limits<uint32_t>::max();
        {
            uint32_t result = 0;
            CHECK_FALSE(CheckedSub<uint32_t>(maxValue, 0, result));
            CHECK(result == maxValue);
        }
        {
            uint32_t result = 0;
            CHECK_FALSE(CheckedSub<uint32_t>(maxValue, maxValue, result));
            CHECK(result == 0);
        }
        {
            uint32_t result = 0;
            CHECK(CheckedSub<uint32_t>(0, maxValue, result));
            CHECK(result == 1);
        }
        {
            uint32_t result = 0;
            CHECK(CheckedSub<uint32_t>(0, 1, result));
            CHECK(result == maxValue);
        }
        {
            uint32_t result = 0;
            CHECK_FALSE(CheckedSub<uint32_t>(1, 0, result));
            CHECK(result == 1);
        }
        {
            uint32_t result = 0;
            CHECK_FALSE(CheckedSub<uint32_t>(1, 1, result));
            CHECK(result == 0);
        }
    }

    SECTION("Signed")
    {
        constexpr int32_t maxValue = std::numeric_limits<int32_t>::max();
        constexpr int32_t minValue = std::numeric_limits<int32_t>::min();
        {
            int32_t result = 0;
            CHECK_FALSE(CheckedSub<int32_t>(maxValue, 0, result));
            CHECK(result == maxValue);
        }
        {
            int32_t result = 0;
            CHECK_FALSE(CheckedSub<int32_t>(minValue, 0, result));
            CHECK(result == minValue);
        }
        {
            int32_t result = 0;
            CHECK_FALSE(CheckedSub<int32_t>(0, maxValue, result));
            CHECK(result == -maxValue);
        }
        {
            int32_t result = 0;
            CHECK(CheckedSub<int32_t>(0, minValue, result));
            CHECK(result == minValue);
        }
        {
            int32_t result = 0;
            CHECK_FALSE(CheckedSub<int32_t>(maxValue, maxValue, result));
            CHECK(result == 0);
        }
        {
            int32_t result = 0;
            CHECK_FALSE(CheckedSub<int32_t>(minValue, minValue, result));
            CHECK(result == 0);
        }
        {
            int32_t result = 0;
            CHECK(CheckedAdd<int32_t>(maxValue, 1, result));
            CHECK(result == minValue);
        }
        {
            int32_t result = 0;
            CHECK(CheckedAdd<int32_t>(minValue, -1, result));
            CHECK(result == maxValue);
        }
        {
            int32_t result = 0;
            CHECK(CheckedAdd<int32_t>(1, maxValue, result));
            CHECK(result == minValue);
        }
        {
            int32_t result = 0;
            CHECK(CheckedAdd<int32_t>(-1, minValue, result));
            CHECK(result == maxValue);
        }
    }
}

TEST_CASE("CheckedMul Overflow")
{
    using namespace Anemone;

    SECTION("Unsigned")
    {
        constexpr uint32_t maxValue = std::numeric_limits<uint32_t>::max();
        {
            uint32_t result = 0;
            CHECK_FALSE(CheckedMul<uint32_t>(0, maxValue, result));
            CHECK(result == 0);
        }
        {
            uint32_t result = 0;
            CHECK_FALSE(CheckedMul<uint32_t>(maxValue, 0, result));
            CHECK(result == 0);
        }
        {
            uint32_t result = 0;
            CHECK_FALSE(CheckedMul<uint32_t>(1, maxValue, result));
            CHECK(result == maxValue);
        }
        {
            uint32_t result = 0;
            CHECK_FALSE(CheckedMul<uint32_t>(maxValue, 1, result));
            CHECK(result == maxValue);
        }
        {
            uint32_t result = 0;
            CHECK(CheckedMul<uint32_t>(maxValue, maxValue, result));
            CHECK(result == 1);
        }
    }

    SECTION("Signed")
    {
        constexpr int32_t maxValue = std::numeric_limits<int32_t>::max();
        constexpr int32_t minValue = std::numeric_limits<int32_t>::min();
        {
            int32_t result = 0;
            CHECK_FALSE(CheckedMul<int32_t>(0, maxValue, result));
            CHECK(result == 0);
        }
        {
            int32_t result = 0;
            CHECK_FALSE(CheckedMul<int32_t>(maxValue, 0, result));
            CHECK(result == 0);
        }
        {
            int32_t result = 0;
            CHECK_FALSE(CheckedMul<int32_t>(1, maxValue, result));
            CHECK(result == maxValue);
        }
        {
            int32_t result = 0;
            CHECK_FALSE(CheckedMul<int32_t>(maxValue, 1, result));
            CHECK(result == maxValue);
        }
        {
            int32_t result = 0;
            CHECK_FALSE(CheckedMul<int32_t>(-1, maxValue, result));
            CHECK(result == minValue + 1);
        }
        {
            int32_t result = 0;
            CHECK_FALSE(CheckedMul<int32_t>(maxValue, -1, result));
            CHECK(result == minValue + 1);
        }
        {
            int32_t result = 0;
            CHECK(CheckedMul<int32_t>(-1, minValue, result));
            CHECK(result == minValue);
        }
        {
            int32_t result = 0;
            CHECK(CheckedMul<int32_t>(minValue, -1, result));
            CHECK(result == minValue);
        }
        {
            int32_t result = 0;
            CHECK(CheckedMul<int32_t>(maxValue, maxValue, result));
            CHECK(result == 1);
        }
        {
            int32_t result = 0;
            CHECK(CheckedMul<int32_t>(minValue, minValue, result));
            CHECK(result == 0);
        }
    }
}
