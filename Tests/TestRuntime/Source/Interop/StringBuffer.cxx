#include "AnemoneRuntime/Interop/StringBuffer.hxx"

#include <catch_amalgamated.hpp>
using namespace Anemone::Interop;

TEST_CASE("string_buffer basic functionality")
{
    SECTION("Default constructor creates empty buffer")
    {
        string_buffer<char, 10> buffer{};
        REQUIRE(buffer.size() == 0);
        REQUIRE(buffer.capacity() == 10);
        REQUIRE(buffer.is_dynamic() == false);
        REQUIRE(buffer.as_view() == "");
    }

    SECTION("Constructor from string_view")
    {
        string_buffer<char, 10> buffer(std::string_view("test"));
        REQUIRE(buffer.size() == 4);
        REQUIRE(buffer.capacity() == 10);
        REQUIRE(buffer.is_dynamic() == false);
        REQUIRE(buffer.as_view() == "test");
    }

    SECTION("Constructor from char* and length")
    {
        const char* str = "test";
        string_buffer<char, 10> buffer(str, 4);
        REQUIRE(buffer.size() == 4);
        REQUIRE(buffer.capacity() == 10);
        REQUIRE(buffer.is_dynamic() == false);
        REQUIRE(buffer.as_view() == "test");
    }
}

TEST_CASE("string_buffer resize operations")
{
    SECTION("resize_for_override with smaller size")
    {
        string_buffer<char, 10> buffer(std::string_view("test"));
        buffer.resize_for_override(2);
        REQUIRE(buffer.size() == 2);
        REQUIRE(buffer.capacity() == 10);
        // Note: after resize_for_override, original content not specified to be preserved
    }

    SECTION("resize_for_override with larger size within static capacity")
    {
        string_buffer<char, 10> buffer(std::string_view("test"));
        buffer.resize_for_override(8);
        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer.capacity() == 10);
        REQUIRE(buffer.is_dynamic() == false);
    }

    SECTION("resize_for_override with size larger than static capacity")
    {
        string_buffer<char, 10> buffer(std::string_view("test"));
        buffer.resize_for_override(15);
        REQUIRE(buffer.size() == 15);
        REQUIRE(buffer.capacity() >= 15);
        REQUIRE(buffer.is_dynamic() == true);
    }

    SECTION("resize with same size")
    {
        string_buffer<char, 10> buffer(std::string_view("test"));
        buffer.resize(4);
        REQUIRE(buffer.size() == 4);
        REQUIRE(buffer.as_view() == "test");
    }

    SECTION("resize with smaller size")
    {
        string_buffer<char, 10> buffer(std::string_view("test"));
        buffer.resize(2);
        REQUIRE(buffer.size() == 2);
        REQUIRE(buffer.as_view() == "te");
    }

    SECTION("resize with larger size within capacity")
    {
        string_buffer<char, 10> buffer(std::string_view("test"));
        buffer.resize(8, 'x');
        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer.capacity() == 10);
        REQUIRE(buffer.as_view() == "testxxxx");
    }

    SECTION("resize with larger size beyond capacity")
    {
        string_buffer<char, 10> buffer(std::string_view("test"));
        buffer.resize(15, 'x');
        REQUIRE(buffer.size() == 15);
        REQUIRE(buffer.capacity() >= 15);
        REQUIRE(buffer.is_dynamic() == true);
        REQUIRE(buffer.as_view() == "testxxxxxxxxxxx");
    }

    SECTION("resize to zero size")
    {
        string_buffer<char, 10> buffer(std::string_view("test"));
        buffer.resize(0);
        REQUIRE(buffer.size() == 0);
        REQUIRE(buffer.as_view() == "");
    }

    SECTION("trim operation")
    {
        string_buffer<char, 10> buffer(std::string_view("test"));
        buffer.trim(2);
        REQUIRE(buffer.size() == 2);
        REQUIRE(buffer.as_view() == "te");
    }
}

TEST_CASE("string_buffer edge cases")
{
    SECTION("Empty string")
    {
        string_buffer<char, 10> buffer(std::string_view(""));
        REQUIRE(buffer.size() == 0);
        REQUIRE(buffer.capacity() == 10);
        REQUIRE(buffer.is_dynamic() == false);
        REQUIRE(buffer.as_view() == "");
    }

    SECTION("String exactly at static capacity")
    {
        string_buffer<char, 10> buffer(std::string_view("1234567890"));
        REQUIRE(buffer.size() == 10);
        REQUIRE(buffer.capacity() == 10);
        REQUIRE(buffer.is_dynamic() == false);
        REQUIRE(buffer.as_view() == "1234567890");
    }

    SECTION("String just over static capacity")
    {
        string_buffer<char, 10> buffer(std::string_view("12345678901"));
        REQUIRE(buffer.size() == 11);
        REQUIRE(buffer.capacity() >= 11);
        REQUIRE(buffer.is_dynamic() == true);
        REQUIRE(buffer.as_view() == "12345678901");
    }

    SECTION("Very large string")
    {
        std::string largeString(1000, 'a');
        string_buffer<char, 10> buffer{std::string_view(largeString)};

        REQUIRE(buffer.size() == 1000);
        REQUIRE(buffer.capacity() >= 1000);
        REQUIRE(buffer.is_dynamic() == true);
        REQUIRE(buffer.as_view() == largeString);
    }
}

TEST_CASE("adapt_string_buffer function")
{
    SECTION("Successful adaptation within capacity")
    {
        string_buffer<char, 10> buffer;

        auto callback = [](std::span<char> span, size_t& requiredSize)
        {
            const char* str = "test";
            std::copy(str, str + 4, span.data());
            requiredSize = 4;
            return true;
        };

        bool result = adapt_string_buffer(buffer, callback);

        REQUIRE(result == true);
        REQUIRE(buffer.size() == 4);
        REQUIRE(buffer.as_view() == "test");
    }

    SECTION("Adaptation requiring resize")
    {
        string_buffer<char, 10> buffer;

        int callCount = 0;
        auto callback = [&callCount](std::span<char> span, size_t& requiredSize)
        {
            callCount++;
            if (callCount == 1)
            {
                // First call indicates we need more space
                requiredSize = 15;
                return true;
            }
            else
            {
                // Second call fills the resized buffer
                const char* str = "expanded buffer";
                std::copy(str, str + 15, span.data());
                requiredSize = 15;
                return true;
            }
        };

        bool result = adapt_string_buffer(buffer, callback);

        REQUIRE(result == true);
        REQUIRE(callCount == 2);
        REQUIRE(buffer.size() == 15);
        REQUIRE(buffer.as_view() == "expanded buffer");
    }

    SECTION("Failed adaptation")
    {
        string_buffer<char, 10> buffer(std::string_view("test"));

        auto callback = [](std::span<char>, size_t&)
        {
            return false; // Indicate failure
        };

        bool result = adapt_string_buffer(buffer, callback);

        REQUIRE(result == false);
        REQUIRE(buffer.size() == 0); // Buffer should be empty after failure
        REQUIRE(buffer.as_view() == "");
    }
}
