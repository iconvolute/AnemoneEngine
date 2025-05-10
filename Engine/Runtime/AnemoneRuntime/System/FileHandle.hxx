#pragma once
#include "AnemoneRuntime/System/Platform/FileHandle.hxx"
#include "AnemoneRuntime/Base/ErrorCode.hxx"
#include "AnemoneRuntime/Base/Flags.hxx"

#include <utility>
#include <string_view>
#include <expected>
#include <span>

namespace Anemone
{
    enum class FileMode : uint32_t
    {
        // Creates new file. Truncate if exists.
        CreateAlways,

        // Create new file. Fail if exists.
        CreateNew,

        // Create new file. Open if exists.
        OpenAlways,

        // Open if exists. Fail otherwise.
        OpenExisting,

        // Truncates existing file. Fails otherwise.
        TruncateExisting,
    };

    enum class FileAccess : uint32_t
    {
        Read = 1u << 0u,
        Write = 1u << 1u,
        ReadWrite = Read | Write,
    };

    enum class FileOption : uint32_t
    {
        None = 0u,
        ShareRead = 1u << 0u,
        ShareWrite = 1u << 1u,
        Inheritable = 1u << 2u, // Is it really needed?
        DeleteOnClose = 1u << 3u,
        RandomAccess = 1u << 4u,
        SequentialScan = 1u << 5u,
        WriteThrough = 1u << 6u,
        NoBuffering = 1u << 7u,
        ShareDelete = 1u << 8u,
    };

    class RUNTIME_API FileHandle final
    {
    private:
        Internal::PlatformFileHandle _handle;

    public:
        explicit FileHandle(Internal::PlatformFileHandle handle)
            : _handle{std::move(handle)}
        {
        }

        FileHandle() = default;
        FileHandle(FileHandle const&) = delete;
        FileHandle(FileHandle&&) noexcept = default;
        FileHandle& operator=(FileHandle const&) = delete;
        FileHandle& operator=(FileHandle&&) noexcept = default;
        ~FileHandle() = default;

    public:
        [[nodiscard]] explicit operator bool() const
        {
            return this->_handle.IsValid();
        }

        [[nodiscard]] bool IsValid() const
        {
            return this->_handle.IsValid();
        }

        [[nodiscard]] Internal::PlatformFileHandle const& GetNativeHandle() const
        {
            return this->_handle;
        }

    public:
        static std::expected<FileHandle, ErrorCode> Create(
            std::string_view path,
            FileMode mode,
            Flags<FileAccess> access,
            Flags<FileOption> options);

        static std::expected<FileHandle, ErrorCode> Create(
            std::string_view path,
            FileMode mode,
            Flags<FileAccess> access)
        {
            return Create(path, mode, access, FileOption::None);
        }

        static std::expected<FileHandle, ErrorCode> Create(
            std::string_view path,
            FileMode mode)
        {
            return Create(path, mode, FileAccess::ReadWrite, FileOption::None);
        }

        static std::expected<void, ErrorCode> CreatePipe(FileHandle& read, FileHandle& write);

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
}
