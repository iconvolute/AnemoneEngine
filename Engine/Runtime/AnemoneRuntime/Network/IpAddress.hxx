#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Network/Types.hxx"
#include "AnemoneRuntime/Platform/UninitializedStorage.hxx"

#include <span>
#include <string>
#include <string_view>
#include <optional>

namespace Anemone::Platform
{
#if ANEMONE_PLATFORM_WINDOWS
    using NativeIpAddressStorage = UninitializedStorage<struct NativeIpAddress, 32, 8>;
#elif ANEMONE_PLATFORM_LINUX
    using NativeIpAddressStorage = UninitializedStorage<struct NativeIpAddress, 32, 8>;
#elif ANEMONE_PLATFORM_ANDROID
    using NativeIpAddressStorage = UninitializedStorage<struct NativeIpAddress, 32, 8>;
#else
#error "Not implemented"
#endif
}

namespace Anemone::Network
{
    class MacAddress;
    class UnixAddress;

    class IpEndPoint;

    class RUNTIME_API IpAddress final
    {
        friend class IpEndPoint;

    public:
        Platform::NativeIpAddressStorage Native;

    public:
        IpAddress(Platform::NativeIpAddress const& native);

    public:
        IpAddress();
        IpAddress(IpAddress const& other);
        IpAddress(IpAddress&& other) noexcept;
        IpAddress& operator=(IpAddress const& other);
        IpAddress& operator=(IpAddress&& other) noexcept;
        ~IpAddress();

    public:
        static std::optional<IpAddress> FromBytes(std::span<std::byte const> bytes);
        static std::optional<IpAddress> FromBytes(std::span<std::byte const> bytes, uint32_t scopeId);

        static constexpr size_t IpV4AddressBytes = 4;
        static constexpr size_t IpV6AddressBytes = 16;

    public:
        AddressFamily Family() const;
        uint32_t ScopeId() const;

        bool IsV4() const;
        bool IsV6() const;
        bool IsAny() const;
        bool IsBroadcast() const;
        bool IsLoopback() const;
        //bool IsMulticast() const;
        //bool IsUnicast() const;
        //bool IsLinkLocal() const;
        //bool IsSiteLocal() const;
        //bool IsV4Compatible() const;
        //bool IsV4Mapped() const;
        //bool IsWellKnownMulticast() const;
        //bool IsNodeLocalMulticast() const;
        //bool IsLinkLocalMulticast() const;
        //bool IsSiteLocalMulticast() const;
        //bool IsOrgLocalMulticast() const;
        //bool IsGlobalMulticast() const;

        //IpAddress MapToV4() const;
        //IpAddress MapToV6() const;

        std::span<std::byte const> ToBytes() const;
        std::span<std::byte> ToBytes();
        //uint32_t PrefixLength() const;

        static IpAddress Mask(const IpAddress& address, const IpAddress& mask);
        static IpAddress Mask(const IpAddress& address, const IpAddress& mask, const IpAddress& set);

        bool operator==(const IpAddress& other) const;

        bool operator!=(const IpAddress& other) const
        {
            return !(*this == other);
        }

        static IpAddress Any(AddressFamily family);
        static IpAddress Broadcast(AddressFamily family);
        static IpAddress Loopback(AddressFamily family);
        static IpAddress None(AddressFamily family);

        bool ToString(std::string& result) const;
        static bool TryParse(IpAddress& result, std::string_view address);
        static bool TryParse(IpAddress& result, std::string_view address, AddressFamily family);
    };
}
