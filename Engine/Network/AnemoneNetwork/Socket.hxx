#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Platform/Types.hxx"
#include "AnemoneRuntime/Diagnostics/Error.hxx"

#include <expected>
#include <span>

namespace Anemone::Network
{
    enum class SocketType
    {
        Stream,
        Datagram,
    };

    enum class ProtocolType
    {
        Tcp,
        Udp,
    };

    enum class AddressFamily
    {
        InterNetwork,
        InterNetworkV6,
    };

    struct SocketEndPoint final
    {
        alignas(8) std::byte Buffer[128];
    };

    class NETWORK_API Socket final
    {
    private:
        Interop::NativeSocket m_native;

    private:
        explicit Socket(Interop::NativeSocket const& native);

    public:
        Socket(Socket const&) = delete;
        Socket(Socket&& other) noexcept;
        Socket& operator=(Socket const&) = delete;
        Socket& operator=(Socket&& other) noexcept;
        ~Socket();

        static std::expected<Socket, Error> Create(
            AddressFamily addressFamily,
            SocketType socketType,
            ProtocolType protocolType);

    public:
        std::expected<void, Error> Close();

        std::expected<void, Error> Bind(SocketEndPoint const& endPoint);

        std::expected<void, Error> Listen(int backlog);

        std::expected<Socket, Error> Accept();

        std::expected<void, Error> Connect(SocketEndPoint const& endPoint);

        std::expected<size_t, Error> Receive(std::span<std::byte> buffer);

        std::expected<size_t, Error> Send(std::span<std::byte const> buffer);

        std::expected<size_t, Error> ReceiveFrom(std::span<std::byte> buffer, SocketEndPoint& endPoint);

        std::expected<size_t, Error> SendTo(std::span<std::byte const> buffer, SocketEndPoint const& endPoint);

        std::expected<SocketEndPoint, Error> GetLocalEndPoint() const;

        std::expected<SocketEndPoint, Error> GetRemoteEndPoint() const;
    };
}
