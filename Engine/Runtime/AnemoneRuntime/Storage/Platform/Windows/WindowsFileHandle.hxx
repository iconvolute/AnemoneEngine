#pragma once
#include "AnemoneRuntime/Storage/FileHandle.hxx"
#include "AnemoneRuntime/Interop/Windows/SafeHandle.hxx"

namespace Anemone
{
    class WindowsFileSystem;

    class WindowsFileHandle final
        : public FileHandle
    {
    private:
        Interop::Windows::SafeFileHandle _handle{};

    public:
        explicit WindowsFileHandle(Interop::Windows::SafeFileHandle handle);

        WindowsFileHandle() = delete;

        WindowsFileHandle(WindowsFileHandle const&) = delete;

        WindowsFileHandle(WindowsFileHandle&&) noexcept = delete;

        ~WindowsFileHandle() override;

        WindowsFileHandle& operator=(WindowsFileHandle const&) = delete;

        WindowsFileHandle& operator=(WindowsFileHandle&&) noexcept = delete;

    public:
        Interop::Windows::SafeFileHandle const& GetNativeHandle() const
        {
            return this->_handle;
        }

    public:
        std::expected<void, Error> Flush() override;

        std::expected<uint64_t, Error> GetLength() const override;

        std::expected<void, Error> SetLength(uint64_t length) override;

        std::expected<uint64_t, Error> GetPosition() const override;

        std::expected<void, Error> SetPosition(uint64_t position) override;

        std::expected<size_t, Error> Read(std::span<std::byte> buffer) override;

        std::expected<size_t, Error> ReadAt(std::span<std::byte> buffer, uint64_t position) override;

        std::expected<size_t, Error> Write(std::span<std::byte const> buffer) override;

        std::expected<size_t, Error> WriteAt(std::span<std::byte const> buffer, uint64_t position) override;
    };
}
