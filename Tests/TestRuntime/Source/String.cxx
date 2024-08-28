#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/String.hxx"

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

ANEMONE_EXTERNAL_HEADERS_END

TEST_CASE("String Split")
{
    SECTION("Callback function")
    {
        std::string_view str = "Hello, World!";
        std::vector<std::string> result;
        Anemone::Split(
            str,
            ',',
            [&result](std::string_view token)
            {
                result.push_back(std::string{token});
            });

        REQUIRE(result.size() == 2);
        REQUIRE(result[0] == "Hello");
        REQUIRE(result[1] == " World!");
    }

    SECTION("Callback function with multiple separators")
    {
        std::string_view str = "Hello, World!";
        std::vector<std::string> result;
        Anemone::Split(
            str,
            std::string_view{", "},
            [&result](std::string_view token)
            {
                result.push_back(std::string{token});
            });

        REQUIRE(result.size() == 2);
        REQUIRE(result[0] == "Hello");
        REQUIRE(result[1] == "World!");
    }

    SECTION("Enumerator")
    {
        std::string_view str = "     a    bb     ccc       dddd";
        Anemone::StringSplitEnumerator<char> enumerator{str};

        REQUIRE(enumerator.MoveNext(" ", true));
        REQUIRE(enumerator.Current() == "a");

        REQUIRE(enumerator.MoveNext(" ", true));
        REQUIRE(enumerator.Current() == "bb");

        REQUIRE(enumerator.MoveNext(" ", true));
        REQUIRE(enumerator.Current() == "ccc");

        REQUIRE(enumerator.MoveNext(" ", true));
        REQUIRE(enumerator.Current() == "dddd");

        REQUIRE_FALSE(enumerator.MoveNext(" ", true));
    }
}

TEST_CASE("String Join")
{
    std::array tokens{
        std::string_view{"a"},
        std::string_view{"bb"},
        std::string_view{"ccc"},
        std::string_view{"dddd"},
        std::string_view{"eeeee"},
    };

    std::string result{};
    Anemone::Join(result, tokens, std::string_view{", "});

    REQUIRE(result == "a, bb, ccc, dddd, eeeee");
}

TEST_CASE("Match Wildcard")
{
    SECTION("Generic")
    {
        CHECK(Anemone::MatchWildcard<char>("Hello, World!", "Hello, World!"));
        CHECK(Anemone::MatchWildcard<char>("Hello, *", "Hello, World!"));
        CHECK(Anemone::MatchWildcard<char>("*, World!", "Hello, World!"));
        CHECK(Anemone::MatchWildcard<char>("*?", "Hello, World!"));
        CHECK(Anemone::MatchWildcard<char>("?*?", "Hello, World!"));
        CHECK(Anemone::MatchWildcard<char>("?*", "Hello, World!"));
        CHECK(Anemone::MatchWildcard<char>("*?*", "Hello, World!"));
    }

    SECTION("Identity matching")
    {
        CHECK(Anemone::MatchWildcard<char>("", ""));
        CHECK(Anemone::MatchWildcard<char>("a", "a"));
        CHECK(Anemone::MatchWildcard<char>("ab", "ab"));
        CHECK(Anemone::MatchWildcard<char>("abc", "abc"));
        CHECK(Anemone::MatchWildcard<char>("abcd", "abcd"));
    }

    SECTION("Definitely not matching")
    {
        CHECK_FALSE(Anemone::MatchWildcard<char>("a", "z"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("a", ""));
        CHECK_FALSE(Anemone::MatchWildcard<char>("", "z"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("abcd", "xyzw"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("ab", "xyzw"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("abcd", "xw"));
    }

    SECTION("Matching single characters - without pattern chars")
    {
        CHECK_FALSE(Anemone::MatchWildcard<char>("?", ""));
        CHECK_FALSE(Anemone::MatchWildcard<char>("??", ""));
        CHECK_FALSE(Anemone::MatchWildcard<char>("???", ""));
        CHECK(Anemone::MatchWildcard<char>("?", "a"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("??", "a"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("???", "a"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("?", "ab"));
        CHECK(Anemone::MatchWildcard<char>("??", "ab"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("???", "ab"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("?", "abc"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("??", "abc"));
        CHECK(Anemone::MatchWildcard<char>("???", "abc"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("?", "abcd"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("??", "abcd"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("???", "abcd"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("?", "abcde"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("??", "abcde"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("???", "abcde"));
    }

    SECTION("Matching single characters - with pattern chars")
    {
        CHECK_FALSE(Anemone::MatchWildcard<char>("a?cd", ""));
        CHECK_FALSE(Anemone::MatchWildcard<char>("a?c?", ""));
        CHECK_FALSE(Anemone::MatchWildcard<char>("?b??", ""));
        CHECK_FALSE(Anemone::MatchWildcard<char>("a?cd", "a"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("??cd", "a"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("??c?", "a"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("a?cd", "ab"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("?b?d", "ab"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("?b??", "ab"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("ab?d", "abc"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("?b?d", "abc"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("??c?", "abc"));
        CHECK(Anemone::MatchWildcard<char>("?bcd", "abcd"));
        CHECK(Anemone::MatchWildcard<char>("?b?d", "abcd"));
        CHECK(Anemone::MatchWildcard<char>("?b??", "abcd"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("?bcd", "abcde"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("?b?d", "abcde"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("?b??", "abcde"));
    }

    SECTION("Matching single characters - without pattern chars")
    {
        CHECK(Anemone::MatchWildcard<char>("*", ""));
        CHECK(Anemone::MatchWildcard<char>("**", ""));
        CHECK(Anemone::MatchWildcard<char>("***", ""));
        CHECK(Anemone::MatchWildcard<char>("*", "a"));
        CHECK(Anemone::MatchWildcard<char>("**", "a"));
        CHECK(Anemone::MatchWildcard<char>("***", "a"));
        CHECK(Anemone::MatchWildcard<char>("*", "ab"));
        CHECK(Anemone::MatchWildcard<char>("**", "ab"));
        CHECK(Anemone::MatchWildcard<char>("***", "ab"));
        CHECK(Anemone::MatchWildcard<char>("*", "abc"));
        CHECK(Anemone::MatchWildcard<char>("**", "abc"));
        CHECK(Anemone::MatchWildcard<char>("***", "abc"));
        CHECK(Anemone::MatchWildcard<char>("*", "abcd"));
        CHECK(Anemone::MatchWildcard<char>("**", "abcd"));
        CHECK(Anemone::MatchWildcard<char>("***", "abcd"));
        CHECK(Anemone::MatchWildcard<char>("*", "abcde"));
        CHECK(Anemone::MatchWildcard<char>("**", "abcde"));
        CHECK(Anemone::MatchWildcard<char>("***", "abcde"));
    }

    SECTION("Matching single characters - with pattern chars")
    {
        CHECK_FALSE(Anemone::MatchWildcard<char>("a*cd", ""));
        CHECK_FALSE(Anemone::MatchWildcard<char>("a*c*", ""));
        CHECK_FALSE(Anemone::MatchWildcard<char>("*b**", ""));
        CHECK_FALSE(Anemone::MatchWildcard<char>("a*cd", "a"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("**cd", "a"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("**c*", "a"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("a*cd", "ab"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("*b*d", "ab"));
        CHECK(Anemone::MatchWildcard<char>("*b**", "ab"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("ab*d", "abc"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("*b*d", "abc"));
        CHECK(Anemone::MatchWildcard<char>("**c*", "abc"));
        CHECK(Anemone::MatchWildcard<char>("*bcd", "abcd"));
        CHECK(Anemone::MatchWildcard<char>("*b*d", "abcd"));
        CHECK(Anemone::MatchWildcard<char>("*b**", "abcd"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("*bcd", "abcde"));
        CHECK_FALSE(Anemone::MatchWildcard<char>("*b*d", "abcde"));
        CHECK(Anemone::MatchWildcard<char>("*b**", "abcde"));
    }
}
