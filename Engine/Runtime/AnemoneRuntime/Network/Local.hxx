#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Diagnostics/Error.hxx"

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
        static std::expected<LocalServer, Error> Create(std::string_view path);

    public:
        std::expected<void, Error> Close();

        std::expected<LocalClient, Error> Accept();
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
        static std::expected<LocalClient, Error> Connect(std::string_view path);

    public:
        std::expected<void, Error> Close();

        std::expected<size_t, Error> Read(std::span<std::byte> buffer);

        std::expected<size_t, Error> Write(std::span<std::byte const> buffer);
    };
}
