#include "AnemoneRuntime/Network/Socket.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

#include "AnemoneRuntime/Diagnostics/Platform/Error.hxx"

#include <optional>
#include <utility>

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Platform/Windows/Types.hxx"
#endif

namespace Anemone::Network::Detail
{
    static Status GetLastSocketError()
    {
#if ANEMONE_PLATFORM_WINDOWS
        return Internal::TranslateErrorCodeWin32(WSAGetLastError());
#else
        return Internal::TranslateErrorCodeErrno(errno);
#endif
    }

    static std::optional<int> ConvertSocketType(SocketType value)
    {
        switch (value)
        {
        case SocketType::Stream:
            return SOCK_STREAM;

        case SocketType::Datagram:
            return SOCK_DGRAM;

        default:
            break;
        }

        return {};
    }

    static std::optional<int> ConvertProtocolType(ProtocolType value)
    {
        switch (value)
        {
        case ProtocolType::Tcp:
            return IPPROTO_TCP;

        case ProtocolType::Udp:
            return IPPROTO_UDP;

        default:
            break;
        }

        return {};
    }

    static std::optional<int> ConvertAddressFamily(AddressFamily value)
    {
        switch (value)
        {
        case AddressFamily::InterNetwork:
            return AF_INET;

        case AddressFamily::InterNetworkV6:
            return AF_INET6;

        default:
            break;
        }

        return {};
    }
}

namespace Anemone::Network
{
    Socket::Socket(Interop::NativeSocket const& native)
        : m_native{native}
    {
    }

    Socket::Socket(Socket&& other) noexcept
        : m_native{std::exchange(other.m_native, Interop::NativeSocket{})}
    {
    }

    Socket& Socket::operator=(Socket&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            if (this->m_native.Inner != 0)
            {
#if ANEMONE_PLATFORM_WINDOWS
                if (not closesocket(this->m_native.Inner))
                {
                    AE_TRACE(Error, "Failed to close socket: {}", WSAGetLastError());
                }
#else
                if (not close(this->m_native.Inner))
                {
                    AE_TRACE(Error, "Failed to close socket: {}", errno);
                }
#endif
            }

            this->m_native = std::exchange(other.m_native, Interop::NativeSocket{});
        }

        return *this;
    }

    Socket::~Socket()
    {
        if (this->m_native.Inner != 0)
        {
#if ANEMONE_PLATFORM_WINDOWS
            if (not closesocket(this->m_native.Inner))
            {
                AE_TRACE(Error, "Failed to close socket: {}", WSAGetLastError());
            }
#else
            if (not close(this->m_native.Inner))
            {
                AE_TRACE(Error, "Failed to close socket: {}", errno);
            }
#endif
        }
    }

    std::expected<Socket, Status> Socket::Create(AddressFamily addressFamily, SocketType socketType, ProtocolType protocolType)
    {
        auto nativeAddressFamily = Detail::ConvertAddressFamily(addressFamily);
        auto nativeSocketType = Detail::ConvertSocketType(socketType);
        auto nativeProtocolType = Detail::ConvertProtocolType(protocolType);

        if (not nativeAddressFamily or not nativeSocketType or not nativeProtocolType)
        {
            return std::unexpected(Status::InvalidArgument);
        }

        auto handle = socket(
            *nativeAddressFamily,
            *nativeSocketType,
            *nativeProtocolType);

#if ANEMONE_PLATFORM_WINDOWS
        if (handle == INVALID_SOCKET)
        {
            return std::unexpected(Internal::TranslateErrorCodeWin32(WSAGetLastError()));
        }
#else
        if (handle < 0)
        {
            return std::unexpected(Internal::TranslateErrorCodeErrno(errno));
        }
#endif

        return Socket{Interop::NativeSocket{handle}};
    }

    std::expected<void, Status> Socket::Close()
    {
        if (this->m_native.Inner != 0)
        {
#if ANEMONE_PLATFORM_WINDOWS
            if (not closesocket(this->m_native.Inner))
            {
                return std::unexpected(Internal::TranslateErrorCodeWin32(WSAGetLastError()));
            }
#else
            if (not close(this->m_native.Inner))
            {
                return std::unexpected(Internal::TranslateErrorCodeErrno(errno));
            }
#endif

            this->m_native.Inner = 0;
        }

        return {};
    }

    std::expected<void, Status> Socket::Bind(SocketEndPoint const& endPoint)
    {
        if (bind(this->m_native.Inner, reinterpret_cast<sockaddr const*>(endPoint.Buffer), sizeof(sockaddr)) < 0)
        {
            return std::unexpected(Detail::GetLastSocketError());
        }

        return {};
    }

    std::expected<void, Status> Socket::Listen(int backlog)
    {
        if (listen(this->m_native.Inner, backlog) < 0)
        {
            return std::unexpected(Detail::GetLastSocketError());
        }

        return {};
    }

    std::expected<Socket, Status> Socket::Accept()
    {
        auto handle = accept(this->m_native.Inner, nullptr, nullptr);

#if ANEMONE_PLATFORM_WINDOWS
        if (handle == INVALID_SOCKET)
#else
        if (handle < 0)
#endif
        {
            return std::unexpected(Detail::GetLastSocketError());
        }

        return Socket{Interop::NativeSocket{handle}};
    }

    std::expected<void, Status> Socket::Connect(SocketEndPoint const& endPoint)
    {
        if (connect(this->m_native.Inner, reinterpret_cast<sockaddr const*>(endPoint.Buffer), sizeof(sockaddr)) < 0)
        {
            return std::unexpected(Detail::GetLastSocketError());
        }

        return {};
    }

    std::expected<size_t, Status> Socket::Receive(std::span<std::byte> buffer)
    {
        auto result = recv(this->m_native.Inner, reinterpret_cast<char*>(buffer.data()), static_cast<int>(buffer.size()), 0);

        if (result < 0)
        {
            return std::unexpected(Detail::GetLastSocketError());
        }

        return static_cast<size_t>(result);
    }

    std::expected<size_t, Status> Socket::Send(std::span<std::byte const> buffer)
    {
        auto result = send(this->m_native.Inner, reinterpret_cast<char const*>(buffer.data()), static_cast<int>(buffer.size()), 0);

        if (result < 0)
        {
            return std::unexpected(Detail::GetLastSocketError());
        }

        return static_cast<size_t>(result);
    }

    std::expected<size_t, Status> Socket::ReceiveFrom(std::span<std::byte> buffer, SocketEndPoint& endPoint)
    {
        socklen_t addressSize = sizeof(sockaddr);

        auto result = recvfrom(this->m_native.Inner, reinterpret_cast<char*>(buffer.data()), static_cast<int>(buffer.size()), 0, reinterpret_cast<sockaddr*>(endPoint.Buffer), &addressSize);

        if (result < 0)
        {
            return std::unexpected(Detail::GetLastSocketError());
        }

        return static_cast<size_t>(result);
    }

    std::expected<size_t, Status> Socket::SendTo(std::span<std::byte const> buffer, SocketEndPoint const& endPoint)
    {
        auto result = sendto(this->m_native.Inner, reinterpret_cast<char const*>(buffer.data()), static_cast<int>(buffer.size()), 0, reinterpret_cast<sockaddr const*>(endPoint.Buffer), sizeof(sockaddr));

        if (result < 0)
        {
            return std::unexpected(Detail::GetLastSocketError());
        }

        return static_cast<size_t>(result);
    }

    std::expected<SocketEndPoint, Status> Socket::GetLocalEndPoint() const
    {
        SocketEndPoint result;
        socklen_t addressSize = sizeof(result.Buffer);

        if (getsockname(this->m_native.Inner, reinterpret_cast<sockaddr*>(result.Buffer), &addressSize) < 0)
        {
            return std::unexpected(Detail::GetLastSocketError());
        }

        return result;
    }

    std::expected<SocketEndPoint, Status> Socket::GetRemoteEndPoint() const
    {
        SocketEndPoint result;
        socklen_t addressSize = sizeof(result.Buffer);

        if (getpeername(this->m_native.Inner, reinterpret_cast<sockaddr*>(result.Buffer), &addressSize) < 0)
        {
            return std::unexpected(Detail::GetLastSocketError());
        }

        return result;
    }
}
