#include <catch_amalgamated.hpp>

#include "AnemoneRuntime/Hash/FNV.hxx"

TEST_CASE("FNV Hash")
{
    using namespace Anemone;

    SECTION("FNV1A32")
    {
        REQUIRE(FNV1A32::FromString("") == 0x811c9dc5);
        REQUIRE(FNV1A32::FromString("Hello, World!") == 0x5aecf734);
        REQUIRE(FNV1A32::FromString("Lazy fox jumps over the brown dog") == 0x5f5100f6);
    }

    SECTION("FNV1A64")
    {
        REQUIRE(FNV1A64::FromString("") == 0xcbf29ce484222325);
        REQUIRE(FNV1A64::FromString("Hello, World!") == 0x6ef05bd7cc857c54);
        REQUIRE(FNV1A64::FromString("Lazy fox jumps over the brown dog") == 0x94619724ef3db416);
    }

    SECTION("FNV1A128")
    {
        REQUIRE(FNV1A128::FromString("") == std::array<uint64_t, 2>{0x62B821756295C58Du, 0x6C62272E07BB0142u});
        REQUIRE(FNV1A128::FromString("Hello, World!") == std::array<uint64_t, 2>{0x097d27e62306f88c, 0x72d78473fa044a13});
        REQUIRE(FNV1A128::FromString("Lazy fox jumps over the brown dog") == std::array<uint64_t, 2>{0xbc1261935a86cece, 0x0fbae148ca8ed10d});
    }
}
