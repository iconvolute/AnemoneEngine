#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Network/IpAddress.hxx"

#include <cstdint>

namespace Anemone::Network
{
    class RUNTIME_API IpEndPoint final
    {
    public:
        IpAddress Address{};
        uint32_t Port{};

    public:
        IpEndPoint() = default;
        IpEndPoint(IpEndPoint const& other) = default;
        IpEndPoint(IpEndPoint&& other) noexcept = default;
        IpEndPoint& operator=(IpEndPoint const& other) = default;
        IpEndPoint& operator=(IpEndPoint&& other) noexcept = default;

        IpEndPoint(IpAddress const& address, uint32_t port)
            : Address{address}
            , Port{port}
        {
        }

    public:
        [[nodiscard]] bool operator==(IpEndPoint const&) const = default;
        [[nodiscard]] bool operator!=(IpEndPoint const&) const = default;

        bool ToString(std::string& result) const;

        static bool TryParse(IpEndPoint& result, std::string_view value);
    };
}
