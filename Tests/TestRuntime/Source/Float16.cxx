#include <catch_amalgamated.hpp>

#include "AnemoneRuntime/Float.hxx"

TEST_CASE("Float16 conversion")
{
    using namespace Anemone::Numerics;
    using namespace Anemone;

    SECTION("To Float - single values")
    {
        SECTION("+nan")
        {
            Half const value = std::bit_cast<Half, uint16_t>(0x7FFFu);
            float const r = FromHalf(value);
            CHECK(Float32::IsNaN(r));
        }

        SECTION("-nan")
        {
            Half const value = std::bit_cast<Half, uint16_t>(0xFFFFu);
            float const r = FromHalf(value);
            CHECK(Float32::IsNaN(r));
        }

        SECTION("+inf")
        {
            Half const value = std::bit_cast<Half, uint16_t>(0x7C00u);
            float const r = FromHalf(value);
            CHECK(Float32::IsInfinity(r));
        }

        SECTION("-inf")
        {
            Half const value = std::bit_cast<Half, uint16_t>(0xFC00u);
            float const r = FromHalf(value);
            CHECK(Float32::IsInfinity(r));
        }

        SECTION("0")
        {
            Half const value = std::bit_cast<Half, uint16_t>(0x0000u);
            float const r = FromHalf(value);
            CHECK(r == 0.0f);
        }

        SECTION("-0")
        {
            Half const value = std::bit_cast<Half, uint16_t>(0x8000u);
            float const r = FromHalf(value);
            CHECK(r == -0.0f);
        }

        SECTION("1")
        {
            Half const value = std::bit_cast<Half, uint16_t>(0x3c00u);
            float const r = FromHalf(value);
            CHECK(r == 1.0f);
        }

        SECTION("-1")
        {
            Half const value = std::bit_cast<Half, uint16_t>(0xbc00u);
            float const r = FromHalf(value);
            CHECK(r == -1.0f);
        }

        SECTION("2")
        {
            Half const value = std::bit_cast<Half, uint16_t>(0x4000u);
            float const r = FromHalf(value);
            CHECK(r == 2.0f);
        }

        SECTION("+Max")
        {
            Half const value = std::bit_cast<Half, uint16_t>(0x7BFFu);
            float const r = FromHalf(value);
            CHECK(r == 65504.0f);
        }

        SECTION("-Max")
        {
            Half const value = std::bit_cast<Half, uint16_t>(0xFBFFu);
            float const r = FromHalf(value);
            CHECK(r == -65504.0f);
        }

        SECTION("-8192")
        {
            Half const value = std::bit_cast<Half, uint16_t>(0xF000u);
            float const r = FromHalf(value);
            CHECK(r == -8192.0f);
        }
    }

    SECTION("From Float - single values")
    {
        SECTION("+nan")
        {
            float const value = std::numeric_limits<float>::quiet_NaN();
            Half const r = ToHalf(value);
            CHECK(Float16::IsNaN(r));
        }

        SECTION("-nan")
        {
            float const value = -std::numeric_limits<float>::quiet_NaN();
            Half const r = ToHalf(value);
            CHECK(Float16::IsNaN(r));
        }

        SECTION("+inf")
        {
            float const value = std::numeric_limits<float>::infinity();
            Half const r = ToHalf(value);
            CHECK(Float16::IsInfinity(r));
        }

        SECTION("-inf")
        {
            float const value = -std::numeric_limits<float>::infinity();
            Half const r = ToHalf(value);
            CHECK(Float16::IsInfinity(r));
        }

        SECTION("0")
        {
            float const value = 0.0f;
            Half const r = ToHalf(value);
            CHECK(Float16::IsZero(r));
        }

        SECTION("-0")
        {
            float const value = -0.0f;
            Half const r = ToHalf(value);
            CHECK(Float16::IsZero(r));
        }

        SECTION("1")
        {
            float const value = 1.0f;
            uint16_t const r = std::bit_cast<uint16_t>(ToHalf(value));
            CHECK(r == 0x3c00u);
        }

        SECTION("2")
        {
            float const value = 2.0f;
            uint16_t const r = std::bit_cast<uint16_t>(ToHalf(value));
            CHECK(r == 0x4000u);
        }

        SECTION("+Max")
        {
            float const value = 65504.0f;
            uint16_t const r = std::bit_cast<uint16_t>(ToHalf(value));
            CHECK(r == 0x7BFFu);
        }

        SECTION("-Max")
        {
            float const value = -65504.0f;
            uint16_t const r = std::bit_cast<uint16_t>(ToHalf(value));
            CHECK(r == 0xFBFFu);
        }

        SECTION("-8192")
        {
            float const value = -8192.0f;
            uint16_t const r = std::bit_cast<uint16_t>(ToHalf(value));
            CHECK(r == 0xF000u);
        }
    }

    SECTION("To Float - multiple values")
    {
        {
            std::array<Half, 4> const values{
                std::bit_cast<Half, uint16_t>(0x7FFFu),
                std::bit_cast<Half, uint16_t>(0xFFFFu),
                std::bit_cast<Half, uint16_t>(0x7C00u),
                std::bit_cast<Half, uint16_t>(0xFC00u),
            };
            std::array<float, 4> const r = FromHalf(values);
            CHECK(Float32::IsNaN(r[0]));
            CHECK(Float32::IsNaN(r[1]));
            CHECK(Float32::IsInfinity(r[2]));
            CHECK(Float32::IsInfinity(r[3]));
        }

        {
            std::array<Half, 4> const values{
                std::bit_cast<Half, uint16_t>(0x0000u),
                std::bit_cast<Half, uint16_t>(0x8000u),
                std::bit_cast<Half, uint16_t>(0x3c00u),
                std::bit_cast<Half, uint16_t>(0xbc00u),
            };
            std::array<float, 4> const r = FromHalf(values);
            CHECK(r[0] == 0.0f);
            CHECK(r[1] == -0.0f);
            CHECK(r[2] == 1.0f);
            CHECK(r[3] == -1.0f);
        }
        {
            std::array<Half, 4> const values{
                std::bit_cast<Half, uint16_t>(0x4000u),
                std::bit_cast<Half, uint16_t>(0x7BFFu),
                std::bit_cast<Half, uint16_t>(0xFBFFu),
                std::bit_cast<Half, uint16_t>(0xF000u),
            };
            std::array<float, 4> const r = FromHalf(values);
            CHECK(r[0] == 2.0f);
            CHECK(r[1] == 65504.0f);
            CHECK(r[2] == -65504.0f);
            CHECK(r[3] == -8192.0f);
        }
    }

    SECTION("From Float - multiple values")
    {
        {
            std::array<float, 4> const values{
                std::numeric_limits<float>::quiet_NaN(),
                -std::numeric_limits<float>::quiet_NaN(),
                std::numeric_limits<float>::infinity(),
                -std::numeric_limits<float>::infinity(),
            };
            auto const r = ToHalf(values);
            CHECK(Float16::IsNaN(r[0]));
            CHECK(Float16::IsNaN(r[1]));
            CHECK(Float16::IsInfinity(r[2]));
            CHECK(Float16::IsInfinity(r[3]));
        }

        {
            std::array<float, 4> const values{
                0.0f,
                -0.0f,
                1.0f,
                -1.0f,
            };
            auto const r = ToHalf(values);
            CHECK(Float16::IsZero(r[0]));
            CHECK(Float16::IsZero(r[1]));
            CHECK(std::bit_cast<uint16_t>(r[2]) == 0x3c00u);
            CHECK(std::bit_cast<uint16_t>(r[3]) == 0xbc00u);
        }

        {
            std::array<float, 4> const values{
                2.0f,
                65504.0f,
                -65504.0f,
                -8192.0f,
            };
            auto const r = ToHalf(values);
            CHECK(std::bit_cast<uint16_t>(r[0]) == 0x4000u);
            CHECK(std::bit_cast<uint16_t>(r[1]) == 0x7BFFu);
            CHECK(std::bit_cast<uint16_t>(r[2]) == 0xFBFFu);
            CHECK(std::bit_cast<uint16_t>(r[3]) == 0xF000u);
        }
    }
}
