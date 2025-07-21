#pragma once
#include "AnemoneRuntime/Storage/FileHandle.hxx"
#include "AnemoneRuntime/Base/Intrusive.hxx"
#include "AnemoneRuntime/Interop/Windows/SafeHandle.hxx"

namespace Anemone
{
    class WindowsFileSystem;

    class WindowsFileHandle final
        : public FileHandle
        , public IntrusiveListNode<WindowsFileHandle>
    {
    private:
        Interop::Windows::SafeFileHandle _handle{};
        WindowsFileSystem* _owner{};

    public:
        explicit WindowsFileHandle(Interop::Windows::SafeFileHandle handle, WindowsFileSystem* owner);

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
