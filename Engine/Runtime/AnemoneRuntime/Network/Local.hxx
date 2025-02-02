#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"

#include <expected>
#include <span>

namespace Anemone::Network
{
    class LocalServer;
    class LocalClient;

    class LocalServer final
    {
    public:
        LocalServer();
        LocalServer(LocalServer const&) = delete;
        LocalServer(LocalServer&& other) noexcept;
        LocalServer& operator=(LocalServer const&) = delete;
        LocalServer& operator=(LocalServer&& other) noexcept;
        ~LocalServer();

    public:
        static std::expected<LocalServer, ErrorCode> Create(std::string_view path);

    public:
        std::expected<void, ErrorCode> Close();

        std::expected<LocalClient, ErrorCode> Accept();
    };

    class LocalClient final
    {
    public:
        LocalClient();
        LocalClient(LocalClient const&) = delete;
        LocalClient(LocalClient&& other) noexcept;
        LocalClient& operator=(LocalClient const&) = delete;
        LocalClient& operator=(LocalClient&& other) noexcept;
        ~LocalClient();

    public:
        static std::expected<LocalClient, ErrorCode> Connect(std::string_view path);

    public:
        std::expected<void, ErrorCode> Close();

        std::expected<size_t, ErrorCode> Read(std::span<std::byte> buffer);

        std::expected<size_t, ErrorCode> Write(std::span<std::byte const> buffer);
    };
}
