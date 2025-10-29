#include <catch_amalgamated.hpp>

#include "AnemoneBase/Flags.hxx"

enum class TestFlag : uint32_t
{
    None = 0u,
    A = 1u << 0,
    B = 1u << 1,
    C = 1u << 2,
    D = 1u << 3,
};

TEST_CASE("Flags")
{
    Anemone::Flags<TestFlag> flag{};
    REQUIRE(flag.Value == TestFlag::None);

    flag |= TestFlag::A;
    REQUIRE(flag.Value == TestFlag::A);

    flag |= TestFlag::B;
    REQUIRE(flag.All(TestFlag::A));
}
