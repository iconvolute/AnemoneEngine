#include "AnemoneRuntime/Network/IpEndPoint.hxx"
#include "AnemoneRuntime/Network/IpAddress.hxx"

#include <catch_amalgamated.hpp>
#if false

TEST_CASE("IpEndPoint parsing - String conversion")
{
    using namespace Anemone::Network;

    IpEndPoint address{};
    std::string as_string{};

    SECTION("TryParse")
    {
        SECTION("Empty string")
        {
            REQUIRE_FALSE(IpEndPoint::TryParse(address, ""));
        }

        SECTION("Single digit")
        {
            REQUIRE_FALSE(IpEndPoint::TryParse(address, "1"));
        }

        SECTION("Valid IPv4 address")
        {
            REQUIRE(IpEndPoint::TryParse(address, "192.168.0.1"));
            REQUIRE(address.Address.Family() == AddressFamily::IPv4);
            REQUIRE(address.ToString(as_string));
            REQUIRE(address.Port == 0);
            REQUIRE(address.Address.ScopeId() == 0);
            REQUIRE(as_string == "192.168.0.1:0");
        }

        SECTION("Valid IPv4 address with port")
        {
            REQUIRE(IpEndPoint::TryParse(address, "192.168.0.1:2137"));
            REQUIRE(address.Address.Family() == AddressFamily::IPv4);
            REQUIRE(address.ToString(as_string));
            REQUIRE(address.Port == 2137);
            REQUIRE(address.Address.ScopeId() == 0);
            REQUIRE(as_string == "192.168.0.1:2137");
        }

        SECTION("Valid IPv4 address with port and scope id")
        {
            REQUIRE_FALSE(IpEndPoint::TryParse(address, "192.168.0.1%1337:2137"));
        }

        SECTION("Valid IPv6 address")
        {
            REQUIRE(IpEndPoint::TryParse(address, "2001:0db8:85a3:0000:0000:8a2e:0370:7334"));
            REQUIRE(address.Address.Family() == AddressFamily::IPv6);
            REQUIRE(address.ToString(as_string));
            REQUIRE(address.Port == 0);
            REQUIRE(address.Address.ScopeId() == 0);
            REQUIRE(as_string == "[2001:db8:85a3::8a2e:370:7334]:0");
        }

        SECTION("Valid IPv6 address (with brackets)")
        {
            REQUIRE(IpEndPoint::TryParse(address, "[2001:0db8:85a3:0000:0000:8a2e:0370:7334]"));
            REQUIRE(address.Address.Family() == AddressFamily::IPv6);
            REQUIRE(address.ToString(as_string));
            REQUIRE(address.Port == 0);
            REQUIRE(address.Address.ScopeId() == 0);
            REQUIRE(as_string == "[2001:db8:85a3::8a2e:370:7334]:0");
        }

        SECTION("Valid IPv6 address with port (with brackets)")
        {
            REQUIRE(IpEndPoint::TryParse(address, "[2001:0db8:85a3:0000:0000:8a2e:0370:7334]:2137"));
            REQUIRE(address.Address.Family() == AddressFamily::IPv6);
            REQUIRE(address.ToString(as_string));
            REQUIRE(address.Port == 2137);
            REQUIRE(address.Address.ScopeId() == 0);
            REQUIRE(as_string == "[2001:db8:85a3::8a2e:370:7334]:2137");
        }

        SECTION("Valid IPv6 address with port and scope id (with brackets)")
        {
            REQUIRE(IpEndPoint::TryParse(address, "[2001:0db8:85a3:0000:0000:8a2e:0370:7334%65535]:2137"));
            REQUIRE(address.Address.Family() == AddressFamily::IPv6);
            REQUIRE(address.ToString(as_string));
            REQUIRE(address.Port == 2137);
            REQUIRE(address.Address.ScopeId() == 65535);
            REQUIRE(as_string == "[2001:db8:85a3::8a2e:370:7334%65535]:2137");
        }

        SECTION("Valid short IPv6")
        {
            REQUIRE(IpEndPoint::TryParse(address, "2001::7334"));
            REQUIRE(address.Address.Family() == AddressFamily::IPv6);
            REQUIRE(address.ToString(as_string));
            REQUIRE(address.Port == 0);
            REQUIRE(address.Address.ScopeId() == 0);
            REQUIRE(as_string == "[2001::7334]:0");
        }
    }
}
#endif
