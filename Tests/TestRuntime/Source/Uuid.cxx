#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Base/Uuid.hxx"

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

ANEMONE_EXTERNAL_HEADERS_END

TEST_CASE("Uuid Ordering")
{
    using namespace Anemone;

    Uuid const a{1, 2};
    Uuid const b{1, 3};

    REQUIRE(a < b);
    REQUIRE(b > a);
    REQUIRE(a <= b);
    REQUIRE(b >= a);
    REQUIRE(a != b);
    REQUIRE(b != a);
    REQUIRE(a == a);
    REQUIRE(b == b);
    REQUIRE(a <= a);
    REQUIRE(b <= b);
    REQUIRE(a >= a);
    REQUIRE(b >= b);
}

TEST_CASE("Uuid Derivation")
{
    using namespace Anemone;

    Uuid const base = Anemone::NAMESPACE_DNS;

    Uuid const actual1 = UuidGenerator::CreateNamed(base, "Test", 2137);
    Uuid const expected1{0x68, 0x54, 0x9d, 0xe9, 0x06, 0xc7, 0xf3, 0xac, 0x06, 0x2d, 0x7d, 0xca, 0x04, 0x9f, 0xdf, 0xa4};
    REQUIRE(actual1 == expected1);

    Uuid const actual2 = UuidGenerator::CreateNamed(actual1, "Test", 2137);
    Uuid const expected2{0x17, 0xd9, 0x2e, 0x4c, 0xeb, 0x81, 0x12, 0x69, 0x43, 0x18, 0xf0, 0x1f, 0x56, 0x64, 0x93, 0x1c};
    REQUIRE(actual2 == expected2);
}

TEST_CASE("Uuid Formatting")
{
    using namespace Anemone;

    SECTION("Null Uuid")
    {
        Uuid const value{};
        std::string formatted = fmt::format("{}", value);
        REQUIRE(formatted == "{00000000-0000-0000-0000-000000000000}");
    }

    SECTION("Default")
    {
        Uuid const value = NAMESPACE_DNS;
        std::string formatted = fmt::format("{}", value);
        REQUIRE(formatted == "{6ba7b810-9dad-11d1-80b4-00c04fd430c8}");
    }

    SECTION("Braces")
    {
        Uuid const value = NAMESPACE_DNS;
        std::string formatted = fmt::format("{:b}", value);
        REQUIRE(formatted == "{6ba7b8109dad11d180b400c04fd430c8}");
    }

    SECTION("Dashes")
    {
        Uuid const value = NAMESPACE_DNS;
        std::string formatted = fmt::format("{:d}", value);
        REQUIRE(formatted == "6ba7b810-9dad-11d1-80b4-00c04fd430c8");
    }

    SECTION("Full")
    {
        Uuid const value = NAMESPACE_DNS;
        std::string formatted = fmt::format("{:f}", value);
        REQUIRE(formatted == "{6ba7b810-9dad-11d1-80b4-00c04fd430c8}");
    }

    SECTION("Raw")
    {
        Uuid const value = NAMESPACE_DNS;
        std::string formatted = fmt::format("{:r}", value);
        REQUIRE(formatted == "6ba7b8109dad11d180b400c04fd430c8");
    }
}

TEST_CASE("Uuid Parsing")
{
    using namespace Anemone;

    SECTION("Too short string")
    {
        auto value = UuidParser::Parse("{dead");
        REQUIRE(!value.has_value());
    }

    SECTION("Valid +braces +dashes")
    {
        auto value = UuidParser::Parse("{6ba7b810-9dad-11d1-80b4-00c04fd430c8}");
        REQUIRE(value.has_value());
        REQUIRE(value == NAMESPACE_DNS);
    }

    SECTION("Invalid dashes +braces +dashes")
    {
        auto value = UuidParser::Parse("{6ba7b810x9dad-11d1-80b4-00c04fd430c8}");
        REQUIRE(!value.has_value());
    }

    SECTION("Invalid braces front +braces +dashes")
    {
        auto value = UuidParser::Parse("[6ba7b810-9dad-11d1-80b4-00c04fd430c8}");
        REQUIRE(!value.has_value());
    }

    SECTION("Invalid braces back +braces +dashes")
    {
        auto value = UuidParser::Parse("{6ba7b810-9dad-11d1-80b4-00c04fd430c8]");
        REQUIRE(!value.has_value());
    }

    SECTION("Valid +braces -dashes")
    {
        auto value = UuidParser::Parse("{6ba7b8109dad11d180b400c04fd430c8}");
        REQUIRE(value.has_value());
        REQUIRE(value == NAMESPACE_DNS);
    }

    SECTION("Invalid braces front +braces -dashes")
    {
        auto value = UuidParser::Parse("[6ba7b8109dad11d180b400c04fd430c8}");
        REQUIRE(!value.has_value());
    }

    SECTION("Invalid braces back +braces -dashes")
    {
        auto value = UuidParser::Parse("{6ba7b8109dad11d180b400c04fd430c8]");
        REQUIRE(!value.has_value());
    }

    SECTION("Valid -braces +dashes")
    {
        auto value = UuidParser::Parse("6ba7b810-9dad-11d1-80b4-00c04fd430c8");
        REQUIRE(value.has_value());
        REQUIRE(value == NAMESPACE_DNS);
    }

    SECTION("Invalid dashes -braces +dashes")
    {
        auto value = UuidParser::Parse("6ba7b810x9dad-11d1-80b4-00c04fd430c8");
        REQUIRE(!value.has_value());
    }

    SECTION("Valid -braces -dashes")
    {
        auto value = UuidParser::Parse("6ba7b8109dad11d180b400c04fd430c8");
        REQUIRE(value.has_value());
        REQUIRE(value == NAMESPACE_DNS);
    }
}
