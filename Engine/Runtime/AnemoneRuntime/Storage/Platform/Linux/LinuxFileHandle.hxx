#pragma once
#include "AnemoneRuntime/Storage/FileHandle.hxx"
#include "AnemoneRuntime/Interop/Linux/SafeHandle.hxx"

namespace Anemone
{
    class LinuxFileSystem;

    class LinuxFileHandle final
        : public FileHandle
    {
    private:
        Interop::Linux::SafeFdHandle _handle{};

    public:
        explicit LinuxFileHandle(
            Interop::Linux::SafeFdHandle handle);

        LinuxFileHandle() = delete;

        LinuxFileHandle(
            LinuxFileHandle const&) = delete;

        LinuxFileHandle(
            LinuxFileHandle&&) noexcept = delete;

        ~LinuxFileHandle() override;

        LinuxFileHandle& operator=(
            LinuxFileHandle const&) = delete;

        LinuxFileHandle& operator=(
            LinuxFileHandle&&) noexcept = delete;

    public:
        Interop::Linux::SafeFdHandle const& GetNativeHandle() const
        {
            return this->_handle;
        }

    public:
        std::expected<void, Error> Flush() override;

        std::expected<uint64_t, Error> GetLength() const override;

        std::expected<void, Error> SetLength(
            uint64_t length) override;

        std::expected<uint64_t, Error> GetPosition() const override;

        std::expected<void, Error> SetPosition(
            uint64_t position) override;

        std::expected<size_t, Error> Read(
            std::span<std::byte> buffer) override;

        std::expected<size_t, Error> ReadAt(
            std::span<std::byte> buffer,
            uint64_t position) override;

        std::expected<size_t, Error> Write(
            std::span<std::byte const> buffer) override;

        std::expected<size_t, Error> WriteAt(
            std::span<std::byte const> buffer,
            uint64_t position) override;
    };
}
