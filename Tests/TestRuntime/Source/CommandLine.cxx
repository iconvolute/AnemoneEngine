#include <catch_amalgamated.hpp>

#include "AnemoneRuntime/CommandLine.hxx"

TEST_CASE("Command Line - From String")
{
    using namespace Anemone;

    SECTION("Empty string")
    {
        CommandlineParser parser{};

        parser.Parse("");

        CHECK(parser.GetPositional().empty());
        CHECK(parser.GetNamed().empty());
    }

    SECTION("Positional only")
    {
        CommandlineParser parser{};

        parser.Parse("positional1 positional2 positional3");

        auto positional = parser.GetPositional();

        REQUIRE(positional.size() == 3);
        CHECK(positional[0] == "positional1");
        CHECK(positional[1] == "positional2");
        CHECK(positional[2] == "positional3");

        CHECK(parser.GetNamed().empty());
    }

    SECTION("Named only")
    {
        CommandlineParser parser{};

        parser.Parse("--name1=value1 --name2=value2 --name3=value3 --flag");

        auto named = parser.GetNamed();

        REQUIRE(named.size() == 4);
        CHECK(named[0].first == "name1");
        CHECK(named[0].second == "value1");
        CHECK(named[1].first == "name2");
        CHECK(named[1].second == "value2");
        CHECK(named[2].first == "name3");
        CHECK(named[2].second == "value3");
        CHECK(named[3].first == "flag");
        CHECK(named[3].second.empty());

        CHECK(parser.GetPositional().empty());
    }

    SECTION("Mixed 1")
    {
        CommandlineParser parser{};

        parser.Parse("positional1 --name1=value1 positional2 --name2=value2 positional3 --name3=value3");

        auto positional = parser.GetPositional();
        auto named = parser.GetNamed();

        REQUIRE(positional.size() == 3);
        CHECK(positional[0] == "positional1");
        CHECK(positional[1] == "positional2");
        CHECK(positional[2] == "positional3");

        REQUIRE(named.size() == 3);
        CHECK(named[0].first == "name1");
        CHECK(named[0].second == "value1");
        CHECK(named[1].first == "name2");
        CHECK(named[1].second == "value2");
        CHECK(named[2].first == "name3");
        CHECK(named[2].second == "value3");
    }

    SECTION("Mixed 2")
    {
        CommandlineParser parser{};

        parser.Parse("--name1=value1 positional1 --name2=value2 \"quoted\" --name3=value3 positional2");

        auto positional = parser.GetPositional();
        auto named = parser.GetNamed();

        REQUIRE(positional.size() == 3);
        CHECK(positional[0] == "positional1");
        CHECK(positional[1] == "quoted");
        CHECK(positional[2] == "positional2");

        REQUIRE(named.size() == 3);
        CHECK(named[0].first == "name1");
        CHECK(named[0].second == "value1");
        CHECK(named[1].first == "name2");
        CHECK(named[1].second == "value2");
        CHECK(named[2].first == "name3");
        CHECK(named[2].second == "value3");
    }

    SECTION("Quoted at end")
    {
        CommandlineParser parser{};
        parser.Parse("positional1 --name1=value1 positional2 --name2=value2 \"quoted\"");

        auto positional = parser.GetPositional();
        auto named = parser.GetNamed();

        REQUIRE(positional.size() == 3);
        CHECK(positional[0] == "positional1");
        CHECK(positional[1] == "positional2");
        CHECK(positional[2] == "quoted");

        REQUIRE(named.size() == 2);
        CHECK(named[0].first == "name1");
        CHECK(named[0].second == "value1");
        CHECK(named[1].first == "name2");
        CHECK(named[1].second == "value2");
    }

    SECTION("Unterminated quoted at end")
    {
        CommandlineParser parser{};
        parser.Parse("positional1 --name1=value1 positional2 --name2=value2 \"quoted");

        auto positional = parser.GetPositional();
        auto named = parser.GetNamed();

        REQUIRE(positional.size() == 3);
        CHECK(positional[0] == "positional1");
        CHECK(positional[1] == "positional2");
        CHECK(positional[2] == "quoted");

        REQUIRE(named.size() == 2);
        CHECK(named[0].first == "name1");
        CHECK(named[0].second == "value1");
        CHECK(named[1].first == "name2");
        CHECK(named[1].second == "value2");
    }

    SECTION("Quoted named parameter value")
    {
        CommandlineParser parser{};
        parser.Parse("positional1 --name1=\"value1\" positional2 --name2=\"value of name2\" \"unterminated");

        auto positional = parser.GetPositional();
        auto named = parser.GetNamed();

        REQUIRE(positional.size() == 3);
        CHECK(positional[0] == "positional1");
        CHECK(positional[1] == "positional2");
        CHECK(positional[2] == "unterminated");

        REQUIRE(named.size() == 2);
        CHECK(named[0].first == "name1");
        CHECK(named[0].second == "value1");
        CHECK(named[1].first == "name2");
        CHECK(named[1].second == "value of name2");
    }
}

TEST_CASE("Command Line - From Argc/Argv")
{
    using namespace Anemone;

    SECTION("Empty string")
    {
        CommandlineParser parser{};

        int const argc = 0;
        const char* const argv[] = {""};

        parser.Parse(argc, argv);

        CHECK(parser.GetPositional().empty());
        CHECK(parser.GetNamed().empty());
    }

    SECTION("Positional only")
    {
        CommandlineParser parser{};

        int const argc = 3;
        const char* const argv[] = {
            "positional1",
            "positional2",
            "positional3",
        };

        parser.Parse(argc, argv);

        auto positional = parser.GetPositional();

        REQUIRE(positional.size() == 3);
        CHECK(positional[0] == "positional1");
        CHECK(positional[1] == "positional2");
        CHECK(positional[2] == "positional3");

        CHECK(parser.GetNamed().empty());
    }

    SECTION("Named only")
    {
        CommandlineParser parser{};

        int const argc = 4;
        const char* const argv[] = {
            "--name1=value1",
            "-name2=value2",
            "--name3:value3",
            "--flag",
        };

        parser.Parse(argc, argv);

        auto named = parser.GetNamed();

        REQUIRE(named.size() == 4);
        CHECK(named[0].first == "name1");
        CHECK(named[0].second == "value1");
        CHECK(named[1].first == "name2");
        CHECK(named[1].second == "value2");
        CHECK(named[2].first == "name3");
        CHECK(named[2].second == "value3");
        CHECK(named[3].first == "flag");
        CHECK(named[3].second.empty());

        CHECK(parser.GetPositional().empty());
    }

    SECTION("Mixed 1")
    {
        CommandlineParser parser{};

        int const argc = 6;
        const char* const argv[] = {
            "positional1",
            "--name1=value1",
            "positional2",
            "--name2=value2",
            "positional3",
            "--name3=value3",
        };

        parser.Parse(argc, argv);

        auto positional = parser.GetPositional();
        auto named = parser.GetNamed();

        REQUIRE(positional.size() == 3);
        CHECK(positional[0] == "positional1");
        CHECK(positional[1] == "positional2");
        CHECK(positional[2] == "positional3");

        REQUIRE(named.size() == 3);
        CHECK(named[0].first == "name1");
        CHECK(named[0].second == "value1");
        CHECK(named[1].first == "name2");
        CHECK(named[1].second == "value2");
        CHECK(named[2].first == "name3");
        CHECK(named[2].second == "value3");
    }

    SECTION("Mixed 2")
    {
        CommandlineParser parser{};

        int const argc = 6;
        const char* const argv[] = {
            "--name1=value1",
            "positional1",
            "--name2=value2",
            "quoted",
            "--name3=value3",
            "positional2",
        };

        parser.Parse(argc, argv);

        auto positional = parser.GetPositional();
        auto named = parser.GetNamed();

        REQUIRE(positional.size() == 3);
        CHECK(positional[0] == "positional1");
        CHECK(positional[1] == "quoted");
        CHECK(positional[2] == "positional2");

        REQUIRE(named.size() == 3);
        CHECK(named[0].first == "name1");
        CHECK(named[0].second == "value1");
        CHECK(named[1].first == "name2");
        CHECK(named[1].second == "value2");
        CHECK(named[2].first == "name3");
        CHECK(named[2].second == "value3");
    }
}
