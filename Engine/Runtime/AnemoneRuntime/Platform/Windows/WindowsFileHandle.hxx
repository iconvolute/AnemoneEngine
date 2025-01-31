#pragma once
#include "AnemoneRuntime/Platform/Base/BaseFileHandle.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/Flags.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"

#include <utility>
#include <string_view>
#include <expected>
#include <span>

namespace Anemone
{
    class RUNTIME_API WindowsFileHandle final
    {
    private:
        HANDLE _handle{};

    public:
        explicit WindowsFileHandle(HANDLE handle);

        WindowsFileHandle() = default;

        WindowsFileHandle(WindowsFileHandle const&) = delete;

        WindowsFileHandle(WindowsFileHandle&& other) noexcept;

        WindowsFileHandle& operator=(WindowsFileHandle const&) = delete;

        WindowsFileHandle& operator=(WindowsFileHandle&& other) noexcept;

        ~WindowsFileHandle() noexcept;

    public:
        [[nodiscard]] explicit operator bool() const
        {
            return this->IsValid();
        }

        [[nodiscard]] bool IsValid() const
        {
            return this->_handle != nullptr;
        }

        [[nodiscard]] HANDLE GetNativeHandle() const
        {
            return this->_handle;
        }

    public:
        static std::expected<WindowsFileHandle, ErrorCode> Create(
            std::string_view path,
            FileMode mode,
            Flags<FileAccess> access,
            Flags<FileOptions> options);

        static std::expected<WindowsFileHandle, ErrorCode> Create(
            std::string_view path,
            FileMode mode,
            Flags<FileAccess> access)
        {
            return Create(path, mode, access, FileOptions::None);
        }

        static std::expected<WindowsFileHandle, ErrorCode> Create(
            std::string_view path,
            FileMode mode)
        {
            return Create(path, mode, FileAccess::ReadWrite, FileOptions::None);
        }

        static std::expected<void, ErrorCode> CreatePipe(WindowsFileHandle& read, WindowsFileHandle& write);

    public:
        std::expected<void, ErrorCode> Close();

        std::expected<void, ErrorCode> Flush();

        std::expected<int64_t, ErrorCode> GetLength() const;

        std::expected<void, ErrorCode> Truncate(int64_t length);

        std::expected<int64_t, ErrorCode> GetPosition() const;

        std::expected<void, ErrorCode> SetPosition(int64_t position);

        std::expected<size_t, ErrorCode> Read(std::span<std::byte> buffer);

        std::expected<size_t, ErrorCode> ReadAt(std::span<std::byte> buffer, int64_t position);

        std::expected<size_t, ErrorCode> Write(std::span<std::byte const> buffer);

        std::expected<size_t, ErrorCode> WriteAt(std::span<std::byte const> buffer, int64_t position);
    };

    using FileHandle = WindowsFileHandle;
}
