#pragma once
#include "AnemoneRuntime/Storage/FileHandle.hxx"
#include "AnemoneRuntime/Base/Intrusive.hxx"
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

        ~WindowsFileHandle() override;

        WindowsFileHandle() = delete;

        WindowsFileHandle(WindowsFileHandle const&) = delete;

        WindowsFileHandle(WindowsFileHandle&&) noexcept = default;

        WindowsFileHandle& operator=(WindowsFileHandle const&) = delete;

        WindowsFileHandle& operator=(WindowsFileHandle&&) noexcept = default;

    public:
        Interop::Windows::SafeFileHandle const& GetNativeHandle() const
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
