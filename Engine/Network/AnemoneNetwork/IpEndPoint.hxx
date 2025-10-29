#pragma once
#include "AnemoneInterop/Headers.hxx"
#include "AnemoneNetwork/IpAddress.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneInterop/Windows/Headers.hxx"
#elif ANEMONE_PLATFORM_LINUX || ANEMONE_PLATFORM_ANDROID
#include "AnemoneInterop/Linux/Headers.hxx"
#else
#error "Unsupported platform"
#endif

namespace Anemone::Network
{
    class NETWORK_API IpEndPoint final
    {
    public:
        union Native final
        {
            sockaddr header;
            sockaddr_in v4;
            sockaddr_in6 v6;
        };

    private:
        Native m_native;

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
