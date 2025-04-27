#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Platform/Types.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"

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

    class RUNTIME_API Socket final
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

        static std::expected<Socket, ErrorCode> Create(
            AddressFamily addressFamily,
            SocketType socketType,
            ProtocolType protocolType);

    public:
        std::expected<void, ErrorCode> Close();

        std::expected<void, ErrorCode> Bind(SocketEndPoint const& endPoint);

        std::expected<void, ErrorCode> Listen(int backlog);

        std::expected<Socket, ErrorCode> Accept();

        std::expected<void, ErrorCode> Connect(SocketEndPoint const& endPoint);

        std::expected<size_t, ErrorCode> Receive(std::span<std::byte> buffer);

        std::expected<size_t, ErrorCode> Send(std::span<std::byte const> buffer);

        std::expected<size_t, ErrorCode> ReceiveFrom(std::span<std::byte> buffer, SocketEndPoint& endPoint);

        std::expected<size_t, ErrorCode> SendTo(std::span<std::byte const> buffer, SocketEndPoint const& endPoint);

        std::expected<SocketEndPoint, ErrorCode> GetLocalEndPoint() const;

        std::expected<SocketEndPoint, ErrorCode> GetRemoteEndPoint() const;
    };
}
