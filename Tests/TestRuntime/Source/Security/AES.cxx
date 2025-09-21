#include "AnemoneRuntime/Security/AES.hxx"

#include <catch_amalgamated.hpp>

TEST_CASE("Cryptography AES")
{
    using namespace Anemone;

    Anemone::AES aes{};

    std::array const key{
        std::byte{0x00}, std::byte{0x01}, std::byte{0x02}, std::byte{0x03},
        std::byte{0x04}, std::byte{0x05}, std::byte{0x06}, std::byte{0x07},
        std::byte{0x08}, std::byte{0x09}, std::byte{0x0A}, std::byte{0x0B},
        std::byte{0x0C}, std::byte{0x0D}, std::byte{0x0E}, std::byte{0x0F}};

    std::array const input{
        std::byte{0x00}, std::byte{0x01}, std::byte{0x02}, std::byte{0x03},
        std::byte{0x04}, std::byte{0x05}, std::byte{0x06}, std::byte{0x07},
        std::byte{0x08}, std::byte{0x09}, std::byte{0x0A}, std::byte{0x0B},
        std::byte{0x0C}, std::byte{0x0D}, std::byte{0x0E}, std::byte{0x0F}};

    REQUIRE(aes.Initialize(key) == Error::Success);

    std::array<std::byte, 16> encrypted{};
    REQUIRE(aes.Encrypt(input, encrypted) == Error::Success);

    std::array<std::byte, 16> decrypted{};
    REQUIRE(aes.Decrypt(encrypted, decrypted) == Error::Success);

    REQUIRE(input == decrypted);
}
