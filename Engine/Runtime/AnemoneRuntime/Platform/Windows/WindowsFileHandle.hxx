#pragma once
#include "AnemoneRuntime/Platform/Base/BaseFileHandle.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsSafeHandle.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

#include <utility>
#include <string_view>
#include <expected>
#include <span>

namespace Anemone
{
    class RUNTIME_API WindowsFileHandle final
    {
    private:
        Interop::Win32SafeFileHandle _handle{};

    public:
        explicit WindowsFileHandle(Interop::Win32SafeFileHandle handle)
            : _handle{std::move(handle)}
        {
        }

        WindowsFileHandle() = default;

    public:
        [[nodiscard]] explicit operator bool() const
        {
            return this->_handle.IsValid();
        }

        [[nodiscard]] bool IsValid() const
        {
            return this->_handle.IsValid();
        }

        [[nodiscard]] Interop::Win32SafeFileHandle const& GetNativeHandle() const
        {
            return this->_handle;
        }

    public:
        static std::expected<WindowsFileHandle, ErrorCode> Create(
            std::string_view path,
            FileMode mode,
            Flags<FileAccess> access,
            Flags<FileOption> options);

        static std::expected<WindowsFileHandle, ErrorCode> Create(
            std::string_view path,
            FileMode mode,
            Flags<FileAccess> access)
        {
            return Create(path, mode, access, FileOption::None);
        }

        static std::expected<WindowsFileHandle, ErrorCode> Create(
            std::string_view path,
            FileMode mode)
        {
            return Create(path, mode, FileAccess::ReadWrite, FileOption::None);
        }

        static std::expected<void, ErrorCode> CreatePipe(WindowsFileHandle& read, WindowsFileHandle& write);

    public:
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
