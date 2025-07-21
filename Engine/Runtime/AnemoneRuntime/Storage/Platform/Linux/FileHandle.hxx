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
        explicit LinuxFileHandle(Interop::Linux::SafeFdHandle handle);

        ~LinuxFileHandle() override;

        LinuxFileHandle() = delete;

        LinuxFileHandle(LinuxFileHandle const&) = delete;

        LinuxFileHandle(LinuxFileHandle&&) noexcept = default;

        LinuxFileHandle& operator=(LinuxFileHandle const&) = delete;

        LinuxFileHandle& operator=(LinuxFileHandle&&) noexcept = default;

    public:
        Interop::Linux::SafeFdHandle const& GetNativeHandle() const
        {
            return this->_handle;
        }

    public:
        std::expected<void, Status> Flush() override;

        std::expected<int64_t, Status> GetLength() const override;

        std::expected<void, Status> Truncate(int64_t length) override;

        std::expected<int64_t, Status> GetPosition() const override;

        std::expected<void, Status> SetPosition(int64_t position) override;

        std::expected<size_t, Status> Read(std::span<std::byte> buffer) override;

        std::expected<size_t, Status> ReadAt(std::span<std::byte> buffer, int64_t position) override;

        std::expected<size_t, Status> Write(std::span<std::byte const> buffer) override;

        std::expected<size_t, Status> WriteAt(std::span<std::byte const> buffer, int64_t position) override;
    };
}
