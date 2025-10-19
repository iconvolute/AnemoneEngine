#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Platform/Types.hxx"
#include "AnemoneNetwork/IpAddress.hxx"

namespace Anemone::Network
{
    class NETWORK_API IpEndPoint final
    {
    private:
        Interop::NativeIpEndPoint m_native;

    public:
        IpEndPoint() = default;
        IpEndPoint(IpEndPoint const& other) = default;
        IpEndPoint(IpEndPoint&& other) noexcept = default;
        IpEndPoint& operator=(IpEndPoint const& other) = default;
        IpEndPoint& operator=(IpEndPoint&& other) noexcept = default;
        ~IpEndPoint() = default;

        anemone_noinline IpEndPoint(IpAddress const& address, uint16_t port);

    public:
        [[nodiscard]] bool operator==(IpEndPoint const& other) const;
        [[nodiscard]] bool operator!=(IpEndPoint const& other) const
        {
            return !(*this == other);
        }

    public:
        std::optional<IpAddress> GetAddress() const;

        std::optional<uint16_t> GetPort() const;
    };
}
