#include "AnemoneRuntime/Network/IpEndPoint.hxx"

#include "Private.hxx"

#include <iterator>
#include <fmt/format.h>
#include <charconv>

namespace Anemone::Network
{
    bool IpEndPoint::ToString(std::string& result) const
    {
        Platform::NativeIpAddress const& nativeAddress = Platform::Get(this->Address.Native);

        if (std::string address{}; this->Address.ToString(address))
        {
            result.clear();
            auto out = std::back_inserter(result);

            if (nativeAddress.AddressFamily == AF_INET6)
            {
                fmt::format_to(out, "[{}]:{}", address, this->Port);
            }
            else
            {
                fmt::format_to(out, "{}:{}", address, this->Port);
            }

            return true;
        }

        return false;
    }

    bool IpEndPoint::TryParse(IpEndPoint& result, std::string_view value)
    {
        if (value.empty())
        {
            return false;
        }

        size_t address_length = value.size();
        size_t last_colon = value.find_last_of(':');

        if (last_colon != std::string_view::npos)
        {
            if (value[last_colon - 1] == ']')
            {
                address_length = last_colon;
            }
            else if (value.substr(0, last_colon).find_last_of(':') == std::string_view::npos)
            {
                address_length = last_colon;
            }
        }

        if (IpAddress::TryParse(result.Address, value.substr(0, address_length)))
        {
            uint32_t port = 0;

            if (address_length == value.size())
            {
                result.Port = port;
                return true;
            }

            auto [ptr, ec] = std::from_chars(value.data() + address_length + 1, value.data() + value.size(), port);

            if ((ec != std::errc()) or (*ptr != '\0') or (port > 65535))
            {
                return false;
            }

            result.Port = port;
            return true;
        }

        return false;
    }
}
