#include "AnemoneInterop/Headers.hxx"
#include "AnemoneBase/XXHash64.hxx"

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

ANEMONE_EXTERNAL_HEADERS_END

// Test copied from https://github.com/Cyan4973/xxHash/blob/dev/cli/xsum_sanity_check.c

TEST_CASE("XXHash64")
{
    using namespace Anemone;

    uint8_t buffer[256];

    constexpr uint64_t prime32 = 2654435761u;
    constexpr uint64_t prime64 = 11400714785074694797u;

    uint64_t byteGen = prime32;

    for (unsigned char& i : buffer)
    {
        i = static_cast<uint8_t>(byteGen >> 56);
        byteGen *= prime64;
    }

    SECTION("Length = 0, Seed = 0")
    {
        REQUIRE(XXHash64::FromBuffer(std::as_bytes(std::span{buffer, 0}), 0) == 0xEF46DB3751D8E999u);
    }

    SECTION("Length = 0, Seed = prime32")
    {
        REQUIRE(XXHash64::FromBuffer(std::as_bytes(std::span{buffer, 0}), prime32) == 0xAC75FDA2929B17EFu);
    }

    SECTION("Length = 1, Seed = 0")
    {
        REQUIRE(XXHash64::FromBuffer(std::as_bytes(std::span{buffer, 1}), 0) == 0xE934A84ADB052768u);
    }

    SECTION("Length = 1, Seed = prime32")
    {
        REQUIRE(XXHash64::FromBuffer(std::as_bytes(std::span{buffer, 1}), prime32) == 0x5014607643A9B4C3u);
    }

    SECTION("Length = 4, Seed = 0")
    {
        REQUIRE(XXHash64::FromBuffer(std::as_bytes(std::span{buffer, 4}), 0) == 0x9136A0DCA57457EEu);
    }

    SECTION("Length = 4, Seed = prime32")
    {
        REQUIRE(XXHash64::FromBuffer(std::as_bytes(std::span{buffer, 4}), prime32) == 0xCAAB286BD8E9FDB5u);
    }

    SECTION("Length = 14, Seed = 0")
    {
        REQUIRE(XXHash64::FromBuffer(std::as_bytes(std::span{buffer, 14}), 0) == 0x8282DCC4994E35C8u);
    }

    SECTION("Length = 14, Seed = prime32")
    {
        REQUIRE(XXHash64::FromBuffer(std::as_bytes(std::span{buffer, 14}), prime32) == 0xC3BD6BF63DEB6DF0u);
    }

    SECTION("Length = 222, Seed = 0")
    {
        REQUIRE(XXHash64::FromBuffer(std::as_bytes(std::span{buffer, 222}), 0) == 0xB641AE8CB691C174u);
    }

    SECTION("Length = 222, Seed = prime32")
    {
        REQUIRE(XXHash64::FromBuffer(std::as_bytes(std::span{buffer, 222}), prime32) == 0x20CB8AB7AE10C14Au);
    }
}
