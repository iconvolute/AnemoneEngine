#include "AnemoneRuntime/Network/Dns.hxx"
#include "AnemoneRuntime/Diagnostic/Trace.hxx"
#include "AnemoneRuntime/Network/Private.hxx"

#include <array>

#if ANEMONE_PLATFORM_WINDOWS

#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

#elif ANEMONE_PLATFORM_LINUX || ANEMONE_PLATFORM_ANDROID
#else
#error "Not implemented"
#endif

#if ANEMONE_PLATFORM_WINDOWS

namespace Anemone::Network::Private
{
    constexpr ErrorCode MapGAIResultToErrorCode(int value)
    {
        switch (value)
        {
        case EAI_AGAIN:
            return ErrorCode::ResourceUnavailableTryAgain;
        case EAI_BADFLAGS:
            return ErrorCode::InvalidArgument;
        case EAI_MEMORY:
            return ErrorCode::NotEnoughMemory;
        case EAI_NOSECURENAME:
        case EAI_NODATA:
            return ErrorCode::HostUnreachable;
        case EAI_SOCKTYPE:
        case EAI_FAIL:
        case EAI_SERVICE:
            return ErrorCode::NotSupported;
        case EAI_IPSECPOLICY:
            return ErrorCode::OperationNotPermitted;

        default:
            break;
        }

        return ErrorCode::NotSupported;
    }
}

namespace Anemone::Network
{
    std::expected<void, ErrorCode> Resolve(
        std::vector<IpEndPoint>& result,
        std::string_view name,
        std::optional<std::string_view> const& service)
    {
        ADDRINFOW hints{};
        hints.ai_family = AF_UNSPEC; // ipv4 or ipv6
        hints.ai_socktype = 0; // any socket type
        hints.ai_protocol = 0; // any protocol

        ADDRINFOW* ai = nullptr;

        Platform::win32_string_buffer<WCHAR, NI_MAXHOST> wName{};
        if (not Platform::win32_WidenString(wName, name))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        Platform::win32_string_buffer<WCHAR, NI_MAXSERV> wService{};

        if (service)
        {
            if (not Platform::win32_WidenString(wService, *service))
            {
                return std::unexpected(ErrorCode::InvalidArgument);
            }
        }

        int rc = GetAddrInfoW(wName.data(), service ? wService.data() : nullptr, &hints, &ai);

        if (rc == 0)
        {
            while (ai != nullptr)
            {
                if (ai->ai_family == AF_INET)
                {
                    sockaddr_in const* address = reinterpret_cast<sockaddr_in*>(ai->ai_addr);
                    result.emplace_back(
                        Private::into_NativeIpAddress(*address),
                        ntohs(address->sin_port));
                }
                else if (ai->ai_family == AF_INET6)
                {
                    sockaddr_in6 const* address = reinterpret_cast<sockaddr_in6*>(ai->ai_addr);
                    result.emplace_back(
                        Private::into_NativeIpAddress(*address),
                        ntohs(address->sin6_port));
                }

                ai = ai->ai_next;
            }

            FreeAddrInfoW(ai);

            return {};
        }

        return std::unexpected(Private::MapGAIResultToErrorCode(rc));
    }

    RUNTIME_API std::expected<void, ErrorCode> Resolve(
        std::vector<std::string>& result,
        IpEndPoint const& endpoint)
    {
        std::array<wchar_t, NI_MAXHOST> wHost{};
        std::array<wchar_t, NI_MAXSERV> wService{};

        SOCKADDR* address = nullptr;

        SOCKADDR_IN6 ipv6;
        SOCKADDR_IN ipv4;

        if (endpoint.Address.IsV6())
        {
            ipv6.sin6_family = AF_INET6;
            ipv6.sin6_port = htons(static_cast<u_short>(endpoint.Port));
            ipv6.sin6_addr = Platform::Get(endpoint.Address.Native).Address.IPv6;

            address = reinterpret_cast<SOCKADDR*>(&ipv6);
        }
        else
        {
            ipv4.sin_family = AF_INET;
            ipv4.sin_port = htons(static_cast<u_short>(endpoint.Port));
            ipv4.sin_addr = Platform::Get(endpoint.Address.Native).Address.IPv4;

            address = reinterpret_cast<SOCKADDR*>(&ipv4);
        }

        int const rc = GetNameInfoW(
            address,
            sizeof(ipv6),
            wHost.data(),
            static_cast<DWORD>(NI_MAXHOST),
            wService.data(),
            static_cast<DWORD>(NI_MAXSERV),
            NI_NAMEREQD);

        if (rc == 0)
        {
            Platform::win32_string_buffer<char, NI_MAXHOST> sHost{};
            Platform::win32_string_buffer<char, NI_MAXHOST> sService{};

            if (Platform::win32_NarrowString(sHost, wHost.data()) and
                Platform::win32_NarrowString(sService, wService.data()))
            {
                result.emplace_back(fmt::format("[{}]:{}", sHost.as_view(), sService.as_view()));
                return {};
            }

            return std::unexpected(ErrorCode::BadAddress);
        }

        return std::unexpected(ErrorCode::BadAddress);
    }
}

#else

// #error "Not implemented"

#endif
