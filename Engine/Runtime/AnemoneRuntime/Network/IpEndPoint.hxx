#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Platform/UninitializedStorage.hxx"
#include "AnemoneRuntime/Network/IpAddress.hxx"

namespace Anemone::Platform
{
#if ANEMONE_PLATFORM_WINDOWS
    using NativeIpEndPointStorage = UninitializedStorage<struct NativeIpEndPoint, 32, 8>;
#elif ANEMONE_PLATFORM_LINUX
    using NativeIpEndPointStorage = UninitializedStorage<struct NativeIpEndPoint, 32, 8>;
#elif ANEMONE_PLATFORM_ANDROID
    using NativeIpEndPointStorage = UninitializedStorage<struct NativeIpEndPoint, 32, 8>;
#else
#error "Not implemented"
#endif
}

namespace Anemone::Network
{
    class RUNTIME_API IpEndPoint final
    {
    private:
        Platform::NativeIpEndPointStorage m_native;

    public:
        IpEndPoint();
        IpEndPoint(IpEndPoint const& other);
        IpEndPoint(IpEndPoint&& other) noexcept;
        IpEndPoint& operator=(IpEndPoint const& other);
        IpEndPoint& operator=(IpEndPoint&& other) noexcept;
        ~IpEndPoint();

        IpEndPoint(IpAddress const& address, uint16_t port);

    public:
        [[nodiscard]] bool operator==(IpEndPoint const& other) const;
        [[nodiscard]] bool operator!=(IpEndPoint const& other) const
        {
            return !(*this == other);
        }

    public:
        IpAddress GetAddress() const;

        uint16_t GetPort() const;
    };
}
