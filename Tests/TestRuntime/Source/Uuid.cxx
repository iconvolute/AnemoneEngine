#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Uuid.hxx"

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

TEST_CASE("Uuid from bytes")
{
    using namespace Anemone;

    SECTION("Create lower upper")
    {
        Uuid const value = Uuid::Create(0xF0E0D0C0B0A09080, 0x0706050403020100);
        REQUIRE(value.Elements[0] == 0x00);
        REQUIRE(value.Elements[1] == 0x01);
        REQUIRE(value.Elements[2] == 0x02);
        REQUIRE(value.Elements[3] == 0x03);
        REQUIRE(value.Elements[4] == 0x04);
        REQUIRE(value.Elements[5] == 0x05);
        REQUIRE(value.Elements[6] == 0x06);
        REQUIRE(value.Elements[7] == 0x07);
        REQUIRE(value.Elements[8] == 0x80);
        REQUIRE(value.Elements[9] == 0x90);
        REQUIRE(value.Elements[10] == 0xA0);
        REQUIRE(value.Elements[11] == 0xB0);
        REQUIRE(value.Elements[12] == 0xC0);
        REQUIRE(value.Elements[13] == 0xD0);
        REQUIRE(value.Elements[14] == 0xE0);
        REQUIRE(value.Elements[15] == 0xF0);
    }
}

TEST_CASE("Uuid Version")
{
    using namespace Anemone;

    SECTION("Empty")
    {
        Uuid const value = Uuid::Empty();
        REQUIRE(value.GetVersion() == UuidVersion::Empty);
    }

    SECTION("Max")
    {
        Uuid const value = Uuid::Max();
        REQUIRE(value.GetVersion() == UuidVersion::Max);
    }

    SECTION("Version - Empty")
    {
        Uuid value{};
        value.SetVersion(UuidVersion::Empty);
        REQUIRE(value.GetVersion() == UuidVersion::Empty);

        std::string formatted{};
        REQUIRE(TryFormat(formatted, value));

        REQUIRE(formatted == "{00000000-0000-0000-0000-000000000000}");
    }

    SECTION("Version - Mac")
    {
        Uuid value{};
        value.SetVersion(UuidVersion::Mac);
        REQUIRE(value.GetVersion() == UuidVersion::Mac);

        std::string formatted{};
        REQUIRE(TryFormat(formatted, value));

        REQUIRE(formatted == "{00000000-0000-1000-0000-000000000000}");
    }

    SECTION("Version - Dce")
    {
        Uuid value{};
        value.SetVersion(UuidVersion::Dce);
        REQUIRE(value.GetVersion() == UuidVersion::Dce);

        std::string formatted{};
        REQUIRE(TryFormat(formatted, value));

        REQUIRE(formatted == "{00000000-0000-2000-0000-000000000000}");
    }

    SECTION("Version - Md5")
    {
        Uuid value{};
        value.SetVersion(UuidVersion::Md5);
        REQUIRE(value.GetVersion() == UuidVersion::Md5);

        std::string formatted{};
        REQUIRE(TryFormat(formatted, value));

        REQUIRE(formatted == "{00000000-0000-3000-0000-000000000000}");
    }

    SECTION("Version - Random")
    {
        Uuid value{};
        value.SetVersion(UuidVersion::Random);
        REQUIRE(value.GetVersion() == UuidVersion::Random);

        std::string formatted{};
        REQUIRE(TryFormat(formatted, value));

        REQUIRE(formatted == "{00000000-0000-4000-0000-000000000000}");
    }

    SECTION("Version - Sha1")
    {
        Uuid value{};
        value.SetVersion(UuidVersion::Sha1);
        REQUIRE(value.GetVersion() == UuidVersion::Sha1);

        std::string formatted{};
        REQUIRE(TryFormat(formatted, value));

        REQUIRE(formatted == "{00000000-0000-5000-0000-000000000000}");
    }

    SECTION("Version - SortMac")
    {
        Uuid value{};
        value.SetVersion(UuidVersion::SortMac);
        REQUIRE(value.GetVersion() == UuidVersion::SortMac);

        std::string formatted{};
        REQUIRE(TryFormat(formatted, value));

        REQUIRE(formatted == "{00000000-0000-6000-0000-000000000000}");
    }

    SECTION("Version - SortRand")
    {
        Uuid value{};
        value.SetVersion(UuidVersion::SortRand);
        REQUIRE(value.GetVersion() == UuidVersion::SortRand);

        std::string formatted{};
        REQUIRE(TryFormat(formatted, value));

        REQUIRE(formatted == "{00000000-0000-7000-0000-000000000000}");
    }

    SECTION("Version - Custom")
    {
        Uuid value{};
        value.SetVersion(UuidVersion::Custom);
        REQUIRE(value.GetVersion() == UuidVersion::Custom);

        std::string formatted{};
        REQUIRE(TryFormat(formatted, value));

        REQUIRE(formatted == "{00000000-0000-8000-0000-000000000000}");
    }

    SECTION("Version - Max")
    {
        Uuid value{};
        value.SetVersion(UuidVersion::Max);
        REQUIRE(value.GetVersion() == UuidVersion::Max);

        std::string formatted{};
        REQUIRE(TryFormat(formatted, value));

        REQUIRE(formatted == "{00000000-0000-f000-0000-000000000000}");
    }
}

TEST_CASE("Uuid Derivation")
{
    using namespace Anemone;

    Uuid const base = Anemone::NAMESPACE_DNS;
    Uuid const derived1 = Uuid::CreateDerived(base, "Test", 2137);

    REQUIRE(derived1 == Uuid{0x68, 0x54, 0x9d, 0xe9, 0x06, 0xc7, 0x83, 0xac, 0x06, 0x2d, 0x7d, 0xca, 0x04, 0x9f, 0xdf, 0xa4});

    Uuid const derived2 = Uuid::CreateDerived(derived1, "Test", 2137);
    REQUIRE(derived2 == Uuid{0xcd, 0x92, 0x48, 0xcb, 0x02, 0xa6, 0x8f, 0x0a, 0x52, 0xd4, 0xda, 0x1e, 0x4e, 0x2c, 0xc0, 0x1e});
}

TEST_CASE("Uuid Formatting")
{
    using namespace Anemone;

    SECTION("Null Uuid")
    {
        Uuid const value{};
        std::string formatted{};
        REQUIRE(TryFormat(formatted, value, UuidStringFormat::None));
        REQUIRE(formatted == "00000000000000000000000000000000");
    }

    SECTION("Non-null Uuid")
    {
        Uuid const value = NAMESPACE_DNS;
        std::string formatted{};
        REQUIRE(TryFormat(formatted, value, UuidStringFormat::None));
        REQUIRE(formatted == "6ba7b8109dad11d180b400c04fd430c8");
    }

    SECTION("Braces Dashes")
    {
        Uuid const value = NAMESPACE_DNS;
        std::string formatted{};
        REQUIRE(TryFormat(formatted, value, UuidStringFormat::BracesDashes));
        REQUIRE(formatted == "{6ba7b810-9dad-11d1-80b4-00c04fd430c8}");
    }

    SECTION("Braces")
    {
        Uuid const value = NAMESPACE_DNS;
        std::string formatted{};
        REQUIRE(TryFormat(formatted, value, UuidStringFormat::Braces));
        REQUIRE(formatted == "{6ba7b8109dad11d180b400c04fd430c8}");
    }

    SECTION("Dashes")
    {
        Uuid const value = NAMESPACE_DNS;
        std::string formatted{};
        REQUIRE(TryFormat(formatted, value, UuidStringFormat::Dashes));
        REQUIRE(formatted == "6ba7b810-9dad-11d1-80b4-00c04fd430c8");
    }

    SECTION("Default")
    {
        Uuid const value = NAMESPACE_DNS;
        std::string formatted{};
        REQUIRE(TryFormat(formatted, value));
        REQUIRE(formatted == "{6ba7b810-9dad-11d1-80b4-00c04fd430c8}");
    }
}

TEST_CASE("Uuid Parsing")
{
    using namespace Anemone;

    SECTION("Too short string")
    {
        Uuid value{};
        REQUIRE_FALSE(TryParse(value, "{dead"));
    }

    SECTION("Unterminated braces")
    {
        Uuid value{};
        REQUIRE_FALSE(TryParse(value, "{6ba7b810-9dad-11d1-80b4-00c04fd430c8dade"));
    }

    SECTION("Valid with braces and dashes")
    {
        Uuid value{};
        REQUIRE(TryParse(value, "{6ba7b810-9dad-11d1-80b4-00c04fd430c8}"));
        REQUIRE(value == NAMESPACE_DNS);
    }

    SECTION("Valid with braces")
    {
        Uuid value{};
        REQUIRE(TryParse(value, "{6ba7b8109dad11d180b400c04fd430c8}"));
        REQUIRE(value == NAMESPACE_DNS);
    }

    SECTION("Valid with dashes")
    {
        Uuid value{};
        REQUIRE(TryParse(value, "6ba7b810-9dad-11d1-80b4-00c04fd430c8"));
        REQUIRE(value == NAMESPACE_DNS);
    }

    SECTION("Valid with only numbers")
    {
        Uuid value{};
        REQUIRE(TryParse(value, "6ba7b8109dad11d180b400c04fd430c8"));
        REQUIRE(value == NAMESPACE_DNS);
    }
}
