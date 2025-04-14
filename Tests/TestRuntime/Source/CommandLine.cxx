#include <catch_amalgamated.hpp>

#include "AnemoneRuntime/System/CommandLine.hxx"

TEST_CASE("Command Line - Parsing")
{
    using namespace Anemone;

    SECTION("Empty")
    {
        std::vector<std::string_view> positional{};
        std::vector<std::string_view> options{};
        std::vector<std::pair<std::string_view, std::string_view>> params{};

        CommandLine::Parse("", positional, options, params);

        CHECK(positional.empty());
        CHECK(options.empty());
        CHECK(params.empty());
    }

    SECTION("Positional only")
    {
        std::vector<std::string_view> positional{};
        std::vector<std::string_view> options{};
        std::vector<std::pair<std::string_view, std::string_view>> params{};

        CommandLine::Parse("positional1 positional2 positional3", positional, options, params);

        REQUIRE(positional.size() == 3);
        CHECK(positional[0] == "positional1");
        CHECK(positional[1] == "positional2");
        CHECK(positional[2] == "positional3");

        CHECK(options.empty());
        CHECK(params.empty());
    }

    SECTION("Named only")
    {
        std::vector<std::string_view> positional{};
        std::vector<std::string_view> options{};
        std::vector<std::pair<std::string_view, std::string_view>> params{};

        CommandLine::Parse("--name1=value1 --name2=value2 --name3=value3 --flag", positional, options, params);

        CHECK(positional.empty());

        REQUIRE(options.size() == 1);
        CHECK(options[0] == "flag");

        REQUIRE(params.size() == 3);
        CHECK(params[0].first == "name1");
        CHECK(params[0].second == "value1");
        CHECK(params[1].first == "name2");
        CHECK(params[1].second == "value2");
        CHECK(params[2].first == "name3");
        CHECK(params[2].second == "value3");
    }

    SECTION("Mixed 1")
    {
        std::vector<std::string_view> positional{};
        std::vector<std::string_view> options{};
        std::vector<std::pair<std::string_view, std::string_view>> params{};

        CommandLine::Parse("positional1 --name1=value1 positional2 -flag --name2=value2 positional3 --name3=value3", positional, options, params);

        REQUIRE(positional.size() == 3);
        CHECK(positional[0] == "positional1");
        CHECK(positional[1] == "positional2");
        CHECK(positional[2] == "positional3");

        CHECK(options.size() == 1);
        CHECK(options[0] == "flag");

        REQUIRE(params.size() == 3);
        CHECK(params[0].first == "name1");
        CHECK(params[0].second == "value1");
        CHECK(params[1].first == "name2");
        CHECK(params[1].second == "value2");
        CHECK(params[2].first == "name3");
        CHECK(params[2].second == "value3");
    }

    SECTION("Mixed 2")
    {
        std::vector<std::string_view> positional{};
        std::vector<std::string_view> options{};
        std::vector<std::pair<std::string_view, std::string_view>> params{};

        CommandLine::Parse("--name1=value1 positional1 --name2=value2 \"quoted\" --name3=value3 positional2", positional, options, params);

        REQUIRE(positional.size() == 3);
        CHECK(positional[0] == "positional1");
        CHECK(positional[1] == "quoted");
        CHECK(positional[2] == "positional2");

        CHECK(options.empty());

        REQUIRE(params.size() == 3);
        CHECK(params[0].first == "name1");
        CHECK(params[0].second == "value1");
        CHECK(params[1].first == "name2");
        CHECK(params[1].second == "value2");
        CHECK(params[2].first == "name3");
        CHECK(params[2].second == "value3");
    }

    SECTION("Quoted at end")
    {
        std::vector<std::string_view> positional{};
        std::vector<std::string_view> options{};
        std::vector<std::pair<std::string_view, std::string_view>> params{};

        CommandLine::Parse("positional1 --name1=value1 positional2 --name2=value2 \"quoted\"", positional, options, params);

        REQUIRE(positional.size() == 3);
        CHECK(positional[0] == "positional1");
        CHECK(positional[1] == "positional2");
        CHECK(positional[2] == "quoted");

        CHECK(options.empty());

        REQUIRE(params.size() == 2);
        CHECK(params[0].first == "name1");
        CHECK(params[0].second == "value1");
        CHECK(params[1].first == "name2");
        CHECK(params[1].second == "value2");
    }

    SECTION("Unterminated quoted at end")
    {
        std::vector<std::string_view> positional{};
        std::vector<std::string_view> options{};
        std::vector<std::pair<std::string_view, std::string_view>> params{};

        CommandLine::Parse("positional1 --name1=value1 positional2 --name2=value2 \"quoted", positional, options, params);

        REQUIRE(positional.size() == 3);
        CHECK(positional[0] == "positional1");
        CHECK(positional[1] == "positional2");
        CHECK(positional[2] == "quoted");

        CHECK(options.empty());

        REQUIRE(params.size() == 2);
        CHECK(params[0].first == "name1");
        CHECK(params[0].second == "value1");
        CHECK(params[1].first == "name2");
        CHECK(params[1].second == "value2");
    }

    SECTION("Quoted named parameter value")
    {
        std::vector<std::string_view> positional{};
        std::vector<std::string_view> options{};
        std::vector<std::pair<std::string_view, std::string_view>> params{};

        CommandLine::Parse("positional1 --name1=\"value of 1\" positional2 --name2=\"value of name2\" \"unterminated", positional, options, params);

        REQUIRE(positional.size() == 3);
        CHECK(positional[0] == "positional1");
        CHECK(positional[1] == "positional2");
        CHECK(positional[2] == "unterminated");

        CHECK(options.empty());

        REQUIRE(params.size() == 2);
        CHECK(params[0].first == "name1");
        CHECK(params[0].second == "value of 1");
        CHECK(params[1].first == "name2");
        CHECK(params[1].second == "value of name2");
    }

    SECTION("Unterminated quoted named parameter")
    {
        std::vector<std::string_view> positional{};
        std::vector<std::string_view> options{};
        std::vector<std::pair<std::string_view, std::string_view>> params{};

        CommandLine::Parse("positional1 --name1=\"value of 1 positional2 --name2=\"value of name2\" \"unterminated", positional, options, params);

        REQUIRE(positional.size() == 5);
        CHECK(positional[0] == "positional1");
        CHECK(positional[1] == "value");
        CHECK(positional[2] == "of");
        CHECK(positional[3] == "name2\" \"");
        CHECK(positional[4] == "unterminated");

        REQUIRE(options.empty());
        REQUIRE(params.size() == 1);
        CHECK(params[0].first == "name1");
        CHECK(params[0].second == "value of 1 positional2 --name2=");
    }

    SECTION("Positional with quote")
    {
        std::vector<std::string_view> positional{};
        std::vector<std::string_view> options{};
        std::vector<std::pair<std::string_view, std::string_view>> params{};

        CommandLine::Parse(R"(positional1"with \"terminated\" quote")", positional, options, params);

        REQUIRE(positional.size() == 1);
        CHECK(positional[0] == R"(positional1"with \"terminated\" quote")");
        CHECK(options.empty());
        CHECK(params.empty());
    }
}
