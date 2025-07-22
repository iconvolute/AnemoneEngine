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
        void Flush() override;

        uint64_t GetLength() const override;

        void SetLength(uint64_t length) override;

        uint64_t GetPosition() const override;

        void SetPosition(uint64_t position) override;

        size_t Read(std::span<std::byte> buffer) override;

        size_t ReadAt(std::span<std::byte> buffer, uint64_t position) override;

        size_t Write(std::span<std::byte const> buffer) override;

        size_t WriteAt(std::span<std::byte const> buffer, uint64_t position) override;
    };
}
