#include <catch_amalgamated.hpp>
#if false
#include "AnemoneBase/Network/IpAddress.hxx"

TEST_CASE("IPAddress parsing - String conversion")
{
    using namespace Anemone::Network;

    IpAddress address{};
    std::string as_string{};

    SECTION("TryParse")
    {
        SECTION("Empty string")
        {
            REQUIRE_FALSE(IpAddress::TryParse(address, ""));
        }

        SECTION("Single digit")
        {
            REQUIRE_FALSE(IpAddress::TryParse(address, "1"));
        }

        SECTION("Valid IPv4 address")
        {
            REQUIRE(IpAddress::TryParse(address, "192.168.0.1"));
            REQUIRE(address.Family() == AddressFamily::IPv4);
            REQUIRE(address.ToString(as_string));
            REQUIRE(as_string == "192.168.0.1");
        }

        SECTION("Valid IPv4 address with port")
        {
            REQUIRE_FALSE(IpAddress::TryParse(address, "192.168.0.1:2137"));
        }

        SECTION("Valid IPv4 address with port and scope id")
        {
            REQUIRE_FALSE(IpAddress::TryParse(address, "192.168.0.1%1337:2137"));
        }

        SECTION("Valid IPv6 address")
        {
            REQUIRE(IpAddress::TryParse(address, "2001:0db8:85a3:0000:0000:8a2e:0370:7334"));
            REQUIRE(address.Family() == AddressFamily::IPv6);
            REQUIRE(address.ToString(as_string));
            REQUIRE(as_string == "2001:db8:85a3::8a2e:370:7334");
        }

        SECTION("Valid IPv6 address (with brackets)")
        {
            REQUIRE(IpAddress::TryParse(address, "[2001:0db8:85a3:0000:0000:8a2e:0370:7334]"));
            REQUIRE(address.Family() == AddressFamily::IPv6);
            REQUIRE(address.ToString(as_string));
            REQUIRE(as_string == "2001:db8:85a3::8a2e:370:7334");
        }

        SECTION("Valid IPv6 address with port (with brackets)")
        {
            REQUIRE_FALSE(IpAddress::TryParse(address, "[2001:0db8:85a3:0000:0000:8a2e:0370:7334]:2137"));
        }

        SECTION("Valid IPv6 address with port and scope id (with brackets)")
        {
            REQUIRE_FALSE(IpAddress::TryParse(address, "[2001:0db8:85a3:0000:0000:8a2e:0370:7334%65535]:2137"));
        }

        SECTION("Valid short IPv6")
        {
            REQUIRE(IpAddress::TryParse(address, "2001::7334"));
            REQUIRE(address.Family() == AddressFamily::IPv6);
            REQUIRE(address.ToString(as_string));
            REQUIRE(as_string == "2001::7334");
        }
    }
}
#endif
